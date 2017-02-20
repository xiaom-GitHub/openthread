/******************************************************************************
* ZclOTA.h
*
* This module contains code that handles ZigBee Cluster Library commands and
* clusters for OTA (Over The Air upgrading cluster).
*
* Copyright (c) 2011, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
* Documents used in this specification:
* [R1] - 053474r20 ZigBee Pro spec
* [R2] - docs-07-5123-04-0afg-zigbee-cluster-library-specification.pdf
*******************************************************************************/
#ifndef _ZCL_OTA_H
#define _ZCL_OTA_H

#include "AfApsInterface.h"
#include "AppAfInterface.h"
#include "BeeStackInterface.h"
#include "ZCL.h"
#include "ZclOptions.h"
#include "SEProfile.h"

/******************************************************************************
*******************************************************************************
* Private macros
*******************************************************************************
******************************************************************************/
#if gZclEnableOTAServer_d || gZclEnableOTAClient_d
#define OTA_MANUFACTURER_CODE_FSL         0x1004
#define OTA_MANUFACTURER_CODE_MATCH_ALL   0xFFFF


#define OTA_IMAGE_TYPE_CURRENT            0x0000

#define OTA_IMAGE_TYPE_MAX_MANUFACTURER   0xFFBF
#define OTA_IMAGE_TYPE_CREDENTIAL         0xFFC0
#define OTA_IMAGE_TYPE_CONFIGURATION      0xFFC1
#define OTA_IMAGE_TYPE_LOG                0xFFC2  
#define OTA_IMAGE_TYPE_MATCH_ALL          0xFFFF

#define OTA_FILE_IDENTIFIER               0x0BEEF11E
#define OTA_FILE_VERSION_CURRENT          0x40034003
#define OTA_FILE_VERSION_DL_DEFAULT       0x00000000
#define OTA_FILE_VERSION_MATCH_ALL        0xFFFFFFFF


#ifdef PROCESSOR_KINETIS
#define OTA_HW_VERSION_CURRENT            0x2421
#else  /*  PROCESSOR_KINETIS */
#if gTargetQE128EVB_d                    
#define OTA_HW_VERSION_CURRENT            0x2028
#else
#define OTA_HW_VERSION_CURRENT            0x1323
#endif
#endif

/* Header control field bit definitions */
#define SECURITY_CREDENTIAL_VERSION_PRESENT	(1<<0)
#define DEVICE_SPECIFIC_FILE		        (1<<1)
#define HARDWARE_VERSION_PRESENT		(1<<2)


/* local file version policies definitions */
#define OTA_FILE_VERSION_SUPPORTED_UPGRADE      (1<<0)
#define OTA_FILE_VERSION_SUPPORTED_REINSTALL    (1<<1)
#define OTA_FILE_VERSION_SUPPORTED_DOWNGRADE    (1<<2)


#define OTA_CLIENT_MAX_DATA_SIZE          0x38  
#define OTA_CLIENT_MAX_RANDOM_JITTER      100
#define gZclOTANextImageRequest_HwVersionPresent  0x01 /* 0x01 hw version is present in QueryImageReq payload
                                                          0x00 hw version is not present in QueryImageReq payload*/
#define gMinforNextImageRequest_c	 20    /* time in minute for the next query image request*/
#define gMaxRetransmisionCounter 	0x05

#define gZclOTABlockRequest_IeeePresent_c               0x01 
#define gZclOTABlockRequest_BlockRequestDelayPresent_c  0x02 /* 0x02 Block Request Delay is present in QueryImageReq payload
                                                                0x00 Block Request Delay is not present in QueryImageReq payload*/

#endif
/******************************************************************************
*******************************************************************************
* Public macros and data types definitions.
*******************************************************************************
******************************************************************************/

/******************************************
	OTA Cluster
*******************************************/

enum{
/* 6.7.7	ImageUpgradeStatus Attribute Values */
   gOTAUpgradeStatusNormal_c = 0x00,
   gOTAUpgradeStatusDownloadInProgress_c = 0x01,
   gOTAUpgradeStatusDownloadComplete_c = 0x02,
   gOTAUpgradeStatusWaitingToUpgrade_c = 0x03,
   gOTAUpgradeStatusCountDown_c = 0x04,
   gOTAUpgradeStatusWaitForMore_c = 0x05,
   gOTAUpgradeStatusMax_c = 0x06,
};

