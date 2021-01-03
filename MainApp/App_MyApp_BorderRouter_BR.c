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
#include <string.h>

static const uint8_t resourceName2[]="data";				// Resource name used to communicate with leaf node
static const uint8_t resourceName[]="/api/v1/1IeLMuBZ6oI76pAmoskq/telemetry/";	// Cloud resource
//static const uint8_t resourceName3[]="/api/v1/1IeLMuBZ6oI76pAmoskq/attributes/";	// Cloud resource



const uint8_t serverIP4Addr[4]={104,196,24,70};				// Cloud IPv4 adress
const uint8_t ipMask[4]={255,255,255,0};			
const uint8_t ipGateway[4]={192,168,100,1};

const IPAddr childNodeIPAddr={.byte={						// Address of leaf node
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0xe2, 0x9d}};

uint8_t startupTimerHandler;
const uint16_t timerPeriod=30000;
const uint16_t buttonTimerPeriod=200;
const uint32_t coapTimerPeriod = 2000;
static const uint32_t ledTimerPeriod=500; 
static const uint32_t buttonCheckTimerPeriod=4000; 

static uint8_t buttonCheckTimerHandler, sensorTimerHandler, oneShotTimerHandler, timerHandle; // ,ledTimerHandler;


static uint32_t userButton;
static uint32_t lightsOn;

static bool connectedToServer;


/**
* @brief Function using for debugging purposes. Toggle state of LED diode.
* @retval void
*/

static void LED()
{
	// GPIO.toggle(GPIO_0);
	// GPIO.toggle(GPIO_1);
	return;
}


/**
* @brief Read user button status. Assing corresponding value to userButton variable.
* @retval void
*/
static void ReadUserButton()
{
    userButton=GPIO.getValue(GPIO_2);	
    if(userButton==0) {
		Util.printf("Pushed");
		lightsOn = 1;
		Timer.start(oneShotTimerHandler);
	}else{ 
		lightsOn = 0;
	}
	
	return;
}


/**
* @brief Send CoAP message to leaf node with "light ON/OFF" request.
* @retval void
*/
static void sendCoAPtoSensorBoard()
{
	uint8_t payload[100];
    int payloadLength;
	
    CoAP.connectToServer6(childNodeIPAddr, false);

    payloadLength=Util.sprintf((char*)payload, "%i", lightsOn);
    CoAP.send(CoAP_PUT, false, resourceName2, payload, payloadLength);
    
    return;
}


/**
* @brief Main block of border router code. <br>
* Print avaiable connection between nodes in network to console. <br>
* Connect to cloud and send data about network in JSON format.
* @retval void
*/
static void timerHandler()
{
    uint8_t addr[4];
    static Link nwLinks[50];
    static uint16_t numLinks;
    //static uint16_t i,j;
    static uint8_t payload[100];
	static uint8_t address_v4[100];
    static uint8_t payloadSize;
    static uint8_t numNodes;
	static uint8_t address_v6[100];
    uint16_t shortAddr;

    //static uint16_t linkIndex;
	
	IPAddr adresV6;
	Network.getAddress(&adresV6);
	shortAddr=(adresV6.byte[14]<<8) | adresV6.byte[15];

	Util.snprintf((char*)address_v6, sizeof(address_v6), "%.04x", shortAddr);
	  
    numLinks=Network.getNetworkLinks(0,50, nwLinks);
    numNodes=numLinks==0 ? 1 : numLinks;
	
	// Get and format board IPv4 addr. It is sending to cloud to inform user that network status is OK. (if node's ipv4 addr is 0.0.0.0 => network is down)
	Network.getAddress4(addr);
	Util.snprintf((char*)address_v4, sizeof(address_v4), "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
	//Util.printf("%s",address_v4);
	
	/*
	// Print all links to UART
	Util.printf("\nLinks:\n");
    linkIndex=0;

    do{
        numLinks=Network.getNetworkLinks(linkIndex,10, nwLinks);
        linkIndex += numLinks;
        for(i=0;i<numLinks;i++){
            for(j=0;j<8;j++){
                Util.printf("%.02x", nwLinks[i].Node1_IID[j]);
            }
            Util.printf(" , ");
            for(j=0;j<8;j++){
                Util.printf("%.02x", nwLinks[i].Node2_IID[j]);
            }
            Util.printf("\n");
        }
    }while(numLinks > 0);
	*/

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
		
        payloadSize=Util.snprintf((char*)payload, sizeof(payload), "{\"NumNodesInNetwork\":%i, \"addr_V4_Border\":%s, \"IP_V6_Border\":%s}", numNodes, address_v4, address_v6);
        //Util.printf("Sending CoAP message : %s\n",payload);
        CoAP.send(CoAP_POST, false, resourceName, payload, payloadSize); 
		
    } else {
        Util.printf("No IPv4 address yet....\n");
    }
	
	return;
}


