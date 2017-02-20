/*! @file	ZclSEMetering.c
 *
 * @brief	This source file describes specific functionality implemented
 *			for the Metering cluster.
 *
 * @copyright Copyright(c) 2013, Freescale, Inc. All rights reserved.
 *
 * @license	Redistribution and use in source and binary forms, with or without modification,
 *			are permitted provided that the following conditions are met:
 *
 *			o Redistributions of source code must retain the above copyright notice, this list
 *			of conditions and the following disclaimer.
 *
 *			o Redistributions in binary form must reproduce the above copyright notice, this
 *   		list of conditions and the following disclaimer in the documentation and/or
 *   		other materials provided with the distribution.
 *
 * 			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   		contributors may be used to endorse or promote products derived from this
 *   		software without specific prior written permission.
 *
 * 			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * 			ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * 			WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 			DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * 			ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * 			(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * 			LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * 			ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * 			(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * 			SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "EmbeddedTypes.h"
#include "zigbee.h"

#include "ZclSEMetering.h"
#include "SEProfile.h"
#include "AppAfInterface.h"
#include "EndpointConfig.h"
#include "ASL_ZdpInterface.h"
#include "zcl.h"
#include "zclSE.h"
#include "beeapp.h"
#include "ZclSEPrepayment.h"
#include "ZdoApsInterface.h"
#ifdef gHostApp_d  
#include "ZtcHandler.h"  
#endif

static Consmp consumptionValue;
static Summ_t LastRISCurrSummDlvrd;
static uint8_t ProfIntrvHead;
static uint8_t ProfIntrvTail;
static zclGetProfEntry_t ProfileIntervalTable[gMaxNumberOfPeriodsDelivered_c];
tmrTimerID_t gUpdatePowerConsumptionTimerID;

#if gASL_ZclSE_12_Features_d
static zclMet_SendSnapshot_t gSnapshotToSend;
#endif

static zbStatus_t ZCL_SmplMet_ProcessReqFastPollEvt(zbApsdeDataIndication_t *pIndication);
#if gZclFastPollMode_d
static void FastPollTimerCallBack(tmrTimerID_t tmrID);
#endif
void GetProfileTestTimerCallBack(tmrTimerID_t tmrID);
static Consmp *CalculateConsumptionFrom6ByteArray(uint8_t *pNew, uint8_t *pOld);
static void Add8ByteTo6ByteArray(uint8_t value, uint8_t *pSumm_t);
static void UpdatePowerConsumptionTimerCallBack(tmrTimerID_t tmrID);
static uint16_t ZCL_BuildGetProfResponse(uint8_t IntrvChannel, ZCLTime_t EndTime, uint8_t NumberOfPeriods, afToApsdeMessage_t *pMsg);
#if gASL_ZclSE_12_Features_d
static zbStatus_t ZclSmplMet_ProcessGetSnapshotReq(zbApsdeDataIndication_t *pIndication);
static void ZclMet_SetPayloadSnapshot(zclMet_SnapshotsTable_t *pSnapshot);
static void SendGetSnapshotResponse(index_t snapshotIdx);
#endif

static ZCLTime_t mcFastPollEndTime = 0;
static uint16_t mcFastPollRemainingTime = 0;
#if gZclFastPollMode_d
static uint16_t mcTimeInFastPollMode = 0;
#endif
tmrTimerID_t gFastPollTimerID;
bool_t gAcceptFastPoll = TRUE;
tmrTimerID_t gGetProfileTestTimerID;

tmrTimerID_t gMirrorReportAttrRspTimerID;
afAddrInfo_t gMirrorAddrInfo; 
uint8_t gMirrorTransactionID;

#if gNum_MirrorEndpoints_c
zclMet_MirroringTable_t gMirroringTable[gNum_MirrorEndpoints_c];
#endif

zclMet_CachedCmdsTable_t gCacheCmdsTable[gNum_CachedCommands_c];
zclMet_SnapshotsTable_t gZclMetSnapshotsTable[gNum_Snapshots_c];

extern zbEndPoint_t appEndPoint;

extern zclMetReadInfoSetAttr_t         gZclMetReadInfoSetAttr;
#if gASL_ZclSE_TiersNumber_d > 0
extern zclMetTOUInfoSetAttr_t          gZclMetTOUInfoSetAttr;
#endif
extern zclMetStatusSetAttr_t           gZclMetStatusSetAttr;
extern zclMetFormattingSetAttr_t       gZclMetFormattingSetAttr;
extern zclMetESPHistoricalSetAttr_t    gZclMetESPHistoricalSetAttr;
extern zclMetLdProfileConfigSetAttr_t  gZclMetLdProfileConfigSetAttr;
extern zclMetAlarmSetAttr_t            gZclMetAlarmsSetAttr;
extern zclMetSupplyLimitSetAttr_t      gZclMetSupplyLimitSetAttr;
extern zclMetBlockInfoDlvrdSetAttr_t   gZclMetBlockInfoDlvrdSetAttr;
extern zclMetBlockInfoRcvdSetAttr_t    gZclMetBlockInfoRcvdSetAttr;
extern zclMetBillingSetAttr_t          gZclMetBillingSetAttr;

const zclAttrDef_t gaZclMetReadInfoSetAttrDef[] = {
  {gZclAttrIdMetRISCurrSummDlvrd_c, gZclDataTypeUint48_c, gZclAttrFlagsRdOnly_c | gZclAttrFlagsReportable_c | gZclAttrFlagsAsynchronous_c, sizeof(Summ_t), (void *) &gZclMetReadInfoSetAttr.RISCurrSummDlvrd},
#if gASL_ZclMet_Optionals_d
  {gZclAttrIdMetRISCurrSummRcvd_c,                          gZclDataTypeUint48_c,gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) &gZclMetReadInfoSetAttr.RISCurrSummRcvd},
  {gZclAttrIdMetRISCurrMaxDmndDlvrd_c,                      gZclDataTypeUint48_c,gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) &gZclMetReadInfoSetAttr.RISCurrMaxDmndDlvrd},
  {gZclAttrIdMetRISCurrMaxDmndRcvd_c,                       gZclDataTypeUint48_c,gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) &gZclMetReadInfoSetAttr.RISCurrMaxDmndRcvd},
  {gZclAttrIdMetRISDFTSumm_c,                               gZclDataTypeUint48_c,gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) &gZclMetReadInfoSetAttr.RISDFTSumm},
  {gZclAttrIdMetRISDailyFreezeTime_c,                       gZclDataTypeUint16_c,gZclAttrFlagsRdOnly_c,sizeof(uint16_t),(void *) &gZclMetReadInfoSetAttr.RISDailyFreezeTime},
  {gZclAttrIdMetRISPowerFactor_c,                           gZclDataTypeInt8_c,  gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) &gZclMetReadInfoSetAttr.RISPowerFactor},
  {gZclAttrIdMetRISReadingSnapShotTime_c,                   gZclDataTypeUTCTime_c, gZclAttrFlagsRdOnly_c,sizeof(ZCLTime_t),(void *) &gZclMetReadInfoSetAttr.RISReadingSnapShotTime},
  {gZclAttrIdMetRISCurrMaxDmndDlvrdTimer_c,                 gZclDataTypeUTCTime_c, gZclAttrFlagsRdOnly_c,sizeof(ZCLTime_t),(void *) &gZclMetReadInfoSetAttr.RISCurrMaxDmndDlvrdTimer},
  {gZclAttrIdMetRISCurrMaxDmndRcvdTime_c,                   gZclDataTypeUTCTime_c, gZclAttrFlagsRdOnly_c,sizeof(ZCLTime_t),(void *) &gZclMetReadInfoSetAttr.RISCurrMaxDmndRcvdTime},
  {gZclAttrIdMetRISDefaultUpdatePeriod_c,                   gZclDataTypeUint8_c, gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) &gZclMetReadInfoSetAttr.RISDefaultUpdatePeriod},
  {gZclAttrIdMetRISFastPollUpdatePeriod_c,                  gZclDataTypeUint8_c, gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) &gZclMetReadInfoSetAttr.RISFastPollUpdatePeriod},
  {gZclAttrIdMetRISCurrBlockPeriodConsumpDlvrd_c,           gZclDataTypeUint48_c, gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) &gZclMetReadInfoSetAttr.RISCurrBlockPeriodConsumptionDelivered},
  {gZclAttrIdMetRISDailyConsumptionTarget_c,                gZclDataTypeUint24_c, gZclAttrFlagsRdOnly_c,sizeof(Consmp),(void *) &gZclMetReadInfoSetAttr.RISDailyConsumptionTarget},
  {gZclAttrIdMetRISCurrBlock_c,                             gZclDataTypeEnum8_c, gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) &gZclMetReadInfoSetAttr.RISCurrBlock},
  {gZclAttrIdMetRISProfileIntPeriod_c,                      gZclDataTypeEnum8_c, gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) &gZclMetReadInfoSetAttr.RISProfileIntPeriod},
  {gZclAttrIdMetRISIntReadReportingPeriod_c,                gZclDataTypeUint16_c, gZclAttrFlagsRdOnly_c,sizeof(uint16_t),(void *) &gZclMetReadInfoSetAttr.RISIntReadReportingPeriod},
  {gZclAttrIdMetRISPresetReadingTime_c,                     gZclDataTypeUint16_c, gZclAttrFlagsRdOnly_c,sizeof(uint16_t),(void *) &gZclMetReadInfoSetAttr.RISPresetReadingTime},
  {gZclAttrIdMetRISVolumePerReport_c,                       gZclDataTypeUint16_c, gZclAttrFlagsRdOnly_c,sizeof(uint16_t),(void *) &gZclMetReadInfoSetAttr.RISVolumePerReport},
  {gZclAttrIdMetRISFlowRestriction_c,                       gZclDataTypeUint8_c, gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) &gZclMetReadInfoSetAttr.RISFlowRestrictio},
  {gZclAttrIdMetRISSupplyStatus_c,                          gZclDataTypeEnum8_c, gZclAttrFlagsRdOnly_c,sizeof(uint8_t),(void *) &gZclMetReadInfoSetAttr.RISSupplyStatus},
  {gZclAttrIdMetRISCurrInletEnergyCarrierSummation_c,       gZclDataTypeUint48_c, gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) &gZclMetReadInfoSetAttr.RISCurrInletEnergyCarrierSummation},
  {gZclAttrIdMetRISCurrOutletEnergyCarrierSummation_c,      gZclDataTypeUint48_c, gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) &gZclMetReadInfoSetAttr.RISCurrOutletEnergyCarrierSummation},
  {gZclAttrIdMetRISInletTemp_c,                              gZclDataTypeInt16_c,      gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) &gZclMetReadInfoSetAttr.RISInletTemp},
  {gZclAttrIdMetRISOutletTemp_c,                             gZclDataTypeInt16_c,     gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) &gZclMetReadInfoSetAttr.RISOutletTemp},
  {gZclAttrIdMetRISCtrlTemp_c,                              gZclDataTypeInt16_c,       gZclAttrFlagsRdOnly_c,sizeof(int16_t),(void *) &gZclMetReadInfoSetAttr.RISCtrlTemp},
  {gZclAttrIdMetRISCurrInletEnergyCarrierDemand_c,          gZclDataTypeInt24_c, gZclAttrFlagsRdOnly_c,sizeof(Consmp),(void *) &gZclMetReadInfoSetAttr.RISCurrInletEnergyCarrierDemand},
  {gZclAttrIdMetRISCurrOutletEnergyCarrierDemand_c,         gZclDataTypeInt24_c, gZclAttrFlagsRdOnly_c,sizeof(Consmp),(void *) &gZclMetReadInfoSetAttr.RISCurrOutletEnergyCarrierDemand},
  {gZclAttrIdMetRISPrevBlockPeriodConsumptionDlvrd_c,       gZclDataTypeUint48_c, gZclAttrFlagsRdOnly_c,sizeof(Summ_t),(void *) &gZclMetReadInfoSetAttr.RISPrevBlockPeriodConsumptionDlvrd},
#endif
};
                                                                                                                                  
#if gASL_ZclMet_Optionals_d & (gASL_ZclSE_TiersNumber_d > 0)
const zclAttrDef_t gaZclMetTOUInfoSetAttrDef[] = {
  {gZclAttrIdMetTOUCurrTier1SummDlvrd_c, gZclDataTypeUint48_c,gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsVector_c , 2 * (gASL_ZclSE_TiersNumber_d + gASL_ZclSE_ExtendedPriceTiersNumber_d) - 1, (void *) &gZclMetTOUInfoSetAttr}
#if gASL_ZclSE_12_Features_d  
  ,{gZclAttrIdMetTOUCPP1SummDlvrd_c, gZclDataTypeUint48_c,gZclAttrFlagsRdOnly_c , sizeof(Summ_t), (void *) &gZclMetTOUInfoSetAttr.TOUCurrTierSumm[2 * gASL_ZclSE_TiersNumber_d]},
  {gZclAttrIdMetTOUCPP2SummDlvrd_c, gZclDataTypeUint48_c,gZclAttrFlagsRdOnly_c , sizeof(Summ_t), (void *) &gZclMetTOUInfoSetAttr.TOUCurrTierSumm[2 * gASL_ZclSE_TiersNumber_d + 1]}
#endif
};
#endif
                                                                                                                                  
/* Mandatory Meter Status Set */                                                                                                                             
const zclAttrDef_t gaZclMetStatusSetAttrDef[] = {
  {gZclAttrIdMetMSStatus_c,                 gZclDataTypeBitmap8_c,  gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetStatusSetAttr.MSStatus },
#if gASL_ZclMet_Optionals_d 
  {gZclAttrIdMetMSSRemainingBatteryLife_c,  gZclDataTypeUint8_c,    gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetStatusSetAttr.RemainingBatteryLife },
  {gZclAttrIdMetMSSHoursInOperation_c,      gZclDataTypeUint24_c,   gZclAttrFlagsRdOnly_c, sizeof(Consmp),  (void *) &gZclMetStatusSetAttr.HoursInOperation },
  {gZclAttrIdMetMSSHoursInFault_c,          gZclDataTypeUint24_c,   gZclAttrFlagsRdOnly_c, sizeof(Consmp),  (void *) &gZclMetStatusSetAttr.HoursInFault },
#endif
};

