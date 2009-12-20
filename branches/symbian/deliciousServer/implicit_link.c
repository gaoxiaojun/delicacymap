#define PANTHEIOS_IMPLICIT_LINK_SUPPORT

#include <pantheios/implicit_link/core.h>
#include <pantheios/implicit_link/util.h>
#include <pantheios/implicit_link/fe.simple.h>
#include <pantheios/implicit_link/be.lrsplit.h>
#include <pantheios/implicit_link/ber.file.h>

#ifdef WIN32
#include <pantheios/implicit_link/bel.WindowsConsole.WithCallback.h>
#include <pantheios/backends/bec.WindowsConsole.h>

PANTHEIOS_CALL(void) pantheios_be_WindowsConsole_getAppInit(int backEndId, pan_be_WindowsConsole_init_t* init)
{
    init->flags |=  PANTHEIOS_BE_INIT_F_NO_PROCESS_ID;
    init->flags |=  PANTHEIOS_BE_INIT_F_HIDE_DATE;
    init->flags |=  PANTHEIOS_BE_INIT_F_HIGH_RESOLUTION;

    //init->colours[PANTHEIOS_SEV_DEBUG] = init->colours[PANTHEIOS_SEV_INFORMATIONAL];
    //init->colours[PANTHEIOS_SEV_NOTICE] = init->colours[PANTHEIOS_SEV_INFORMATIONAL];
}

#else   //assume linux
#include <pantheios/implicit_link/bel.fprintf.h>
#include <pantheios/backends/bec.fprintf.h>
#include <stdio.h>
int pantheios_be_fprintf_init (char const *processIdentity, int id, pan_be_fprintf_init_t const *unused, void *reserved, void **ptoken)
{
    return 0;
}

void pantheios_be_fprintf_uninit (void *token)
{
}

int pantheios_be_fprintf_logEntry (void *feToken, void *beToken, int severity, char const *entry, size_t cchEntry)
{
    return puts(entry);
}
#endif