/**
* @brief Response handler used to check if border router got correct response for "Turn on the light" request send to leaf node.<br>
* If so, GPIO corresponding with LED diode is setting in HIGH state.<br>
* Similary, if border router got response for "Turn off the light", corresponding with LED diode GPIO is setting in LOW state.<br>
* @param[in] ipv6_payload: response from leaf node.
* @param[in] payload_size: size of response from leaf node.
* @retval void
*/
void responseHandler(const uint8_t *payload, uint8_t payload_size)
{
	
	char * key = "Light: OFF";
	char * key2 = "Light: ON";	
	if(strcmp((const char *)payload,key) == 0) {
		GPIO.setValue(GPIO_1, LOW);
		//GPIO.setValue(GPIO_0, LOW);
		Util.printf("Light: OFF\n");
	}else if (strcmp((const char *)payload,key2) == 0){
		GPIO.setValue(GPIO_1, HIGH);
		//GPIO.setValue(GPIO_0, HIGH);	
		Util.printf("Light: ON\n");
	}
	return;
}


//static void coapHandler(RequestType type, IPAddr srcAddr, uint8_t *payload, uint8_t payloadSize, uint8_t *response, uint8_t *responseSize)
//{
//   Util.printf(" :\n%.*s\n", payloadSize, payload);
//   return;
//}


const uint8_t ipAddr[4]={0,0,0,0};	// Set default values for IPv4 addr to avoid errors during network setup state. 


/**
* @brief Entry point of application. It is
* called only once during startup. <br> It is responsible for
* setting up callbacks, timers etc.
* @retval UAPI_OK
*/
RIIM_SETUP()
{
	
    Util.printf("Starting RIIM Root Node\n");
    
	
    // Initialize variables
    connectedToServer=false;
	userButton = 0;
	lightsOn = 0;
	
	
	// Setup User button input
    GPIO.setDirection(GPIO_2, INPUT);
    GPIO.setPull(GPIO_2, PULL_UP);
	
	
	// Setup LEDs
    //GPIO.setDirection(GPIO_0, OUTPUT);
    GPIO.setDirection(GPIO_1, OUTPUT);
    //GPIO.setValue(GPIO_0, LOW);
    GPIO.setValue(GPIO_1, LOW);
	
	
    // Setup network and RF
    const uint8_t nwKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    Network.setNWKey((uint8_t*)nwKey);
    Network.startBorderRouter(NULL,ipAddr,ipMask,ipGateway);
    Network.setChannel(18);

    // Setup timers
    timerHandle=Timer.create(PERIODIC, timerPeriod, timerHandler);
	sensorTimerHandler = Timer.create(PERIODIC, 100*MILLISECOND, ReadUserButton); // TO DO
	oneShotTimerHandler = Timer.create(ONE_SHOT, 1*SECOND, sendCoAPtoSensorBoard);
	//ledTimerHandler=Timer.create(PERIODIC, ledTimerPeriod, LED);
		
	CoAP.registerResponseHandler(responseHandler);					
			
	// Start timers
    Timer.start(timerHandle);
    Timer.start(sensorTimerHandler);
	//Timer.start(ledTimerHandler);
	
    Debug.printSetup();		//Print information about node to console
    
    return UAPI_OK;
}

