/*! @file 	ZclSEKeyEstab.h
 *
 * @brief	Types, definitions and prototypes for the Key Establishment cluster implementation.
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
 *			o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   		contributors may be used to endorse or promote products derived from this
 *   		software without specific prior written permission.
 *
 *			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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

#ifndef _ZCLSEKEYESTAB_H
#define _ZCLSEKEYESTAB_H

/******************************************************************************
*******************************************************************************
* Public Macros
*******************************************************************************
******************************************************************************/

/************************************************************************************
*************************************************************************************
* Public type definitions
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
************************************************************************************/

/*!
 * @fn 		zbStatus_t ZCL_KeyEstabClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Key Establishment Cluster client, 
 *			only one session at a time is supported.
 *
 */
zbStatus_t ZCL_KeyEstabClusterClient(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		zbStatus_t ZCL_KeyEstabClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev) 
 *
 * @brief	Processes the requests received on the Key Establishment Cluster server, 
 *			only one session at a time is supported.
 *
 */
zbStatus_t ZCL_KeyEstabClusterServer(zbApsdeDataIndication_t *pIndication, afDeviceDef_t *pDev);

/*!
 * @fn 		bool_t ZCL_InitiateKeyEstab(zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr) 
 *
 * @brief	Initiates the CBKE process with a remote node
 *
 */
bool_t ZCL_InitiateKeyEstab(zbEndPoint_t DstEndpoint,zbEndPoint_t SrcEndpoint, zbNwkAddr_t DstAddr);

/*!
 * @fn 		zbStatus_t zclKeyEstab_InitKeyEstabReq(ZclKeyEstab_InitKeyEstabReq_t *pReq) 
 *
 * @brief	Sends a Initiate Key Establishment Request over-the-air
 *
 */
zbStatus_t zclKeyEstab_InitKeyEstabReq(ZclKeyEstab_InitKeyEstabReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_EphemeralDataReq(ZclKeyEstab_EphemeralDataReq_t *pReq) 
 *
 * @brief	Sends over-the-air an Ephemeral Data Request from the client
 *
 */
zbStatus_t zclKeyEstab_EphemeralDataReq(ZclKeyEstab_EphemeralDataReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_ConfirmKeyDataReq(ZclKeyEstab_ConfirmKeyDataReq_t *pReq) 
 *
 * @brief	Sends over-the-air an Confirm Key Data Request from the client
 *
 */
zbStatus_t zclKeyEstab_ConfirmKeyDataReq(ZclKeyEstab_ConfirmKeyDataReq_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_TerminateKeyEstabServer(ZclKeyEstab_TerminateKeyEstabServer_t *pReq) 
 *
 * @brief	Sends over-the-air a Terminate Key Establishment from the client
 *
 */
zbStatus_t zclKeyEstab_TerminateKeyEstabServer(ZclKeyEstab_TerminateKeyEstabServer_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_InitKeyEstabRsp(ZclKeyEstab_InitKeyEstabRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an Initiate Key Establishment Response from the server
 *
 */
zbStatus_t zclKeyEstab_InitKeyEstabRsp(ZclKeyEstab_InitKeyEstabRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_EphemeralDataRsp(ZclKeyEstab_EphemeralDataRsp_t *pReq) 
 *
 * @brief	Sends over-the-air an Ephemeral Data Response from the server
 *
 */
zbStatus_t zclKeyEstab_EphemeralDataRsp(ZclKeyEstab_EphemeralDataRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_ConfirmKeyDataRsp(ZclKeyEstab_ConfirmKeyDataRsp_t *pReq) 
 *
 * @brief	Sends over-the-air a Confirm Key Data Response from the server
 *
 */
zbStatus_t zclKeyEstab_ConfirmKeyDataRsp(ZclKeyEstab_ConfirmKeyDataRsp_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_TerminateKeyEstabClient(ZclKeyEstab_TerminateKeyEstabClient_t *pReq) 
 *
 * @brief	Sends over-the-air a Terminate Key Establishment from the server
 *
 */
zbStatus_t zclKeyEstab_TerminateKeyEstabClient(ZclKeyEstab_TerminateKeyEstabClient_t *pReq);

/*!
 * @fn 		zbStatus_t zclKeyEstab_SetSecurityMaterial(ZclKeyEstab_SetSecurityMaterial_t *pReq) 
 *
 * @brief	Sets the CBKE Security Material (Certificate, Private Key, Public Key)
 *
 */
zbStatus_t zclKeyEstab_SetSecurityMaterial(ZclKeyEstab_SetSecurityMaterial_t *pReq);

/*******************************************************************************
********************************************************************************
* Public memory declarations
********************************************************************************
*******************************************************************************/
extern const zclAttrDef_t gZclKeyEstabServerAttrDef[];


#endif /* _ZCLSEKEYESTAB_H */