/* 6.10.1	OTA Cluster Command Identifiers */
#define gZclCmdOTA_ImageNotify_c                0x00 /* O - Direction 0x01 */
#define gZclCmdOTA_QueryNextImageRequest_c      0x01 /* M - Direction 0x00 */
#define gZclCmdOTA_QueryNextImageResponse_c     0x02 /* M - Direction 0x01 */
#define gZclCmdOTA_ImageBlockRequest_c          0x03 /* M - Direction 0x00 */
#define gZclCmdOTA_ImagePageRequest_c           0x04 /* O - Direction 0x00 */
#define gZclCmdOTA_ImageBlockResponse_c         0x05 /* M - Direction 0x01 */
#define gZclCmdOTA_UpgradeEndRequest_c          0x06 /* M - Direction 0x00 */
#define gZclCmdOTA_UpgradeEndResponse_c         0x07 /* M - Direction 0x01 */
#define gZclCmdOTA_QuerySpecificFileRequest_c   0x08 /* O - Direction 0x00 */
#define gZclCmdOTA_QuerySpecificFileResponse_c  0x09 /* O - Direction 0x01 */

/* 6.7	OTA Cluster Attributes */
#if (TRUE == gBigEndian_c)
#define gZclAttrOTA_UpgradeServerId_c                 0x0000 /* M - Upgrade Server ID */
#define gZclAttrOTA_FileOffsetId_c                    0x0100 /* O - File Offset */
#define gZclAttrOTA_CurrentFileVersionId_c            0x0200 /* O - Current File Version */
#define gZclAttrOTA_CurrentZigBeeStackVersionId_c     0x0300 /* O - Current ZigBee Stack Version */
#define gZclAttrOTA_DownloadedFileVersionId_c         0x0400 /* O - Downloaded File Version */
#define gZclAttrOTA_DownloadedZigBeeStackVersionId_c  0x0500 /* O - Downloaded ZigBee Stack Version */
#define gZclAttrOTA_ImageUpgradeStatusId_c            0x0600 /* M - Image Upgrade Status */
#define gZclAttrOTA_ManufacturerId_c                  0x0700 /* O - Manufacturer Id */
#define gZclAttrOTA_ImageTypeId_c                     0x0800 /* O - Image Type Id */  
#define gZclAttrOTA_MinimumBlockRequestDelayId_c      0x0900 /* O - Minimum Block Request Delay */ 
#else
#define gZclAttrOTA_UpgradeServerId_c                 0x0000 /* M - Upgrade Server ID */
#define gZclAttrOTA_FileOffsetId_c                    0x0001 /* O - File Offset */
#define gZclAttrOTA_CurrentFileVersionId_c            0x0002 /* O - Current File Version */
#define gZclAttrOTA_CurrentZigBeeStackVersionId_c     0x0003 /* O - Current ZigBee Stack Version */
#define gZclAttrOTA_DownloadedFileVersionId_c         0x0004 /* O - Downloaded File Version */
#define gZclAttrOTA_DownloadedZigBeeStackVersionId_c  0x0005 /* O - Downloaded ZigBee Stack Version */
#define gZclAttrOTA_ImageUpgradeStatusId_c            0x0006 /* M - Image Upgrade Status */
#define gZclAttrOTA_ManufacturerId_c                  0x0007 /* O - Manufacturer Id */
#define gZclAttrOTA_ImageTypeId_c                     0x0008 /* O - Image Type Id */  
#define gZclAttrOTA_MinimumBlockRequestDelayId_c      0x0009 /* O - Minimum Block Request Delay */ 
#endif
#define gZclAttrOTASet_c                                0x00
#define gZclAttrIdOTA_UpgradeServerId_c                 0x00 /* M - Upgrade Server ID */
#define gZclAttrIdOTA_FileOffsetId_c                    0x01 /* O - File Offset */
#define gZclAttrIdOTA_CurrentFileVersionId_c            0x02 /* O - Current File Version */
#define gZclAttrIdOTA_CurrentZigBeeStackVersionId_c     0x03 /* O - Current ZigBee Stack Version */
#define gZclAttrIdOTA_DownloadedFileVersionId_c         0x04 /* O - Downloaded File Version */
#define gZclAttrIdOTA_DownloadedZigBeeStackVersionId_c  0x05 /* O - Downloaded ZigBee Stack Version */
#define gZclAttrIdOTA_ImageUpgradeStatusId_c            0x06 /* M - Image Upgrade Status */
#define gZclAttrIdOTA_ManufacturerId_c                  0x07 /* O - Manufacturer Id */
#define gZclAttrIdOTA_ImageTypeId_c                     0x08 /* O - Image Type Id */  
#define gZclAttrIdOTA_MinimumBlockRequestDelayId_c      0x09 /* O - Minimum Block Request Delay */ 

