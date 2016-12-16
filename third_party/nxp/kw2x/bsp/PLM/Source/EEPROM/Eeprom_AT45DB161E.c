/******************************************************************************
 * Filename: Eeprom_AT45DB161E.c
 *
 * Description: EEPROM implementation file for ARM CORTEX-M4 based platforms
 *
 * Copyright (c) 2013, Freescale Semiconductor, Inc. All rights reserved.
 *
 ******************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/
 
#include "EmbeddedTypes.h"
#include "PortConfig.h"
#include "Eeprom.h"

#if gEepromType_d == gEepromDevice_AT45DB161E_c
/******************************************************************************
*******************************************************************************
* Private Macros
*******************************************************************************
******************************************************************************/
#define  gEepromWriteEnable_d   TRUE

#define  EEPROM_RDSR            0xD7
#define  EEPROM_READ            0x03
#define  EEPROM_READ_HF         0x0B

#define  EEPROM_ERASE_512B      0x81
#define  EEPROM_ERASE_4K        0x50

#define  EEPROM_WRITE_BYTES     0x02

#define  EEPROM_BUSY_FLAG_MASK  0x80
#define  EEPROM_PAGE_SIZE       (512)
#define  EEPROM_PAGE_MASK       (EEPROM_PAGE_SIZE - 1)

/* adress mask */
#define  ADDRESS_MASK 0x000000FF


/****************************** SPI MACROS ***********************************/
#define KEEP_CS_ASSERTED          1
#define DEASSERT_CS               0

#define gSPI_BaudRate_d           (SPI_CTAR_PBR(0)    | SPI_CTAR_BR(0x01))
#define gSPI_BeforeTransfDelay_d  (SPI_CTAR_PCSSCK(1) | SPI_CTAR_CSSCK(0x04))
#define gSPI_AfterTransfDelay_d   (SPI_CTAR_PASC(3)   | SPI_CTAR_ASC(0x04))
#define gSPI_InterTransfDelay_d   (SPI_CTAR_PDT(3)    | SPI_CTAR_DT(0x05))


#if defined(MCU_MK60D10) || defined(MCU_MK60N512VMD100)
  #define EEPROM_SPI_MCR   SPI2_MCR
  #define EEPROM_SPI_SR    SPI2_SR
  #define EEPROM_SPI_CTAR0 SPI2_CTAR0
  #define EEPROM_SPI_PUSHR SPI2_PUSHR
  #define EEPROM_SPI_POPR  SPI2_POPR

#elif defined(MCU_MK21DN512) || defined(MCU_MK21DX256)
  #define EEPROM_SPI_MCR   SPI0_MCR
  #define EEPROM_SPI_SR    SPI0_SR
  #define EEPROM_SPI_CTAR0 SPI0_CTAR0
  #define EEPROM_SPI_PUSHR SPI0_PUSHR
  #define EEPROM_SPI_POPR  SPI0_POPR
#endif

#define EEPROM_FlushSPIFifo()     EEPROM_SPI_MCR |= (uint32_t)((SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK))
#define EEPROM_StartSPITransfer() EEPROM_SPI_MCR &= (uint32_t)(~(SPI_MCR_HALT_MASK))
#define EEPROM_StopSPITransfer()  EEPROM_SPI_MCR |= (uint32_t)((SPI_MCR_HALT_MASK))

/******************************************************************************
*******************************************************************************
* Private Prototypes
*******************************************************************************
******************************************************************************/
#if gEepromWriteEnable_d
static ee_err_t EEPROM_PrepareForWrite(uint32_t NoOfBytes, uint32_t Addr);
static ee_err_t EEPROM_WritePage(uint32_t NoOfBytes, uint32_t Addr, uint8_t *Outbuf);
#endif
static ee_err_t EEPROM_WaitForReady(void);
static uint16_t EEPROM_ReadStatusReq(void);
static void     EEPROM_Command(uint8_t opCode, uint32_t Addr);

static void     SPI_Init(void);
static uint8_t  SPI_Receive_byte(uint8_t);
static void     SPI_Send_byte(uint8_t data, uint8_t);

