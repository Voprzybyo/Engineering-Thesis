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
 @brief This ICI application demonstrates all sensors on the Sensor Board (SB)
		It also demonstrates CoAP transmission of data to a root node
 @version 1.00 : First revision
 @date 2019.06.26
*/

#include "RIIM_UAPI.h"

#define SGPC3_I2C_ADDRESS 0x58

static const uint32_t sensorTimerPeriod = 100; // 1/10th second
static const uint32_t printoutTimerPeriod = 2000; // 2 seconds
static const uint32_t coapTimerPeriod = 10000; // 10 seconds
static const uint32_t ledTimerPeriod = 500; // 0.5 seconds

static const uint8_t resourceName[] = "data";
static uint8_t coapTimerHandler, sensorTimerHandler, printoutTimerHandler, ledTimerHandler;
static int32_t accX, accY, accZ;
static uint16_t shtTemp, shtTempFraction, shtHumidity;
static uint16_t hdcTemp, hdcTempFraction, hdcHumidity;
static uint32_t milliVolts;
static uint32_t light;
static uint32_t userButton;
static int32_t hall;
static uint32_t vocValue;

/**
 * @brief Function for reading the accelerometer
 */
static void ReadAccelerometer()
{
	static uint8_t wbuf[6];
	static uint8_t rbuf[6];

	wbuf[0] = 0xE9; // Write bit 1 (read), MS=1 (increment address), 
	GPIO.setValue(GPIO_3, LOW);
	SPI.transfer(6, wbuf, rbuf);
	GPIO.setValue(GPIO_3, HIGH);

	// Convert to mg (milli-g)
	// A value of 64 is 1 g
	accX = ((int8_t)rbuf[1] * 1000) / 64;
	accY = ((int8_t)rbuf[3] * 1000) / 64;
	accZ = ((int8_t)rbuf[5] * 1000) / 64;

	return;
}

/**
 * @brief Handler called periodically by a timer. It uses the
 *        I2C-bus to read temperature and humidity values from
 *        the Sensirion SHT35 sensor
 */
static void ReadSHT()
{
	static uint8_t wbuf[2];
	static uint8_t rbuf[10];

	// The command 0x2C0D starts a Medium repeatibility measuring 
	// with clock stretching enabled. See Sensirion SHT35 datasheet.
	wbuf[0] = 0x2C;
	wbuf[1] = 0x0D;
	I2C.transfer(0x44, wbuf, 2, rbuf, 7);

	// We convert the raw values to decidegrees (1/10) Celsius and percent humidity
	// Conversion is done as fixed point, and as according to datasheet of the
	// Sensirion SHT35
	float tTemp;
	tTemp = (int)((rbuf[0] << 8) | rbuf[1]);
	tTemp /= 65535;
	tTemp *= 175;
	tTemp -= 45;
	shtTemp = (int)tTemp;
	shtTempFraction = (int)(tTemp - shtTemp) * 10;

	uint32_t tHum;
	tHum = (int)((rbuf[3] << 8) | rbuf[4]);
	tHum *= 100;
	tHum /= 65535;
	shtHumidity = tHum;

	return;
}

/**
 * @brief Handler called periodically to read the Light Sensor/ADC.
 */
static void ReadLight()
{

	ADC.convertToMicroVolts(ADC0, &light);
	light /= 1000;

	return;
}

/**
 * @brief Handler called periodically to read the ADC.
 */
static void ReadADC()
{

	ADC.convertToMicroVolts(ADC1, &milliVolts);
	milliVolts /= 1000;

	return;
}

/**
 * @brief Handler called periodically to read the hall effect sensor.
 */
static void ReadHall()
{
	hall = GPIO.getValue(GPIO_2);
	return;
}

/**
 * @brief Handler called periodically to read the pushbutton.
 */
static void ReadUserButton()
{
	userButton = GPIO.getValue(GPIO_8);
	return;
}

/**
 * @brief Handler called periodically to read the HDC2010.
 */
