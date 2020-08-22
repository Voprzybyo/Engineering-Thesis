/*****************************************************************************
 * Copyright ©2019. Radiocrafts AS (Radiocrafts).  All Rights Reserved. 
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation, without fee and without a signed licensing 
 * agreement, is hereby granted, provided that the above copyright 
 * notice, this paragraph and the following two paragraphs appear in 
 * all copies, modifications, and distributions.
 * 
 * IN NO EVENT SHALL RADIOCRFTS BE LIABLE TO ANY PARTY FOR DIRECT, 
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING 
 * LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS 
 * DOCUMENTATION, EVEN IF RADIOCRAFTS HAS BEEN ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE. 
 * 
 * RADIOCRAFTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT 
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING 
 * DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS PROVIDED "AS IS". 
 * RADIOCRAFTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
 * UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
****************************************************************************/

/**
 @file
 @brief This is a data-to-cloud example for RIIM
 @version 1.00 : First revision
 @date 2019.02.25
*/

#include "RIIM_UAPI.h"


///////////////////////////////////////////////////
// Cloud server setup variables : Resource_Name and server_ip4_addr
///////////////////////////////////////////////////

// Change Resource_Name to your CoAP resource address
static const uint8_t resourceName[]="/api/v1/1IeLMuBZ6oI76pAmoskq/telemetry/";
// Change server_ip4_addr to the address of your server.
const uint8_t serverIP4Addr[4]={104,196,24,70};



uint8_t startupTimerHandle;
const uint32_t coapTimerPeriod=10000;
uint8_t coapTimerHandle;

/**
 * @brief This function sends a CoAP message to the server (cloud)
 */
static void sendCoAP()
{
    // We first check if we are part of a network and the address of the root node
    if(Network.getNetworkState() != ONLINE){
        Util.printf("Not part of network yet\n");
        return;
    }

    uint8_t payload[100];
    int payloadLength;

    payloadLength=Util.snprintf((char*)payload, sizeof(payload), "{\"Data1\":%i}", Util.getTemperature());

    Util.printf("Sending CoAP message : %s\n", payload);
    if(CoAP.send(CoAP_POST, false, resourceName, payload, payloadLength) != UAPI_OK){
        Util.printf("Failed to send CoAP message\n");
    };
    
    return;
}

/**
 * @brief This function checks that we are part of a network before continuing
 */
static void startup()
{
    // Wait until we are part of network
    if(Network.getNetworkState() != ONLINE){
        Util.printf("Waiting for network...\n");
        return;
    }

    IPAddr rootNodeIPAddr;
    if(Network.getBorderRouterAddress(&rootNodeIPAddr)!=UAPI_OK){
        Util.printf("# No root node registered!\n");
        return;
    }

    // We are now connected
    Util.printf("We are connected to the network!\n");

    Util.printf("Connecting to CoAP server\n");

    if(CoAP.connectToServer4(serverIP4Addr, false) != UAPI_OK){
        Util.printf("FAILED to connect4 to server\n");
        return;
    }

    Timer.stop(startupTimerHandle);
    Timer.start(coapTimerHandle);

    return;
}

/**
 * @brief Empty response handler. We do not need it in this example
 */
void responseHandler(const uint8_t *payload, uint8_t payload_size)
{
    return;
}

/**
 * @brief This is the entry point of the user application. It is 
 *        called only once during startup. It is responsible for
 *        setting up callbacks, timers etc.
 */
RIIM_SETUP()
{
    Util.printf("Starting RIIM Router Node\n");

    startupTimerHandle=Timer.create(PERIODIC, 5000, startup);
    Timer.start(startupTimerHandle);

    CoAP.registerResponseHandler(responseHandler);
    coapTimerHandle=Timer.create(PERIODIC, coapTimerPeriod, sendCoAP);

    const uint8_t nwKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    Network.setNWKey((uint8_t*)nwKey);
    Network.startMeshRouter();
    Network.setChannel(18);

    Debug.printSetup();

    return UAPI_OK;
}

