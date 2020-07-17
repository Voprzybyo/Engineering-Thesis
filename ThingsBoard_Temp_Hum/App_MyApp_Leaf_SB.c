#include "RIIM_UAPI.h"


///////////////////////////////////////////////////
// Cloud server setup variables : Resource_Name and server_ip4_addr
///////////////////////////////////////////////////

// Change Resource_Name to your CoAP resource address
static const uint8_t resourceName[] = "/api/v1/3UhfDTKzS9tn6meK7Wpm/telemetry/";
// Change server_ip4_addr to the address of your server.
const uint8_t serverIP4Addr[4] = { 104,196,24,70 };

uint8_t startupTimerHandle;
const uint32_t coapTimerPeriod = 10000;

static const uint32_t timerPeriod = 10000;
static uint8_t coapTimerHandle, sensorTimerHandle;

static uint16_t temp, tempFraction, humidity;

/**
 * @brief This function sends a CoAP message to the server (cloud)
 */

static void readSensor()
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
	Util.printf("SHT35 sensor read: Temp(Degrees C): %i.%i , Hum(Percent): %i\n", temp, tempFraction, humidity);
	return;
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
	payloadLength = Util.sprintf((char*)payload, "{\"Temp\": %i.%i, \"Hum\": %i}", temp, tempFraction, humidity);
	CoAP.send(CoAP_POST, false, resourceName, payload, payloadLength);
	return;
}

/**
 * @brief This function checks that we are part of a network before continuing
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

	Timer.stop(startupTimerHandle);
	Timer.start(coapTimerHandle);

	return;
}

/**
 * @brief Empty response handler. We do not need it in this example
 */
void responseHandler(const uint8_t* payload, uint8_t payload_size)
{
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
	I2C.init(I2C_400KHZ);
	startupTimerHandle = Timer.create(PERIODIC, 5000, startup);
	Timer.start(startupTimerHandle);
	sensorTimerHandle = Timer.create(PERIODIC, timerPeriod, readSensor);
	coapTimerHandle = Timer.create(PERIODIC, timerPeriod, sendCoAP);
	Timer.start(sensorTimerHandle);
	Network.startLeaf();
	Debug.printSetup();
	return UAPI_OK;
}