/* 6.3.2.6	Image Type */
#if (TRUE == gBigEndian_c)
#define gZclImageType_FSLSpecific_c                   0x0000 /* Manufacturer specific - 0x0000 � 0xffbf*/
#define gZclImageType_SecurityCredential_c            0xC0FF /* Security credential */
#define gZclImageType_Configuration_c                 0xC1FF /* Configuration */
#define gZclImageType_Log_c                           0xC2FF /* Log */
#define gZclImageType_WildCard_c                      0xFFFF /* Wild card */
#else
#define gZclImageType_FSLSpecific_c                   0x0000 /* Manufacturer specific - 0x0000 � 0xffbf*/
#define gZclImageType_SecurityCredential_c            0xFFC0 /* Security credential */
#define gZclImageType_Configuration_c                 0xFFC1 /* Configuration */
#define gZclImageType_Log_c                           0xFFC2 /* Log */
#define gZclImageType_WildCard_c                      0xFFFF /* Wild card */
#endif

/* 6.3.2.8	ZigBee Stack Version */
#if (TRUE == gBigEndian_c)
#define gZclStackVer_ZigBee2006_c                         0x0000 /* ZigBee 2006 */
#define gZclStackVer_ZigBee2007_c                         0x0100 /* ZigBee 2007 */
#define gZclStackVer_ZigBeePro_c                          0x0200 /* ZigBee Pro */
#define gZclStackVer_ZigBeeIP_c                           0x0300 /* ZigBee IP */
#else
#define gZclStackVer_ZigBee2006_c                         0x0000 /* ZigBee 2006 */
#define gZclStackVer_ZigBee2007_c                         0x0001 /* ZigBee 2007 */
#define gZclStackVer_ZigBeePro_c                          0x0002 /* ZigBee Pro */
#define gZclStackVer_ZigBeeIP_c                           0x0003 /* ZigBee IP */
#endif

/* USED sub elements tag ID's */
#if (TRUE == gBigEndian_c)
#define gZclOtaSectorBitmapTagId_c	0x00F0
#define gZclOtaUpgradeImageTagId_c	0x0000
#define gZclOtaUpgradeCRCTagId_c        0x00F1
#define gZclOtaECDSASignatureTagId_c    0x0100
#define gZclOtaECDSASigningCertTagId_c  0x0200
#define gZclOtaImageIntegrityCodeTagId  0x0300
#else
#define gZclOtaSectorBitmapTagId_c	0xF000
#define gZclOtaUpgradeImageTagId_c	0x0000
#define gZclOtaUpgradeCRCTagId_c        0xF100
#define gZclOtaECDSASignatureTagId_c    0x0001
#define gZclOtaECDSASigningCertTagId_c  0x0002
#define gZclOtaImageIntegrityCodeTagId  0x0003
#endif

#if (TRUE == gBigEndian_c)
#define gECDSASignatureLengthField      0x32000000
#define gECDSASigningCertLengthField    0x30000000
#define gImageIntegrityCodeLengthField  0x10000000
#else
#define gECDSASignatureLengthField      0x00000032
#define gECDSASigningCertLengthField    0x00000030
#define gImageIntegrityCodeLengthField  0x00000010
#endif

