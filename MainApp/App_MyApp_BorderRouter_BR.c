
#include "RIIM_UAPI.h"
#include<string.h>
static const uint8_t resourceName2[]="data";
static const uint8_t resourceName[]="/api/v1/1IeLMuBZ6oI76pAmoskq/telemetry/";
//static const uint8_t resourceName[]="/api/v1/1IeLMuBZ6oI76pAmoskq/attributes/";

const uint8_t serverIP4Addr[4]={104,196,24,70};
const uint8_t ipMask[4]={255,255,255,0};
const uint8_t ipGateway[4]={192,168,100,1};

const IPAddr childNodeIPAddr={.byte={
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0xe2, 0x9d}};

uint8_t startupTimerHandler;
static uint8_t timerHandle;
const uint16_t timerPeriod=5000;
const uint16_t buttonTimerPeriod=200;
const uint32_t coapTimerPeriod = 2000;
static const uint32_t ledTimerPeriod=500; 
static const uint32_t buttonCheckTimerPeriod=4000; 

static uint8_t ledTimerHandler, buttonCheckTimerHandler, sensorTimerHandler, coapTimerHandler, buttonTimerHandler, oneShotTimerHandler;


 enum {off, on} LightButton; //to do

//static uint8_t buttonHandler;


static uint32_t userButton;
static uint32_t lightsOn;


static bool connectedToServer;


static void LED()
{
  // GPIO.toggle(GPIO_0);
  // GPIO.toggle(GPIO_1);
}

static void ReadUserButton()
{
    userButton=GPIO.getValue(GPIO_2);
	
	//Util.printf("Button sensor read:  Button: %i\n", userButton);
    return;
}


static void ReadSensor()
{
		ReadUserButton();
	return;
}



static void ButtonCheck()
{
   // Util.printf("\n\n");
  //  Util.printf("User Button  : ");
        if(userButton==0) {
			Util.printf("Pushed");
			lightsOn = 1;
			Timer.start(oneShotTimerHandler);
		}else{ 
		//	Util.printf("Not Pushed");
			lightsOn = 0;
		}
    return;
}


static void sendCoAPtoSensorBoard()
{
   
    CoAP.connectToServer6(childNodeIPAddr, false);

    uint8_t payload[100];
    int payloadLength;
    payloadLength=Util.sprintf((char*)payload,"%i", lightsOn);
    CoAP.send(CoAP_PUT, false, resourceName2, payload, payloadLength);
    
    return;
}

// TO DO
static void sendCoAP()
{

	uint8_t payload[100];
	int payloadLength;
	payloadLength = Util.sprintf((char*)payload, "{LLL:1}");
	Util.printf("Wysylam do chmury...\n");
	//CoAP.send(CoAP_POST, false, resourceName, payload, payloadLength);
	return;
}


static void startup()
{

	// We are now connected
	Util.printf("We are connected to the network!\n");

	Util.printf("Connecting to CoAP server\n");

	if (CoAP.connectToServer4(serverIP4Addr, false) != UAPI_OK) {
		Util.printf("FAILED to connect4 to server\n");
		return;
	}

	Timer.stop(startupTimerHandler);
	Timer.start(coapTimerHandler);

	return;
}



void buttonHandler()
{
	Util.printf("# Button Handler! \n");

		sendCoAPtoSensorBoard();
	
	return;
}



static void timerHandler()
{
	uint8_t addr[4];
	Network.getAddress4(addr);
	
		if(addr[0] != 0){
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
	
	Util.printf("%s\n", payload);
	
	char * key = "Light: OFF";
		
	if(strcmp((const char *)payload,key) == 0) {
		GPIO.setValue(GPIO_1, LOW);
		//GPIO.setValue(GPIO_0, LOW);
	}else{
		GPIO.setValue(GPIO_1, HIGH);
		//GPIO.setValue(GPIO_0, HIGH);	
	}
	
	return;
}

const uint8_t ipAddr[4]={0,0,0,0};

static void coapHandler(RequestType type, IPAddr srcAddr, uint8_t *payload, uint8_t payloadSize, uint8_t *response, uint8_t *responseSize)
{
    Util.printf(" :\n%.*s\n", payloadSize, payload);
    return;
}



RIIM_SETUP()
{
	Util.printf("Starting RIIM Root Node\n");
	// Initialize variables
	LightButton = off;
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
    GPIO.setValue(GPIO_1, LOW);
	
	
	startupTimerHandler = Timer.create(PERIODIC, 30000, startup);
	// Setup timer
	sensorTimerHandler = Timer.create(PERIODIC, 100*MILLISECOND, ReadSensor); // TO DO
	ledTimerHandler=Timer.create(PERIODIC, ledTimerPeriod, LED);
	timerHandle=Timer.create(PERIODIC, timerPeriod, timerHandler);
	buttonCheckTimerHandler=Timer.create(PERIODIC, 100*MILLISECOND, ButtonCheck);
	//coapTimerHandler = Timer.create(PERIODIC, timerPeriod, sendCoAPtoSensorBoard);
	CoAP.registerResponseHandler(responseHandler);
	
	coapTimerHandler = Timer.create(PERIODIC, buttonTimerPeriod, buttonHandler);
	oneShotTimerHandler = Timer.create(ONE_SHOT, 1*SECOND, buttonHandler);
	
	coapTimerHandler = Timer.create(PERIODIC, timerPeriod, sendCoAP);
	Timer.start(startupTimerHandler);
	Timer.start(coapTimerHandler);
	Timer.start(ledTimerHandler);
	Timer.start(timerHandle);
	Timer.start(buttonCheckTimerHandler);
	Timer.start(sensorTimerHandler);
	Timer.start(oneShotTimerHandler);
	//Timer.start(coapTimerHandler);
		
	Debug.printSetup();
	return UAPI_OK;
}

