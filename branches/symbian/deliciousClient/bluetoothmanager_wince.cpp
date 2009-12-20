#include "bluetoothmanager_wince.h"
#include "bluetoothmanager_win_common.h"
#include <cassert>
#include <exception>
#include <bt_api.h>
#include <bthutil.h>
#include <bthapi.h>
#include <QDebug>

#define MAX_NAME_SIZE 256
#define MAX_MESSAGE_SIZE 256

using namespace std;

// a pseudo device represents the local bluetooth device
static bluetoothdevice_win *localdevice = NULL;

bluetoothmanager_wince::bluetoothmanager_wince(void)
:m_callback(NULL)
{
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD( 2, 2 );
    WSAStartup( wVersionRequested, &wsaData );

    m_hReadThread = CreateThread(NULL, 0, ReadThread, this, 0, NULL);
    m_listensock = INVALID_SOCKET;

    BthGetMode(&m_bthmode);
}

bluetoothmanager_wince::~bluetoothmanager_wince(void)
{
    for (int i=0;i<NumOfDevices();++i)
        delete m_devices[i];
    WSACleanup();
}

bool bluetoothmanager_wince::IsUseable()
{
    return m_bthmode != BTH_POWER_OFF;
}

bool bluetoothmanager_wince::IsDiscoverable()
{
    return m_bthmode == BTH_DISCOVERABLE;
}

int bluetoothmanager_wince::Discover()
{
    INT				iResult = 0;
    LPWSAQUERYSET	pwsaResults;
    DWORD			dwSize = 0;
    WSAQUERYSET		wsaq;
    HANDLE			hLookup = 0;

    memset (&wsaq, 0, sizeof(wsaq));
    wsaq.dwSize      = sizeof(wsaq);
    wsaq.dwNameSpace = NS_BTH;
    wsaq.lpcsaBuffer = NULL;

    // initialize searching procedure
    iResult = WSALookupServiceBegin(&wsaq, 
        LUP_CONTAINERS, 
        &hLookup);

    if (iResult != 0)
    {
        char tszErr[32];
        iResult = WSAGetLastError();		
        sprintf(tszErr, "Socket Error: %d", iResult);
        WSALookupServiceEnd(hLookup);
        throw std::exception(tszErr);
    }

    union {
        CHAR buf[5000];				// returned struct can be quite large 
        SOCKADDR_BTH	__unused;	// properly align buffer to BT_ADDR requirements
    };
    pwsaResults = (LPWSAQUERYSET) buf;
    dwSize  = sizeof(buf);

    for (; ;)
    {
        memset(pwsaResults,0,sizeof(WSAQUERYSET));
        pwsaResults->dwSize      = sizeof(WSAQUERYSET);
        // namespace MUST be NS_BTH for bluetooth queries
        pwsaResults->dwNameSpace = NS_BTH;
        pwsaResults->lpBlob      = NULL;

        // iterate through all found devices, returning name and address
        // (this sample only uses the name, but address could be used for
        // further queries)
        iResult = WSALookupServiceNext (hLookup, 
            LUP_RETURN_NAME | LUP_RETURN_ADDR, 
            &dwSize, 
            pwsaResults);


        if (iResult != 0)
        {
            iResult = WSAGetLastError();
            if (iResult != WSA_E_NO_MORE)
            {
                char tszErr[32];
                iResult = WSAGetLastError();		
                sprintf(tszErr, "Socket Error: %d", iResult);
                WSALookupServiceEnd(hLookup);
                throw std::exception(tszErr);
            }
            //finished
            break;
        }

        // add the name to the listbox
        if (pwsaResults->lpszServiceInstanceName)
        {
            char buf[255] = {0};
            wcstombs(buf, pwsaResults->lpszServiceInstanceName, sizeof(buf));
            m_devices.push_back(new bluetoothdevice_win(buf, pwsaResults->lpcsaBuffer->RemoteAddr));
        }
    }

    WSALookupServiceEnd(hLookup);

    return NumOfDevices();
}

int bluetoothmanager_wince::NumOfDevices()
{
    return m_devices.size();
}

bluetoothdevice* bluetoothmanager_wince::GetDevice( int id )
{
    assert(id >= 0);
    return id >= NumOfDevices() ? NULL : m_devices[id];
}

bluetoothdevice* bluetoothmanager_wince::GetLocalDevice()
{
    HKEY hKey;
    DWORD dwRegType, dwRegSize;
    char devicename[MAX_NAME_SIZE];

    if (!localdevice)
    {
        if(RegOpenKeyEx(HKEY_CURRENT_USER,L"ControlPanel\\Owner",0,0,&hKey)==ERROR_SUCCESS)
        {
            if(RegQueryValueEx(hKey,L"Name",0,&dwRegType,(LPBYTE)devicename,&dwRegSize)==ERROR_SUCCESS)
            {
                if (dwRegSize>MAX_NAME_SIZE) 
                {
                    RegCloseKey(hKey);
                    return localdevice; 
                }
                RegCloseKey(hKey);
            }
            RegCloseKey(hKey);
        }
        localdevice = new bluetoothdevice_win(devicename);
    }
    return localdevice;
}

