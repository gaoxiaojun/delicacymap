// deliciousServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tcpserver.h"
#include "naked_conn.h"
#include "deliciousDataAdapter.h"
#include <string>
#include <exception>
#include <pantheios/backends/bec.file.h>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost::asio;

extern "C"
const char PANTHEIOS_FE_PROCESS_IDENTITY[]  =   "deliciousServer";

static const int MonitorPort = 24000;

boost::asio::io_service io;
rclib::network::TCPServer<naked_conn> *serverd;

bool InitializeComponents();

int _tmain(int argc, _TCHAR* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    //////////////////////////////////////////////////////////////////////////
    pantheios_be_file_setFilePath("delicious.log");
    //////////////////////////////////////////////////////////////////////////

    pantheios::log_NOTICE("Server launched.");
    
    if (!InitializeComponents())
    {
        pantheios::log_NOTICE("Initialize Faile. Terminating...");
        return -1;
    }
    serverd->start();    
    pantheios::log_NOTICE("Server Start.");
    boost::array<boost::thread, 4> threadpool;
    BOOST_FOREACH(boost::thread& t, threadpool)
        t.swap(boost::thread(boost::bind(&io_service::run, &io)));

    io.run();

    BOOST_FOREACH(boost::thread& t, threadpool)
        t.join();
    pantheios::log_NOTICE("Server Stoped normally.");
    return 0;
}

bool InitializeComponents()
{
    pantheios::log_NOTICE("Begin Initialize.");
    try
    {
        serverd = new rclib::network::TCPServer<naked_conn>(io, MonitorPort);
        pantheios::log_INFORMATIONAL("Network daemon initialized.");
        deliciousDataAdapter::Initialize();
        pantheios::log_INFORMATIONAL("Database initialized.");

        pantheios::log_NOTICE("Initialization Finished Successfully.");
        return true;
    }
    catch (exception& e)
    {
        pantheios::log_CRITICAL(e.what());

        return false;
    }
}
