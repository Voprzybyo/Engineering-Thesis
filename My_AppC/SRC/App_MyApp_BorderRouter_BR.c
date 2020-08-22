
#include "RIIM_UAPI.h"
#include <string.h>
//static const char resourceName2[]="data";


// Change Resource_Name to your CoAP resource address
//static const uint8_t resourceName[]="/api/v1/1IeLMuBZ6oI76pAmoskq/telemetry/";
static const uint8_t resourceName[]="/api/v1/1IeLMuBZ6oI76pAmoskq/rpc";
static const char resourceName2[]="/api/v1/1IeLMuBZ6oI76pAmoskq/rpc";
// Change server_ip4_addr to the address of your server.
const uint8_t serverIP4Addr[4]={104,196,24,70};
// Change IP mask and gateway to your setup
const uint8_t ipMask[4]={255,255,255,0};
const uint8_t ipGateway[4]={192,168,100,1};


static uint8_t timerHandle;
const uint16_t timerPeriod=5000;

static bool connectedToServer;


static void timerHandler()
{
    uint8_t addr[4];
    static Link nwLinks[50];
    static uint16_t numLinks;
    static uint16_t i,j;
    static uint8_t payload[100];
    static uint8_t payloadSize;
    static uint8_t numNodes;
    

    Network.getAddress4(addr);
    
    numLinks=Network.getNetworkLinks(0,50, nwLinks);
    numNodes=numLinks==0 ? 1 : numLinks;

    //////////////////////////////////////////////////////
    // Print out all links in the network
    //////////////////////////////////////////////////////
    for(i=0;i<numLinks;i++){
        Util.printf("[LINK] %i,", i);
        for(j=0;j<8;j+=2){
            Util.printf("%.02x%.02x", nwLinks[i].Node1_IID[j], nwLinks[i].Node1_IID[j+1]);
        }
        Util.printf(",");
        for(j=0;j<8;j+=2){
            Util.printf("%.02x%.02x", nwLinks[i].Node2_IID[j], nwLinks[i].Node2_IID[j+1]);
        }
        Util.printf("\n");
    }

    //////////////////////////////////////////////////////
    // Prepare and send CoAP message
    //////////////////////////////////////////////////////
    if(addr[0] != 0){ // Check if we have a valid IPv4-address
        if(connectedToServer == false){
            if(CoAP.connectToServer4(serverIP4Addr, false) != UAPI_OK){
                Util.printf("FAILED to connect4 to server\n");
                return;
            }
            connectedToServer=true;
            Debug.printSetup();
        }

        payloadSize=Util.snprintf((char*)payload, sizeof(payload), "{\"method\": \"getTemperature\", \"params\":{\"id\":%d}}", numNodes);
        Util.printf("Sending CoAP message : %s\n",payload);
        CoAP.send(CoAP_POST, false, resourceName, payload,  payloadSize); 
		 CoAP.send(CoAP_GET, false, resourceName, payload,  payloadSize); 
    } else {
        Util.printf("No IPv4 address yet....\n");
    }
}




/**
 * @brief CoAP response handler
 */
void responseHandler(const uint8_t *payload, uint8_t payload_size)
{
	 Util.printf("Otrzymano: %c\n", payload_size, payload);
	 Util.printf("Otrzymano: %.*s\n", payload_size, payload);
    return;
}

void responseHandler2(char *payload)
{
	 Util.printf("Otrzymano: %c\n", payload);
	 Util.printf("Otrzymano: %.*s\n", payload);
    return;
}



static void coapHandler(RequestType type, IPAddr srcAddr, uint8_t *payload, uint8_t payloadSize, uint8_t *response, uint8_t *responseSize)
{
    // In this example we only print the payload directly to the UART.
    Util.printf("%.*s\n", payloadSize, payload);

    return;
}


const uint8_t ipAddr[4]={0,0,0,0};
/**
 * @brief This is the entry point of the user application. It is 
 *        called only once during startup. It is responsible for
 *        setting up callbacks, timers etc.
 */

RIIM_SETUP()
{
    Util.printf("Starting RIIM Root Node\n");
    
    // Initialize variables
    connectedToServer=false;
    
    // Setup network and RF
    const uint8_t nwKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    Network.setNWKey((uint8_t*)nwKey);
    Network.startBorderRouter(NULL,ipAddr,ipMask,ipGateway);
    Network.setChannel(18);

    // Setup timer
	CoAP.registerResponseHandler(responseHandler);
	CoAP.registerResponseHandler(responseHandler2);
	CoAP.registerResource(resourceName2,0,coapHandler);
    timerHandle=Timer.create(PERIODIC, timerPeriod, timerHandler);
    Timer.start(timerHandle);
    
    Debug.printSetup();
    
    return UAPI_OK;
}

