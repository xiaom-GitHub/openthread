/******************************************************************************
* AspZtc.h
* This module contains a fresh implementation of the ZigBee Test Client (ZTC).
*
* Copyright (c) 2012, Freescale, Inc.  All rights reserved.
*
*
* No part of this document may be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from  Freescale Semiconductor.
*
* This file contains declarations for the ASP functions
*
******************************************************************************/
#ifndef _AspZtc_h
#define _AspZtc_h

#include "EmbeddedTypes.h"

/****************************************************************************/
/* ASP types */

typedef uint8_t AppAspMsgType_t;
#define aspMsgTypeGetTimeReq_c          0x00
#define aspMsgTypeGetInactiveTimeReq_c  0x01
#define aspMsgTypeGetMacStateReq_c      0x02
#define aspMsgTypeDozeReq_c             0x03
#define aspMsgTypeAutoDozeReq_c         0x04
#define aspMsgTypeAcomaReq_c            0x05
#define aspMsgTypeHibernateReq_c        0x06
#define aspMsgTypeWakeReq_c             0x07
#define aspMsgTypeEventReq_c            0x08
#define aspMsgTypeClkoReq_c             0x09
#define aspMsgTypeTrimReq_c             0x0A
#define aspMsgTypeDdrReq_c              0x0B
#define aspMsgTypePortReq_c             0x0C
#define aspMsgTypeSetMinDozeTimeReq_c   0x0D
#define aspMsgTypeSetNotifyReq_c        0x0E
#define aspMsgTypeSetPowerLevel_c       0x0F
#define aspMsgTypeGetPowerLevel_c       0x1F
#define aspMsgTypeTelecTest_c           0x10
#define aspMsgTypeTelecSetFreq_c        0x11
#define aspMsgTypeGetInactiveTimeCnf_c  0x12
#define aspMsgTypeGetMacStateCnf_c      0x13
#define aspMsgTypeDozeCnf_c             0x14
#define aspMsgTypeAutoDozeCnf_c         0x15
#define aspMsgTypeTelecSendRawData_c    0x16
#define aspMsgTypeSetFADState_c         0x17
#define aspMsgTypeSetFADThreshold_c     0x18
#define aspMsgTypeGetFADThreshold_c     0x19
#define aspMsgTypeGetFADState_c         0x1A
#define aspMsgTypeSetActivePromState_c  0x1B
#define aspMsgTypeXcvrWriteReq_c        0x1C
#define aspMsgTypeXcvrReadReq_c         0x1D
#define aspMsgTypeSetANTXState_c        0x20
#define aspMsgTypeGetANTXState_c        0x21
#define aspMsgTypeSetLQIMode_c          0x22
#define aspMsgTypeGetRSSILevel_c        0x23

typedef PACKED_STRUCT aspGetTimeReq_tag
{         /* AspGetTime.Request            */
  zbClock24_t time;
} aspGetTimeReq_t;


typedef PACKED_STRUCT aspGetInactiveTimeReq_tag
{ /* AspGetInactiveTime.Request    */
  zbClock24_t time;
} aspGetInactiveTimeReq_t;


typedef PACKED_STRUCT aspGetMacStateReq_tag
{     /* AspGetMacState.Request        */
  uint8_t placeHolder;
} aspGetMacStateReq_t;


typedef PACKED_STRUCT aspDozeReq_tag
{            /* AspDoze.Request               */
  zbClock24_t dozeDuration;
  uint8_t clko_en;
} aspDozeReq_t;


typedef PACKED_STRUCT aspAutoDozeReq_tag
{        /* AspAutoDoze.Request           */
  bool_t autoEnable;
  bool_t enableWakeIndication;
  zbClock24_t autoDozeInterval;
  uint8_t clko_en;
} aspAutoDozeReq_t;


typedef PACKED_STRUCT aspAcomaReq_tag
{           /* AspAcoma.Request              */
  uint8_t clko_en;
} aspAcomaReq_t;


typedef PACKED_STRUCT aspHibernateReq_tag
{       /* AspHibernate.Request          */
  uint8_t placeHolder;
} aspHibernateReq_t;


typedef PACKED_STRUCT aspWakeReq_tag
{            /* AspWake.Request               */
  uint8_t placeHolder;
} aspWakeReq_t;


