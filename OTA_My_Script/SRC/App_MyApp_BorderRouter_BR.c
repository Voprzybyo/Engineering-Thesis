
#include "RIIM_UAPI.h"
#include "App_NewImage_Leaf_SB.h"


// Child node IP address (the node to receive the OTA) is hardcoded in 
// this example. Please substitute it with your own IP address
const IPAddr childNodeIPAddr = { .byte = {
		0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0xe2, 0x9d} };


static const uint32_t   timerPeriod = 2000;
static const uint8_t    resourceName[] = "OTA";

static uint8_t          coapTimerHandle;
static uint32_t         imageAddressLocation;
static uint8_t          imageTransferred;

static uint8_t          responseCommand;

typedef enum otaState_Type { OTA_Init, OTA_Transferring, OTA_Switch, OTA_Reset, OTA_Done }otaState_Type;

otaState_Type otaState;




/**
 * @brief Implementation of OTA command 1: Start new OTA
 */
UAPI_Status sendCMD1()
{
	uint8_t payload[100]; // Payload size is limited to 100 bytes in this example

	payload[0] = 0x83; // Array of 3 data items
	payload[1] = 0x01; // Command 1
	payload[2] = 0x1A; // Next value is 32-bit unsigned integer
	payload[3] = 0x00;
	payload[4] = 0x00;
	payload[5] = 0x00;
	payload[6] = 0x00;
	payload[7] = 0x58; // Next is uint8_t block size
	payload[8] = 0x00; // Num bytes in block

	CoAP.send(CoAP_PUT, false, resourceName, (uint8_t*)&payload, 9);
	if (responseCommand == 1)
		return UAPI_FINISHED;
	else {
		return UAPI_IN_PROGRESS;
	}

}

/**
 * @brief Implementation of OTA command 2: Transfer data block
 */
UAPI_Status sendCMD2()
{

	uint8_t payload[100]; // Payload size is limited to 100 bytes in this example
	int byteNum;

	// Create header. Header is CBOR encoded
	// This is so simple we dont need any CBOR libraries
	payload[0] = 0x83; // Array of 3 data items
	payload[1] = 0x02; // Command 2
	payload[2] = 0x1A; // Next value is 32-bit unsigned integer
	payload[3] = imageAddressLocation >> 24; // Addr
	payload[4] = imageAddressLocation >> 16; // Addr
	payload[5] = imageAddressLocation >> 8; // Addr
	payload[6] = imageAddressLocation; // Addr
	payload[7] = 0x58; // Next is uint8_t block size
	payload[8] = 0x00; // Num bytes in block, updated later in this function

	const int HEADER_SIZE = 9;


	if (imageTransferred != 0) {
		return UAPI_FINISHED;
	}

	// Copy the block into the payload
	for (byteNum = 0; byteNum < 16; byteNum++) {
		if ((byteNum + imageAddressLocation) >= imageBinLen) { // Ensure we do not send more than the image
			imageTransferred = 1;
			break;
		}
		payload[byteNum + HEADER_SIZE] = imageBin[byteNum + imageAddressLocation];
	}

	// Update the header with size of block
	payload[8] = byteNum;

	// Send the block
	CoAP.send(CoAP_PUT, false, resourceName, payload, HEADER_SIZE + byteNum);

	return UAPI_IN_PROGRESS;
}

/**
 * @brief Implementation of OTA command 3 : Finish OTA and prepare for update
 */
UAPI_Status sendCMD3()
{
	uint8_t payload[100]; // Payload size is limited to 100 bytes in this example

	payload[0] = 0x83; // Array of 3 data items
	payload[1] = 0x03; // Command 3
	payload[2] = 0x1A; // Next value is 32-bit unsigned integer
	payload[3] = 0x00;
	payload[4] = 0x00;
	payload[5] = 0x00;
	payload[6] = 0x00;
	payload[7] = 0x58; // Next is uint8_t block size
	payload[8] = 0x00; // Num bytes in block - 0 for CMD 3
	const int HEADER_SIZE = 9;

	CoAP.send(CoAP_PUT, false, resourceName, (uint8_t*)&payload, HEADER_SIZE);

	return UAPI_FINISHED;
}