/* Mandatory Consmp Formating Set */
const zclAttrDef_t gaZclMetConsmpFormatSetAttrDef[] = {
  {gZclAttrIdMetCFSUnitofMeasure_c,          gZclDataTypeEnum8_c,  gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetFormattingSetAttr.SmplMetCFSUnitofMeasure },
#if gASL_ZclMet_Optionals_d  
  {gZclAttrIdMetCFSMultiplier_c,              gZclDataTypeUint24_c,gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *) &gZclMetFormattingSetAttr.Mult },
  {gZclAttrIdMetCFSDivisor_c,                 gZclDataTypeUint24_c,gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *) &gZclMetFormattingSetAttr.Div },
#endif  
  {gZclAttrIdMetCFSSummFormat_c,              gZclDataTypeBitmap8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetFormattingSetAttr.CFSSummFormat },
#if gASL_ZclMet_Optionals_d    
  {gZclAttrIdMetCFSDmndFormat_c,              gZclDataTypeBitmap8_c,gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetFormattingSetAttr.DmndFormat },
  {gZclAttrIdMetCFSHistoricalConsmpFormat_c,  gZclDataTypeBitmap8_c,gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetFormattingSetAttr.HistoricalConsmpFormat },
#endif  
  {gZclAttrIdMetCFSMetDevType_c,              gZclDataTypeBitmap8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetFormattingSetAttr.CFSMetDevType },
#if gASL_ZclMet_Optionals_d
  {gZclAttrIdMetCFSSiteID_c,                            gZclDataTypeOctetStr_c, gZclAttrFlagsRdOnly_c, sizeof(zclStr32Oct_t), (void *) &gZclMetFormattingSetAttr.CFSSiteID },
  {gZclAttrIdMetCFSMeterSerialNumber_c,                 gZclDataTypeOctetStr_c, gZclAttrFlagsRdOnly_c, sizeof(zclStr24Oct_t), (void *) &gZclMetFormattingSetAttr.CFSMeterSerialNumber },
  {gZclAttrIdMetCFSEnergyCarrierUnitOfMeasure_c,        gZclDataTypeEnum8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetFormattingSetAttr.CFSEnergyCarrierUnitOfMeasure },  
  {gZclAttrIdMetCFSEnergyCarrierSummationFormatting_c,  gZclDataTypeBitmap8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetFormattingSetAttr.CFSEnergyCarrierSummationFormatting },   
  {gZclAttrIdMetCFSEnergyCarrierDemandFormatting_c,     gZclDataTypeBitmap8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetFormattingSetAttr.CFSEnergyCarrierDemandFormatting },   
  {gZclAttrIdMetCFSTempUnitOfMeasure_c,                 gZclDataTypeEnum8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetFormattingSetAttr.CFSTempUnitOfMeasure }, 
  {gZclAttrIdMetCFSTempFormatting_c,                    gZclDataTypeBitmap8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetFormattingSetAttr.CFSTempFormatting },
#endif  
};

 /*Attributes of the Simple Metering Meter ESP Historical attribute set */  
const zclAttrDef_t gaZclMetESPHistoricalConsmpSetAttrDef[] = {
  {gZclAttrIdMetEHCInstantaneousDmnd_c,               gZclDataTypeInt24_c, gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *)  &gZclMetESPHistoricalSetAttr.InstantaneousDmnd},
  {gZclAttrIdMetEHCCurrDayConsmpDlvrd_c,              gZclDataTypeUint24_c,gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *)  &gZclMetESPHistoricalSetAttr.CurrDayConsmpDlvrd},
  {gZclAttrIdMetEHCCurrDayConsmpRcvd_c,               gZclDataTypeUint24_c,gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *)  &gZclMetESPHistoricalSetAttr.CurrDayConsmpRcvd},
  {gZclAttrIdMetEHCPreviousDayConsmpDlvrd_c,          gZclDataTypeUint24_c,gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *)  &gZclMetESPHistoricalSetAttr.PreviousDayConsmpDlvrd},
  {gZclAttrIdMetEHCPreviousDayConsmpRcvd_c,           gZclDataTypeUint24_c,gZclAttrFlagsRdOnly_c, sizeof(Consmp),(void *)  &gZclMetESPHistoricalSetAttr.PreviousDayConsmpRcvd},
  {gZclAttrIdMetEHCCurrPrtlProfIntrvStartTimeDlvrd_c, gZclDataTypeUTCTime_c, gZclAttrFlagsRdOnly_c, sizeof(ZCLTime_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrPartialProfileIntrvStartTimeDlvrd},
  {gZclAttrIdMetEHCCurrPrtlProfIntrvStartTimeRcvd_c,  gZclDataTypeUTCTime_c, gZclAttrFlagsRdOnly_c, sizeof(ZCLTime_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrPartialProfileIntrvStartTimeRcvd},
  {gZclAttrIdMetEHCCurrPrtlProfIntrvValueDlvrd_c,     gZclDataTypeUint24_c,gZclAttrFlagsRdOnly_c, sizeof(IntrvForProfiling_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrPartialProfileIntrvValueDlvrd},
  {gZclAttrIdMetEHCCurrPrtlProfIntrvValueRcvd_c,      gZclDataTypeUint24_c,gZclAttrFlagsRdOnly_c, sizeof(IntrvForProfiling_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrPartialProfileIntrvValueRcvd},
  {gZclAttrIdMetEHCCurrDayMaxPressure_c,              gZclDataTypeUint48_c,gZclAttrFlagsRdOnly_c, sizeof(Pressure_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrDayMaxPressure},
  {gZclAttrIdMetEHCCurrDayMinPressure_c,              gZclDataTypeUint48_c,gZclAttrFlagsRdOnly_c, sizeof(Pressure_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrDayMinPressure},
  {gZclAttrIdMetEHCPrevDayMaxPressure_c,              gZclDataTypeUint48_c,gZclAttrFlagsRdOnly_c, sizeof(Pressure_t),(void *)  &gZclMetESPHistoricalSetAttr.PrevDayMaxPressure},
  {gZclAttrIdMetEHCPrevDayMinPressure_c,              gZclDataTypeUint48_c,gZclAttrFlagsRdOnly_c, sizeof(Pressure_t),(void *)  &gZclMetESPHistoricalSetAttr.PrevDayMinPressure},
  {gZclAttrIdMetEHCCurrDayMaxDemand_c,                gZclDataTypeInt24_c,gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrDayMaxDemand},
  {gZclAttrIdMetEHCPrevDayMaxDemand_c,                gZclDataTypeInt24_c,gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *)  &gZclMetESPHistoricalSetAttr.PrevDayMaxDemand},
  {gZclAttrIdMetEHCCurrMonthMaxDemand_c,              gZclDataTypeInt24_c,gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrMonthMaxDemand},
  {gZclAttrIdMetEHCCurrYearMaxDemand_c,               gZclDataTypeInt24_c,gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrYearMaxDemand},
  {gZclAttrIdMetEHCCurrDayMaxEnergyCarrierDemand_c,   gZclDataTypeInt24_c,gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrDayMaxEnergyCarrierDemand},
  {gZclAttrIdMetEHCPrevDayMaxEnergyCarrierDemand_c,   gZclDataTypeInt24_c,gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *)  &gZclMetESPHistoricalSetAttr.PrevDayMaxEnergyCarrierDemand},
  {gZclAttrIdMetEHCCurrMonthMaxEnergyCarrierDemand_c, gZclDataTypeInt24_c,gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrMonthMaxEnergyCarrierDemand},
  {gZclAttrIdMetEHCCurrMonthMinEnergyCarrierDemand_c, gZclDataTypeInt24_c,gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrMonthMinEnergyCarrierDemand},
  {gZclAttrIdMetEHCCurrYearMaxEnergyCarrierDemand_c,  gZclDataTypeInt24_c,gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrYearMaxEnergyCarrierDemand},
  {gZclAttrIdMetEHCCurrYearMinEnergyCarrierDemand_c,  gZclDataTypeInt24_c,gZclAttrFlagsRdOnly_c, sizeof(Demmand_t),(void *)  &gZclMetESPHistoricalSetAttr.CurrYearMinEnergyCarrierDemand},
};

/*Attributes of the Simple Metering Meter Load Profile Configuration attribute set */  
const zclAttrDef_t gaZclMetLdProfConfigSetAttrDef[] = {
  {gZclAttrIdMetLPCMaxNumOfPeriodsDlvrd_c,      gZclDataTypeUint8_c,gZclAttrFlagsRdOnly_c, sizeof(uint8_t),(void *)  &gZclMetLdProfileConfigSetAttr}  
};

/*Attributes of the Simple Metering Alarm attribute set */ 
const zclAttrDef_t gaZclMetAlarmsSetAttrDef[] = {
  {gZclAttrIdMetASGenericAlarmMask_c,                 gZclDataTypeBitmap16_c, gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) &gZclMetAlarmsSetAttr.ASGenericAlarmMask },   
  {gZclAttrIdMetASElectricityAlarmMask_c,             gZclDataTypeBitmap16_c, gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) &gZclMetAlarmsSetAttr.ASElectricityAlarmMask },     
  {gZclAttrIdMetASGenericFlowPressureAlarmMask_c,     gZclDataTypeBitmap16_c, gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) &gZclMetAlarmsSetAttr.ASGenericFlowPressureAlarmMask },     
  {gZclAttrIdMetASWaterSpecificAlarmMask_c,           gZclDataTypeBitmap16_c, gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) &gZclMetAlarmsSetAttr.ASWaterSpecificAlarmMask },     
  {gZclAttrIdMetASHeatAndCoolingSpecificAlarmMask_c,  gZclDataTypeBitmap16_c, gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) &gZclMetAlarmsSetAttr.ASHeatAndCoolingSpecificAlarmMask },     
  {gZclAttrIdMetASGasSpecificAlarmMask_c,             gZclDataTypeBitmap16_c, gZclAttrFlagsRdOnly_c, sizeof(uint16_t), (void *) &gZclMetAlarmsSetAttr.ASGasSpecificAlarmMask },
};