#if  (gZclEnableOTAProgressReport_d == TRUE)
#define otaStartProgress_c   0
#define otaProgress33_c      1
#define otaProgress66_c      2
#define otaProgress100_c     3
#endif

/* Supported header version*/
#if (TRUE == gBigEndian_c)
#define gZclOtaHeaderVersion_c  0x0001
#else
#define gZclOtaHeaderVersion_c  0x0100
#endif

#if (gZclEnableOTAClientECCLibrary_d)
#define SECT163K1_SIGNATURE_ELEMENT_LENGTH (21)
#endif
#define AES_MMO_SIZE (16)



typedef PACKED_STRUCT zclOTAAttrsRAM_tag
{
  zbIeeeAddr_t       UpgradeServerId;        /* Upgrade server IEEE address */
  uint8_t            ImageUpgradeStatus;     /* Upgrade image status */
#if gZclClusterOptionals_d
  uint32_t    FileOffset; 
  uint32_t    CurrentFileVersion;
  uint32_t    DownloadedFileVersion;
  uint16_t    CurrentZigBeeStackVersion;
  uint16_t    DownloadedZigBeeStackVersion;
  uint16_t    ManufacturerId;
  uint16_t    ImageType;
  uint16_t    MinimumBlockReqDelay;
#endif
} zclOTAAttrsRAM_t;

/* 6.8	OTA Server Cluster Parameters */
typedef PACKED_STRUCT zclOTAServerInitParams_tag
{
  uint8_t   querryJitter;
  uint32_t  currentTime;
  uint32_t  upgradeRequestTime;
  uint32_t  upgradeTime;
  uint16_t  blockRequestDelay;    
}zclOTAServerInitParams_t;

typedef PACKED_STRUCT ztcServerUtility_tag
{
  uint8_t   operationMode;  /*  with external memory, without it  */
#if gZclOtaExternalMemorySupported_d  
  uint8_t   imageLoadingState;  /* start, loading, complete, noImage */
  uint32_t  currentAddress;     /* current address to write block of image */
  uint8_t   localFileVersionPolicies; /* local file version Server policies: upgrade and/or downgrade and/or reinstall */
#endif 
  bool_t    otaServerIsActive;   /* ota server is active or not  - used for dongle solution*/  
  bool_t    blockResponseInProgress; /* block send (or not) to client over the air*/
}ztcServerUtility_t;

#if gZclOtaExternalMemorySupported_d 
typedef PACKED_STRUCT ztcOtaServerImageListParam_tag
{
  uint32_t  addressOfCurrentImage;   /* address Of Current Image */
  uint16_t  imageType;               /* image type */
  uint32_t  totalImageSize;          /* total image size */
  uint16_t  manufacturerCode;        /* manufacturer code */ 
  uint32_t  fileVersion;             /* file version  */ 
}ztcOtaServerImageListParam_t;
#endif


/* OTA Client Cluster Parameters */
typedef PACKED_STRUCT zclOTAClientParams_tag
{
  uint16_t manufacturerCode;
  uint16_t imageType;
  uint32_t currentFileVersion;
  uint32_t downloadedFileVersion;
  uint16_t hardwareVersion;
  uint8_t  maxDataSize;
} zclOTAClientParams_t;

/* OTA Client upgrade session states */
typedef enum clientSessionState_tag
{
  OTAClient_InitState_c,
  OTAClient_ProcessHeaderState_c,
  OTAClient_ProcessUpgradeImageState_c,
  OTAClient_ProcessSubElementTagState_c, 
  OTAClient_ProcessBitmapState_c,
#if (!gZclEnableOTAClientECCLibrary_d)
  OTAClient_ProcessImgIntegrityCodeState_c,   
  OTAClient_ProcessCRCState_c,
#else
  OTAClient_ProcessECDSASignCertState_c,
  OTAClient_ProcessECDSASigningState_c,
#endif  
  OTAClient_StateMax_c
} clientSessionState_t;