static void ReadHDC()
{
	static uint8_t wbuf[2];
	static uint8_t rbuf[10];

	wbuf[0] = 0x00;
	I2C.transfer(0x40, wbuf, 1, rbuf, 4);

	// Start a new conversion
	wbuf[0] = 0x0F;
	wbuf[1] = 0x01;
	I2C.transfer(0x40, wbuf, 2, NULL, 0);

	// We convert the raw values to decidegrees (1/10) Celsius and percent humidity
	float tTemp;
	tTemp = (int)((rbuf[1] << 8) | rbuf[0]);
	tTemp /= 65535;
	tTemp *= 165;
	tTemp -= 40;
	hdcTemp = (int)tTemp;
	hdcTempFraction = (tTemp - hdcTemp) * 10;

	float tHum;
	tHum = (int)((rbuf[3] << 8) | rbuf[2]);
	tHum /= 65536;
	tHum *= 100;
	hdcHumidity = tHum;
	return;
}

/**
 * @brief Handler called periodically to read the VOC sensor.
 */
static void ReadVOC()
{
	static uint8_t wbuf[2];
	static uint8_t rbuf[10];

	I2C.transfer(SGPC3_I2C_ADDRESS, wbuf, 0,
		rbuf, 3);

	vocValue = (rbuf[1] << 8) | rbuf[0];

	//Start MeasureAirQuality
	wbuf[0] = 0x20;
	wbuf[1] = 0x08;

	I2C.transfer(SGPC3_I2C_ADDRESS, wbuf, 2, rbuf, 0);

	return;
}

/**
 * @brief Handler called periodically by a timer to read
 * various sensors on the sensor board
 */
static void ReadSensor()
{
	static uint8_t sensorNumber;

	sensorNumber++;

	if (sensorNumber == 1) {
		ReadAccelerometer();
	}
	else if (sensorNumber == 2) {
		ReadSHT();
	}
	else if (sensorNumber == 3) {
		ReadADC();
	}
	else if (sensorNumber == 4) {
		ReadLight();
	}
	else if (sensorNumber == 5) {
		ReadHall();
	}
	else if (sensorNumber == 6) {
		ReadUserButton();
	}
	else if (sensorNumber == 7) {
		ReadHDC();
	}
	else if (sensorNumber == 8) {
		ReadVOC();
	}
	else {
		sensorNumber = 0;
	}

	return;
}

/**
 * @brief Print out the values of all the sensors
 */
static void Printout()
{
	Util.printf("\n\n");
	Util.printf("LIS3DE       : X: %i mg, Y: %i mg, Z: %i mg\n", accX, accY, accZ);
	Util.printf("SHT35        : %i.%i (deg. C) , %i %% Humidity\n", shtTemp, shtTempFraction, shtHumidity);
	Util.printf("HDC2010      : %i.%i (deg. C) , %i %% Humidity\n", hdcTemp, hdcTempFraction, hdcHumidity);
	Util.printf("VOC          : %i\n", vocValue);
	Util.printf("ADC1         : %i mV\n", milliVolts);
	Util.printf("Light (ADC0) : %i mV\n", light);
	Util.printf("Hall         : ");
	if (hall == 0) Util.printf("ON\n");
	else Util.printf("OFF\n");
	Util.printf("User Button  : ");
	if (userButton == 0) Util.printf("Pushed");
	else Util.printf("Not Pushed");
	return;
}


/**
 * @brief Toggle the LEDs
 */
static void LED()
{
	GPIO.toggle(GPIO_0);
	GPIO.toggle(GPIO_4);
}

/**
 * @brief Timer callback function that sends a CoAP PUT message to
 *        the root node. The message contains the latest temperature
 *        and humidity values in a human readable JSON format.
 */