/*Attributes of the Simple Metering Supply Limit Attribute Set */
const zclAttrDef_t gaZclMetSupplyLimitSetAttrDef[] = {
  {gZclAttrIdMetSLSCurrDmndDlvrd_c,                 gZclDataTypeUint24_c, gZclAttrFlagsRdOnly_c, sizeof(Consmp), (void *) &gZclMetSupplyLimitSetAttr.CurrDmndDlvrd },   
  {gZclAttrIdMetSLSDmndLimit_c,                     gZclDataTypeUint24_c, gZclAttrFlagsRdOnly_c, sizeof(Consmp), (void *) &gZclMetSupplyLimitSetAttr.DmndLimit },     
  {gZclAttrIdMetSLSDmndIntegrationPeriod_c,         gZclDataTypeUint8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetSupplyLimitSetAttr.DmndIntegrationPeriod },     
  {gZclAttrIdMetSLSNumOfDmndSubIntrvs_c,            gZclDataTypeUint8_c, gZclAttrFlagsRdOnly_c, sizeof(uint8_t), (void *) &gZclMetSupplyLimitSetAttr.NumOfDmndSubIntrvs },
};

 /*Attributes of the Simple Metering Block Information Delivered Attribute Set */
const zclAttrDef_t gaZclMetBlockInfoDlvrdSetAttrDef[] = {
  {gZclAttrIdMetBISCurrNoTierBlock1SummationDelivered_c,   gZclDataTypeUint48_c,  gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsVector_c, (gASL_ZclSE_TiersNumber_d +1) * gASL_ZclSE_BlocksNumber_d - 1 , (void *) &gZclMetBlockInfoDlvrdSetAttr},
};


 /*Attributes of the Simple Metering Block Information Received Attribute Set */
const zclAttrDef_t gaZclMetBlockInfoRcvdSetAttrDef[] = {
  {gZclAttrIdMetBISCurrNoTierBlock1SummationReceived_c,   gZclDataTypeUint48_c,  gZclAttrFlagsRdOnly_c|gZclAttrFlagsIsVector_c, (gASL_ZclSE_TiersNumber_d +1) * gASL_ZclSE_BlocksNumber_d - 1, (void *) &gZclMetBlockInfoRcvdSetAttr},
};

  /*Attributes of the Simple Metering Billing Attribute Set */
const zclAttrDef_t gaZclMettBillingSetAttrDef[] = {
  {gZclAttrIdMetBillingBillToDate_c,              gZclDataTypeUint32_c,  gZclAttrFlagsRdOnly_c, sizeof(Bill_t), (void *) &gZclMetBillingSetAttr.BillToDate},
  {gZclAttrIdMetBillingBillToDateTimeStamp_c,     gZclDataTypeUTCTime_c,  gZclAttrFlagsRdOnly_c, sizeof(Bill_t), (void *) &gZclMetBillingSetAttr.BillToDateTimeStamp},
  {gZclAttrIdMetBillingProjectedBill_c,           gZclDataTypeUint32_c,  gZclAttrFlagsRdOnly_c, sizeof(Bill_t), (void *) &gZclMetBillingSetAttr.ProjectedBill},
  {gZclAttrIdMetBillingProjectedBillTimeStamp_c,  gZclDataTypeUTCTime_c,  gZclAttrFlagsRdOnly_c, sizeof(Bill_t), (void *) &gZclMetBillingSetAttr.ProjectedBillTimeStamp},
};

const zclAttrSet_t gaZclMetServerAttrSet[] = {
  {gZclAttrMetReadInfoSet_c,              (void *)&gaZclMetReadInfoSetAttrDef,            NumberOfElements(gaZclMetReadInfoSetAttrDef)},
#if gASL_ZclMet_Optionals_d & (gASL_ZclSE_TiersNumber_d > 0)  
  {gZclAttrMetTOUInfoSet_c,               (void *)&gaZclMetTOUInfoSetAttrDef,             NumberOfElements(gaZclMetTOUInfoSetAttrDef)},
#endif  
  {gZclAttrMetMeterStatusSet_c,           (void *)&gaZclMetStatusSetAttrDef,              NumberOfElements(gaZclMetStatusSetAttrDef)},
  {gZclAttrMetConsmpFormatSet_c,          (void *)&gaZclMetConsmpFormatSetAttrDef,        NumberOfElements(gaZclMetConsmpFormatSetAttrDef)},
  {gZclAttrMetESPHistoricalConsmpSet_c,   (void *)&gaZclMetESPHistoricalConsmpSetAttrDef, NumberOfElements(gaZclMetESPHistoricalConsmpSetAttrDef)},
  {gZclAttrMetLdProfConfigSet_c,          (void *)&gaZclMetLdProfConfigSetAttrDef,        NumberOfElements(gaZclMetLdProfConfigSetAttrDef)},
  {gZclAttrMetSupplyLimitSet_c,           (void *)&gaZclMetSupplyLimitSetAttrDef,         NumberOfElements(gaZclMetSupplyLimitSetAttrDef)},
  {gZclAttrMetBlockInfoDlvrdSet_c,        (void *)&gaZclMetBlockInfoDlvrdSetAttrDef,      NumberOfElements(gaZclMetBlockInfoDlvrdSetAttrDef)},
  {gZclAttrMetAlarmsSet_c,                (void *)&gaZclMetAlarmsSetAttrDef,              NumberOfElements(gaZclMetAlarmsSetAttrDef)},
#if gASL_ZclSE_12_Features_d  
  {gZclAttrMetBlockInfoRcvdSet_c,         (void *)&gaZclMetBlockInfoRcvdSetAttrDef,       NumberOfElements(gaZclMetBlockInfoRcvdSetAttrDef)},
  {gZclAttrMetBillingSet_c,               (void *)&gaZclMettBillingSetAttrDef,            NumberOfElements(gaZclMettBillingSetAttrDef)},
#endif  
};

const zclAttrSetList_t gZclMetServerAttrSetList = {
  NumberOfElements(gaZclMetServerAttrSet),
  gaZclMetServerAttrSet
};

/* Attributes for the Meter Mirror*/

seESIMirrorMeteringData_t gZclMirrorMeterAttr;

const zclAttrDef_t gaZclMetMirrorReadInfoSetAttrDef[] = {
  {gZclAttrIdMetRISCurrSummDlvrd_c,        gZclDataTypeUint48_c,   gZclAttrFlagsRdOnly_c, sizeof(Summ_t),    (void *)&gZclMirrorMeterAttr.RISCurrSummDlvrd}
};

const zclAttrDef_t gaZclMetMirrorStatusSetAttrDef[] = {
  {gZclAttrIdMetMSStatus_c,                gZclDataTypeBitmap8_c,  gZclAttrFlagsRdOnly_c, sizeof(uint8_t),   (void *)&gZclMirrorMeterAttr.MSStatus}
};

const zclAttrDef_t gaZclMetMirrorConsmpFormatSetAttrDef[] = {
  {gZclAttrIdMetCFSUnitofMeasure_c,        gZclDataTypeEnum8_c,    gZclAttrFlagsRdOnly_c, sizeof(uint8_t),   (void *)&gZclMirrorMeterAttr.SmplMetCFSUnitofMeasure},
  {gZclAttrIdMetCFSSummFormat_c,           gZclDataTypeBitmap8_c,  gZclAttrFlagsRdOnly_c, sizeof(uint8_t),   (void *)&gZclMirrorMeterAttr.CFSSummFormat},    
  {gZclAttrIdMetCFSMetDevType_c,           gZclDataTypeBitmap8_c,  gZclAttrFlagsRdOnly_c, sizeof(uint8_t),   (void *)&gZclMirrorMeterAttr.CFSMetDevType},   
};

/* Attributes for the Meter Client */
extern zclMetNotifSetAttr_t    gZclMetNotifSetAttr;
Profile_t               gZclMetChangeReportProfile;

/*Attributes of the Metering Notification Set */
const zclAttrDef_t gaZclMetNotificationSetAttrDef[] = {
  {gZclAttrIdMetNotifCtlFlags_c,                gZclDataTypeBitmap8_c,  gZclAttrFlagsIsClientAttribute_c, sizeof(uint8_t), (void *) &gZclMetNotifSetAttr.NotifCtlFlags},
  {gZclAttrIdMetNotifFlags_c,                   gZclDataTypeBitmap8_c,  gZclAttrFlagsRdOnly_c | gZclAttrFlagsIsClientAttribute_c,  sizeof(uint8_t), (void *) &gZclMetNotifSetAttr.NotifFlags},
  {gZclAttrIdMetPriceNotifCtlFlags_c,           gZclDataTypeBitmap16_c, gZclAttrFlagsRdOnly_c | gZclAttrFlagsIsClientAttribute_c,  sizeof(uint16_t), (void *) &gZclMetNotifSetAttr.PriceNotifFlags},
  {gZclAttrIdMetCalendarNotifCtlFlags_c,        gZclDataTypeBitmap8_c,  gZclAttrFlagsRdOnly_c | gZclAttrFlagsIsClientAttribute_c,  sizeof(uint8_t), (void *) &gZclMetNotifSetAttr.CalendarNotifFlags},
  {gZclAttrIdMetPrePayNotifCtlFlags_c,          gZclDataTypeBitmap16_c, gZclAttrFlagsRdOnly_c | gZclAttrFlagsIsClientAttribute_c,  sizeof(uint16_t), (void *) &gZclMetNotifSetAttr.PrePayNotifFlags},
  {gZclAttrIdMetDevMgmtNotifCtlFlags_c,         gZclDataTypeBitmap8_c,  gZclAttrFlagsRdOnly_c | gZclAttrFlagsIsClientAttribute_c,  sizeof(uint8_t), (void *) &gZclMetNotifSetAttr.DevMgmtNotifFlags}
};

