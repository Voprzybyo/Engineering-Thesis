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
 @brief Documentation for border router functions implemented for wireless sensor mesh network
 @version 2.00
 @date 16.10.2020
 @author Wojciech Przybyło
*/

#include "RIIM_UAPI.h"


static const uint8_t resourceName[]="/api/v1/1IeLMuBZ6oI76pAmoskq/telemetry/"; //if I want to send CoAP from mesh_node to cloud
// Change server_ip4_addr to the address of your server.
const uint8_t serverIP4Addr[4]={104,196,24,70};

uint8_t startupTimerHandle;
const uint32_t RSSITimerPeriod=10000;
uint8_t RSSITimerHandle;


/**
* @brief Startup function used for connect to network and cloud only once at the beginning.
* @retval void
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
    Timer.start(RSSITimerHandle);

    return;
}


/**
 * @brief Empty response handler.
 * @retval void
 */
void responseHandler(const uint8_t *payload, uint8_t payload_size)
{
	
    return;
}


/**
* @brief Startup function used for connect to network and cloud only once at the beginning.
* @retval void
*/
void sendRSSI()
{
	int RSSI = 0;
	uint8_t payload[100];
	int payloadLength;
	static uint8_t address_v6[100];
    uint16_t shortAddr;

	IPAddr adres;
	Network.getAddress(&adres);
	shortAddr=(adres.byte[14]<<8) | adres.byte[15];

    //Util.printf("fe80:0000:0000:0000:0000:00ff:fe00:%.04x \n", shortAddr);
	Util.snprintf((char*)address_v6, sizeof(address_v6), "%.04x", shortAddr);

	
	RSSI = Network.getParentRSSI();
	//Util.printf("RSSI = %d \n", RSSI);
	//Util.printf("AdresV6= %s \n", address_v6);
	payloadLength = Util.sprintf((char*)payload, "{\"RSSI_MeshR\":%i, \"IP_V6_MeshR\":%s}", RSSI, address_v6);
	CoAP.send(CoAP_POST, false, resourceName, payload, payloadLength);
	
    return;
}


/**
* @brief Entry point of application. It is
* called only once during startup. <br> It is responsible for
* setting up callbacks, timers etc.
* @retval UAPI_OK
*/
RIIM_SETUP()
{
    Util.printf("Starting RIIM Router Node\n");

    startupTimerHandle=Timer.create(PERIODIC, 5000, startup);
    Timer.start(startupTimerHandle);

    CoAP.registerResponseHandler(responseHandler);

	
    RSSITimerHandle=Timer.create(PERIODIC, RSSITimerPeriod, sendRSSI);

    const uint8_t nwKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    Network.setNWKey((uint8_t*)nwKey);
    Network.startMeshRouter();
    Network.setChannel(18);

    Debug.printSetup();

    return UAPI_OK;
}