/**
 * @brief Implementation of OTA command 4 : Reset
 */
UAPI_Status sendCMD4()
{
	uint8_t payload[100]; // Payload size is limited to 100 bytes in this example

	payload[0] = 0x83; // Array of 3 data items
	payload[1] = 0x04; // Command 3
	payload[2] = 0x1A; // Next value is 32-bit unsigned integer
	payload[3] = 0x00;
	payload[4] = 0x00;
	payload[5] = 0x00;
	payload[6] = 0x00;
	payload[7] = 0x58; // Next is uint8_t block size
	payload[8] = 0x00; // Num bytes in block - 0 for CMD 3
	const int HEADER_SIZE = 9;

	CoAP.send(CoAP_PUT, false, resourceName, (uint8_t*)&payload, HEADER_SIZE);

	return UAPI_FINISHED;
}

static bool connected;


/**
 * @brief Function for sending CoAP messages to OTA recepient
 */
static void sendCoAP()
{
	// Check if node is part of network
	if (Network.isPartOfNetwork((IPAddr*)&childNodeIPAddr) == false)
	{
		Util.printf("Node not part of network yet\n");
		return;
	}

	if (connected == false) {
		if (CoAP.connectToServer6(childNodeIPAddr, false) == UAPI_OK) {
			Util.printf("# Connect to server OK\n");
			connected = true;
		}
		else {
			Util.printf("# Not connected to server yet\n");
			return;
		}
	}

	if (otaState == OTA_Init) {
		Util.printf("# OTA Init\n");
		if (sendCMD1() == UAPI_FINISHED) {
			otaState = OTA_Transferring;
		}
	}
	else if (otaState == OTA_Transferring) {
		Util.printf("# OTA transfer block\n");
		if (sendCMD2() == UAPI_FINISHED) {
			otaState = OTA_Switch;
		}
	}
	else if (otaState == OTA_Switch) {
		Util.printf("# OTA prepare for update\n");
		if (sendCMD3() == UAPI_FINISHED) {
			otaState = OTA_Reset;
		}
	}
	else if (otaState == OTA_Reset) {
		Util.printf("# Resetting destination board\n");
		if (sendCMD4() == UAPI_FINISHED) {
			otaState = OTA_Done;
		}
	}
	else if (otaState == OTA_Done) {
		// Do nothing more
	}
	else {
		Util.printf("# ERROR: This should never happen!\n");
	}

	return;
}

/**
 * @brief CoAP response handler
 */
void responseHandler(const uint8_t* payload, uint8_t payloadSize)
{
	uint32_t imageAddress;
	uint32_t blockSize;

	imageAddress = (payload[6] << 24) |
		(payload[7] << 16) |
		(payload[8] << 8) |
		(payload[9]);

	blockSize = payload[11];

	responseCommand = payload[4];

	// We check if image address is as the last we sent. If so, we know
	// that the block was received correctly and we can send next block
	if (responseCommand == 0x02) {
		if (imageAddressLocation == imageAddress) {
			imageAddressLocation += blockSize;
			Util.printf("Addresses are the same, incrementing to %.08x\n", imageAddressLocation);
		}
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
	Util.printf("Starting RIIM Root Node - OTA Demo\n");

	imageAddressLocation = 0;
	imageTransferred = 0;
	responseCommand = 0;
	otaState = OTA_Init;
	connected = false;

	// Start as a border router
	const uint8_t nwKey[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	Network.setNWKey((uint8_t*)nwKey);
	Network.startBorderRouter(NULL, NULL, NULL, NULL);
	Network.setChannel(18);

	// Create CoAP resource
	CoAP.registerResponseHandler(responseHandler);

	// Create timer that sends the CoAP GET message
	coapTimerHandle = Timer.create(PERIODIC, timerPeriod, sendCoAP);
	Timer.start(coapTimerHandle);

	Debug.printSetup();

	return UAPI_OK;
}