typedef PACKED_STRUCT zclOTAClientSession_tag
{
  bool_t                sessionStarted;
  uint32_t              fileLength;
  uint32_t              currentOffset;
  uint32_t              downloadingFileVersion;
  uint8_t               bytesNeededForState;
  uint8_t*              pStateBuffer;
  uint8_t               stateBufferIndex;  
  clientSessionState_t  state;  
  uint8_t               steps;
  uint8_t               serverEndpoint;
  uint16_t              blockRequestDelay;
}zclOTAClientSession_t;

typedef PACKED_STRUCT zclOTABlockCallbackState_tag
{
  uint8_t               dstAddr[2];
  uint8_t               dstEndpoint;
  uint8_t               blockSize;
  uint8_t               blockData[1];
} zclOTABlockCallbackState_t;

typedef PACKED_STRUCT zclInitiateOtaClientProcess_tag
{
  uint8_t endPoint;
} zclInitiateOtaClientProcess_t;

typedef PACKED_STRUCT zclInitiateOtaProcess_tag
{
  bool_t isServer;  
  zclInitiateOtaClientProcess_t clientInit;
  zclOTAServerInitParams_t  serverInit;  
}zclInitiateOtaProcess_t;

#if (gZclEnableOTAProgressReportToExternalApp_d == TRUE)
typedef PACKED_STRUCT zclZtcImageOtaProgressReport_tag
{
  zbNwkAddr_t  deviceAddr;
  uint32_t     currentOffset;
  uint32_t     imageLength;
}zclZtcImageOtaProgressReport_t;
#endif

typedef PACKED_STRUCT ztcOtaServerQueryImageRequest_tag
{
  zbNwkAddr_t  deviceID;
  uint16_t     manufacturerCode; 
  uint16_t     imageType;
  uint32_t     fileVersion;
}ztcOtaServerQueryImageRequest_t;

typedef PACKED_STRUCT ztcOtaServerQueryImageRsp_tag
{
  uint8_t      imageStatus;
  zbNwkAddr_t  deviceID;
  uint16_t     manufacturerCode; 
  uint16_t     imageType;
  uint32_t     fileVersion;
  uint32_t     totalImageSize;
}ztcOtaServerQueryImageRsp_t;

typedef PACKED_STRUCT ztcOtaServerChunkRequest_tag 
{
  zbNwkAddr_t  deviceID;
  uint32_t     currentOffset;
  uint8_t      blockSize;
}ztcOtaServerChunkRequest_t;

typedef PACKED_STRUCT ztcOtaServerImageNotify_tag 
{
  zbNwkAddr_t  deviceID;
  uint16_t     manufacturerCode;
  uint16_t     imageType;
  uint32_t     fileVersion;
}ztcOtaServerImageNotify_t;

typedef PACKED_STRUCT zclStartClientNextImageTransfer_tag
{
  uint8_t multipleUpgradeImage;
}zclStartClientNextImageTransfer_t;

extern bool_t gApsAckConfirmStatus;

typedef PACKED_STRUCT zclOTAFileHeader_tag
{
    uint32_t	fileIdentifier;
    uint16_t	headerVersion;
    uint16_t	headerLength;
    uint16_t	fieldControl;
    uint16_t	manufacturerCode;
    uint16_t	imageType;
    uint32_t	fileVersion;
    uint16_t	stackVersion;
    uint8_t	headerString[32];
    uint32_t	totalImageSize;
    uint16_t    minHWVersion;
    uint16_t    maxHWVersion;
}zclOTAFileHeader_t;

typedef PACKED_STRUCT zclOTAFileSubElement_tag
{
	uint16_t	id;
	uint32_t        length;
}zclOTAFileSubElement_t;

typedef PACKED_STRUCT zclOTaImageIntegritySubElement_tag
{
  	uint16_t	id;
	uint32_t        length;
        uint8_t         hash[AES_MMO_SIZE];
}zclOTaImageIntegritySubElement_t;

/* 6.10.2	OTA Cluster Status Codes */
typedef uint8_t zclOTAStatus_t;

