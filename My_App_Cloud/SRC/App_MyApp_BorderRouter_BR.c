
#include "RIIM_UAPI.h"
static const uint8_t resourceName2[]="data";
static const uint8_t resourceName[]="/api/v1/1IeLMuBZ6oI76pAmoskq/telemetry/";
const uint8_t serverIP4Addr[4]={104,196,24,70};
const uint8_t ipMask[4]={255,255,255,0};
const uint8_t ipGateway[4]={192,168,100,1};

const IPAddr childNodeIPAddr={.byte={
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0xe2, 0x9d}};


static uint8_t timerHandle;
const uint16_t timerPeriod=5000;

const uint32_t coapTimerPeriod = 2000;
static const uint32_t ledTimerPeriod=500; 
static const uint32_t printoutTimerPeriod=2000; // 2 seconds

static uint8_t ledTimerHandler, printoutTimerHandler, sensorTimerHandler, coapTimerHandler;


//static uint8_t buttonHandler;


static uint32_t userButton;
static uint32_t lightsOn;


static bool connectedToServer;


static void LED()
{
   GPIO.toggle(GPIO_0);
   GPIO.toggle(GPIO_1);
}

static void ReadUserButton()
{
    userButton=GPIO.getValue(GPIO_2);
	
		if(userButton == 0){
			
		}
	
	//Util.printf("Button sensor read:  Button: %i\n", userButton);
    return;
}


static void ReadSensor()
{
		ReadUserButton();
	return;
}



static void Printout()
{
    Util.printf("\n\n");
   

    Util.printf("User Button  : ");
        if(userButton==0) {
			Util.printf("Pushed");
			lightsOn = 1;
		}else{ 
			Util.printf("Not Pushed");
			lightsOn = 0;
		}
    return;
}


static void sendCoAPtoSensorBoard()
{
   
    CoAP.connectToServer6(childNodeIPAddr, false);

    // Send sensor data to root node. The URI is "coap://[root-node-ip]/data"
    // For this demo, we'll use JSON formatting. JSON is not size efficient, 
    // but easy to read and parse
    uint8_t payload[100];
    int payloadLength;
    Util.printf("Wysylam do Sensorboarda:{\"mV\": %i}\n", lightsOn);
    payloadLength=Util.sprintf((char*)payload,"{\"mV\": %i}", lightsOn);
    CoAP.send(CoAP_PUT, false, resourceName2, payload, payloadLength);
    
    return;
}

void buttonHandler()
{
	Util.printf("# Button Handler! \n");
	if(lightsOn == 1){
		sendCoAPtoSensorBoard();
	}
	return;
}



static void timerHandler()
{
	uint8_t addr[4];
	Network.getAddress4(addr);
	
		if(addr[0] != 0){ // Check if we have a valid IPv4-address
			if(connectedToServer == false){
				if(CoAP.connectToServer4(serverIP4Addr, false) != UAPI_OK){
					Util.printf("FAILED to connect4 to server\n");
				return;
				}
			connectedToServer=true;
			Debug.printSetup();
			}
		} else {
			Util.printf("No IPv4 address yet....\n");
		}
}


void responseHandler(const uint8_t *payload, uint8_t payload_size)
{
	Util.printf("# Got CoAP Response. Doing nothing with it...\n");
	return;
}

const uint8_t ipAddr[4]={0,0,0,0};


RIIM_SETUP()
{
	Util.printf("Starting RIIM Root Node\n");
	// Initialize variables
	
	userButton = 0;
	lightsOn = 0;
	connectedToServer=false;
	    // Setup network and RF
    const uint8_t nwKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    Network.setNWKey((uint8_t*)nwKey);
    Network.startBorderRouter(NULL,ipAddr,ipMask,ipGateway);
    Network.setChannel(18);
	
	
	// Setup User button input
    GPIO.setDirection(GPIO_2, INPUT);
    GPIO.setPull(GPIO_2, PULL_UP);
	
	
	
	// Setup LEDs
    GPIO.setDirection(GPIO_0, OUTPUT);
    GPIO.setDirection(GPIO_1, OUTPUT);
    GPIO.setValue(GPIO_0, LOW);
    GPIO.setValue(GPIO_1, HIGH);
	
	
	
	// Setup timer
	sensorTimerHandler = Timer.create(PERIODIC, timerPeriod, ReadSensor);
	ledTimerHandler=Timer.create(PERIODIC, ledTimerPeriod, LED);
	timerHandle=Timer.create(PERIODIC, timerPeriod, timerHandler);
	printoutTimerHandler=Timer.create(PERIODIC, printoutTimerPeriod, Printout);
	//coapTimerHandler = Timer.create(PERIODIC, timerPeriod, sendCoAPtoSensorBoard);
	CoAP.registerResponseHandler(responseHandler);
	
	coapTimerHandler = Timer.create(PERIODIC, timerPeriod, buttonHandler);
	
	
	Timer.start(ledTimerHandler);
	Timer.start(timerHandle);
	Timer.start(printoutTimerHandler);
	Timer.start(sensorTimerHandler);
	Timer.start(coapTimerHandler);
	
	
	
	
	Debug.printSetup();
	return UAPI_OK;
}

