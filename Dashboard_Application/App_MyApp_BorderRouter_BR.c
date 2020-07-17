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
 @brief This is a demonstration of large network test for RIIM
 @version 1.00 : First revision
 @date 2019.02.25
*/

#include "RIIM_UAPI.h"

static const char resourceName[] = "data";
static uint8_t printoutTimerHandle;
const uint16_t timerPeriod = 5000;
const uint32_t ethTimerPeriod = 5000;
static uint8_t ethTimerHandle; 
static const uint32_t natKeepaliveTimerPeriod = 1000 * 60 * 4; // 4 minutes
static uint8_t natKeepaliveTimerHandle; 
const uint8_t keepaliveAddr[4]={192,168,100,3}; // Set this to the address of the PC running the RIIM Dashboard. 
static uint8_t myAddr[4];

const uint8_t IP_Addr[4] = { 0,0,0,0 };
const uint8_t IP_Mask[4] = { 255,255,255,0 }; 
const uint8_t IP_GW[4] = { 192,168,100,1 };

/**
 * @brief Function to print all links between nodes onto the UART
 */
static void printAllLinks()
{
	static Link nwLinks[10];
	static int16_t numLinks;
	static uint16_t linkIndex;
	static uint16_t i, j;

	Util.printf("\nLinks:\n");

	linkIndex = 0;

	do {
		numLinks = Network.getNetworkLinks(linkIndex, 10, nwLinks);
		linkIndex += numLinks;
		for (i = 0; i < numLinks; i++) {
			for (j = 0; j < 8; j++) {
				Util.printf("%.02x", nwLinks[i].Node1_IID[j]);
			}
			Util.printf(" , ");
			for (j = 0; j < 8; j++) {
				Util.printf("%.02x", nwLinks[i].Node2_IID[j]);
			}
			Util.printf("\n");
		}
	} while (numLinks > 0);

	Util.printf("Total number of links: %i (+1 in root)\n", linkIndex);

	return;
}

/**
 * @brief CoAP receive handler. All CoAP messages coming to this node are processed here
 */
static void coapHandler(RequestType type, IPAddr srcAddr, uint8_t* payload, uint8_t payloadSize, uint8_t* response, uint8_t* responseSize)
{
	// In this example we only print the payload directly to the UART.
	uint16_t shortAddr;
	uint32_t pLoad;
	shortAddr = (srcAddr.byte[14] << 8) | srcAddr.byte[15];

	pLoad = (payload[0] << 24) | (payload[1] << 16) | (payload[2] << 8) | payload[3];

	Util.printf("[CoAP] from %.04x : %.08lx\n", shortAddr, pLoad);

	return;
}

/**
 * @brief CoAP response handler
 */
void ResponseHandler(const uint8_t* payload, uint8_t payload_size)
{
	// We'll do nothing with any responses
	return;
}


/**
 * @brief This is the entry point of the user application. It is
 *        called only once during startup. It is responsible for
 *        setting up callbacks, timers etc.
 */

void natKeepAlive() 
{
	CoAP.disconnectFromServer();
	CoAP.connectToServer4(keepaliveAddr, false);
	CoAP.send(CoAP_GET, false, "none", "none", 2);
} 

void ethConnect() 
{ 
	Network.getAddress4(myAddr); 
	if (myAddr[0] > 0) 
	{ 
		Util.printf("I have IP address: %i.%i.%i.%i\n", myAddr[0], myAddr[1], myAddr[2], myAddr[3]);
		Timer.stop(ethTimerHandle); 
		natKeepAlive(); 
		Timer.start(natKeepaliveTimerHandle); 
	} 
}


RIIM_SETUP()
{
	Util.printf("Starting RIIM Root Node\n");


	// Start as a router, leaf or border router.
	const uint8_t nwKey[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	Network.setNWKey((uint8_t*)nwKey);
	Network.startBorderRouter(NULL, IP_Addr, IP_Mask, IP_GW);
	Network.setChannel(18);

	CoAP.registerResource(resourceName, 1, coapHandler);

	printoutTimerHandle = Timer.create(PERIODIC, timerPeriod, printAllLinks);
	Timer.start(printoutTimerHandle);

	// Set initial IP address to zero
	myAddr[0]=myAddr[1]=myAddr[2]=myAddr[3]=0; 
	// Setup timer for checking ethernet 
	ethTimerHandle=Timer.create(PERIODIC, ethTimerPeriod, ethConnect); Timer.start(ethTimerHandle);
	// Setup timer for periodic NAT keepalive
	natKeepaliveTimerHandle=Timer.create(PERIODIC, natKeepaliveTimerPeriod, natKeepAlive);

	Debug.printSetup();

	return UAPI_OK;
}