#define gZclOTASuccess_c              0x00 /* Success Operation */
#define gZclOTAAbort_c                0x95 /* Failed case when a client or a server decides to abort the upgrade process. */
#define gZclOTANotAuthorized_c        0x7E /* Server is not authorized to upgrade the client. */
#define gZclOTAInvalidImage_c         0x96 /* Invalid OTA upgrade image. */
#define gZclOTAWaitForData_c          0x97 /* Server does not have data block available yet. */
#define gZclOTANoImageAvailable_c     0x98 /* No OTA upgrade image available for a particular client. */
#define gZclOTAMalformedCommand_c     0x80 /* The command received is badly formatted. */
#define gZclOTAUnsupClusterCommand_c  0x81 /* Such command is not supported on the device. */
#define gZclOTARequireMoreImage_c     0x99 /* The client still requires more OTA upgrade image files in order to successfully upgrade. */

/*  Ota Server states:  */
enum{
   gOtaServerLoadingImageStart_c,
   gOtaServerLoadingImageProcess_c,
   gOtaServerLoadingImageComplete_c,
};

/*  Operation Modes  */
enum{
  gUpgradeImageOnCurrentDevice_c,
  gUseExternalMemoryForOtaUpdate_c,
  gDoNotUseExternalMemoryForOtaUpdate_c,
  gOtaTestingProcess
};

/* Commands definition */

/********************************************/
/*                                          */
/* 6.10.3	Image Notify Command              */
/*                                          */
/********************************************/
typedef enum
{
  gJitter_c             = 0x00,
  gJitterMfg_c          = 0x01,
  gJitterMfgImage_c     = 0x02,
  gJitterMfgImageFile_c = 0x03,
  gPayloadMax_c         = 0x04
} zclOTAImageNotifyPayload_t;

#define gHeaderStrLength_c             32

typedef PACKED_STRUCT zclOTAImageNotify_tag {
  zclOTAImageNotifyPayload_t payloadType;
  uint8_t   queryJitter;
  uint16_t  manufacturerCode;
  uint16_t  imageType;
  uint32_t  fileVersion;
} zclOTAImageNotify_t;

/* ZTC support structure for the image notify command */
typedef PACKED_STRUCT zclZtcOTAImageNotify_tag {
  zbNwkAddr_t   aNwkAddr;
  zbEndPoint_t  endPoint;
  zclOTAImageNotify_t zclOTAImageNotify;
} zclZtcOTAImageNotify_t;

/********************************************/
/*                                          */
/* 6.10.4	Query Next Image Request Command  */
/*                                          */
/********************************************/
typedef PACKED_STRUCT zclOTANextImageRequest_tag
{
  uint8_t   fieldControl;
  uint16_t  manufacturerCode;
  uint16_t  imageType;
  uint32_t  fileVersion;
  uint16_t  hardwareVersion;
} zclOTANextImageRequest_t;

/* ZTC support structure for the querry next image request*/
typedef PACKED_STRUCT zclZtcOTANextImageRequest_tag {
  zbNwkAddr_t   aNwkAddr;
  zbEndPoint_t  endPoint;
  zclOTANextImageRequest_t zclOTANextImageRequest;
} zclZtcOTANextImageRequest_t;

/********************************************/
/*                                          */
/* 6.10.5	Query Next Image Response Command */
/*                                          */
/********************************************/
typedef PACKED_STRUCT zclOTANextImageResponse_tag
{
  zclOTAStatus_t  status;
  uint16_t        manufacturerCode;
  uint16_t        imageType;
  uint32_t        fileVersion;
  uint32_t        imageSize;
} zclOTANextImageResponse_t;

/* ZTC support structure for the querry next image response*/
typedef PACKED_STRUCT zclZtcOTANextImageResponse_tag {
  zbNwkAddr_t   aNwkAddr;
  zbEndPoint_t  endPoint;
  zclOTANextImageResponse_t zclOTANextImageResponse;
} zclZtcOTANextImageResponse_t;


/********************************************/
/*                                          */
/* 6.10.6	Image Block Request Command       */
/*                                          */
/********************************************/

typedef PACKED_STRUCT zclOTABlockRequest_tag
{
  uint8_t   fieldControl;     /* Always 0 for now*/
  uint16_t  manufacturerCode;
  uint16_t  imageType;
  uint32_t  fileVersion;
  uint32_t  fileOffset;
  uint8_t   maxDataSize;
  uint16_t  blockRequestDelay;
} zclOTABlockRequest_t;