typedef PACKED_STRUCT aspEventReq_tag
{           /* AspEvent.Request              */
  zbClock24_t eventTime;
} aspEventReq_t;


typedef PACKED_STRUCT aspClkoReq_tag
{            /* AspClko.Request               */
  uint8_t clkoEnable;
  uint8_t clkoRate;
} aspClkoReq_t;


typedef PACKED_STRUCT aspTrimReq_tag
{            /* AspTrim.Request               */
  uint8_t trimValue;
  uint8_t padding;
} aspTrimReq_t;


typedef PACKED_STRUCT aspDdrReq_tag
{             /* AspDdr.Request                */
  uint8_t directionMask;
} aspDdrReq_t;


typedef PACKED_STRUCT aspPortReq_tag
{            /* AspPort.Request               */
  uint8_t portWrite;
  uint8_t portValue;
} aspPortReq_t;


typedef PACKED_STRUCT aspSetMinDozeTimeReq_tag
{  /* AspSetMinDozeTime.Request     */
  zbClock24_t minDozeTime;
} aspSetMinDozeTimeReq_t;


typedef PACKED_STRUCT aspSetNotifyReq_tag
{       /* AspSetNotify.Request          */
  uint8_t notifications;
} aspSetNotifyReq_t;


typedef PACKED_STRUCT aspSetPowerLevelReq_tag
{   /* AspSetPowerLevel.Request      */
  uint8_t powerLevel;
} aspSetPowerLevelReq_t;


typedef PACKED_STRUCT aspGetPowerLevelReq_tag
{   /* AspGetPowerLevel.Request      */
  uint8_t powerLevel;
} aspGetPowerLevelReq_t;


typedef PACKED_STRUCT aspTelecTest_tag
{          /* AspTelecTest.Request          */
  uint8_t mode;
} aspTelecTest_t;


typedef PACKED_STRUCT aspTelecsetFreq_tag
{       /* AspTelecSetFreq.Request       */
  uint8_t channel;
} aspTelecsetFreq_t;


typedef PACKED_STRUCT aspTelecSendRawData_tag
{   /* AspTelecSendRawData.Request   */
  uint8_t  length;
  uint8_t* dataPtr;
} aspTelecSendRawData_t;

typedef uint8_t aspFADThreshold_t;            // FAD
typedef uint8_t aspRSSILevel_t;

typedef PACKED_STRUCT aspXcvrReq_tag
{
  uint8_t  mode;
  uint16_t addr;
  uint8_t  len;
  uint8_t* data;
} aspXcvrReq_t;

typedef PACKED_UNION aspReq_tag
{
  aspGetTimeReq_t         aspGetTimeReq;
  aspGetInactiveTimeReq_t aspGetInactiveTimeReq;
  aspDozeReq_t            aspDozeReq;
  aspAutoDozeReq_t        aspAutoDozeReq;
  aspAcomaReq_t           aspAcomaReq;
  aspHibernateReq_t       aspHibernateReq;
  aspWakeReq_t            aspWakeReq;
  aspEventReq_t           aspEventReq;
  aspClkoReq_t            aspClkoReq;
  aspTrimReq_t            aspTrimReq;
  aspDdrReq_t             aspDdrReq;
  aspPortReq_t            aspPortReq;
  aspSetMinDozeTimeReq_t  aspSetMinDozeTimeReq;
  aspSetNotifyReq_t       aspSetNotifyReq;
  aspSetPowerLevelReq_t   aspSetPowerLevelReq;
  aspGetPowerLevelReq_t   aspGetPowerLevelReq;
  aspTelecTest_t          aspTelecTest;
  aspTelecsetFreq_t       aspTelecsetFreq;
  aspTelecSendRawData_t   aspTelecSendRawData;
  aspFADThreshold_t       aspFADThreshold;
  bool_t                  aspFADState;
  bool_t                  aspANTXState;
  bool_t                  aspLQIMode;
  aspRSSILevel_t          aspRSSILevel;  
  bool_t                  aspActivePromState;
  aspXcvrReq_t            aspXcvrData;
} aspReq_t;


/* APP to ASP message */
typedef PACKED_STRUCT AppToAspMessage_tag
{
  AppAspMsgType_t msgType;
  PACKED_STRUCT
  {
    aspReq_t  req;
  } msgData;
} AppToAspMessage_t;

#endif

