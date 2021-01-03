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
#define SGPC3_I2C_ADDRESS 0x58
#include <string.h>

//static const uint8_t resourceName[] = "/api/v1/1IeLMuBZ6oI76pAmoskq/telemetry/";
static const uint8_t resourceName[] = "/api/v1/1IeLMuBZ6oI76pAmoskq/telemetry/";
static const char resourceName2[] = "data";

const uint8_t serverIP4Addr[4] = { 104,196,24,70 };

uint8_t startupTimerHandler;
const uint32_t coapTimerPeriod = 30000;
static const uint32_t ledTimerPeriod=500; 
static const uint32_t sensorTimerPeriod=100; 

static const uint32_t timerPeriod = 20000;
static uint8_t coapTimerHandler, sensorTimerHandler, ledTimerHandler; // , printoutTimerHandler;
static const uint32_t printoutTimerPeriod=2000; 

static uint16_t temp, tempFraction, humidity;
static uint32_t light, userButton, vocValue;
static uint32_t lightState;


/**
 * @brief Handler called periodically by a timer. It uses the
 *        I2C-bus to read temperature and humidity values from
 *        the Sensirion SHT35 sensor
 * @retval void
 */
static void ReadSHT()
{
	static uint8_t wbuf[2];
	static uint8_t rbuf[10];

	wbuf[0] = 0x2C;
	wbuf[1] = 0x0D;
	I2C.transfer(0x44, wbuf, 2, rbuf, 7);

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

	return;
}


/**
 * @brief Handler called periodically to read the Light Sensor/ADC. 
 * @retval void
 */
static void ReadLight()
{
    ADC.convertToMicroVolts(ADC0, &light);
    light /= 1000;
	//Util.printf("Light sensor read:  Light[mV]: %i\n", light);
    return;
}


/**
 * @brief Handler called periodically to read the pushbutton.
 * @retval void
 */
static void ReadUserButton()
{
    userButton=GPIO.getValue(GPIO_8);
	//Util.printf("Button sensor read:  Button: %i\n", userButton);
    return;
}


/**
 * @brief Handler called periodically to read the VOC sensor.
 * @retval void
 */
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


/**
 * @brief Handler called periodically by a timer to read
 * various sensors on the sensor board
 * @retval void
 */
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


/**
* @brief Function using for debugging purposes. Toggle state of LED diode.
* @retval void
*/
static void LED()
{
   // GPIO.toggle(GPIO_0);
   // GPIO.toggle(GPIO_4);
}


/**
 * @brief Timer callback function that sends a CoAP POST message to the cloud. <br>
 *  Message format is JSON.
 * @retval void
 */
static void sendCoAP()
{
	// We first check if we are part of a network and the address of the root node
	if (Network.getNetworkState() != ONLINE)
	{
		Util.printf("Not part of network yet\n");
		return;
	}
	uint8_t payload[150];
	int payloadLength;
	int RSSI = 0;
	
	static uint8_t address_v6[50];
    uint16_t shortAddr;

	IPAddr adres;
	Network.getAddress(&adres);
	shortAddr=(adres.byte[14]<<8) | adres.byte[15];

	Util.snprintf((char*)address_v6, sizeof(address_v6), "%.04x", shortAddr);
	
	RSSI = Network.getParentRSSI();
	Util.printf("RSSI: %i\n",RSSI);
	
	payloadLength = Util.sprintf((char*)payload, "{\"Temp\": %i.%i, \"Hum\": %i,\"Light\": %i,\"VOC\": %i, \"Button\": %i, \"LightState\": %i,\"RSSI_Leaf\": %d, \"IP_V6_Leaf\": %s}",
	temp, tempFraction, humidity, light,vocValue, userButton, lightState, RSSI, address_v6);

	CoAP.send(CoAP_POST, false, resourceName, payload, payloadLength);
		
	//payloadLength = Util.sprintf((char*)payload, "{\"IP_V6_Leaf\": %s}", address_v6);

	//CoAP.send(CoAP_POST, false, resourceName, payload, payloadLength);
	
	return;
}


/**
 * @brief Handler called when CoAP receives a response. 
 * @param payload[in]: Array of bytes containing the payload
 * @param payload_size[in]: The number of bytes in the payload
 * @retval void
 */
void responseHandler(const uint8_t *payload, uint8_t payload_size)
{
	return;
}

	//Util.printf("Otrzymalem: %s\n", payload);
	
	return;
}


/**
 * @brief Print out the values of all the sensors. <br>
 * Function used for debugging purposes.
 * @retval void
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

 
 /**
* @brief Startup function used for connect to network and cloud only once at the beginning.
* @retval void
*/
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


 /**
* @brief CoapHandler to capture request from other nodes in network. In this case handler is used to turn on/off light by leaf node
* @retval void
*/
static void coapHandler(RequestType type, IPAddr srcAddr, uint8_t *payload, uint8_t payloadSize, uint8_t *response, uint8_t *responseSize)
{
	char * key = "0";
	
if(strcmp((const char *)payload,key) == 0) {
		 GPIO.toggle(GPIO_4);
		 GPIO.toggle(GPIO_0);
	}
		
	 lightState=GPIO.getValue(GPIO_0);
	 
	 if(lightState == 0){
	 *responseSize=Util.sprintf((char*)response, "Light: OFF");
	 
	 }else{
	 *responseSize=Util.sprintf((char*)response, "Light: ON");

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