/* ZTC support structure for the block request*/
typedef PACKED_STRUCT zclZtcOTABlockRequest_tag
{
  zbNwkAddr_t   aNwkAddr;
  zbEndPoint_t  endPoint;
  zclOTABlockRequest_t  zclOTABlockRequest;
} zclZtcOTABlockRequest_t;

/* 6.10.7	Image Page Request Command */
/* OPTIONAL COMMAND, CURRENTLY NOT SUPPORTED */

/********************************************/
/*                                          */
/* 6.10.8	Image Block Response Command      */
/*                                          */
/********************************************/
typedef PACKED_STRUCT zclOTABlockResponseStatusWait_tag
{
  uint32_t  currentTime;
  uint32_t  requestTime;
  uint16_t  blockRequestDelay;
} zclOTABlockResponseStatusWait_t;

typedef PACKED_STRUCT zclOTABlockResponseStatusSuccess_tag
{
  uint16_t  manufacturerCode;
  uint16_t  imageType;
  uint32_t  fileVersion;
  uint32_t  fileOffset;
  uint8_t   dataSize;
  uint8_t   data[1];
} zclOTABlockResponseStatusSuccess_t;

typedef PACKED_STRUCT zclOTABlockResponse_tag
{
  zclOTAStatus_t  status;
  PACKED_UNION {
    zclOTABlockResponseStatusWait_t     wait;   /* Valid when the command status is "WaitForData" */
    zclOTABlockResponseStatusSuccess_t  success; /* Valid when the command status is "Success" */
  } msgData;
}zclOTABlockResponse_t;

/* ZTC support structure for the block response*/
typedef PACKED_STRUCT zclZtcOTABlockResponse_tag
{
  zbNwkAddr_t   aNwkAddr;
  zbEndPoint_t  endPoint;
  zclOTABlockResponse_t  zclOTABlockResponse;
} zclZtcOTABlockResponse_t;

/********************************************/
/*                                          */
/* 6.10.9	Upgrade End Request Command       */
/*                                          */
/********************************************/
typedef PACKED_STRUCT zclOTAUpgradeEndRequest_tag
{
  zclOTAStatus_t  status;
  uint16_t        manufacturerCode;
  uint16_t        imageType;
  uint32_t        fileVersion;
} zclOTAUpgradeEndRequest_t;

/* ZTC support structure for the upgrade end request*/
typedef PACKED_STRUCT zclZtcOTAUpgradeEndRequest_tag
{
  zbNwkAddr_t   aNwkAddr;
  zbEndPoint_t  endPoint;
  zclOTAUpgradeEndRequest_t zclOTAUpgradeEndRequest;
} zclZtcOTAUpgradeEndRequest_t;


/********************************************/
/*                                          */
/* 6.10.10	Upgrade End Response Command    */
/*                                          */
/********************************************/
typedef PACKED_STRUCT zclOTAUpgradeEndResponse_tag
{
  uint16_t        manufacturerCode;
  uint16_t        imageType;
  uint32_t        fileVersion;
  uint32_t        currentTime;
  uint32_t        upgradeTime;
} zclOTAUpgradeEndResponse_t;

typedef PACKED_STRUCT ztcZclOTAUpgradeEndResponse_tag
{
  zbNwkAddr_t   aNwkAddr;
  zbEndPoint_t  endPoint;
  zclOTAUpgradeEndResponse_t zclOTAUpgradeEndResponse;
}ztcZclOTAUpgdareEndResponse_t;

/* 6.10.11	Query Specific File Request Command */
/* OPTIONAL COMMAND, CURRENTLY NOT SUPPORTED */

/* 6.10.12	Query Specific File Response Command */
/* OPTIONAL COMMAND, CURRENTLY NOT SUPPORTED */

/* ZCL ZTC structures */

/**/
typedef PACKED_STRUCT zclQueryReqList_tag
{
  zbNwkAddr_t   aNwkAddr;
  zbEndPoint_t  endPoint; 
}zclQueryReqList_t;