const zclAttrDef_t gaZclMetMirrorCfgSetAttrDef[] = {
  {gZclAttrIdMetChangeReportingProfile_c,       gZclDataTypeUint24_c, gZclAttrFlagsIsClientAttribute_c, sizeof(Profile_t), (void *) &gZclMetChangeReportProfile}
};

const zclAttrSet_t gaZclMetMirrorAttrSet[] = {
  {gZclAttrMetNotificationSet_c,          (void *)&gaZclMetNotificationSetAttrDef,     NumberOfElements(gaZclMetNotificationSetAttrDef)},
  {gZclAttrMetMirrorCfgSet_c,             (void *)&gaZclMetMirrorCfgSetAttrDef,        NumberOfElements(gaZclMetMirrorCfgSetAttrDef)},
  {gZclAttrMetReadInfoSet_c,              (void *)&gaZclMetMirrorReadInfoSetAttrDef,            NumberOfElements(gaZclMetMirrorReadInfoSetAttrDef)},
  {gZclAttrMetMeterStatusSet_c,           (void *)&gaZclMetMirrorStatusSetAttrDef,              NumberOfElements(gaZclMetMirrorStatusSetAttrDef)},
  {gZclAttrMetConsmpFormatSet_c,          (void *)&gaZclMetMirrorConsmpFormatSetAttrDef,        NumberOfElements(gaZclMetMirrorConsmpFormatSetAttrDef)}
};

const zclAttrSetList_t gZclMetMirrorAttrSetList = {
  NumberOfElements(gaZclMetMirrorAttrSet),
  gaZclMetMirrorAttrSet
};

/*!
 * @fn 		void ZCL_SmplMet_MeterInit(void)
 *
 * @brief	Initializes the Metering Server functionality.
 *
 */
void ZCL_SmplMet_MeterInit(void)
{
  /*A timer is used for demo purpose to generate consumption data.*/
  gGetProfileTestTimerID = TMR_AllocateTimer();
  gUpdatePowerConsumptionTimerID = TMR_AllocateTimer();
  TMR_StartSecondTimer(gUpdatePowerConsumptionTimerID, gUpdateConsumption_c, UpdatePowerConsumptionTimerCallBack);
#if(gProfIntrvPeriod_c < 5)
  TMR_StartMinuteTimer(gGetProfileTestTimerID, gTimerValue_c, GetProfileTestTimerCallBack);
#else
  TMR_StartSecondTimer(gGetProfileTestTimerID, gTimerValue_c, GetProfileTestTimerCallBack);
#endif
    
}

/*!
 * @fn 		zbStatus_t ZclSmplMet_ClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Metering Cluster server. 
 *
 */
zbStatus_t ZclSmplMet_ClusterServer
(
	zbApsdeDataIndication_t *pIndication, /* IN: */
	afDeviceDef_t *pDev                /* IN: */
) 
{
  zclCmd_t Cmd;
  zclFrame_t *pFrame;
  zbStatus_t status = gZclSuccessDefaultRsp_c;

  (void) pDev;
  pFrame = (void *)pIndication->pAsdu;
  
   /* Check if we need to send Default Response */
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
    status = gZclSuccess_c;
  
  Cmd = ((zclFrame_t *)pIndication->pAsdu)->command;
  
  switch(Cmd) {
    case gZclCmdSmplMet_GetProfReq_c: 
    {
      uint16_t iPayloadLen = 0;
      afToApsdeMessage_t *pMsg;
      zclCmdSmplMet_GetProfReq_t *pCmdFrame;
      afAddrInfo_t addrInfo;
      
      /* prepare for response in the address info (back to sender) */
      AF_PrepareForReply(&addrInfo, pIndication);
      
      pCmdFrame = (void *)(pFrame + 1);
	        
      pMsg = ZCL_CreateFrame( &addrInfo, 
                          gZclCmdSmplMet_GetProfRsp_c,
                          gZclFrameControl_FrameTypeSpecific|gZclFrameControl_DirectionRsp|gZclFrameControl_DisableDefaultRsp, 
                          &pFrame->transactionId, 
                          (uint8_t*)&iPayloadLen,
                          NULL);
      if(!pMsg)
        return gZclNoMem_c;
      
      iPayloadLen = ZCL_BuildGetProfResponse(pCmdFrame->IntrvChannel, pCmdFrame->EndTime, pCmdFrame->NumOfPeriods, pMsg);

#if gFragmentationCapability_d        
      /* Check if fragmentation is required*/
      if(iPayloadLen > AF_MaxPayloadLen(&addrInfo))
      {
        addrInfo.txOptions |= gApsTxOptionFragmentationRequested_c;
        

      }
#endif
      /* Send response OTA */
      status = ZCL_DataRequestNoCopy(&addrInfo, iPayloadLen + sizeof(zclFrame_t), pMsg);
    }
    break;  
#if gZclMirroring_d   
    
  case gZclCmdSmplMet_RequestMirrorRsp_c:
    {
      zclCmdMet_RequestMirrorRsp_t *pCmdFrame = (void *)(pIndication->pAsdu + sizeof(zclFrame_t));
#if gZclEnableReporting_c      
      if (pCmdFrame->EndPointID != 0xFFFF)    
        ZclMet_ConfigureReportingToMirror(pIndication, (uint8_t)OTA2Native16(pCmdFrame->EndPointID));
#else
      (void)pCmdFrame;      
#endif      
    }
    break;
    
  case gZclCmdSmplMet_MirrorRemovedRsp_c:
    /* Disable Reporting */    
#if gZclEnableReporting_c
    {
      uint8_t i = 0;
      for(i=0;i<gZClMaxAttrReporting_c; i++)
        gZclReportingSetup[i].sendReportMask = 0x00;
      TMR_StopTimer(gZclReportingTimerID);   
    } 
#endif
    break;
    
  case gZclCmdSmplMet_MirrorReportAttr_Rsp_c:
    {
      zclCmdSmplMet_MirrorReportAttrRsp_t *pCmdFrame = (void *)(pIndication->pAsdu + sizeof(zclFrame_t));
      if (pCmdFrame->notifFlags & gZclMet_StayAwakeReqHESFlag_c)
      {      
 /* Change the poll rate to retrieve messages from  */    
#if gEndDevCapability_d || gComboDeviceCapability_d
#if gComboDeviceCapability_d
        if(NlmeGetRequest(gDevType_c) == gEndDevice_c)
#endif
        {
          BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SEMetering_EnterAwakeState, 0, 0, NULL);
        }
#endif      
      }
    }
    break;
#endif /* gZclMirroring_d */  
  case gZclCmdSmplMet_ReqFastPollModeReq_c:
    status = ZCL_SmplMet_ProcessReqFastPollEvt(pIndication);
    break;
#if gASL_ZclSE_12_Features_d
  case gZclCmdSmplMet_GetSnapshot_Req_c:
    (void)ZclSmplMet_ProcessGetSnapshotReq(pIndication);
    break;
  case gZclCmdSmplMet_TakeSnapshot_Req_c:
    (void)ZclMet_TakeSnapshot();
    break;
#endif      
  default:
    status = gZclUnsupportedClusterCommand_c;
    break;
  }
  
  return status;
}

/*!
 * @fn 		zbStatus_t ZclSmplMet_ClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Metering Cluster client. 
 *
 */
zbStatus_t ZclSmplMet_ClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  
  /* prevent compiler warning */
  (void)pDevice;
  pFrame = (void *)pIndication->pAsdu;
  
   /* Check if we need to send Default Response */
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
    status = gZclSuccess_c;
  
  /* handle the command */
  command = pFrame->command;
  
  switch(command) {
    case gZclCmdSmplMet_GetProfRsp_c:
      break;
      
#if gZclMirroring_d      
    case gZclCmdSmplMet_RequestMirrorReq_c:
      {
#if !gNum_MirrorEndpoints_c        
        return gZclFailure_c;
#else        
        zclMet_RequestMirrorRsp_t *pRsp;
        zbEndPoint_t mirrorEp = ZclMet_CreateAndRegisterMirrorEndpoint(pIndication->aSrcAddr);

        pRsp = MSG_Alloc(sizeof(zclMet_RequestMirrorRsp_t));
        if(pRsp)
        {
          /* Prepare for response in the address info (back to sender) */
          AF_PrepareForReply(&pRsp->addrInfo, pIndication);
          pRsp->zclTransactionId = pFrame->transactionId;
          
          /* Build the response */
          if(mirrorEp != 0xFF)
            pRsp->cmdFrame.EndPointID = Native2OTA16(mirrorEp);
          else
            pRsp->cmdFrame.EndPointID = 0xFFFF;
          
          /* Send the response*/
          (void)ZclMet_RequestMirrorRsp(pRsp);          
          MSG_Free(pRsp);
          return gZbSuccess_c;
        }
#endif          
      }
      break;
      

#endif /* gZclMirroring_d */     
#if gZclFastPollMode_d
    case gZclCmdSmplMet_ReqFastPollModeRsp_c:
      break;
#endif
#if gASL_ZclSE_12_Features_d
    case gZclCmdSmplMet_GetSnapshot_Rsp_c:
      break;
#endif        

  default:
      status = gZclUnsupportedClusterCommand_c;
      break;
  }
  
  return status;
}

/*!
 * @fn 		zbStatus_t ZclMet_MirrorClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Metering Cluster client on the mirror endpoint. 
 *
 */
zbStatus_t ZclMet_MirrorClusterClient
  (
  zbApsdeDataIndication_t *pIndication, 
  afDeviceDef_t *pDevice
  )
{
  zclFrame_t *pFrame;
  zclCmd_t command;
  zbStatus_t status = gZclSuccessDefaultRsp_c;
  
  /* prevent compiler warning */
  (void)pDevice;
  pFrame = (void *)pIndication->pAsdu;
  
   /* Check if we need to send Default Response */
  if(pFrame->frameControl & gZclFrameControl_DisableDefaultRsp) 
    status = gZclSuccess_c;
  
  /* handle the command */
  command = pFrame->command;
  
  switch(command) {
    case gZclCmdSmplMet_RemoveMirrorReq_c:
      {
#if !gNum_MirrorEndpoints_c        
        status = gZclFailure_c;
#else         
        zclMet_MirrorRemovedRsp_t *pRsp;
        zclMet_MirroringTable_t* pMTE;        
        
        /* Check address for authorization*/
        pMTE = getMirroringTableEntry(pIndication->aSrcAddr, gInvalidAppEndPoint_c);
        if (!pMTE)
          return gZclNotAuthorized_c;
        
        pRsp = MSG_Alloc(sizeof(zclMet_RequestMirrorRsp_t));
        if(pRsp)
        {
          /* Prepare for response in the address info (back to sender) */
          AF_PrepareForReply(&pRsp->addrInfo, pIndication);
          pRsp->zclTransactionId = pFrame->transactionId;                    
          
          /* Build the response */
          pRsp->addrInfo.srcEndPoint = appEndPoint;
          
#ifdef gHostApp_d         
          AF_DeRegisterEndPointOnBlackBox(endPointList[1].pEndpointDesc->pSimpleDesc->endPoint);
#endif            
          if (AF_DeRegisterEndPoint(endPointList[1].pEndpointDesc->pSimpleDesc->endPoint) == gZbSuccess_c)
          {
            pRsp->cmdFrame.EndPointID = Native2OTA16(endPointList[1].pEndpointDesc->pSimpleDesc->endPoint);                   
            pMTE->isActive = FALSE;
          }
          else
            pRsp->cmdFrame.EndPointID = 0xFFFF;
          
          /* Send the response*/
          status = ZclMet_MirrorRemovedRsp(pRsp);          
          MSG_Free(pRsp);
        }
#endif /*  !gNum_MirrorEndpoints_c  */           
      }    
      break;
    default:
      return gZclUnsupportedClusterCommand_c;
  }
  
  return status;
}