/******************************************************************************
*******************************************************************************
* Private type definitions
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Private Memory Declarations
*******************************************************************************
******************************************************************************/
#if gEepromWriteEnable_d
static uint8_t mEepromEraseBitmap[64];
#endif

/******************************************************************************
*******************************************************************************
* Public Memory
*******************************************************************************
******************************************************************************/

/******************************************************************************
*******************************************************************************
* Public Functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
*  EEPROM_Init
*
*  Initializes the EEPROM peripheral
*
*****************************************************************************/
ee_err_t EEPROM_Init(void)
{
  ee_err_t retval;
#if gEepromWriteEnable_d
  uint32_t i;

  // Mark Flash as Unerased
  for(i = 0; i < 64; i++)
    mEepromEraseBitmap[i] = 0;
#endif

  SPI_Init();

  retval = EEPROM_WaitForReady();
  if (retval != ee_ok)
    return retval;
  
  // Set page size to 512bits:
  SPI_Send_byte(0x3D, KEEP_CS_ASSERTED);
  SPI_Send_byte(0x2A, KEEP_CS_ASSERTED);
  SPI_Send_byte(0x80, KEEP_CS_ASSERTED);
  SPI_Send_byte(0xA6, DEASSERT_CS);
  
  return ee_ok;
}

/*****************************************************************************
*  EEPROM_ChipErase
*
*  Erase all memory to 0xFF
*
*****************************************************************************/
ee_err_t EEPROM_ChipErase(void)
{
  while (EEPROM_isBusy());

  SPI_Send_byte(0xC7, KEEP_CS_ASSERTED);
  SPI_Send_byte(0x94, KEEP_CS_ASSERTED);
  SPI_Send_byte(0x80, KEEP_CS_ASSERTED);
  SPI_Send_byte(0x9A, DEASSERT_CS);

  return ee_ok;
}

/*****************************************************************************
*  EEPROM_EraseBlock
*
*  Erase a block of memory to 0xFF
*
*****************************************************************************/
ee_err_t EEPROM_EraseBlock(uint32_t Addr, uint32_t size)
{
  uint8_t cmd;

  while (EEPROM_isBusy());

  switch (size)
  {
    case (4*1024):
      cmd = EEPROM_ERASE_4K;
      break;
    case (512):
      cmd = EEPROM_ERASE_512B;
      break;
    default:
      return ee_error;
  }

  SPI_Send_byte(cmd,                                     KEEP_CS_ASSERTED);
  SPI_Send_byte((uint8_t) ((Addr >> 16) & ADDRESS_MASK), KEEP_CS_ASSERTED);
  SPI_Send_byte((uint8_t) ((Addr >> 8)  & ADDRESS_MASK), KEEP_CS_ASSERTED);
  SPI_Send_byte((uint8_t) ((Addr >> 0)  & ADDRESS_MASK), DEASSERT_CS);

  return ee_ok;
}

/*****************************************************************************
*  EEPROM_WriteData
*
*  Writes a data buffer into EEPROM, at a given address
*
*****************************************************************************/
#if gEepromWriteEnable_d
ee_err_t EEPROM_WriteData(uint32_t NoOfBytes, uint32_t Addr, uint8_t *Outbuf)
{
  ee_err_t retval;

  if (NoOfBytes == 0)
    return ee_ok;

  while (EEPROM_isBusy());

  retval = EEPROM_PrepareForWrite(NoOfBytes, Addr);
  if (retval != ee_ok)
    return retval;

  while ((Addr & EEPROM_PAGE_MASK) + NoOfBytes > EEPROM_PAGE_MASK)
  {
    uint32_t bytes = EEPROM_PAGE_SIZE - (Addr & EEPROM_PAGE_MASK);

    retval = EEPROM_WritePage(bytes, Addr, Outbuf);
    NoOfBytes -= bytes;
    Addr += bytes;
    Outbuf += bytes;

    if (retval != ee_ok)
      return retval;
  }

  retval = EEPROM_WritePage(NoOfBytes, Addr, Outbuf);

  return retval;
}
#endif

