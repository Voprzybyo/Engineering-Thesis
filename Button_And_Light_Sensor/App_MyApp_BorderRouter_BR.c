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
 @brief This is the Border Router firmware for the Sensor Board demo application
 @version 1.00 : First revision
 @date 2019.02.25
*/

#include "RIIM_UAPI.h"

static const char resourceName[]="data";


/**
 * @brief Handler called when a CoAP message arrives
 *
 * @param type[in]: CoAP request type. Can be GET, PUT, POST or DELETE
 * @param message[in]: The CoAP payload
 * @param messageSize[in]: Mumber of bytes in the CoAP payload
 * @param response[out]: The repsonse payload to be sent back
 * @param responseSize[out]: Number of bytes in the response
 */
static void CoAP_Handler(RequestType type, IPAddr srcAddr, uint8_t *payload, uint8_t payloadSize, uint8_t *response, uint8_t *responseSize)
{
    // In this example we only print the payload directly to the UART.
    // Uncomment the next two lines to print additional information to the console

//    Util.printf("Got CoAP message with payload size %i from ", payloadSize);
//    Debug.printIPAddr(&srcAddr);
    Util.printf("%.*s\n", payloadSize, payload);
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
    Util.printf("Starting RIIM ROOT\n");
    
    // This example does not use the Ethernet. We provide only NULL
    // as IPv6 and IPv4 configuration
    const uint8_t nwKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    Network.setNWKey((uint8_t*)nwKey);
    Network.startBorderRouter(NULL, NULL, NULL, NULL);
    Network.setChannel(18);

    // Setup coap resource
    CoAP.registerResource(resourceName,1,CoAP_Handler);
    
    // Print out the configuration
    Debug.printSetup();
    
    return UAPI_OK;
}

