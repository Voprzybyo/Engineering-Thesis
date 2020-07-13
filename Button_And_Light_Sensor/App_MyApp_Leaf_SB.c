#include "RIIM_UAPI.h"

#define SGPC3_I2C_ADDRESS 0x58

static const uint32_t sensorTimerPeriod=100; // 1/10th second
static const uint32_t coapTimerPeriod=5000; // 5 seconds
static const uint32_t ledTimerPeriod=500; // 0.5 seconds

static const uint8_t resourceName[]="data";
static uint8_t coapTimerHandler, sensorTimerHandler, ledTimerHandler;
static uint32_t light;
static uint32_t userButton;

/**
 * @brief Handler called periodically to read the pushbutton.
 */
static void ReadUserButton()
{
	userButton = GPIO.getValue(GPIO_8);
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
 * @brief Handler called periodically by a timer to read
 * various sensors on the sensor board
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
	} else {
		sensorNumber = 0;
	}

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
    if(Network.getBorderRouterAddress(&rootNodeIPAddr)!=UAPI_OK){
        Util.printf("# No root node registered! Not possible to send CoAP message yet\n");
        return;
    }

    CoAP.connectToServer6(rootNodeIPAddr, false);

    // For this demo, we'll use JSON formatting. JSON is not size efficient, 
    // but easy to read and parse
    uint8_t payload[100];
    int payloadLength;
    
    payloadLength=Util.sprintf((char*)payload,"{\"Light\": %i, \"Button\": %i}", light, userButton);
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
void ResponseHandler(const uint8_t *payload, uint8_t payload_size)
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

	light = 0;

    // Set up GPIO. We must use one GPIO for SPI chip select
    GPIO.setDirection(GPIO_3, OUTPUT);
    GPIO.setValue(GPIO_3, HIGH); 
    

    // Setup LEDs
    GPIO.setDirection(GPIO_0, OUTPUT);
    GPIO.setDirection(GPIO_4, OUTPUT);
    GPIO.setValue(GPIO_0, LOW);
    GPIO.setValue(GPIO_4, HIGH);

    // Setup light sensor
    GPIO.setDirection(GPIO_5, OUTPUT);
    GPIO.setValue(GPIO_5, HIGH);

	// Setup User button input
	GPIO.setDirection(GPIO_8, INPUT);
	GPIO.setPull(GPIO_8, PULL_UP);

    // Create timer that sends the CoAP PUT message
    sensorTimerHandler=Timer.create(PERIODIC, sensorTimerPeriod, ReadSensor);
    coapTimerHandler=Timer.create(PERIODIC, coapTimerPeriod, SendCoAP);
    ledTimerHandler=Timer.create(PERIODIC, ledTimerPeriod, LED);
    Timer.start(sensorTimerHandler);
    Timer.start(coapTimerHandler);
    Timer.start(ledTimerHandler);

    // Start the node
    const uint8_t nwKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    Network.setNWKey((uint8_t*)nwKey);
    Network.startLeaf();
    Network.setChannel(18);

    // Print out our setup
    Debug.printSetup();
    
    // Return UAPI_OK to indicate setup went well
    return UAPI_OK;
}