/*****************************************************************************
*  EEPROM_ReadData
*
*  Reads a data buffer from EEPROM, from a given address
*
*****************************************************************************/
ee_err_t EEPROM_ReadData(uint16_t NoOfBytes, uint32_t Addr, uint8_t *inbuf)
{

  while (EEPROM_isBusy());

  EEPROM_Command(EEPROM_READ,Addr);

  while(NoOfBytes-- > 1)
  {
      *inbuf++ = SPI_Receive_byte(KEEP_CS_ASSERTED);
  }

  *inbuf++ = SPI_Receive_byte(DEASSERT_CS);

  return ee_ok;
}

/*****************************************************************************
*  EEPROM_ReadStatusReq
*
*
*****************************************************************************/
uint8_t EEPROM_isBusy(void)
{
  return !(EEPROM_ReadStatusReq() & EEPROM_BUSY_FLAG_MASK);
}

/******************************************************************************
*******************************************************************************
* Private Functions
*******************************************************************************
******************************************************************************/

/*****************************************************************************
*  EEPROM_ReadStatusReq
*
*
*****************************************************************************/
static uint16_t EEPROM_ReadStatusReq(void)
{
  uint16_t status;

  SPI_Send_byte(EEPROM_RDSR,KEEP_CS_ASSERTED);
  status = SPI_Receive_byte(KEEP_CS_ASSERTED);
  status <<= 8;
  status = SPI_Receive_byte(DEASSERT_CS);

  return status;
}

/*****************************************************************************
*  EEPROM_WriteData
*
*  Writes a data buffer into the External Memory, at a given address
*
*****************************************************************************/
#if gEepromWriteEnable_d
static ee_err_t EEPROM_PrepareForWrite(uint32_t NoOfBytes, uint32_t Addr)
{
  uint32_t i;
  ee_err_t ret = ee_ok;
  uint32_t startBlk, endBlk;

  // Obtain the block number
  startBlk = Addr >> 12;
  endBlk   = (Addr + NoOfBytes) >> 12;

  for(i = startBlk; i <= endBlk; i++)
    if ( (mEepromEraseBitmap[i/8] & (1 << (i%8) ) ) == 0)
    {
      ret = EEPROM_EraseBlock(i * 4096, 4096);
      if (ret != ee_ok)
        return ret;

      mEepromEraseBitmap[i/8] |= 1 << (i%8);
      while (EEPROM_isBusy());
    }

  return ret;
}
#endif

/*****************************************************************************
*  EEPROM_WritePage
*
*  Writes maximum 256 bytes into a EEPROM page
*
*****************************************************************************/
#if gEepromWriteEnable_d
static ee_err_t EEPROM_WritePage(uint32_t NoOfBytes, uint32_t Addr, uint8_t *Outbuf)
{

  if (NoOfBytes == 0)
    return ee_ok;

  while (EEPROM_isBusy());

  //SPI_Send_byte(EEPROM_WRITE_ENABLE,DEASSERT_CS);

  EEPROM_Command(EEPROM_WRITE_BYTES, Addr); //CS will remain ASSERTED

  while(NoOfBytes-- > 1)
  {
    SPI_Send_byte((uint8_t)(*Outbuf++),KEEP_CS_ASSERTED);
  }
  SPI_Send_byte((uint8_t)(*Outbuf++),DEASSERT_CS);

  return ee_ok;
}
#endif

/*****************************************************************************
*  EEPROM_WaitForReady
*
*
*****************************************************************************/
static ee_err_t EEPROM_WaitForReady(void)
{
  volatile uint16_t wait = 0x400; // near 50 ms @ 50 us/cycle

  /* Byte1 - Bit:   7       6      5:2       1         0
   *             RDY/BUSY  COMP  DENSITY  PROTECT  PAGE_SIZE
   *
   * Byte2 - Bit:   7       6    5    4    3    2    1    0
   *             RDY/BUSY  RES  EPE  RES  SLE  PS2  PS1  ES
   */

  while( EEPROM_isBusy() && (wait !=0) )
  {
    wait--;
  }

  if(wait != 0)
  {
    return ee_ok;
  }
  return ee_error;
}


