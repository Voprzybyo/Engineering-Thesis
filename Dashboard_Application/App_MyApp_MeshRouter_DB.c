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
 @brief This is the Development Board (DB) firmware for starting as a mesh router
 @version 1.00 : First revision
 @date 2019.02.25
*/

#include "RIIM_UAPI.h"

static const uint32_t timerPeriod=1000;
static uint8_t blinkTimerHandler;

/**
 * @brief Start blinking a LED when were online
 */
static void startup()
{
    // Wait until we are part of network
    if(Network.getNetworkState() != ONLINE){
        return;
    }

    GPIO.toggle(GPIO_6);

}


/**
 * @brief This is the entry point of the user application. It is 
 *        called only once during startup. It is responsible for
 *        setting up callbacks, timers etc.
 */
RIIM_SETUP()
{
    Util.printf("Starting RIIM Router Node\n");

    // Periodically check if we are part of a network. In that case,
    // blink the blue LED on the BR
    GPIO.setDirection(GPIO_6, OUTPUT);
    blinkTimerHandler=Timer.create(PERIODIC, 1000, startup);
    Timer.start(blinkTimerHandler);

    // Simply start as a router node. Nothing more is needed.
    // However, all functions supported by leaf nodes are also
    // supported on the router nodes. You could therefore also
    // use this node as a sensor node, for instance.
    const uint8_t nwKey[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    Network.setNWKey((uint8_t*)nwKey);
    Network.startMeshRouter();
    Network.setChannel(18);
    
    return UAPI_OK;
}