RECVFUNC bluetoothmanager_wince::SetRecvCallback( RECVFUNC func )
{
    std::swap(func, m_callback);
    return func;
}
int bluetoothmanager_wince::SendToDevice( bluetoothdevice* device, void* data, size_t cb )
{
    bluetoothdevice_win* bthdev = (bluetoothdevice_win*)device;//dynamic_cast<bluetoothdevice_wince*>(device);
    assert(bthdev && data && cb);

    if (bthdev && data && cb)
    {
        SOCKET s = socket (AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
        if (connect(s, bthdev->Addr().lpSockaddr, bthdev->Addr().iSockaddrLength))
        {
            char errbuf[32];
            sprintf(errbuf, "Socket Error: %d", WSAGetLastError());
            qDebug()<<errbuf;
            throw std::exception(errbuf);
        }
        size_t sendbyte = send(s, (const char*)data, cb, 0);
        closesocket(s);
        return sendbyte;
        qDebug()<<"send finished.";
    }
    else
        return 0;
}

DWORD WINAPI bluetoothmanager_wince::ReadThread( LPVOID param )
{
    bluetoothmanager_wince *mgr = reinterpret_cast<bluetoothmanager_wince*>(param);
    SOCKADDR_BTH client;
    int addrsize=0, cbBytesRecd=0;
    char pbuf[MAX_MESSAGE_SIZE];

    if (mgr)
    {
        mgr->OpenServerConnection(rgbSdpRecord, SDP_RECORD_SIZE, SDP_CHANNEL_OFFSET);
        while (1)
        {
            BTH_REMOTE_NAME remotename;
            addrsize = sizeof(client);

            SOCKET s = accept (mgr->m_listensock, (SOCKADDR*)&client, &addrsize);
            if (s != INVALID_SOCKET)
            {
                memset(&remotename, sizeof(remotename), 0);
                remotename.bt = client.btAddr;

                setsockopt(mgr->m_listensock,SOL_RFCOMM, SO_BTH_SET_READ_REMOTE_NAME, (char*)&remotename, sizeof(remotename));
                //receive data in pbuf
                cbBytesRecd = recv (s, pbuf, MAX_MESSAGE_SIZE, 0);
                // something was received, then copy the contents in szMessage
                if(cbBytesRecd>0)
                {
                    //mgr->m_callback(pbuf);
                }
                ACK(s, 10);
                closesocket(s);
            }
        }
    }
    return 0;
}

bool bluetoothmanager_wince::ACK( SOCKET s, int retrytimes )
{
    static const char buf[] = "ACK";
    while (retrytimes--)
    {
        if ( send(s, buf, sizeof(buf), 0) == sizeof(buf) )
            return true;
    }
    return false;
}

int bluetoothmanager_wince::OpenServerConnection( BYTE *rgbSdpRecord, int cSdpRecord, int iChannelOffset )
{
    int iNameLen=0;
    if(m_listensock==INVALID_SOCKET)
    {
        m_listensock = socket (AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
        if (m_listensock  == INVALID_SOCKET) 
        {
            return WSAGetLastError ();
        }

        SOCKADDR_BTH sa;
        memset (&sa, 0, sizeof(sa));
        sa.addressFamily = AF_BTH;
        sa.port = 0;
        if (bind (m_listensock, (SOCKADDR *)&sa, sizeof(sa))) 
        {
            return WSAGetLastError ();
        }
        iNameLen = sizeof(sa);
        if (getsockname(m_listensock, (SOCKADDR *)&sa, &iNameLen))	
        {
            return WSAGetLastError ();
        }

        if(RegisterService(rgbSdpRecord, cSdpRecord, iChannelOffset, (UCHAR)sa.port)!=0)
            return WSAGetLastError();

        if (listen (m_listensock, SOMAXCONN)) 
        {
            return WSAGetLastError ();
        }
    }
    return 0;
}

int bluetoothmanager_wince::RegisterService( BYTE *rgbSdpRecord, int cSdpRecord, int iChannelOffset, UCHAR channel )
{
    ULONG recordHandle = 0;

    struct bigBlob
    {
        BTHNS_SETBLOB   b;

    }*pBigBlob;

    pBigBlob = (bigBlob *)malloc(sizeof(struct bigBlob)+cSdpRecord);
    ULONG ulSdpVersion = BTH_SDP_VERSION;
    pBigBlob->b.pRecordHandle   = &recordHandle;
    pBigBlob->b.pSdpVersion     = &ulSdpVersion;
    pBigBlob->b.fSecurity       = 0;
    pBigBlob->b.fOptions        = 0;
    pBigBlob->b.ulRecordLength  = cSdpRecord;

    memcpy (pBigBlob->b.pRecord, rgbSdpRecord, cSdpRecord);
    pBigBlob->b.pRecord[iChannelOffset] = (unsigned char)channel;
    BLOB blob;
    blob.cbSize    = sizeof(BTHNS_SETBLOB) + cSdpRecord - 1;
    blob.pBlobData = (PBYTE) pBigBlob;

    WSAQUERYSET Service;
    memset (&Service, 0, sizeof(Service));
    Service.dwSize = sizeof(Service);
    Service.lpBlob = &blob;
    Service.dwNameSpace = NS_BTH;
    if (WSASetService(&Service,RNRSERVICE_REGISTER,0) == SOCKET_ERROR)
    {
        free(pBigBlob);
        return WSAGetLastError();
    }
    else
    {
        free(pBigBlob);
        return 0;
    }
}