/*!
 * @fn 		zbStatus_t ZclSmplMet_AcceptFastPollModeReq(bool_t acceptFastPollReq)
 *
 * @brief	Sets the current device's accept fast poll mode. 
 *
 */
zbStatus_t ZclSmplMet_AcceptFastPollModeReq(bool_t acceptFastPollReq)
{
  gAcceptFastPoll = acceptFastPollReq;
  return gZclSuccess_c;
}

#if gASL_ZclSE_12_Features_d
/*!
 * @fn 		zbStatus_t ZclMet_TakeSnapshot(void)
 *
 * @brief	Takes a manual snapshot on the Metering server. 
 *
 */
zbStatus_t ZclMet_TakeSnapshot(void)
{
  index_t i;
  zclMet_SnapshotsTable_t *pEntry;
  
  /* Find next empty entry*/
  for (i = 0; i < gNum_Snapshots_c; i++)
    if (gZclMetSnapshotsTable[i].EntryStatus == gEntryNotUsed_c)
      break;
  
  /* No memory available for storing the snapshot */
  if (i == gNum_Snapshots_c)
    return gZbFailed_c;
  
  pEntry = &gZclMetSnapshotsTable[i];
  
  /* Set Unique Issue Id*/
  if (i == 0)
    SetNative32BitInt(pEntry->IssuerEvtID, 0);
  else
  {
    uint32_t id = GetNative32BitInt(gZclMetSnapshotsTable[i-1].IssuerEvtID) + 1;
    SetNative32BitInt(pEntry->IssuerEvtID, id);
  }
  
  pEntry->SnapshotCause = gZclMet_ManualSnapshot_c;
  pEntry->SnapshotTime = ZCL_GetUTCTime();
  pEntry->EntryStatus = gEntryUsed_c;
  
  /* Fill in snapshot payload */
  ZclMet_SetPayloadSnapshot(pEntry);
  return gZbSuccess_c;
}
#endif

#if gASL_ZclSE_12_Features_d
/*!
 * @fn 		static void ZclMet_SetPayloadSnapshot(zclMet_SnapshotsTable_t *pSnapshot)
 *
 * @brief	Sets the snapshot payload on an entry in the snapshots table. 
 *
 */
static void ZclMet_SetPayloadSnapshot(zclMet_SnapshotsTable_t *pSnapshot)
{
  uint8_t noBlocks = 0, noTiers = 0, tierBlockMode = 0;
  zbClusterId_t metClusterId = {gaZclClusterSmplMet_c};
  
  /* Get charging scheme*/
  
  if (noTiers == 0)
  {
    /* Block Charging */
    pSnapshot->SnapshotPayloadType = gZclMet_CurrSummDlvrdRcvdSnapshot_c;
  }
  else if (noBlocks == 0)
  {
    /* TOU Charging */
    pSnapshot->SnapshotPayloadType = gZclMet_TOUInfoSetRcvdSnapshot_c;
  }
  else
  {
    /* Block and TOU Charging */
   
    if (tierBlockMode == 0)
      pSnapshot->SnapshotPayloadType = gZclMet_BlockInfoSetRcvdSnapshot_c;
    else if (tierBlockMode == 1)
      pSnapshot->SnapshotPayloadType = gZclMet_TOUInfoSetRcvdSnapshot_c;
  }
  
  /* Fill in snapshot payload */
  switch(pSnapshot->SnapshotPayloadType)
  {
    case gZclMet_CurrSummDlvrdRcvdSnapshot_c:
      {
        zclSmplMet_CurrSummDlvrdRcvd_t *pPayload = (zclSmplMet_CurrSummDlvrdRcvd_t *)pSnapshot->SnapshotPayload; 
        ZCL_GetAttribute(appEndPoint, metClusterId, gZclAttrMetRISCurrSummDlvrd_c, gZclServerAttr_c, pPayload->currentSummationDelivered, NULL);
        ZCL_GetAttribute(appEndPoint, metClusterId, gZclAttrMetRISCurrSummRcvd_c, gZclServerAttr_c, pPayload->currentSummationReceived, NULL);
      }
      break;
    case gZclMet_TOUInfoSetRcvdSnapshot_c:
    case gZclMet_TOUInfoSetDlvrdSnapshot_c:
      {        
        zclSmplMet_TouInfSet_t *pPayload = (zclSmplMet_TouInfSet_t *)pSnapshot->SnapshotPayload; 
        (void)pPayload;
      }
      break;
    case gZclMet_BlockInfoSetDlvrdSnapshot_c:
    case gZclMet_BlockInfoSetRcvdSnapshot_c:
      {
        zclSmplMet_BlockInfSet_t *pPayload = (zclSmplMet_BlockInfSet_t *)pSnapshot->SnapshotPayload; 
        (void)pPayload;
      }
      break;
    default:
      break;
  }
}
#endif

#if gASL_ZclSE_12_Features_d
/*!
 * @fn 		static zbStatus_t ZclSmplMet_ProcessGetSnapshotReq(zbApsdeDataIndication_t *pIndication)
 *
 * @brief	Processes a Get Snapshot Request frame received on the Metering server. 
 *
 */