/*****************************************************************************
*  EEPROM_Command
*
*
*****************************************************************************/
static void EEPROM_Command(uint8_t opCode, uint32_t Addr)
{
  SPI_Send_byte(opCode,                                  KEEP_CS_ASSERTED);
  SPI_Send_byte((uint8_t) ((Addr >> 16) & ADDRESS_MASK), KEEP_CS_ASSERTED);
  SPI_Send_byte((uint8_t) ((Addr >> 8)  & ADDRESS_MASK), KEEP_CS_ASSERTED);
  SPI_Send_byte((uint8_t) ((Addr >> 0)  & ADDRESS_MASK), KEEP_CS_ASSERTED);
  /* CS will remain asserted to be able to send the data */
}



/*********************************************************
* Name: SPI_Init
* Desc: Initialize SPI2 Module
* Parameter: None
* Return: None
**********************************************************/
static void SPI_Init(void)
{

#if defined(MCU_MK60D10) || defined(MCU_MK60N512VMD100)
   /* Enable clock gate to SPI module */
   SIM_SCGC3 |= SIM_SCGC3_SPI2_MASK;

   /* Enable clock for PORTD GPIOs */
   SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;

   /* set PORTD pin 2 to SPI2.SCK*/
   PORTD_PCR12 =  PORT_PCR_MUX(2);
   /* set PORTD pin 1 to SPI2.SOUT*/
   PORTD_PCR13 =  PORT_PCR_MUX(2);
   /* set PORTD pin 3 to SPI2.SIN*/
   PORTD_PCR14 =  PORT_PCR_MUX(2);
   /* set PORTD pin 11 to SPI2.CS0*/
   PORTD_PCR11 =  PORT_PCR_MUX(2);

#elif defined(MCU_MK21DN512) || defined(MCU_MK21DX256)

   /* Enable clock gate to SPI module */
   SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;

   /* Enable clock for PORTC GPIOs */
   SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;

   /* set PORTC pin 4 to SPI0.CS0*/
   PORTC_PCR4 =  PORT_PCR_MUX(2);
   /* set PORTC pin 5 to SPI0.SCK*/
   PORTC_PCR5 =  PORT_PCR_MUX(2);
   /* set PORTC pin 6 to SPI0.SOUT*/
   PORTC_PCR6 =  PORT_PCR_MUX(2);
   /* set PORTC pin 7 to SPI0.SIN*/
   PORTC_PCR7 =  PORT_PCR_MUX(2);

#endif
   /* Enable master mode, disable both transmit and receive FIFO buffers,
    * set the inactive state for PCS2 high, enable the module (MDIS = 0),
    * delay the sample point from the leading edge of the clock and halt
    * any transfer
    */
   EEPROM_SPI_MCR = ( SPI_MCR_MSTR_MASK   |
                      SPI_MCR_ROOE_MASK   |
                      SPI_MCR_PCSIS(1)    |
                      SPI_MCR_SMPL_PT(2)  |
                      SPI_MCR_HALT_MASK);

   EEPROM_SPI_MCR |= (SPI_MCR_CLR_RXF_MASK | SPI_MCR_CLR_TXF_MASK);
}

