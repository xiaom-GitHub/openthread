/************************************************************************************
* This header file is provided as part of the interface to the Freescale 802.15.4
* MAC and PHY layer.
*
* The file covers the interface to the Application Support Package (ASP).
*
* Copyright (c) 2014, Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* o Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
*
* o Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or
*   other materials provided with the distribution.
*
* o Neither the name of Freescale Semiconductor, Inc. nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
************************************************************************************/

#ifndef _APP_ASP_INTERFACE_H_
#define _APP_ASP_INTERFACE_H_

/************************************************************************************
*************************************************************************************
* Includes
*************************************************************************************
************************************************************************************/

#include "EmbeddedTypes.h"
#include "FunctionalityDefines.h"

#ifdef __cplusplus
    extern "C" {
#endif

/************************************************************************************
*************************************************************************************
* Public types
*************************************************************************************
************************************************************************************/
enum {
  gAspPowerLevel_m40dBm = 0x03,
  gAspPowerLevel_m38dBm = 0x04,
  gAspPowerLevel_m36dBm = 0x05,
  gAspPowerLevel_m34dBm = 0x06,
  gAspPowerLevel_m32dBm = 0x07,
  gAspPowerLevel_m30dBm = 0x08,
  gAspPowerLevel_m28dBm = 0x09,
  gAspPowerLevel_m26dBm = 0x0A,
  gAspPowerLevel_m24dBm = 0x0B,
  gAspPowerLevel_m22dBm = 0x0C,
  gAspPowerLevel_m20dBm = 0x0D,
  gAspPowerLevel_m18dBm = 0x0E,
  gAspPowerLevel_m16dBm = 0x0F,
  gAspPowerLevel_m14dBm = 0x10,
  gAspPowerLevel_m12dBm = 0x11,
  gAspPowerLevel_m10dBm = 0x12,
  gAspPowerLevel_m8dBm  = 0x13,
  gAspPowerLevel_m6dBm  = 0x14,
  gAspPowerLevel_m4dBm  = 0x15,
  gAspPowerLevel_m2dBm  = 0x16,
  gAspPowerLevel_0dBm   = 0x17,
  gAspPowerLevel_2dBm   = 0x18,
  gAspPowerLevel_4dBm   = 0x19,
  gAspPowerLevel_6dBm   = 0x1A,
  gAspPowerLevel_8dBm   = 0x1B,
  gAspPowerLevel_10dBm  = 0x1C,
  gAspPowerLevel_12dBm  = 0x1D,
  gAspPowerLevel_14dBm  = 0x1E,
  gAspPowerLevel_16dBm  = 0x1F
};

#define gAspPowerLevel_m40dBm_c (uint8_t)gAspPowerLevel_m40dBm
#define gAspPowerLevel_m38dBm_c (uint8_t)gAspPowerLevel_m38dBm
#define gAspPowerLevel_m36dBm_c (uint8_t)gAspPowerLevel_m36dBm
#define gAspPowerLevel_m34dBm_c (uint8_t)gAspPowerLevel_m34dBm
#define gAspPowerLevel_m32dBm_c (uint8_t)gAspPowerLevel_m32dBm
#define gAspPowerLevel_m30dBm_c (uint8_t)gAspPowerLevel_m30dBm
#define gAspPowerLevel_m28dBm_c (uint8_t)gAspPowerLevel_m28dBm
#define gAspPowerLevel_m26dBm_c (uint8_t)gAspPowerLevel_m26dBm
#define gAspPowerLevel_m24dBm_c (uint8_t)gAspPowerLevel_m24dBm
#define gAspPowerLevel_m22dBm_c (uint8_t)gAspPowerLevel_m22dBm
#define gAspPowerLevel_m20dBm_c (uint8_t)gAspPowerLevel_m20dBm
#define gAspPowerLevel_m18dBm_c (uint8_t)gAspPowerLevel_m18dBm
#define gAspPowerLevel_m16dBm_c (uint8_t)gAspPowerLevel_m16dBm
#define gAspPowerLevel_m14dBm_c (uint8_t)gAspPowerLevel_m14dBm
#define gAspPowerLevel_m12dBm_c (uint8_t)gAspPowerLevel_m12dBm
#define gAspPowerLevel_m10dBm_c (uint8_t)gAspPowerLevel_m10dBm
#define gAspPowerLevel_m8dBm_c  (uint8_t)gAspPowerLevel_m8dBm
#define gAspPowerLevel_m6dBm_c  (uint8_t)gAspPowerLevel_m6dBm
#define gAspPowerLevel_m4dBm_c  (uint8_t)gAspPowerLevel_m4dBm
#define gAspPowerLevel_m2dBm_c  (uint8_t)gAspPowerLevel_m2dBm
#define gAspPowerLevel_0dBm_c   (uint8_t)gAspPowerLevel_0dBm
#define gAspPowerLevel_2dBm_c   (uint8_t)gAspPowerLevel_2dBm
#define gAspPowerLevel_4dBm_c   (uint8_t)gAspPowerLevel_4dBm
#define gAspPowerLevel_6dBm_c   (uint8_t)gAspPowerLevel_6dBm
#define gAspPowerLevel_8dBm_c   (uint8_t)gAspPowerLevel_8dBm
#define gAspPowerLevel_10dBm_c  (uint8_t)gAspPowerLevel_10dBm
#define gAspPowerLevel_12dBm_c  (uint8_t)gAspPowerLevel_12dBm
#define gAspPowerLevel_14dBm_c  (uint8_t)gAspPowerLevel_14dBm
#define gAspPowerLevel_16dBm_c  (uint8_t)gAspPowerLevel_16dBm

  // Valid values for aspSetNotifyReq_t->notifications.
enum {
  gAspNotifyNone_c,         // No notifications about beacon state
  gAspNotifyIdle_c,         // Notify about remaining time in Idle portion of CAP
  gAspNotifyInactive_c,     // Notify about remaining time in inactive portion of superframe
  gAspNotifyIdleInactive_c, // Notify about remaining time in Idle portion of CAP, and inactive portion of superframe
  gAspNotifyLastEntry_c     // Don't use!
};

  // Valid values for aspTelecTestReq_t->mode
enum {
  gTestForceIdle_c=0  ,
  gTestPulseTxPrbs9_c,
  gTestContinuousRx_c,
  gTestContinuousTxMod_c,
  gTestContinuousTxNoMod_c,
  gTestContinuousTx2Mhz_c,
  gTestContinuousTx200Khz_c,
  gTestContinuousTx1MbpsPRBS9_c,
  gTestContinuousTxExternalSrc_c,  
  gTestContinuousTxNoModZero_c,
  gTestContinuousTxNoModOne_c
};

  // GetMacState status codes
enum {
  gAspMacStateIdle_c = 0,
  gAspMacStateBusy_c,
  gAspMacStateNotEmpty_c
};


//-----------------------------------------------------------------------------------
//     Messages from ASP to application
//-----------------------------------------------------------------------------------
enum {
  gAspErrorCfm_c,
  gAspAppWakeInd_c,
  gAspAppIdleInd_c,
  gAspAppInactiveInd_c,
  gAspAppEventInd_c,
  gAspMaxPrimitives_c
};

// Note:
// For some structs there is no difference if they are packed
// or not, because of their members. Nevertheless, it's better
// that they remain packed for the future extend.

  // Type: gAspAppWakeInd_c

typedef PACKED_STRUCT appWakeInd_tag
	{
	  uint8_t status;
	} appWakeInd_t;

  // Type: gAspAppIdleInd_c

typedef PACKED_STRUCT appIdleInd_tag
    {
	uint8_t timeRemaining[3];
    } appIdleInd_t;

  // Type: gAspAppInactiveInd_c

typedef PACKED_STRUCT appInactiveInd_tag
	{
  uint8_t timeRemaining[3];
	} appInactiveInd_t;

  // Type: gAspAppEventInd_c

typedef PACKED_STRUCT appEventInd_tag
   {
  uint8_t dummy; // This primitive has no parameters.
   } appEventInd_t;


  // Type: gAspErrorCnf_c

typedef PACKED_STRUCT appErrorCfm_tag
   {
  uint8_t  status;
   } appErrorCfm_t;


  // ASP to application message

typedef PACKED_STRUCT aspToAppMsg_tag
{
  uint8_t msgType;
  union {
    appErrorCfm_t           appErrorCfm;
    appWakeInd_t            appWakeInd;
    appIdleInd_t            appIdleInd;
    appInactiveInd_t        appInactiveInd;
    appEventInd_t           appEventInd;
  } msgData;
} aspToAppMsg_t;


/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
* Requests the current value of the transceiver internal event timer.
*
* Interface assumptions:
*   time - pointer to a  zbClock24_t variable, where the time value will be stored
*
* Return value:
*   None
*
************************************************************************************/
void    Asp_GetTimeReq(zbClock24_t *time);

/************************************************************************************
* Get basic state of the MAC. The caller can use this information to determine
* if it is safe to go into one of the deep sleep modes! It checks for the following:
* The MEM, SEQ, and MLME state machines must be in idle state. Otherwise return "busy"
* The queues must be empty. Otherwise return "not empty"
* Otherwise just return "idle".
*
* Interface assumptions:
*   None
*
* Return value:
*   one of the three values
*     gAspMacStateBusy_c;
*    gAspMacStateNotEmpty_c;
*    gAspMacStateIdle_c;
*
************************************************************************************/
uint8_t Asp_GetMacStateReq(void);

/************************************************************************************
* Wake-up the transceiver from Doze/Hibernate mode. The ATTNBi pin of the transceiver must be wired to
* a MCU port pin for this primitive to function. Otherwise, it has no effect. The AttEnable, and AttDisable
* macros must be defined in Target.h. The former must set the MCU port pin to logic high, and the latter
* must set it to logic low. If the two macros are modified e.g. due to changes in the PCB layout, the PHY
* should be modified also.
*
* Interface assumptions:
*   None
*
* Return value:
*   None
*
************************************************************************************/
void    Asp_WakeReq(void);

#if gAspPowerSaveCapability_d
 /************************************************************************************
  * Shut down the transceiver for a given amount of time in symbols. The CLKO output pin will stop providing
  * a clock signal to the CPU if clko_en is 0 (FALSE). The CLKO output pin will continue to provide a clock
  * signal if clko_en is 1 (TRUE). The dozeDuration parameter is the maximum time in number of symbols
  * that the transceiver will be in doze mode. The transceiver can be woken up prematurely from doze mode by
  *  a signal on the ATTNBi pin. CLKO is automatically started again when transceiver leaves doze mode.
  *
  * Interface assumptions:
  *   dozeDuration
  *   clko_en
  *
  * Return value:
  *   None
  *
  ************************************************************************************/
  uint8_t Asp_DozeReq(zbClock24_t *dozeDuration, uint8_t clko_en);

  /************************************************************************************
  * Automatically shut down the transceiver during idle periods. The CLKO output pin will stop providing a
  * clock signal to the CPU if clko_en is 0 (FALSE). The CLKO output pin will continue to provide a clock
  * signal if clko_en is 1 (TRUE). The autoDozeInterval parameter is a suggested period in symbols in
  * which the transceiver will be in doze mode. This interval may be overridden if doze mode is interrupted by
  * an external signal (ATTNBi pin). If the enableWakeIndication parameter is TRUE then an
  * ASP-WAKE.Indication is sent to the APP layer each time the doze interval expires. The indication can be
  * used by the APP layer to do processing. In order to enable auto doze the autoEnable parameter must be
  * TRUE. Auto doze can be disabled by sending another ASP-AUTODOZE.Request with the autoEnable
  * parameter set to FALSE. It is recommended to use the ASP-WAKE. Indication for simple processing
  * during auto doze since it will occur frequently (if enabled) and the auto doze feature is blocked during the
  * processing of the indication in the ASP_APP SAP..
  *
  * Interface assumptions:
  *   dozeDuration
  *   clko_en
  *
  * Return value:
  *   Standard error code
  *
  ************************************************************************************/
  void Asp_AutoDozeReq(bool_t  autoEnable, bool_t  enableWakeIndication, zbClock24_t *autoDozeInterval, uint8_t clko_en);

  /************************************************************************************
  * The hibernate request shuts down the transceiver. The CLKO output pin will stop providing a clock signal
  * to the CPU. Only a signal on the ATTNBi pin of the transceiver or a power loss can bring the transceiver out
  * of hibernate mode. CLKO is automatically started again when transceiver leaves hibernate mode. The
  * hibernate mode is not adequate for beaconed operation. Doze mode should be used instead when
  * transceiver timers are required.
  *
  * Interface assumptions:
  *   None
  *
  * Return value:
  *   Standard error code
  *
  ************************************************************************************/
  uint8_t Asp_HibernateReq(void);

  /************************************************************************************
  * Set the default minimum doze time. If the MAC cannot doze for at least the minimum doze time, then it
  * will not enter doze mode. E.g. if the doze request is issued 3ms before the end of a beacon period the MAC
  * will not enter doze mode since the default minimum doze time is 4ms. However, if the minimum doze time
  * is changed to 2ms, then the MAC will doze for 2ms, and wake up 1ms before the next beacon assuming
  * the same timing in both examples.
  *
  * Interface assumptions:
  *   minDozeTime
  *
  * Return value:
  *   None
  *
  ************************************************************************************/
  void Asp_SetMinDozeTimeReq(zbClock24_t *minDozeTime);

  /************************************************************************************
  * The acoma request shuts down the transceiver. The CLKO output pin will stop providing a clock signal to
  * the CPU if clko_en is 0 (FALSE). The CLKO output pin will continue to provide a clock signal if clko_en
  * is 1 (TRUE). Only a signal on the ATTNBi pin of the transceiver or a power loss can bring the transceiver
  * out of acoma mode. CLKO is automatically started again when transceiver leaves acoma mode. The
  * acoma mode is not suited for beaconed operation. Doze mode should be used instead when transceiver
  * timers are required.
  * The main difference between acoma mode and hibernate mode is that CLKO can be generated during
  * acoma mode – this is not possible in hibernate. Note also that acoma mode does not support timer wakeup
  * (which is possible during doze mode).
  *
  * Interface assumptions:
  *   clko_en
  *
  * Return value:
  *   Standard error code
  *
  ************************************************************************************/
  uint8_t Asp_AcomaReq(uint8_t clko_en);
#endif // gAspPowerSaveCapability_d

#if gAspHwCapability_d
  /************************************************************************************
  *  This primitive sets and/or enables the CLKO output of the transceiver. If clkoEnable is TRUE, CLKO is
  *  made active, otherwise it is disabled. The CLKO output frequency is programmed depending upon the
  *  value contained in clkoRate per the CLKO frequency selection of the transceiver.
  *   clkoRate can be assigned the following values:
  * Interface assumptions:
  *   clkoEnable
  *   clkoRate
  *
  * Return value:
  *   Standard error code
  *
  ************************************************************************************/
  uint8_t Asp_ClkoReq(bool_t  clkoEnable, uint8_t clkoRate);

  /************************************************************************************
  * This primitive sets the trim capacitor value for the transceiver. Upon receipt of the request, the trim
  * capacitor value contained in the 8-bit parameter is programmed to the transceiver.
  *
  * Interface assumptions:
  *   trimValue
  *
  * Return value:
  *   None
  *
  ************************************************************************************/
  void    Asp_TrimReq(uint8_t trimValue);

  /************************************************************************************
  * This primitive sets the GPIO data direction. GPIOs 3-7 are programmed as outputs if the respective bit in
  * mask is a logical 1, otherwise they are programmed as inputs. Bits 0,1 and 2 of mask are ignored.
  *
  * Interface assumptions:
  *   directionMask
  *
  * Return value:
  *   None
  *
  ************************************************************************************/
  void    Asp_DdrReq(uint8_t directionMask);

  /************************************************************************************
  * This primitive reads or writes the GPIO data register. portWrite is a Boolean value. If TRUE, the respective
  *  bits in portValue will be programmed to the GPIO data register (only bits 3-7 are valid). portValue is
  * a pointer to a uint8_t variable. If the function is called with the portWrite set to TRUE, than the value at the
  * address given by portValue will be written to the transceiver port. Otherwise, if the function is called with
  * the portWrite set to FALSE, than the transceiver input port value will be copied at the address given
  * by portValue.
  *
  * Interface assumptions:
  *   portValue
  *
  * Return value:
  *   None
  *
  ************************************************************************************/
  void Asp_PortReq(uint8_t portWrite, uint8_t *portValue);
#endif // gAspHwCapability_d

  /************************************************************************************
  * This primitive writes data into the transceiver.
  *
  * Interface assumptions:
  *   portValue
  *
  * Return value:
  *   Standard error code
  *
  ************************************************************************************/
  uint8_t Asp_XcvrWriteReq (uint8_t mode, uint16_t addr, uint8_t len, uint8_t* pData);

  /************************************************************************************
  * This primitive reads data from the transceiver.
  *
  * Interface assumptions:
  *   portValue
  *
  * Return value:
  *   Standard error code
  *
  ************************************************************************************/
  uint8_t Asp_XcvrReadReq (uint8_t mode, uint16_t addr, uint8_t len, uint8_t* pData);


#if gAspEventCapability_d
  /************************************************************************************
  * This primitive can be used to schedule a notification for an application event. The notification is a single
  * instance event. If there is any conflict with the MAC operation a status of gInvalidParameter_c is
  * returned, otherwise a status of gSuccess_c is returned.
  * The eventTime parameter is a 3-byte little endian integer symbol time. The event time is relative to the
  * instance when the function call is made.
  *
  * Interface assumptions:
  *   None
  *
  * Return value:
  *   Standard error code
  *
  ************************************************************************************/
  uint8_t Asp_EventReq(zbClock24_t *time);
#endif // gAspEventCapability_d

#if gBeaconedCapability_d
  /************************************************************************************
  * Request the remaining time in the inactive portion of the super frame.
  *
  * Interface assumptions:
  *  time - pointer to a  zbClock24_t variable , where the time value will be stored
  *
  * Return value:
  *   Standard error code
  *
  ************************************************************************************/
  uint8_t Asp_GetInactiveTimeReq(zbClock24_t *time);

  /************************************************************************************
  * This primitive controls the indications generated in beaconed operation. The notifications parameter can
  * be any of the following four values:
  * 1.  gAspNotifyNone_c No indications are sent to the APP layer.
  * 2.  gAspNotifyIdle_c ASP-IDLE Indication (See Section 4.2.19, “Idle Indication”) is sent.
  * 3.  gAspNotifyInactive_c ASP-INACTIVE Indication (See Section 4.2.20, “Inactive Indication”)
  * is sent.
  * 4.  gAspNotifyIdleInactive_c ASP-IDLE, and ASP-INACTIVE Indications are sent.
  * If the MAC PIB attribute macRxOnWhenIdle is set then no idle indications are sent.
  *
  * Interface assumptions:
  *   notifications
  *
  * Return value:
  *   Standard error code
  *
  ************************************************************************************/
  uint8_t Asp_SetNotifyReq(uint8_t notifications);
#endif // gBeaconedCapability_d

#if gAspPowerLevelCapability_d
  /************************************************************************************
  * Sets the power level of the transceiver. The powerLevel parameter can be any value between 3 (lowest transmit level),
  * and 31 (highest transmit level). The relationship between the powerLevel parameter and the actual transmit power is not
  * perfectly linear (or exponential). If a power level greater than 31 was specified in the Set Power Level request primitive,
  * the returned value is gInvalidParameter_c, otherwise it is gSuccess_c.
  *
  * Interface assumptions:
  *   powerLevel
  *
  * Return value:
  *   Standard error code
  *
  ************************************************************************************/
  uint8_t Asp_SetPowerLevel(uint8_t powerLevel);

 /************************************************************************************
  * Gets the power level of the transceiver. The returned value can be any value between 3 (lowest transmit level),
  * and 31 (highest transmit level). The relationship between this value and the actual transmit power is not
  * perfectly linear (or exponential).
  *
  * Interface assumptions:
  *   none
  *
  * Return value:
  *   value of the transmit powerLevel
  *
  ************************************************************************************/
  uint8_t Asp_GetPowerLevel(void);
#endif // gAspPowerLevelCapability_d


#if gAspFADCapability_d
/************************************************************************************
* Enable/Disable the Fast Antenna Diversity function (FAD_EN bit).
*
* Interface assumptions:
*   state
*
* Return value:
*   Standard error code
*
************************************************************************************/
uint8_t Asp_SetFADState(bool_t state);

/************************************************************************************
* Fast Antenna Diversity state (FAD_EN bit).
*
* Interface assumptions:
*   None
*
* Return value:
*   Returns 1 if FAD function is enabled and 0 if disabled
*
************************************************************************************/
bool_t Asp_GetFADState(void);

/************************************************************************************
* Correlator threshold at which the FAD will select the antenna.
*
* Interface assumptions:
*   None
*
* Return value:
*   Standard error code
*
************************************************************************************/
uint8_t Asp_SetFADThreshold(uint8_t thresholdFAD);

/************************************************************************************
* Correlator threshold at which the FAD will select the antenna.
*
* Interface assumptions:
*   thresholdFAD
*
* Return value:
*   Current threshold value
*
************************************************************************************/
uint8_t Asp_GetFADThreshold(void);
#endif

/************************************************************************************
* Starting antenna in FAD mode or used antenna in non-FAD mode
*
* Interface assumptions:
*   state
*
* Return value:
*   Standard error code
*
************************************************************************************/
uint8_t Asp_SetANTXState(bool_t state);

/************************************************************************************
* Antenna selected in FAD or non-FAD mode.
*
* Interface assumptions:
*   None
*
* Return value:
*   Chosen antenna by the FAD (FAD_EN=1) or copy of ANTX_IN
*
************************************************************************************/
bool_t Asp_GetANTXState(void);

/************************************************************************************
* LQI mode.
*
* Interface assumptions:
*   None
*
* Return value:
*   Chose LQI Mode: 1 - LQI Based on RSSI / 0 - LQI Based on Correlation Peaks
*
************************************************************************************/
uint8_t Asp_SetLQIMode(uint8_t mode);

/************************************************************************************
* RSSI Level.
*
* Interface assumptions:
*   None
*
* Return value:
*   RSSI Level value, refreshed every 125us
************************************************************************************/
bool_t Asp_GetRSSILevel(void);

/************************************************************************************
* Enable/Disable the Active Promiscuous feature.
*
* Interface assumptions:
*   None
*
* Return value:
*   Standard error code
*
************************************************************************************/
uint8_t Asp_SetActivePromState(bool_t state);

/************************************************************************************
* Application layer service access point for ASP indications.
*
* Interface assumptions:
*   None
*
* Return value:
*   Standard error code
*
************************************************************************************/
#if gAspCapability_d
uint8_t ASP_APP_SapHandler
  (
  aspToAppMsg_t *pMsg
  );

#endif //gAspCapability_d

#ifdef __cplusplus
}
#endif

#endif /* _APP_ASP_INTERFACE_H_ */