static void SendCoAP()
{
	IPAddr rootNodeIPAddr;

	// We first check if we are part of a network and the address of the root node
	if (Network.getBorderRouterAddress(&rootNodeIPAddr) != UAPI_OK) {
		Util.printf("# No root node registered! Not possible to send CoAP message yet\n");
		return;
	}

	CoAP.connectToServer6(rootNodeIPAddr, false);

	// For this demo, we'll use JSON formatting. JSON is not size efficient, 
	// but easy to read and parse
	uint8_t payload[100];
	int payloadLength;

	payloadLength = Util.sprintf((char*)payload, "{\"X\": %i, \"Y\": %i, \"Z\": %i}", accX, accY, accZ);
	CoAP.send(CoAP_PUT, false, resourceName, payload, payloadLength);

	return;
}


/**
 * @brief Handler called when CoAP receives a response. Only used to
 *        indicate that this has actually happened in this demo
 *
 * @param payload[in]: Array of bytes containing the payload
 * @param payload_size[in]: The number of bytes in the payload
 */
void ResponseHandler(const uint8_t* payload, uint8_t payload_size)
{
	Util.printf("# Got CoAP Response. Doing nothing with it...\n");
	return;
}


/**
 * @brief This is the entry point of the user application. It is
 *        called only once during startup. It is responsible for
 *        setting up callbacks, timers etc.
 */
RIIM_SETUP()
{
	UART.init(115200, UART_PARITY_NONE, UART_DATA_8_BITS, UART_STOP_1_BIT);
	Util.printf("# Starting RIIM Sensor Board Node - Sensorboard DEMO\n");

	// Initialize variables. 
	// This MUST be done explicitly in RIIM_SETUP(), except if they are 
	// declared const

	// Set up GPIO. We must use one GPIO for SPI chip select
	GPIO.setDirection(GPIO_3, OUTPUT);
	GPIO.setValue(GPIO_3, HIGH);

	// Set up interface towards the LIS3DE-sensor
	SPI.init(4000000, SPI_POL_0_PHA_0);

	// Set up interface towards the SHT35-sensor
	I2C.init(I2C_400KHZ);

	// Setup LEDs
	GPIO.setDirection(GPIO_0, OUTPUT);
	GPIO.setDirection(GPIO_4, OUTPUT);
	GPIO.setValue(GPIO_0, LOW);
	GPIO.setValue(GPIO_4, HIGH);

	// Setup light sensor
	GPIO.setDirection(GPIO_5, OUTPUT);
	GPIO.setValue(GPIO_5, HIGH);

	// Setup HALL effect input
	GPIO.setDirection(GPIO_2, INPUT);

	// Setup User button input
	GPIO.setDirection(GPIO_8, INPUT);
	GPIO.setPull(GPIO_8, PULL_UP);

	// Setup accelerometer to update every second
	uint8_t wbuf[2];
	wbuf[0] = 0x20; // 0 (write), 0(do not increment), 0x20 (register address)
	wbuf[1] = 0x17; // 1 Hz, enable X,Y and Z axes

	GPIO.setValue(GPIO_3, LOW);
	SPI.transfer(2, wbuf, NULL);
	GPIO.setValue(GPIO_3, HIGH);

	// Setup VOC
	wbuf[0] = 0x20;
	wbuf[1] = 0xAE;

	I2C.transfer(SGPC3_I2C_ADDRESS, wbuf, 2, NULL, 0);

	// Create timer that sends the CoAP PUT message
	sensorTimerHandler = Timer.create(PERIODIC, sensorTimerPeriod, ReadSensor);
	coapTimerHandler = Timer.create(PERIODIC, coapTimerPeriod, SendCoAP);
	printoutTimerHandler = Timer.create(PERIODIC, printoutTimerPeriod, Printout);
	ledTimerHandler = Timer.create(PERIODIC, ledTimerPeriod, LED);
	Timer.start(sensorTimerHandler);
	Timer.start(coapTimerHandler);
	Timer.start(printoutTimerHandler);
	Timer.start(ledTimerHandler);

	// Start the node
	const uint8_t nwKey[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	Network.setNWKey((uint8_t*)nwKey);
	Network.startLeaf();
	Network.setChannel(18);

	// Print out our setup
	Debug.printSetup();

	// Return UAPI_OK to indicate setup went well
	return UAPI_OK;
}