typedef PACKED_STRUCT zclAbortBlockRspList_tag
{
  bool_t        status;          /*slot free:  TRUE, otherwise FALSE */       
  zbNwkAddr_t   aNwkAddr;
}zclAbortBlockRspList_t;

/******************************************************************************
*******************************************************************************
* Public functions prototypes
*******************************************************************************
******************************************************************************/
extern const zclAttrDef_t gaZclOTAClusterAttrDef[];
extern const zclAttrDefList_t gZclOTAClusterAttrDefList;

#if gAddValidationFuncPtrToClusterDef_c
bool_t  ZCL_OTAValidateAttributes(zbEndPoint_t endPoint, zbClusterId_t clusterId, void *pAttrDef);
#else
#define ZCL_OTAValidateAttributes
#endif

zbStatus_t ZCL_OTAClusterServer( zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_OTAClusterClient( zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDevice);
zbStatus_t ZCL_OTAImageNotify(zclZtcOTAImageNotify_t* pZtcImageNotifyParams);
#if gZclEnableOTAServer_d
zbStatus_t newImageReceived(ztcOtaServerImageNotify_t imgNotify);
zbStatus_t ZtcOtaServerBlockReceive(uint8_t *dataBlock, uint8_t length);
zbStatus_t OTAServerStartImageProcess(uint8_t operationMode);
zbStatus_t OTAServerQueryImageRsp(uint8_t *queryImageRsp);
zbStatus_t OTAServerImageNotify(uint8_t *imageNotify);
zbStatus_t OTAServer_CloseOTAProcess(void);
zbStatus_t OTAServer_SetLocalFileVersionPolicies(uint8_t localFileVersionPolicies);
zbStatus_t OTAServer_AddToAbortBlockRspList(zbNwkAddr_t   deviceID) ;
#endif
zbStatus_t ZCL_OTASetClientParams(zclOTAClientParams_t* pClientParams);
zbStatus_t ZCL_OTANextImageResponse(zclZtcOTANextImageResponse_t* pZtcNextImageResponseParams);
zbStatus_t ZCL_OTABlockResponse(zclZtcOTABlockResponse_t *pZtcBlockResponseParams);
zbStatus_t ZCL_OTAUpgradeEndResponse(ztcZclOTAUpgdareEndResponse_t* pZtcOTAUpgdareEndResponse);

#if (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d || gZclOtaServerImgIntegrityCodeValidation_d) 
void aesHashNextBlock(uint8_t* input, uint8_t *keyInit, uint8_t* output);
void OTAClusterAesMMO_hash(uint8_t *blockToProcess, uint8_t length, bool_t lastBlock, uint32_t totalLength, uint8_t *hashValue);
#endif /* (gZclEnableOTAClientECCLibrary_d || gZclOtaClientImgIntegrityCodeValidation_d || gZclOtaServerImgIntegrityCodeValidation_d) */

#if gZclEnableOTAClient_d && gZclEnableOTAClientECCLibrary_d
zbStatus_t OtaSignatureVerification(uint8_t *signerIEEE,IdentifyCert_t certificate, uint8_t* signature);
#endif /* gZclEnableOTAClient_d && gZclEnableOTAClientECCLibrary_d */

zbStatus_t OTA_InitiateOtaClusterProcess (zclInitiateOtaProcess_t* initiateOtaProcess);

#if gZclEnableOTAClient_d
zbStatus_t ZCL_OTAImageRequest(zclZtcOTANextImageRequest_t* pZtcNextImageRequestParams);
zbStatus_t ZCL_OTABlockRequest(zclZtcOTABlockRequest_t *pZtcBlockRequestParams);
zbStatus_t ZCL_OTAUpgradeEndRequest(zclZtcOTAUpgradeEndRequest_t *pZtcUpgradeEndParams);
zbStatus_t OTA_OtaStartClientNextImageTransfer(zclStartClientNextImageTransfer_t* startClientNextImageTransfer);
bool_t InterpretOtaMatchDescriptor(zbNwkAddr_t  aDestAddress, uint8_t endPoint);
void InterpretOtaIEEEAddrReq(zbIeeeAddr_t  aIeeeAddr);
#endif
#endif
