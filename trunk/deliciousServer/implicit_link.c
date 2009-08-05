#include <pantheios/implicit_link/core.h>
#include <pantheios/implicit_link/util.h>
#include <pantheios/implicit_link/fe.simple.h>
#include <pantheios/implicit_link/be.lrsplit.h>
#include <pantheios/implicit_link/bel.WindowsConsole.WithCallback.h>
#include <pantheios/implicit_link/ber.file.h>

#include <pantheios/backends/bec.WindowsConsole.h>

PANTHEIOS_CALL(void) pantheios_be_WindowsConsole_getAppInit(int backEndId, pan_be_WindowsConsole_init_t* init)
{
    init->flags |=  PANTHEIOS_BE_INIT_F_NO_PROCESS_ID;
    init->flags |=  PANTHEIOS_BE_INIT_F_HIDE_DATE;
    init->flags |=  PANTHEIOS_BE_INIT_F_HIGH_RESOLUTION;

    //init->colours[PANTHEIOS_SEV_DEBUG] = init->colours[PANTHEIOS_SEV_INFORMATIONAL];
    //init->colours[PANTHEIOS_SEV_NOTICE] = init->colours[PANTHEIOS_SEV_INFORMATIONAL];
}
