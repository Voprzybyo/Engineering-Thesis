#include "RIIM_UAPI.h"

static uint8_t          printoutTimerHandle;
static const uint32_t   printoutTimerPeriod=5000;


void printout()
{
    Util.printf("Wgrano nowy program na Sensor Board'a\n");
}

RIIM_SETUP()
{
    Util.printf("Nowy program!\n");

    // Start as a router
    Network.startLeaf();

    // Create timer that sends the CoAP GET message
    printoutTimerHandle=Timer.create(PERIODIC, printoutTimerPeriod, printout);
    Timer.start(printoutTimerHandle);

    Debug.printSetup();
    
    return UAPI_OK;
}

