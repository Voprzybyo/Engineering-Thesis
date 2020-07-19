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
 @brief This is a OTA demonstrator project for RIIM
 @version 1.00 : First revision
 @date 2019.02.25
*/

#include "RIIM_UAPI.h"

const unsigned char imageBin[] = {
  0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x23, 0x00,
  0x00, 0x00, 0x8c, 0x00, 0x22, 0xf4, 0x00, 0x00, 0x37, 0x7f, 0x03, 0x68,
  0x2f, 0xd5, 0xe2, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x38, 0xb5, 0x0c, 0x4b, 0x0c, 0x4d, 0x1b, 0x68, 0x0c, 0x48, 0x98, 0x47,
  0x0c, 0x4c, 0xab, 0x6c, 0x0f, 0x20, 0x98, 0x47, 0x6b, 0x68, 0x98, 0x47,
  0x0a, 0x4a, 0x4f, 0xf4, 0x7a, 0x71, 0x23, 0x68, 0x00, 0x20, 0x98, 0x47,
  0x63, 0x68, 0xc0, 0xb2, 0x98, 0x47, 0x07, 0x4b, 0x5b, 0x68, 0x98, 0x47,
  0x00, 0x20, 0x38, 0xbd, 0x40, 0x20, 0x00, 0x00, 0x40, 0x23, 0x00, 0x00,
  0x74, 0x60, 0x04, 0x00, 0x80, 0x20, 0x00, 0x00, 0x51, 0x60, 0x04, 0x00,
  0x00, 0x22, 0x00, 0x00, 0x01, 0x4b, 0x02, 0x48, 0x1b, 0x68, 0x18, 0x47,
  0x40, 0x20, 0x00, 0x00, 0x60, 0x60, 0x04, 0x00, 0x49, 0x27, 0x6d, 0x20,
  0x74, 0x68, 0x65, 0x20, 0x4e, 0x45, 0x57, 0x20, 0x69, 0x6d, 0x61, 0x67,
  0x65, 0x0a, 0x00, 0x00, 0x53, 0x74, 0x61, 0x72, 0x74, 0x69, 0x6e, 0x67,
  0x20, 0x4e, 0x45, 0x57, 0x20, 0x69, 0x6d, 0x61, 0x67, 0x65, 0x21, 0x0a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x5f, 0xa9, 0xae, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned int imageBinLen = 192;

// Child node IP address (the node to receive the OTA) is hardcoded in 
// this example. Please substitute it with your own IP address
const IPAddr childNodeIPAddr={.byte={
        0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0xe6, 0x06}};


static const uint32_t   timerPeriod=2000;
static const uint8_t    resourceName[]="OTA";

static uint8_t          coapTimerHandle;
static uint32_t         imageAddressLocation;
static uint8_t          imageTransferred;

static uint8_t          responseCommand;

typedef enum otaState_Type {OTA_Init, OTA_Transferring, OTA_Switch, OTA_Reset, OTA_Done}otaState_Type;

otaState_Type otaState; 




/**
 * @brief Implementation of OTA command 1: Start new OTA
 */
UAPI_Status sendCMD1()
{
    uint8_t payload[100]; // Payload size is limited to 100 bytes in this example

    payload[0]=0x83; // Array of 3 data items
    payload[1]=0x01; // Command 1
    payload[2]=0x1A; // Next value is 32-bit unsigned integer
    payload[3]=0x00;
    payload[4]=0x00;
    payload[5]=0x00;
    payload[6]=0x00;
    payload[7]=0x58; // Next is uint8_t block size
    payload[8]=0x00; // Num bytes in block
    
    CoAP.send(CoAP_PUT, false, resourceName, (uint8_t*)&payload, 9);
    if(responseCommand == 1)
        return UAPI_FINISHED;
    else{
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
    payload[0]=0x83; // Array of 3 data items
    payload[1]=0x02; // Command 2
    payload[2]=0x1A; // Next value is 32-bit unsigned integer
    payload[3]=imageAddressLocation >> 24; // Addr
    payload[4]=imageAddressLocation >> 16; // Addr
    payload[5]=imageAddressLocation >> 8; // Addr
    payload[6]=imageAddressLocation; // Addr
    payload[7]=0x58; // Next is uint8_t block size
    payload[8]=0x00; // Num bytes in block, updated later in this function

    const int HEADER_SIZE=9;
   
    
    if(imageTransferred != 0){
        return UAPI_FINISHED;
    }
    
    // Copy the block into the payload
    for(byteNum=0;byteNum<16;byteNum++){
        if((byteNum+imageAddressLocation) >= imageBinLen){ // Ensure we do not send more than the image
            imageTransferred = 1;
            break;
        }
        payload[byteNum+HEADER_SIZE]=imageBin[byteNum+imageAddressLocation];
    }

    // Update the header with size of block
    payload[8]=byteNum;
    
    // Send the block
    CoAP.send(CoAP_PUT, false, resourceName, payload, HEADER_SIZE+byteNum);
    
    return UAPI_IN_PROGRESS;
}

/**
 * @brief Implementation of OTA command 3 : Finish OTA and prepare for update
 */
UAPI_Status sendCMD3()
{
    uint8_t payload[100]; // Payload size is limited to 100 bytes in this example
    
    payload[0]=0x83; // Array of 3 data items
    payload[1]=0x03; // Command 3
    payload[2]=0x1A; // Next value is 32-bit unsigned integer
    payload[3]=0x00;
    payload[4]=0x00;
    payload[5]=0x00;
    payload[6]=0x00;
    payload[7]=0x58; // Next is uint8_t block size
    payload[8]=0x00; // Num bytes in block - 0 for CMD 3
    const int HEADER_SIZE=9;

    CoAP.send(CoAP_PUT, false, resourceName, (uint8_t*)&payload, HEADER_SIZE);

    return UAPI_FINISHED;
}

/**
 * @brief Implementation of OTA command 4 : Reset
 */
UAPI_Status sendCMD4()
{
    uint8_t payload[100]; // Payload size is limited to 100 bytes in this example
    
    payload[0]=0x83; // Array of 3 data items
    payload[1]=0x04; // Command 3
    payload[2]=0x1A; // Next value is 32-bit unsigned integer
    payload[3]=0x00;
    payload[4]=0x00;
    payload[5]=0x00;
    payload[6]=0x00;
    payload[7]=0x58; // Next is uint8_t block size
    payload[8]=0x00; // Num bytes in block - 0 for CMD 3
    const int HEADER_SIZE=9;

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
    if(Network.isPartOfNetwork((IPAddr*)&childNodeIPAddr) == false)
    {
        Util.printf("Node not part of network yet\n");
        return;
    }

    if(connected==false){
        if(CoAP.connectToServer6(childNodeIPAddr, false) == UAPI_OK){
            Util.printf("# Connect to server OK\n");
            connected=true;
        } else {
            Util.printf("# Not connected to server yet\n");
            return;
        }
    }

    if(otaState==OTA_Init){
        Util.printf("# OTA Init\n");
        if(sendCMD1() == UAPI_FINISHED){
            otaState=OTA_Transferring;
        }
    } else if(otaState==OTA_Transferring){
        Util.printf("# OTA transfer block\n");
        if(sendCMD2() == UAPI_FINISHED){
            otaState=OTA_Switch;
        }
    } else if(otaState==OTA_Switch){
        Util.printf("# OTA prepare for update\n");
        if(sendCMD3()==UAPI_FINISHED){
            otaState=OTA_Reset;
        }
    } else if(otaState==OTA_Reset){
        Util.printf("# Resetting destination board\n");
        if(sendCMD4()==UAPI_FINISHED){
            otaState=OTA_Done;
        }
    } else if(otaState==OTA_Done){
        // Do nothing more
    } else {
        Util.printf("# ERROR: This should never happen!\n");
    }
    
    return;
}

/**
 * @brief CoAP response handler
 */
void responseHandler(const uint8_t *payload, uint8_t payloadSize)
{
    uint32_t imageAddress;
    uint32_t blockSize;
    
    imageAddress=  (payload[6] << 24) |
                    (payload[7] << 16) |
                    (payload[8] << 8)  |
                    (payload[9]);

    blockSize=     payload[11];

    responseCommand=payload[4];
    
    // We check if image address is as the last we sent. If so, we know
    // that the block was received correctly and we can send next block
    if(responseCommand == 0x02){
        if(imageAddressLocation == imageAddress){
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

    imageAddressLocation=0;
    imageTransferred=0;
    responseCommand=0;
    otaState = OTA_Init;
    connected = false;

    // Start as a border router
    const uint8_t nwKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    Network.setNWKey((uint8_t*)nwKey);
    Network.startBorderRouter(NULL,NULL,NULL,NULL);
    Network.setChannel(18);

    // Create CoAP resource
    CoAP.registerResponseHandler(responseHandler);

    // Create timer that sends the CoAP GET message
    coapTimerHandle=Timer.create(PERIODIC, timerPeriod, sendCoAP);
    Timer.start(coapTimerHandle);

    Debug.printSetup();
    
    return UAPI_OK;
}

