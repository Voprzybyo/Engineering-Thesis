#include "RIIM_UAPI.h"
#define SGPC3_I2C_ADDRESS 0x58
#include <string.h>
// Change Resource_Name to your CoAP resource address
//static const uint8_t resourceName[] = "/api/v1/1IeLMuBZ6oI76pAmoskq/telemetry/";
static const uint8_t resourceName[] = "/api/v1/1IeLMuBZ6oI76pAmoskq/telemetry/";
static const uint8_t resourceName2[] = "data";
// Change server_ip4_addr to the address of your server.
const uint8_t serverIP4Addr[4] = { 104,196,24,70 };

uint8_t startupTimerHandler;
const uint32_t coapTimerPeriod = 30000;
static const uint32_t ledTimerPeriod=500; 
static const uint32_t sensorTimerPeriod=100; // 1/10th second

static const uint32_t timerPeriod = 20000;
static uint8_t coapTimerHandler, sensorTimerHandler, ledTimerHandler, printoutTimerHandler;
static const uint32_t printoutTimerPeriod=2000; // 2 seconds

static uint16_t temp, tempFraction, humidity;
static uint32_t light, userButton, vocValue;
static uint32_t lightState;

/**
 * @brief This function sends a CoAP message to the server (cloud)
 */

static void ReadSHT()
{
	static uint8_t wbuf[2];
	static uint8_t rbuf[10];
	// The command 0x2C0D starts a Medium repeatability measuring
	// with clock stretching enabled. See Sensirion SHT35 datasheet.
	wbuf[0] = 0x2C;
	wbuf[1] = 0x0D;
	I2C.transfer(0x44, wbuf, 2, rbuf, 7);
	// We convert the raw values to decidegrees (1/10) Celsius and percent humidity
// Conversion is done as fixed point, and as according to datasheet of the
// Sensirion SHT35
	uint32_t tTemp;
	tTemp = (int)((rbuf[0] << 8) | rbuf[1]);
	tTemp *= 1750;
	tTemp /= 65535;
	tTemp -= 450;
	temp = tTemp / 10;
	tempFraction = tTemp - (temp * 10);
	uint32_t tHum;
	tHum = (int)((rbuf[3] << 8) | rbuf[4]);
	tHum *= 100;
	tHum /= 65535;
	humidity = tHum;
	//Util.printf("SHT35 sensor read: Temp(Degrees C): %i.%i , Hum(Percent): %i\n", temp, tempFraction, humidity);
	return;
}

static void ReadLight()
{

    ADC.convertToMicroVolts(ADC0, &light);
    light /= 1000;
	//Util.printf("Light sensor read:  Light[mV]: %i\n", light);
    return;
}



static void ReadUserButton()
{
    userButton=GPIO.getValue(GPIO_8);
	//Util.printf("Button sensor read:  Button: %i\n", userButton);
    return;
}

static void ReadVOC()
{
    static uint8_t wbuf[2];
    static uint8_t rbuf[10];

	I2C.transfer(SGPC3_I2C_ADDRESS, wbuf, 0, rbuf, 3);	

    vocValue=(rbuf[1] << 8) | rbuf[0];

	//Start MeasureAirQuality
    wbuf[0] = 0x20; 
    wbuf[1] = 0x08;
	
    I2C.transfer(SGPC3_I2C_ADDRESS, wbuf, 2, rbuf, 0);

    return;
}


static void ReadSensor()
{
	static uint8_t sensorNumber;

	sensorNumber++;

	if (sensorNumber == 1) {
		ReadLight();
	}
	else if (sensorNumber == 2) {
		ReadUserButton();
	}
	else if (sensorNumber == 3) {
		ReadSHT();	
	}
	else if (sensorNumber == 4) {
		ReadVOC();	
	}
	else {
		sensorNumber = 0;
	}
	return;
}

static void LED()
{
   // GPIO.toggle(GPIO_0);
   // GPIO.toggle(GPIO_4);
}


static void sendCoAP()
{
	// We first check if we are part of a network and the address of the root node
	if (Network.getNetworkState() != ONLINE)
	{
		Util.printf("Not part of network yet\n");
		return;
	}
	uint8_t payload[100];
	int payloadLength;
	payloadLength = Util.sprintf((char*)payload, "{\"Temp\": %i.%i, \"Hum\": %i,\"Light\": %i,\"VOC\": %i, \"Button\": %i}", temp, tempFraction, humidity, light, vocValue, userButton);
	Util.printf("Wysylam do chmury...\n");
	CoAP.send(CoAP_POST, false, resourceName, payload, payloadLength);
	//CoAP.send(CoAP_GET, false, resourceName, payload, payloadLength);
	return;
}

void responseHandler(const uint8_t* payload, uint8_t payload_size)
{
	Util.printf("# Got CoAP Response. Doing nothing with it...\n");
	return;
}