static zbStatus_t ZclSmplMet_ProcessGetSnapshotReq
(
zbApsdeDataIndication_t *pIndication
)
{
  zclCmdSmplMet_GetSnapshotReq_t *pReq;
    
  pReq = (zclCmdSmplMet_GetSnapshotReq_t *) ((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  
  /* Initialize structure for sending response */
  gSnapshotToSend.currentIndex = 0;
  AF_PrepareForReply(&gSnapshotToSend.req.addrInfo, pIndication);
  gSnapshotToSend.req.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  FLib_MemCpy(&gSnapshotToSend.req.cmdFrame, pReq, sizeof(zclCmdSmplMet_GetSnapshotReq_t));
  
  /* Set number of snapshots to send */
  if (!gSnapshotToSend.req.cmdFrame.numberOfSnapshots)
    gSnapshotToSend.req.cmdFrame.numberOfSnapshots = gNum_Snapshots_c;
  
  /* Set time */
  if (gSnapshotToSend.req.cmdFrame.startTime == 0)
    gSnapshotToSend.req.cmdFrame.startTime = ZCL_GetUTCTime();  
  
  /* Send event to SE task */
  ZclSE_SendClusterEvt(gZclEvtHandleGetSnapshot_c);
  
  return gZbSuccess_c;
}
#endif

#if gASL_ZclSE_12_Features_d
/*!
 * @fn 		void ZCL_HandleSEMetGetSnapshot(void);
 *
 * @brief	Handles a Get Snapshot Request transaction on the Metering server. 
 *
 */
void ZCL_HandleSEMetGetSnapshot(void)
{
  if (gSnapshotToSend.nextFrag)
  {
    /* Send next snapshot fragment to client */
    SendGetSnapshotResponse(gSnapshotToSend.currentIndex);
    
    /* Send event to SE task */
    ZclSE_SendClusterEvt(gZclEvtHandleGetSnapshot_c);
  }
  else if (gSnapshotToSend.req.cmdFrame.numberOfSnapshots > 0)
  {
    for (;gSnapshotToSend.currentIndex < gNum_Snapshots_c; gSnapshotToSend.currentIndex++)
    {
      zclMet_SnapshotsTable_t *pEntry = &gZclMetSnapshotsTable[0];
      /* Check entry */
      if (((gSnapshotToSend.req.cmdFrame.snapshotCause == pEntry->SnapshotCause) ||
           (gSnapshotToSend.req.cmdFrame.snapshotCause == 0xFFFF))&&
          (gSnapshotToSend.req.cmdFrame.startTime <= pEntry->SnapshotTime) &&
            (pEntry->EntryStatus == gEntryUsed_c))
      {
        /* Reset fragment counter */
        gSnapshotToSend.nextFrag = 0;
          
        /* Send snapshot to client */
        SendGetSnapshotResponse(gSnapshotToSend.currentIndex);
        gSnapshotToSend.req.cmdFrame.numberOfSnapshots--;
        
        /* Send event to SE task */
        TS_SendEvent(gSETaskID, gZclEvtHandleGetSnapshot_c);
        return;
      }
    }
  }
}
#endif

#if gASL_ZclSE_12_Features_d
static void SendGetSnapshotResponse(index_t snapshotIdx)
{
  zclSmplMet_GetSnapshotRsp_t *pRsp;
  zclMet_SnapshotsTable_t *pEntry = &gZclMetSnapshotsTable[snapshotIdx];
  uint8_t length = sizeof(zclCmdSmplMet_GetSnapshotRsp_t) - 1;
  uint8_t maxPayload = AF_MaxPayloadLen(&gSnapshotToSend.req.addrInfo) - sizeof(zclFrame_t) - sizeof(SEEvtId_t) - 1;
  uint8_t maxNoEntries, remEntries, noEntries;
  
  /* Allocate buffer for OTA frame*/
  if (pEntry->SnapshotPayloadType == gZclMet_CurrSummDlvrdRcvdSnapshot_c)
     pRsp = MSG_Alloc(length + sizeof(zclSmplMet_CurrSummDlvrdRcvd_t));
  else
     pRsp = MSG_Alloc(length + maxPayload);
  
  if(!pRsp)
    return;
  
  FLib_MemCpy(&pRsp->addrInfo, &gSnapshotToSend.req.addrInfo, sizeof(afAddrInfo_t));	
  pRsp->zclTransactionId = gSnapshotToSend.req.zclTransactionId;
  FLib_MemCpy(&pRsp->cmdFrame, pEntry, sizeof(zclCmdSmplMet_GetSnapshotRsp_t));
  
  /* Fill in snapshot payload */
  switch(pEntry->SnapshotPayloadType)
  {
    case gZclMet_CurrSummDlvrdRcvdSnapshot_c:
      {
       FLib_MemCpy(&pRsp->cmdFrame.snapshotPayload[0], &pEntry->SnapshotPayload[0], sizeof(zclSmplMet_CurrSummDlvrdRcvd_t));
                    
        /* This payload fits in a single frame*/
        gSnapshotToSend.nextFrag = 0;
        gSnapshotToSend.currentIndex++;
      }
      break;
    case gZclMet_TOUInfoSetRcvdSnapshot_c:
    case gZclMet_TOUInfoSetDlvrdSnapshot_c:
      {        
        zclSmplMet_TouInfSet_t *pPayload = (zclSmplMet_TouInfSet_t *)pEntry->SnapshotPayload; 
        /* First fragment */
        if (!gSnapshotToSend.nextFrag)
          pRsp->cmdFrame.snapshotPayload[0] = pPayload->nrOfTiersInUse;
        
        maxNoEntries = maxPayload / sizeof(Summ_t);
        remEntries = pPayload->nrOfTiersInUse - (gSnapshotToSend.nextFrag * maxNoEntries);
        noEntries = FLib_GetMin(remEntries, maxNoEntries);
        
        FLib_MemCpy(&pRsp->cmdFrame.snapshotPayload[0], 
                    &pEntry->SnapshotPayload[maxPayload * gSnapshotToSend.nextFrag], 
                    noEntries * sizeof(Summ_t));
        /* Next fragment pending*/
        gSnapshotToSend.nextFrag++;
        
        /* Sent all data. No fragments pending */
        if ((gSnapshotToSend.nextFrag * maxNoEntries) >= pPayload->nrOfTiersInUse)
        {
          gSnapshotToSend.nextFrag = 0;
          gSnapshotToSend.currentIndex++;
        }
      }
      break;
    case gZclMet_BlockInfoSetDlvrdSnapshot_c:
    case gZclMet_BlockInfoSetRcvdSnapshot_c:
      {
        zclSmplMet_BlockInfSet_t *pPayload = (zclSmplMet_BlockInfSet_t *)pEntry->SnapshotPayload; 
        /* First fragment */
        if (!gSnapshotToSend.nextFrag)
          pRsp->cmdFrame.snapshotPayload[0] = pPayload->nrOfTiersAndBlockThresholdsInUse;
        
        maxNoEntries = maxPayload / sizeof(Summ_t);
        remEntries = pPayload->nrOfTiersAndBlockThresholdsInUse - (gSnapshotToSend.nextFrag * maxNoEntries);
        noEntries = FLib_GetMin(remEntries, maxNoEntries);
        
        FLib_MemCpy(&pRsp->cmdFrame.snapshotPayload[0], 
                    &pEntry->SnapshotPayload[maxPayload * gSnapshotToSend.nextFrag], 
                    noEntries * sizeof(BlockInf_t));
        /* Next fragment pending*/
        gSnapshotToSend.nextFrag++;
        
        /* Sent all data. No fragments pending */
        if ((gSnapshotToSend.nextFrag * maxNoEntries) >= pPayload->nrOfTiersAndBlockThresholdsInUse)
        {
          gSnapshotToSend.nextFrag = 0;
          gSnapshotToSend.currentIndex++;
        }
      }
      break;
    default:
      break;
  }
  
  /* Send request OTA*/
  (void)ZclSmplMet_GetSnapshotRsp(pRsp);
  MSG_Free(pRsp);
}
#endif

static zbStatus_t ZCL_SmplMet_ProcessReqFastPollEvt
(
zbApsdeDataIndication_t *pIndication  //IN: pointer to APSDE Data Indication
)
{
#if gZclFastPollMode_d
  uint8_t temp8 = 0;
  zclSmplMet_ReqFastPollModeRsp_t response;
    
  zbStatus_t status = gZclSuccess_c;
  zclCmdSmplMet_ReqFastPollModeReq_t *pMsg;
  /* get the request fast poll mode request */
  pMsg = (zclCmdSmplMet_ReqFastPollModeReq_t *)((uint8_t*)pIndication->pAsdu + sizeof(zclFrame_t));
  
  BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_FastPollReqRcv_c, 0, 0, &pMsg);
  
  if(!gAcceptFastPoll)
  {
    response.cmdFrame.appliedUpdatePeriod = 0;
  }
  else
  {
    /*set the values for the response*/
    if(ZCL_GetAttribute(pIndication->dstEndPoint,pIndication->aClusterId, gZclAttrMetRISFastPollUpdatePeriod_c, gZclServerAttr_c, &temp8, NULL))
    {
      return gZclFailure_c;
    }
    if(pMsg->updatePeriod <= temp8)
    {
      response.cmdFrame.appliedUpdatePeriod = temp8;
    }
    else
    {
      response.cmdFrame.appliedUpdatePeriod = pMsg->updatePeriod;
    }
    /*Calculate mcFastPollRemainingTime*/
    /*if the device hasn't allready reached the maximum time it can stay in fast poll mode, recalculate mcFastPollRemainingTime*/
    if(mcFastPollRemainingTime + mcTimeInFastPollMode < gASL_ZclSmplMet_MaxFastPollInterval_d)
    {
      /*If the device isn't in fast poll mode the timer is started*/
      if(!mcFastPollRemainingTime)
      {
        gFastPollTimerID = TMR_AllocateTimer();
        TMR_StartSecondTimer(gFastPollTimerID, 1, FastPollTimerCallBack);
      }
      /*Can the new required fast poll mode request time be included in the maximum fast poll interval time?*/
      if(pMsg->duration * 60 < gASL_ZclSmplMet_MaxFastPollInterval_d - mcTimeInFastPollMode)
      {
        /*If the new duration is higher than the fast poll remaining time update mcFastPollRemainingTime*/
        if(pMsg->duration * 60 > mcFastPollRemainingTime)
        {
          mcFastPollRemainingTime = pMsg->duration * 60;
        }/*else there is no need to update mcFastPollRemainingTime*/
      }
      else
      {
        mcFastPollRemainingTime = gASL_ZclSmplMet_MaxFastPollInterval_d - mcTimeInFastPollMode;
      }
      mcFastPollEndTime = ZCL_GetUTCTime() + (ZCLTime_t)mcFastPollRemainingTime;
    }/*else there is no need to update mcFastPollRemainingTime*/
  }
  /*mcFastPollEndTime is used instead of ZCL_GetUTCTime() + (ZCLTime_t)mcFastPollRemainingTime because in some cases
  there can be a +/-1second difference in the reported time of 2 responses that should have the same end time*/
  response.cmdFrame.EndTime = Native2OTA32(mcFastPollEndTime);
  /*build the response*/
  AF_PrepareForReply(&response.addrInfo, pIndication);
  response.zclTransactionId = ((zclFrame_t *)pIndication->pAsdu)->transactionId;
  status = ZclSmplMet_ReqFastPollModeRsp(&response);
  return status;
#else
  (void)pIndication;
  (void)mcFastPollEndTime;
  return gZclUnsupportedClusterCommand_c;
#endif
}

#if gZclFastPollMode_d
static void FastPollTimerCallBack
(
tmrTimerID_t tmrID
)
{
  (void)tmrID;
  if(mcFastPollRemainingTime ==0)
  {
    TMR_FreeTimer(gFastPollTimerID);
    mcTimeInFastPollMode = 0;
  }
  else
  {
    mcFastPollRemainingTime--;
    mcTimeInFastPollMode++;
    TMR_StartSecondTimer(gFastPollTimerID, 1, FastPollTimerCallBack);
  }
}
#endif
/*!
 * @fn 		uint16_t ZclSmplMet_GetFastPollRemainingTime()
 *
 * @brief	Gets the remaining time in seconds that the device must maintain
 *                fast poll mode. 
 *
 */
uint16_t ZclSmplMet_GetFastPollRemainingTime()
{
  return mcFastPollRemainingTime;
}

/*!
 * @fn 		zbStatus_t ZclSmplMet_GetProfReq(zclSmplMet_GetProfReq_t *pReq)
 *
 * @brief	Sends a Get Profile Request from the Metering client. 
 *
 */
zbStatus_t ZclSmplMet_GetProfReq
(
zclSmplMet_GetProfReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_GetProfReq_c, sizeof(zclCmdSmplMet_GetProfReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t ZclMet_RequestMirrorReq(zclMet_RequestMirrorReq_t *pReq)
 *
 * @brief	Sends a Request Mirror from the Metering server. 
 *
 */
zbStatus_t ZclMet_RequestMirrorReq
(
  zclMet_RequestMirrorReq_t *pReq
)
{
  return ZCL_SendServerReqSeqPassed(gZclCmdSmplMet_RequestMirrorReq_c, 0,(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t ZclMet_RemoveMirrorReq(zclMet_RemoveMirrorReq_t *pReq)
 *
 * @brief	Sends a Remove Mirror from the Metering server. 
 *
 */
 zbStatus_t ZclMet_RemoveMirrorReq
(
  zclMet_RemoveMirrorReq_t *pReq
)
{
  return ZCL_SendServerReqSeqPassed(gZclCmdSmplMet_RemoveMirrorReq_c, 0,(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t ZclMet_RequestMirrorRsp(zclMet_RequestMirrorRsp_t *pReq)
 *
 * @brief	Sends a Request Mirror from the Metering client. 
 *
 */
zbStatus_t ZclMet_RequestMirrorRsp
(
  zclMet_RequestMirrorRsp_t *pRsp
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_RequestMirrorRsp_c, sizeof(zclCmdMet_RequestMirrorRsp_t),(zclGenericReq_t *)pRsp);	
}

/*!
 * @fn 		zbStatus_t ZclMet_MirrorRemovedRsp(zclMet_MirrorRemovedRsp_t *pReq)
 *
 * @brief	Sends a Mirror Removed from the Metering client. 
 *
 */
zbStatus_t ZclMet_MirrorRemovedRsp
(
  zclMet_MirrorRemovedRsp_t *pRsp
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_MirrorRemovedRsp_c, sizeof(zclCmdMet_MirrorRemovedRsp_t),(zclGenericReq_t *)pRsp);	
}

/*!
 * @fn 		zbStatus_t ZclSmplMet_ReqFastPollModeReq(zclSmplMet_ReqFastPollModeReq_t *pReq)
 *
 * @brief	Sends a Request Fast Poll Mode from the Metering client. 
 *
 */
zbStatus_t ZclSmplMet_ReqFastPollModeReq
(
zclSmplMet_ReqFastPollModeReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_ReqFastPollModeReq_c, sizeof(zclCmdSmplMet_ReqFastPollModeReq_t),(zclGenericReq_t *)pReq);	
}

static uint16_t ZCL_BuildGetProfResponse
(   uint8_t IntrvChannel,                 /* IN: */
    ZCLTime_t EndTime,                    /* IN:  note little endian*/
    uint8_t NumberOfPeriods,              /* IN: */
    afToApsdeMessage_t *pMsg  /* IN / OUT: */
)
{

  /* Search for entries that match end time in Profile intervals table */ 
  uint8_t *pTmpIntrvs,i,NumOfPeriodsDlvrd=0, SearchPointer, bufferLen;  
  uint32_t endTime;
  bool_t EntryInTable=FALSE, endTimeValid=FALSE, IntrvChannelSupported=FALSE;
  uint8_t status = gSMGetProfRsp_SuccessStatus_c;
  zbTxFragmentedHdr_t *pFrag = &pMsg->msgData.dataReq.fragmentHdr;
  zclCmdSmplMet_GetProfRsp_t *pProfRsp = (zclCmdSmplMet_GetProfRsp_t *) ((uint8_t *)AF_Payload(pMsg) + sizeof(zclFrame_t));
  
  pTmpIntrvs = (uint8_t *)pProfRsp->Intrvs;
  endTime = OTA2Native32(EndTime);  
  if(ProfIntrvHead == 0)
      SearchPointer = gMaxNumberOfPeriodsDelivered_c;
  else
    SearchPointer = ProfIntrvHead-1;
  
  if(ProfIntrvTail > ProfIntrvHead)
    i = ProfIntrvHead + (gMaxNumberOfPeriodsDelivered_c-ProfIntrvTail)+1;
  else
    i = (ProfIntrvHead - ProfIntrvTail);
  
  if(i)
    EntryInTable = TRUE;

#if gFragmentationCapability_d  
  bufferLen = ApsmeGetMaxFragBufferSize() - (gAsduOffset_c + sizeof(zclFrame_t) + sizeof(zclCmdSmplMet_GetProfRsp_t) - sizeof(IntrvForProfiling_t));
#else
  bufferLen = AF_MaxPayloadLen(NULL) - (sizeof(zclFrame_t) + sizeof(zclCmdSmplMet_GetProfRsp_t) - sizeof(IntrvForProfiling_t));
#endif  
  pFrag->iDataSize = sizeof(zclFrame_t) + sizeof(zclCmdSmplMet_GetProfRsp_t) - sizeof(IntrvForProfiling_t);
  
  for(; i>0 && NumOfPeriodsDlvrd < NumberOfPeriods; --i)
  {
    /* Search the profile table for entries */
    if(ProfileIntervalTable[SearchPointer].IntrvChannel == IntrvChannel)
    {
      IntrvChannelSupported=TRUE;
      /* Check if endTime is valid */
      if(ProfileIntervalTable[SearchPointer].endTime <= endTime || endTime == 0x00000000)
      {
        endTimeValid=TRUE;
        
        /* Not enough space in the current buffer */
        if (bufferLen < sizeof(IntrvForProfiling_t))
#if gFragmentationCapability_d           
        {
          /* Allocate another buffer */
          if (!AF_ActualMsgAllocFragment(pMsg, ApsmeGetMaxFragBufferSize(), &pTmpIntrvs))
            break;
          
          /* Next fragment */
          pFrag = pFrag->pNextDataBlock;
          pFrag->iDataSize = 0;
          bufferLen = ApsmeGetMaxFragBufferSize();
        }
#else
        break;
#endif            
        
        if(NumOfPeriodsDlvrd==0)
          FLib_MemCpy(&pProfRsp->EndTime, &ProfileIntervalTable[SearchPointer].endTime, sizeof(uint32_t));
        NumOfPeriodsDlvrd++;
        FLib_MemCpy(pTmpIntrvs, &ProfileIntervalTable[SearchPointer].Intrv, sizeof(Consmp));
        pTmpIntrvs += sizeof(IntrvForProfiling_t);
        pFrag->iDataSize += sizeof(IntrvForProfiling_t);
        bufferLen -= sizeof(IntrvForProfiling_t);
      }
      else
      {
        /* endTime is invalid, check next entry */
        endTimeValid=FALSE;
      }
      
    }
 //   else
 //   {
 //     IntrvChannelSupported=FALSE;
 //   }
    if(SearchPointer==0)
    {
      SearchPointer = gMaxNumberOfPeriodsDelivered_c;
    }
    --SearchPointer;
    
  }
  
  pFrag->iDataSize = ApsmeGetMaxFragBufferSize() - bufferLen;
 
  pProfRsp->ProfIntrvPeriod = gProfIntrvPeriod_c;
  pProfRsp->NumOfPeriodsDlvrd = NumOfPeriodsDlvrd;
  /* check if status field value is undefined  interval channel requested */
  if((IntrvChannel != gIntrvChannel_ConsmpDlvrd_c) && (IntrvChannel != gIntrvChannel_ConsmpRcvd_c))
     status = gSMGetProfRsp_UndefIntrvChannelStatus_c;
  /* check if status field value is interval channel not supported */
  else if(EntryInTable == TRUE && IntrvChannelSupported == FALSE)
    status = gSMGetProfRsp_IntrvChannelNotSupportedStatus_c;
   /* check if status field value is invalid end time */
  /*else if() 
    status = gSMGetProfRsp_InvalidEndTimeStatus_c;*/  
  /* check if status field value is no intervals available for the requested time */
  else if((EntryInTable == TRUE && IntrvChannelSupported==TRUE && endTimeValid==FALSE)|| 
          (EntryInTable == FALSE))
    status = gSMGetProfRsp_NoIntrvsAvailableStatus_c;
  /* check if status field value is more periods requested than can be returned */
  else if(NumberOfPeriods > NumOfPeriodsDlvrd)
    status = gSMGetProfRsp_MorePeriodsRequestedStatus_c;
  
  pProfRsp->Status=status;
  
  return (pProfRsp->NumOfPeriodsDlvrd - 1) * sizeof(IntrvForProfiling_t) + sizeof(zclCmdSmplMet_GetProfRsp_t);

}

/*!
 * @fn 		zbStatus_t ZclSmplMet_GetProfRsp(zclSmplMet_GetProfRsp_t *pReq)
 *
 * @brief	Sends a Get Profile Response from the Metering client. 
 *
 */
zbStatus_t ZclSmplMet_GetProfRsp
(
zclSmplMet_GetProfRsp_t *pReq
)
{
  uint8_t length;
  length = MbrOfs(zclCmdSmplMet_GetProfRsp_t,Intrvs) + 
    (pReq->cmdFrame.NumOfPeriodsDlvrd * sizeof(pReq->cmdFrame.Intrvs[0]));
  /* Set fragmented transmission if we cannot fit payload in frame */
  if(length > ApsmeGetMaxAsduLength(0) - sizeof(zclFrame_t))
    pReq->addrInfo.txOptions |= gApsTxOptionFragmentationRequested_c;
  return ZCL_SendServerRspSeqPassed(gZclCmdSmplMet_GetProfRsp_c, length,(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZclSmplMet_ReqFastPollModeRsp(zclSmplMet_ReqFastPollModeRsp_t *pReq)
 *
 * @brief	Sends a Request Fast Poll Mode Response from the Metering server. 
 *
 */
zbStatus_t ZclSmplMet_ReqFastPollModeRsp
(
zclSmplMet_ReqFastPollModeRsp_t *pReq
)
{
  return ZCL_SendServerRspSeqPassed(gZclCmdSmplMet_ReqFastPollModeRsp_c, sizeof(zclCmdSmplMet_ReqFastPollModeRsp_t),(zclGenericReq_t *)pReq);
}

/*!
 * @fn 		zbStatus_t ZclSmplMet_GetSnapshotReq(zclSmplMet_GetSnapshotReq_t *pReq)
 *
 * @brief	Sends a Get Snapshot Request from the Metering client. 
 *
 */
zbStatus_t ZclSmplMet_GetSnapshotReq
(
  zclSmplMet_GetSnapshotReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_GetSnapshot_Req_c, sizeof(zclCmdSmplMet_GetSnapshotReq_t),(zclGenericReq_t *)pReq);	
}

/*!
 * @fn 		zbStatus_t ZclMet_MirrorReportAttrRsp(zclSmplMet_MirrorReportAttrRsp_t *pReq)
 *
 * @brief	Sends a Mirror Report Attribute Response from the Metering client. 
 *
 */
zbStatus_t ZclMet_MirrorReportAttrRsp
(
  zclSmplMet_MirrorReportAttrRsp_t *pRsp
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_MirrorReportAttr_Rsp_c, sizeof(zclCmdSmplMet_MirrorReportAttrRsp_t),(zclGenericReq_t *)pRsp);	
}

/*!
 * @fn 		zbStatus_t ZclSmplMet_GetSnapshotRsp(zclSmplMet_GetSnapshotRsp_t *pReq)
 *
 * @brief	Sends a Get Snapshot Response from the Metering server. 
 *
 */
zbStatus_t ZclSmplMet_GetSnapshotRsp
(
  zclSmplMet_GetSnapshotRsp_t *pRsp
)
{
  uint8_t length = sizeof(zclCmdSmplMet_GetSnapshotRsp_t) - 1;
  switch(pRsp->cmdFrame.snapshotPayloadType)
  {
    case gZclMet_CurrSummDlvrdRcvdSnapshot_c:
      length += sizeof(zclSmplMet_CurrSummDlvrdRcvd_t);
      break;
    case gZclMet_TOUInfoSetRcvdSnapshot_c:
    case gZclMet_TOUInfoSetDlvrdSnapshot_c:
      if (pRsp->cmdFrame.snapshotPayload > 0)  
        length += sizeof(zclSmplMet_TouInfSet_t) + sizeof(Summ_t) * (pRsp->cmdFrame.snapshotPayload[0] - 1);
      break;
    case gZclMet_BlockInfoSetDlvrdSnapshot_c:
    case gZclMet_BlockInfoSetRcvdSnapshot_c:
      if (pRsp->cmdFrame.snapshotPayload > 0)  
        length += sizeof(zclSmplMet_BlockInfSet_t) + sizeof(BlockInf_t) * (pRsp->cmdFrame.snapshotPayload[0] - 1);
      break;
    default:
      break;
  }
  return ZCL_SendServerReqSeqPassed(gZclCmdSmplMet_GetSnapshot_Rsp_c, length,(zclGenericReq_t *)pRsp);	
}

/*!
 * @fn 		zbStatus_t ZclMet_TakeSnapshotRsp(zclMet_TakeSnapshotReq_t *pReq)
 *
 * @brief	Sends a Take Snapshot Response from the Metering client. 
 *
 */
zbStatus_t ZclMet_TakeSnapshotRsp
(
  zclMet_TakeSnapshotReq_t *pReq
)
{
  return ZCL_SendClientReqSeqPassed(gZclCmdSmplMet_TakeSnapshot_Req_c, 0,(zclGenericReq_t *)pReq);	
}


void GetProfileTestTimerCallBack(tmrTimerID_t tmrID)
{
  Consmp *consumption;
  Summ_t newRISCurrSummDlvrd;
  zbClusterId_t ClusterId={gaZclClusterSmplMet_c}; 
  (void)tmrID;
  
  ProfileIntervalTable[ProfIntrvHead].IntrvChannel = gIntrvChannel_ConsmpDlvrd_c;
  ProfileIntervalTable[ProfIntrvHead].endTime = ZCL_GetUTCTime();
  /* Simulate used power from last readings */
  (void)ZCL_GetAttribute(appEndPoint, ClusterId, gZclAttrMetRISCurrSummDlvrd_c,  gZclServerAttr_c, &newRISCurrSummDlvrd, NULL);
  consumption = CalculateConsumptionFrom6ByteArray( (uint8_t *)&newRISCurrSummDlvrd, (uint8_t *)&LastRISCurrSummDlvrd);
  FLib_MemCpy(&LastRISCurrSummDlvrd, &newRISCurrSummDlvrd, sizeof(Summ_t));
  
  FLib_MemCpy(&ProfileIntervalTable[ProfIntrvHead++].Intrv, consumption, sizeof(Consmp));
  
  if(ProfIntrvHead >= gMaxNumberOfPeriodsDelivered_c)
    ProfIntrvHead=0;
  if(ProfIntrvHead == ProfIntrvTail)
  {
    ++ProfIntrvTail;
    if(ProfIntrvTail >= gMaxNumberOfPeriodsDelivered_c)
      ProfIntrvTail=0;
  }
  
/* Start the GetProfile test timer */
#if(gProfIntrvPeriod_c < 5)
  TMR_StartMinuteTimer(gGetProfileTestTimerID, gTimerValue_c, GetProfileTestTimerCallBack);
#else
  TMR_StartSecondTimer(gGetProfileTestTimerID, gTimerValue_c, GetProfileTestTimerCallBack);
#endif
}

static void UpdatePowerConsumptionTimerCallBack(tmrTimerID_t tmrID)
{
  UpdatePowerConsumption();
  (void)tmrID; /* Unused parameter. */
}

void UpdatePowerConsumption(void)
{
  Summ_t newRISCurrSummDlvrd;
  uint8_t randomRange;
  uint8_t rangeIndexMax, rangeIndexMin;
  zbClusterId_t ClusterId={gaZclClusterSmplMet_c}; 
  /* This is only for demo, the intervals should be calculated by reading the meter and finding the used power from last reading */
  (void)ZCL_GetAttribute(appEndPoint, ClusterId, gZclAttrMetRISCurrSummDlvrd_c, gZclServerAttr_c, &newRISCurrSummDlvrd, NULL);  
  #if(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_Daily_c)
      rangeIndexMin=150;
      rangeIndexMax=255;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_60mins_c)
      rangeIndexMin=120;
      rangeIndexMax=224;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_30mins_c)  
      rangeIndexMin=80;
      rangeIndexMax=192;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_15mins_c)    
      rangeIndexMin=50;
      rangeIndexMax=160;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_10mins_c)      
      rangeIndexMin=30;
      rangeIndexMax=128;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_7dot5mins_c)
      rangeIndexMin=15;
      rangeIndexMax=96;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_5mins_c)
      rangeIndexMin=10;
      rangeIndexMax=64;
  #elif(gProfIntrvPeriod_c == gZclSEProfIntrvPeriod_2dot5mins_c)  
      rangeIndexMin=0;
      rangeIndexMax=32;
  #endif
  
  randomRange = GetRandomRange(rangeIndexMin, rangeIndexMax);
  Add8ByteTo6ByteArray(randomRange, (uint8_t *)&newRISCurrSummDlvrd);
  (void)ZCL_SetAttribute(appEndPoint, ClusterId, gZclAttrMetRISCurrSummDlvrd_c, gZclServerAttr_c, &newRISCurrSummDlvrd);

  TMR_StartSecondTimer(gUpdatePowerConsumptionTimerID, gUpdateConsumption_c, UpdatePowerConsumptionTimerCallBack);  
}

/*****************************************************************************/

static Consmp *CalculateConsumptionFrom6ByteArray(uint8_t *pNew, uint8_t *pOld)
{
  uint32_t old32Bit, new32Bit;
  
  // OTA/ZCL Domain - Little Endian 
  FLib_MemCpy(&new32Bit, &pNew[0], sizeof(uint32_t));
  FLib_MemCpy(&old32Bit, &pOld[0], sizeof(uint32_t));
  
  new32Bit=OTA2Native32(new32Bit);
  old32Bit=OTA2Native32(old32Bit);
  
   // Native Domain - MC1322x (Little)  MC1321x,QE,HCS08 (BIG) 
  if(new32Bit >= old32Bit)
  {
    new32Bit = (new32Bit - old32Bit);
  }
  else
  {
    new32Bit = (new32Bit + old32Bit);
  }
  
  new32Bit=Native2OTA32 (new32Bit);  
   // OTA/ZCL Domain - Little Endian 
  FLib_MemCpy(&consumptionValue[0], &new32Bit, sizeof(Consmp));
  
  return &consumptionValue;
 }

/*****************************************************************************/

static void Add8ByteTo6ByteArray(uint8_t value, uint8_t *pSumm_t)
{
  uint32_t LSB2Bytes=0, tmp=0, MSB4Bytes=0;
  
  // OTA/ZCL Domain - Little Endian 
  FLib_MemCpy(&LSB2Bytes, &pSumm_t[0], sizeof(uint16_t));
  FLib_MemCpy(&MSB4Bytes, &pSumm_t[2], sizeof(uint32_t));
  
  LSB2Bytes=OTA2Native32(LSB2Bytes);
  MSB4Bytes=OTA2Native32(MSB4Bytes);
  
  // Native Domain - MC1322x (Little)  MC1321x,QE,HCS08 (BIG)
  LSB2Bytes += value;
  
  tmp = LSB2Bytes >> 16;
  
  MSB4Bytes += tmp;

  // OTA/ZCL Domain - Little Endian 
  pSumm_t[0] = (uint8_t) (LSB2Bytes);
  pSumm_t[1] = (uint8_t) (LSB2Bytes>>8);
  pSumm_t[2] = (uint8_t) (MSB4Bytes);
  pSumm_t[3] = (uint8_t) (MSB4Bytes>>8);
  pSumm_t[4] = (uint8_t) (MSB4Bytes>>16);
  pSumm_t[5] = (uint8_t) (MSB4Bytes>>24);
  
}

#if gZclMirroring_d
/*!
 * @fn 		zbEndPoint_t ZclMet_CreateAndRegisterMirrorEndpoint(zbNwkAddr_t aNwkAddress)
 *
 * @brief	Registers the mirror endpoint and initializes the mirror, given the meter's
 *			short address. 
 *
 */
zbEndPoint_t ZclMet_CreateAndRegisterMirrorEndpoint(zbNwkAddr_t aNwkAddress)
{
#if gNum_EndPoints_c != 0 
  uint8_t phyEnvironment;
  zbClusterId_t aClusterId = {gaZclClusterBasic_c};
  zclMet_MirroringTable_t* pMTE;
  
  /* Get Physical Environment Attribute*/
   
  if (ZCL_GetAttribute(endPointList[0].pEndpointDesc->pSimpleDesc->endPoint, aClusterId, gZclAttrBasic_PhysicalEnvironmentId_c, 
                    gZclServerAttr_c, &phyEnvironment, NULL) != gZbSuccess_c)
    return gInvalidAppEndPoint_c;
  
  /* Check if mirroring is supported*/
  if (phyEnvironment != gZclAttrBasicPhylEnvSpecifiedEnvironment)
    return gInvalidAppEndPoint_c;
  
  pMTE = getFreeMirroringTableEntry();
  
  if ((pMTE) && (AF_RegisterEndPoint(endPointList[pMTE->endPointListIdx].pEndpointDesc) == gZbSuccess_c))
  {
#ifdef gHostApp_d  
    AF_RegisterEndPointOnBlackBox(endPointList[pMTE->endPointListIdx].pEndpointDesc);
#endif    
    pMTE->isActive = TRUE; 
    Copy2Bytes(pMTE->aNwkAddress, aNwkAddress);
    return endPointList[pMTE->endPointListIdx].pEndpointDesc->pSimpleDesc->endPoint;
  }
#else
    (void)aNwkAddress;
#endif
    
  return gInvalidAppEndPoint_c;
}
#endif

void ZclMet_ConfigureReportingToMirror(zbApsdeDataIndication_t *pIndication, zbEndPoint_t mirrorEndPoint)
{ 
  zbBindUnbindRequest_t  bindRequest;
  zbAddressMap_t addrMap;
  uint8_t idx;
  afDeviceDef_t *pDevice;
  
  /* Bind our endpoint to the mirror endpoint*/
  (void)AddrMap_SearchTableEntry(NULL, &pIndication->aSrcAddr, &addrMap);
  Set2Bytes(bindRequest.aClusterId, gZclClusterSmplMet_c);
  Copy8Bytes(bindRequest.aSrcAddress, NlmeGetRequest(gNwkIeeeAddress_c));
  bindRequest.srcEndPoint = pIndication->dstEndPoint;
  bindRequest.addressMode = gZbAddrMode64Bit_c;
  Copy8Bytes(bindRequest.destData.extendedMode.aDstAddress, addrMap.aIeeeAddr);
  bindRequest.destData.extendedMode.dstEndPoint = mirrorEndPoint;     
  
  APP_ZDP_BindUnbindRequest(NULL, NlmeGetRequest(gNwkShortAddress_c), gBind_req_c, &bindRequest);
  
  Set2Bytes(bindRequest.aClusterId, gZclClusterBasic_c);
  APP_ZDP_BindUnbindRequest(NULL, NlmeGetRequest(gNwkShortAddress_c), gBind_req_c, &bindRequest);
  
  /* Configure reporting*/
  
  for(idx=0;idx<gZClMaxAttrReporting_c; idx++)
  {
    gZclReportingSetup[idx].sendReportMask = 0;
    gZclReportingSetup[idx].reportTimeout = gMetDefaultUpdatePeriod_c;
    gZclReportingSetup[idx].reportMin = gMetDefaultUpdatePeriod_c;
    gZclReportingSetup[idx].minTimeCounter = gMetDefaultUpdatePeriod_c;

    /* Initialize the counter with the number of seconds we want to report */
    gZclReportingSetup[idx].reportCounter = gZclReportingSetup[idx].reportTimeout;
  }
  ZLC_StartReportingTimer();
  
  /* Get Device */
  pDevice = AF_GetEndPointDevice(pIndication->dstEndPoint);        
  
  if (NULL == pDevice)
    return;
  
  /* Set the report mask */
  for (idx = 0; idx < pDevice->reportCount; idx++)
  {
    BeeUtilSetIndexedBit(pDevice->pData, idx);

    /* Start reporting and set state */
    gZclReportingSetup[idx].sendReportMask |= gZCLReportingAttrValueEnabled_c;
  }
  
  BeeAppUpdateDevice(pIndication->dstEndPoint, gZclUI_SEMetering_MirrorCreated, 0, 0, &bindRequest);
}


#if gZclMirroring_d && gNum_MirrorEndpoints_c 
/*!
 * @fn 		void ZCL_InitMirroring(void)
 *
 * @brief	Initializes the mirroring information structure. 
 *
 */
void ZCL_InitMirroring(void)
{
  uint8_t idx;
  for (idx = 0; idx < gNum_MirrorEndpoints_c; idx++)
  {
    gMirroringTable[idx].endPointListIdx = idx + 1;
    gMirroringTable[idx].isActive = FALSE;
  }
 }

/*!
 * @fn 		zclMet_MirroringTable_t* getMirroringTableEntry(zbNwkAddr_t aNwkAddress, zbEndPoint_t endPoint)
 *
 * @brief	Gets the mirroring information for a given Meter short address or mirror endpoint. 
 *
 */
zclMet_MirroringTable_t* getMirroringTableEntry(zbNwkAddr_t aNwkAddress, zbEndPoint_t endPoint)
{
  uint8_t idx;
  
  for (idx = 0; idx < gNum_MirrorEndpoints_c; idx++)
  {
    if (((aNwkAddress != NULL) && IsEqual2Bytes(gMirroringTable[idx].aNwkAddress, aNwkAddress)) ||
      ((endPoint != gInvalidAppEndPoint_c) && (endPointList[gMirroringTable[idx].endPointListIdx].pEndpointDesc->pSimpleDesc->endPoint == endPoint)))
      return &gMirroringTable[idx];
  }
  return NULL;
}

/*!
 * @fn 		zclMet_MirroringTable_t* getFreeMirroringTableEntry(void)
 *
 * @brief	Gets a free entry in the Mirroring information table.
 *
 */
zclMet_MirroringTable_t* getFreeMirroringTableEntry(void)
{
  uint8_t idx;
  
  for (idx = 0; idx < gNum_MirrorEndpoints_c; idx++)
  {
    if (!(gMirroringTable[idx].isActive))
      return &gMirroringTable[idx];
  }
  return NULL;
}
#endif
