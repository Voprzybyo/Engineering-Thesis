
#include "RIIM_UAPI.h"

///////////////////////////////////////////////////
// Cloud server setup variables : Resource_Name and server_ip4_addr
///////////////////////////////////////////////////

// Change Resource_Name to your CoAP resource address
static const uint8_t resourceName[] = "/api/v1/3UhfDTKzS9tn6meK7Wpm/telemetry/";
// Change server_ip4_addr to the address of your server.
const uint8_t serverIP4Addr[4] = { 104,196,24,70 };
// Change IP mask and gateway to your setup
const uint8_t ipMask[4] = { 255,255,255,0 };
const uint8_t ipGateway[4] = { 192,168,100,1 };



static uint8_t timerHandle;
const uint16_t timerPeriod = 5000;

static bool connectedToServer;

/**
* @brief Periodic function printing out status and sending periodic CoAP messages
*/
static void timerHandler()
{
	uint8_t addr[4];
	Network.getAddress4(addr);
	if (addr[0] != 0) { // Check if we have a valid IPv4-address
		if (connectedToServer == false) {
			if (CoAP.connectToServer4(serverIP4Addr, false) != UAPI_OK) {
				Util.printf("FAILED to connect4 to server\n");
				return;
			}
			connectedToServer = true;
			Debug.printSetup();
		}
	}
	else {
		Util.printf("No IPv4 address yet....\n");
	}
}
/**
* @brief CoAP response handler
*/


void responseHandler(const uint8_t* payload, uint8_t payload_size)
{
	Util.printf("# Got CoAP Response. Doing nothing with it...\n");
	return;
}


const uint8_t ipAddr[4] = { 0,0,0,0 };
/**
 * @brief This is the entry point of the user application. It is
 *        called only once during startup. It is responsible for
 *        setting up callbacks, timers etc.
 */
RIIM_SETUP()
{
	Util.printf("Starting RIIM Root Node\n");
	// Initialize variables
	connectedToServer = false;
	// Setup network and RF
	Network.startBorderRouter(NULL, ipAddr, ipMask, ipGateway);
	// Setup timer
	timerHandle = Timer.create(PERIODIC, timerPeriod, timerHandler);
	Timer.start(timerHandle);
	Debug.printSetup();
	return UAPI_OK;
}