/**
 * @brief This function checks that we are part of a network before continuing
 */
 
 static void Printout()
{
    Util.printf("\n\n");
   
    Util.printf("SHT35        : %i.%i (deg. C) , %i %% Humidity\n",temp, tempFraction, humidity);
   
    Util.printf("VOC          : %i\n",vocValue);
    Util.printf("Light (ADC0) : %i mV\n",light);
    Util.printf("User Button  : ");
        if(userButton==0) Util.printf("Pushed");
        else Util.printf("Not Pushed");
    return;
}
 
 
static void startup()
{
	// Wait until we are part of network
	if (Network.getNetworkState() != ONLINE) {
		Util.printf("Waiting for network...\n");
		return;
	}

	IPAddr rootNodeIPAddr;
	if (Network.getBorderRouterAddress(&rootNodeIPAddr) != UAPI_OK) {
		Util.printf("# No root node registered!\n");
		return;
	}

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


static void coapHandler(RequestType type, IPAddr srcAddr, uint8_t *payload, uint8_t payloadSize, uint8_t *response, uint8_t *responseSize)
{
    // In this example we only print the payload directly to the UART.
    Util.printf("%.*s\n", payloadSize, payload);
	Util.printf("%s\n", payload);
	Util.printf("%d\n", payloadSize);
	
	char * key = "0";
	
if(strcmp((const char *)payload,key) == 0) {
		 GPIO.toggle(GPIO_4);
		 GPIO.toggle(GPIO_0);
	}
	
	
	 lightState=GPIO.getValue(GPIO_0);
	 
	 if(lightState == 0){
	 *responseSize=Util.sprintf((char*)response, "Light: OFF");
	

	 Util.printf("1111 %.*s\n", responseSize, response);
	 
	 }else{
		 *responseSize=Util.sprintf((char*)response, "Light: ON");
		 Util.printf("2222 %.*s\n", responseSize, response);
	 }
	
	
	
	

    return;
}


/**
 * @brief This is the entry point of the user application. It is
 *        called only once during startup. It is responsible for
 *        setting up callbacks, timers etc.
 */
RIIM_SETUP()
{
	Util.printf("# Starting RIIM Sensor Board Node\n");
	// Initialize variables.
	// This MUST be done explicitly in RIIM_SETUP(), except if they are
	// declared const
	temp = 0;
	tempFraction = 0;
	humidity = 0;
	userButton = 0;
	vocValue = 0;
	light = 0;
	lightState = 0;
	
	UART.init(115200, UART_PARITY_NONE, UART_DATA_8_BITS, UART_STOP_1_BIT);
	
	// Set up interface towards the SHT35-sensor
    I2C.init(I2C_400KHZ);
		
	GPIO.setDirection(GPIO_3, OUTPUT);
    GPIO.setValue(GPIO_3, HIGH); 
	
    // Setup LEDs
    GPIO.setDirection(GPIO_0, OUTPUT);
    GPIO.setDirection(GPIO_4, OUTPUT);
    GPIO.setValue(GPIO_0, LOW);
   // GPIO.setValue(GPIO_4, HIGH);
   GPIO.setValue(GPIO_4, LOW);
	
    // Setup light sensor
    GPIO.setDirection(GPIO_5, OUTPUT);
    GPIO.setValue(GPIO_5, HIGH);
	
	// Setup User button input
    GPIO.setDirection(GPIO_8, INPUT);
    GPIO.setPull(GPIO_8, PULL_UP);
	
	// Setup VOC
	uint8_t wbuf[2];
    wbuf[0] = 0x20; 
    wbuf[1] = 0xAE;
		
    I2C.transfer(SGPC3_I2C_ADDRESS, wbuf, 2, NULL, 0);
	
	//
	ledTimerHandler=Timer.create(PERIODIC, ledTimerPeriod, LED);
	//printoutTimerHandler=Timer.create(PERIODIC, printoutTimerPeriod, Printout);
	startupTimerHandler = Timer.create(PERIODIC, 30000, startup);
	Timer.start(startupTimerHandler);
	sensorTimerHandler = Timer.create(PERIODIC, timerPeriod, ReadSensor);
	CoAP.registerResponseHandler(responseHandler);
	CoAP.registerResource(resourceName2,1,coapHandler);
	
	coapTimerHandler = Timer.create(PERIODIC, timerPeriod, sendCoAP);
	Timer.start(sensorTimerHandler);
	Timer.start(coapTimerHandler);
	//Timer.start(printoutTimerHandler);
	Timer.start(ledTimerHandler);
	
	 const uint8_t nwKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    Network.setNWKey((uint8_t*)nwKey);
    Network.setChannel(18);
	
	Network.startLeaf();
	Debug.printSetup();
	
	return UAPI_OK;
}