/*********************************************************
* Name: SPI_Send_uint8_t
* Desc: Send one uint8_t
* Parameter: The uint8_t to be sent
* Return: None
**********************************************************/
static void SPI_Send_byte(uint8_t u8Data, uint8_t contPCSEn)
{
   EEPROM_FlushSPIFifo();

   /* Clear the EOQF, TCF and RFDF flags by writting a 1 to them */
   EEPROM_SPI_SR |= SPI_SR_EOQF_MASK | SPI_SR_TCF_MASK | SPI_SR_RFDF_MASK;

   /* Write the clock and transfer attributes: master clock and frame size (8 bits) */
   EEPROM_SPI_CTAR0 = ( SPI_CTAR_SLAVE_FMSZ(7)     |
                        gSPI_BeforeTransfDelay_d   |
                        gSPI_AfterTransfDelay_d    |
                        gSPI_InterTransfDelay_d    |
                        gSPI_BaudRate_d);

   /* Write the DSPI_PUSHR register */
   if (contPCSEn)
   {
      EEPROM_SPI_PUSHR = ( SPI_PUSHR_CONT_MASK      |
                           SPI_PUSHR_CTAS(0)        |
                           SPI_PUSHR_EOQ_MASK       |
                           SPI_PUSHR_CTCNT_MASK     |
                           SPI_PUSHR_PCS(1)         |
                           SPI_PUSHR_TXDATA(u8Data));
   }
   else
   {
      EEPROM_SPI_PUSHR = ( SPI_PUSHR_CTAS(0)        |
                           SPI_PUSHR_EOQ_MASK       |
                           SPI_PUSHR_CTCNT_MASK     |
                           SPI_PUSHR_PCS(1)         |
                           SPI_PUSHR_TXDATA(u8Data));
   }

   /* Start the transfer */
   EEPROM_StartSPITransfer();

   /* Wait until the transfer has been finished */
   while(!(EEPROM_SPI_SR & SPI_SR_EOQF_MASK)) ;

   /* Clear the EOQF by writting a 1 to it */
   EEPROM_SPI_SR |= SPI_SR_EOQF_MASK;

   /* Stop the transfer */
   EEPROM_StopSPITransfer();
   EEPROM_FlushSPIFifo();
}

/*********************************************************
* Name: SPI_Receive_uint8_t
* Desc: The uint8_t received by SPI
* Parameter: None
* Return: Received uint8_t
**********************************************************/
static uint8_t SPI_Receive_byte( uint8_t contPCSEn)
{
   uint16_t u8Data;

   EEPROM_FlushSPIFifo();

   /* Clear the EOQF, TCF and RFDF flags by writting a 1 to them */
   EEPROM_SPI_SR |= SPI_SR_EOQF_MASK | SPI_SR_TCF_MASK | SPI_SR_RFDF_MASK;

   /* Write the clock and transfer attributes: master clock and frame size (8 bits) */
   EEPROM_SPI_CTAR0 = ( SPI_CTAR_FMSZ(7)            |
                        gSPI_BeforeTransfDelay_d    |
                        gSPI_AfterTransfDelay_d     |
                        gSPI_InterTransfDelay_d     |
                        gSPI_BaudRate_d);


   /* Write the DSPI_PUSHR register */
   if(contPCSEn)
   {
      EEPROM_SPI_PUSHR = ( SPI_PUSHR_CONT_MASK     |
                           SPI_PUSHR_CTAS(0)       |
                           SPI_PUSHR_EOQ_MASK      |
                           SPI_PUSHR_CTCNT_MASK    |
                           SPI_PUSHR_PCS(1)        |
                           SPI_PUSHR_TXDATA(0xFF));
   }
   else
   {
      EEPROM_SPI_PUSHR = ( SPI_PUSHR_CTAS(0)       |
                           SPI_PUSHR_EOQ_MASK      |
                           SPI_PUSHR_CTCNT_MASK    |
                           SPI_PUSHR_PCS(1)        |
                           SPI_PUSHR_TXDATA(0xFF));
   }

   /* Start the transfer */
   EEPROM_StartSPITransfer();

   /* Wait until the transfer has been finished */
   while(!(EEPROM_SPI_SR & SPI_SR_EOQF_MASK)) ;

   /* Clear the EOQF by writting a 1 to it */
   EEPROM_SPI_SR |= SPI_SR_EOQF_MASK | SPI_SR_TCF_MASK | SPI_SR_RFDF_MASK ;

   /* Start the transfer */
   EEPROM_StopSPITransfer();

   /* Read the uint8_t form the DSPI_POPR register */
   u8Data = (uint16_t)SPI_RXFR0_RXDATA(EEPROM_SPI_POPR);
   EEPROM_FlushSPIFifo();

   return ((uint8_t)u8Data);
}
#endif
/* EOF: Eeprom_AT45DB161E */ 
