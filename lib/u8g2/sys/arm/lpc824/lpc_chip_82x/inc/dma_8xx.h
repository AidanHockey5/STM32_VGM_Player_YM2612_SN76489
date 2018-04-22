/*
 * @brief LPC8xx DMA chip driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __DMA_8XX_H_
#define __DMA_8XX_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup DMA_8XX CHIP: LPC8xx DMA Controller driver
 * @ingroup CHIP_8XX_Drivers
 * @{
 */

/**
 * @brief DMA Controller shared registers structure
 */
typedef struct {					/*!< DMA shared registers structure */
	__IO uint32_t  ENABLESET;		/*!< DMA Channel Enable read and Set for all DMA channels */
	__I  uint32_t  RESERVED0;
	__O  uint32_t  ENABLECLR;		/*!< DMA Channel Enable Clear for all DMA channels */
	__I  uint32_t  RESERVED1;
	__I  uint32_t  ACTIVE;			/*!< DMA Channel Active status for all DMA channels */
	__I  uint32_t  RESERVED2;
	__I  uint32_t  BUSY;			/*!< DMA Channel Busy status for all DMA channels */
	__I  uint32_t  RESERVED3;
	__IO uint32_t  ERRINT;			/*!< DMA Error Interrupt status for all DMA channels */
	__I  uint32_t  RESERVED4;
	__IO uint32_t  INTENSET;		/*!< DMA Interrupt Enable read and Set for all DMA channels */
	__I  uint32_t  RESERVED5;
	__O  uint32_t  INTENCLR;		/*!< DMA Interrupt Enable Clear for all DMA channels */
	__I  uint32_t  RESERVED6;
	__IO uint32_t  INTA;			/*!< DMA Interrupt A status for all DMA channels */
	__I  uint32_t  RESERVED7;
	__IO uint32_t  INTB;			/*!< DMA Interrupt B status for all DMA channels */
	__I  uint32_t  RESERVED8;
	__O  uint32_t  SETVALID;		/*!< DMA Set ValidPending control bits for all DMA channels */
	__I  uint32_t  RESERVED9;
	__O  uint32_t  SETTRIG;			/*!< DMA Set Trigger control bits for all DMA channels */
	__I  uint32_t  RESERVED10;
	__O  uint32_t  ABORT;			/*!< DMA Channel Abort control for all DMA channels */
} LPC_DMA_COMMON_T;

/**
 * @brief DMA Controller shared registers structure
 */
typedef struct {					/*!< DMA channel register structure */
	__IO uint32_t  CFG;				/*!< DMA Configuration register */
	__I  uint32_t  CTLSTAT;			/*!< DMA Control and status register */
	__IO uint32_t  XFERCFG;			/*!< DMA Transfer configuration register */
	__I  uint32_t  RESERVED;
} LPC_DMA_CHANNEL_T;

/* Reserved bits masks... */
#define DMA_CFG_RESERVED			((3<<2)|(1<<7)|(3<<12)|0xfffc0000)
#define DMA_CTLSTAT_RESERVED		(~(1|(1<<2)))
#define DMA_XFERCFG_RESERVED		((3<<6)|(3<<10)|(0x3fu<<26))

/* DMA channel mapping - each channel is mapped to an individual peripheral
   and direction or a DMA imput mux trigger */
typedef enum {
	DMAREQ_USART0_RX,					/*!< USART0 receive DMA channel */
	DMA_CH0 = DMAREQ_USART0_RX,
	DMAREQ_USART0_TX,					/*!< USART0 transmit DMA channel */
	DMA_CH1 = DMAREQ_USART0_TX,
	DMAREQ_USART1_RX,					/*!< USART1 receive DMA channel */
	DMA_CH2 = DMAREQ_USART1_RX,
	DMAREQ_USART1_TX,					/*!< USART1 transmit DMA channel */
	DMA_CH3 = DMAREQ_USART1_TX,
	DMAREQ_USART2_RX,					/*!< USART2 receive DMA channel */
	DMA_CH4 = DMAREQ_USART2_RX,
	DMAREQ_USART2_TX,					/*!< USART2 transmit DMA channel */
	DMA_CH5 = DMAREQ_USART2_TX,
	DMAREQ_SPI0_RX,
	DMA_CH6 = DMAREQ_SPI0_RX,           /*!< SPI0 receive DMA channel */
	DMAREQ_SPI0_TX,
	DMA_CH7 = DMAREQ_SPI0_TX,           /*!< SPI0 transmit DMA channel */
	DMAREQ_SPI1_RX,
	DMA_CH8 = DMAREQ_SPI1_RX,           /*!< SPI1 receive DMA channel */
	DMAREQ_SPI1_TX,
	DMA_CH9 = DMAREQ_SPI1_TX,           /*!< SPI1 transmit DMA channel */
	DMAREQ_I2C0_MST,
	DMA_CH10 = DMAREQ_I2C0_MST,         /*!< I2C0 Master DMA channel */
	DMAREQ_I2C0_SLV,
	DMA_CH11 = DMAREQ_I2C0_SLV,         /*!< I2C0 Slave DMA channel */
	DMAREQ_I2C1_MST,
	DMA_CH12 = DMAREQ_I2C1_MST,         /*!< I2C1 Master DMA channel */
	DMAREQ_I2C1_SLV,
	DMA_CH13 = DMAREQ_I2C1_SLV,         /*!< I2C1 Slave DMA channel */
	DMAREQ_I2C2_MST,
	DMA_CH14 = DMAREQ_I2C2_MST,         /*!< I2C2 Master DMA channel */
	DMAREQ_I2C2_SLV,
	DMA_CH15 = DMAREQ_I2C2_SLV,         /*!< I2C2 Slave DMA channel */
	DMAREQ_I2C3_MST,
	DMA_CH16 = DMAREQ_I2C3_MST,         /*!< I2C2 Master DMA channel */
	DMAREQ_I2C3_SLV,
	DMA_CH17 = DMAREQ_I2C3_SLV,         /*!< I2C2 Slave DMA channel */
} DMA_CHID_T;

/* On LPC82x, Max DMA channel is 18 */
#define MAX_DMA_CHANNEL			(DMA_CH17 + 1)

/* Reserved bits masks... */
#define DMA_COMMON_RESERVED         (~(0UL) << MAX_DMA_CHANNEL)
#define DMA_ENABLESET_RESERVED		DMA_COMMON_RESERVED
#define DMA_ENABLECLR_RESERVED		DMA_COMMON_RESERVED
#define DMA_ACTIVE_RESERVED			DMA_COMMON_RESERVED
#define DMA_BUSY_RESERVED			DMA_COMMON_RESERVED
#define DMA_ERRINT_RESERVED			DMA_COMMON_RESERVED
#define DMA_INTENSET_RESERVED		DMA_COMMON_RESERVED
#define DMA_INTENCLR_RESERVED		DMA_COMMON_RESERVED
#define DMA_INTA_RESERVED			DMA_COMMON_RESERVED
#define DMA_INTB_RESERVED			DMA_COMMON_RESERVED
#define DMA_SETVALID_RESERVED		DMA_COMMON_RESERVED
#define DMA_SETTRIG_RESERVED		DMA_COMMON_RESERVED
#define DMA_ABORT_RESERVED			DMA_COMMON_RESERVED

/**
 * @brief DMA Controller register block structure
 */
typedef struct {					/*!< DMA Structure */
	__IO uint32_t  CTRL;			/*!< DMA control register */
	__I  uint32_t  INTSTAT;			/*!< DMA Interrupt status register */
	__IO uint32_t  SRAMBASE;		/*!< DMA SRAM address of the channel configuration table */
	__I  uint32_t  RESERVED2[5];
	LPC_DMA_COMMON_T DMACOMMON[1];	/*!< DMA shared channel (common) registers */
	__I  uint32_t  RESERVED0[225];
	LPC_DMA_CHANNEL_T DMACH[MAX_DMA_CHANNEL];	/*!< DMA channel registers */
} LPC_DMA_T;

/* Reserved bits masks... */
#define DMA_CTRL_RESERVED			(~1)
#define DMA_INTSTAT_RESERVED		(~7)
#define DMA_SRAMBASE_RESERVED		(0xFF)

/** @defgroup DMA_COMMONDRV_8XX CHIP: LPC8xx DMA Controller driver common functions
 * @{
 */

/**
 * @brief	Initialize DMA controller
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_Init(LPC_DMA_T *pDMA)
{
	(void) pDMA;
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_DMA);
}

/**
 * @brief	De-Initialize DMA controller
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_DeInit(LPC_DMA_T *pDMA)
{
	(void) pDMA;
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_DMA);
}

/**
 * @brief	Enable DMA controller
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_Enable(LPC_DMA_T *pDMA)
{
	pDMA->CTRL = 1;
}

/**
 * @brief	Disable DMA controller
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_Disable(LPC_DMA_T *pDMA)
{
	pDMA->CTRL = 0;
}

/* DMA interrupt status bits (common) */
#define DMA_INTSTAT_ACTIVEINT       0x2		/*!< Summarizes whether any enabled interrupts are pending */
#define DMA_INTSTAT_ACTIVEERRINT    0x4		/*!< Summarizes whether any error interrupts are pending */

/**
 * @brief	Get pending interrupt or error interrupts
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	An Or'ed value of DMA_INTSTAT_* types
 * @note	If any DMA channels have an active interrupt or error interrupt
 *			pending, this functional will a common status that applies to all
 *			channels.
 */
STATIC INLINE uint32_t Chip_DMA_GetIntStatus(LPC_DMA_T *pDMA)
{
	return (pDMA->INTSTAT & ~DMA_INTSTAT_RESERVED);
}

/* DMA channel source/address/next descriptor */
typedef struct {
	uint32_t  xfercfg;		/*!< Transfer configuration (only used in linked lists and ping-pong configs) */
	uint32_t  source;		/*!< DMA transfer source end address */
	uint32_t  dest;			/*!< DMA transfer desintation end address */
	uint32_t  next;			/*!< Link to next DMA descriptor, must be 16 byte aligned */
} DMA_CHDESC_T;

/* DMA SRAM table - this can be optionally used with the Chip_DMA_SetSRAMBase()
   function if a DMA SRAM table is needed. */
extern DMA_CHDESC_T Chip_DMA_Table[MAX_DMA_CHANNEL];

/**
 * @brief	Set DMA controller SRAM base address
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	base	: The base address where the DMA descriptors will be stored
 * @return	Nothing
 * @note	A 256 byte block of memory aligned on a 256 byte boundary must be
 *			provided for this function. It sets the base address used for
 *			DMA descriptor table (16 descriptors total that use 16 bytes each).<br>
 *
 *			A pre-defined table with correct alignment can be used for this
 *			function by calling Chip_DMA_SetSRAMBase(LPC_DMA, DMA_ADDR(Chip_DMA_Table));
 */
STATIC INLINE void Chip_DMA_SetSRAMBase(LPC_DMA_T *pDMA, uint32_t base)
{
	pDMA->SRAMBASE = base;
}

/**
 * @brief	Returns DMA controller SRAM base address
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	The base address where the DMA descriptors are stored
 */
STATIC INLINE uint32_t Chip_DMA_GetSRAMBase(LPC_DMA_T *pDMA)
{
	return (pDMA->SRAMBASE & ~DMA_SRAMBASE_RESERVED);
}

/**
 * @}
 */

/** @defgroup DMA_COMMON_8XX CHIP: LPC8xx DMA Controller driver common channel functions
 * @{
 */

/**
 * @brief	Enables a single DMA channel
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_EnableChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	pDMA->DMACOMMON[0].ENABLESET = (1 << ch);
}

/**
 * @brief	Disables a single DMA channel
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_DisableChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	pDMA->DMACOMMON[0].ENABLECLR = (1 << ch);
}

/**
 * @brief	Returns all enabled DMA channels
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	An Or'ed value of all enabled DMA channels (0 - 15)
 * @note	A high values in bits 0 .. 15 in the return values indicates
 *			that the channel for that bit (bit 0 = channel 0, bit 1 -
 *			channel 1, etc.) is enabled. A low state is disabled.
 */
STATIC INLINE uint32_t Chip_DMA_GetEnabledChannels(LPC_DMA_T *pDMA)
{
	return (pDMA->DMACOMMON[0].ENABLESET & ~DMA_ENABLESET_RESERVED);
}

/**
 * @brief	Returns all active DMA channels
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	An Or'ed value of all active DMA channels (0 - 15)
 * @note	A high values in bits 0 .. 15 in the return values indicates
 *			that the channel for that bit (bit 0 = channel 0, bit 1 -
 *			channel 1, etc.) is active. A low state is inactive. A active
 *			channel indicates that a DMA operation has been started but
 *			not yet fully completed.
 */
STATIC INLINE uint32_t Chip_DMA_GetActiveChannels(LPC_DMA_T *pDMA)
{
	return (pDMA->DMACOMMON[0].ACTIVE & ~DMA_ACTIVE_RESERVED);
}

/**
 * @brief	Returns all busy DMA channels
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	An Or'ed value of all busy DMA channels (0 - 15)
 * @note	A high values in bits 0 .. 15 in the return values indicates
 *			that the channel for that bit (bit 0 = channel 0, bit 1 -
 *			channel 1, etc.) is busy. A low state is not busy. A DMA
 *			channel is considered busy when there is any operation
 *			related to that channel in the DMA controller�s internal
 *			pipeline.
 */
STATIC INLINE uint32_t Chip_DMA_GetBusyChannels(LPC_DMA_T *pDMA)
{
	return (pDMA->DMACOMMON[0].BUSY & ~DMA_BUSY_RESERVED);
}

/**
 * @brief	Returns pending error interrupt status for all DMA channels
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	An Or'ed value of all channels (0 - 15) error interrupt status
 * @note	A high values in bits 0 .. 15 in the return values indicates
 *			that the channel for that bit (bit 0 = channel 0, bit 1 -
 *			channel 1, etc.) has a pending error interrupt. A low state
 *			indicates no error interrupt.
 */
STATIC INLINE uint32_t Chip_DMA_GetErrorIntChannels(LPC_DMA_T *pDMA)
{
	return (pDMA->DMACOMMON[0].ERRINT & ~DMA_ERRINT_RESERVED);
}

/**
 * @brief	Clears a pending error interrupt status for a single DMA channel
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_ClearErrorIntChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	pDMA->DMACOMMON[0].ERRINT = (1 << ch);
}

/**
 * @brief	Enables a single DMA channel's interrupt used in common DMA interrupt
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_EnableIntChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	pDMA->DMACOMMON[0].INTENSET = (1 << ch);
}

/**
 * @brief	Disables a single DMA channel's interrupt used in common DMA interrupt
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_DisableIntChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	pDMA->DMACOMMON[0].INTENCLR = (1 << ch);
}

/**
 * @brief	Returns all enabled interrupt channels
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	Nothing
 * @note	A high values in bits 0 .. 15 in the return values indicates
 *			that the channel for that bit (bit 0 = channel 0, bit 1 -
 *			channel 1, etc.) has an enabled interrupt for the channel.
 *			A low state indicates that the DMA channel will not contribute
 *			to the common DMA interrupt status.
 */
STATIC INLINE uint32_t Chip_DMA_GetEnableIntChannels(LPC_DMA_T *pDMA)
{
	return (pDMA->DMACOMMON[0].INTENSET & ~DMA_INTENSET_RESERVED);
}

/**
 * @brief	Returns active A interrupt status for all channels
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	Nothing
 * @note	A high values in bits 0 .. 15 in the return values indicates
 *			that the channel for that bit (bit 0 = channel 0, bit 1 -
 *			channel 1, etc.) has an active A interrupt for the channel.
 *			A low state indicates that the A interrupt is not active.
 */
STATIC INLINE uint32_t Chip_DMA_GetActiveIntAChannels(LPC_DMA_T *pDMA)
{
	return (pDMA->DMACOMMON[0].INTA & ~DMA_INTA_RESERVED);
}

/**
 * @brief	Clears active A interrupt status for a single channel
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_ClearActiveIntAChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	pDMA->DMACOMMON[0].INTA = (1 << ch);
}

/**
 * @brief	Returns active B interrupt status for all channels
 * @param	pDMA	: The base of DMA controller on the chip
 * @return	Nothing
 * @note	A high values in bits 0 .. 15 in the return values indicates
 *			that the channel for that bit (bit 0 = channel 0, bit 1 -
 *			channel 1, etc.) has an active B interrupt for the channel.
 *			A low state indicates that the B interrupt is not active.
 */
STATIC INLINE uint32_t Chip_DMA_GetActiveIntBChannels(LPC_DMA_T *pDMA)
{
	return (pDMA->DMACOMMON[0].INTB & ~DMA_INTB_RESERVED);
}

/**
 * @brief	Clears active B interrupt status for a single channel
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_ClearActiveIntBChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	pDMA->DMACOMMON[0].INTB = (1 << ch);
}

/**
 * @brief	Sets the VALIDPENDING control bit for a single channel
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 * @note	See the User Manual for more information for what this bit does.
 *
 */
STATIC INLINE void Chip_DMA_SetValidChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	pDMA->DMACOMMON[0].SETVALID = (1 << ch);
}

/**
 * @brief	Sets the TRIG bit for a single channel
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 * @note	See the User Manual for more information for what this bit does.
 */
STATIC INLINE void Chip_DMA_SetTrigChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	pDMA->DMACOMMON[0].SETTRIG = (1 << ch);
}

/**
 * @brief	Aborts a DMA operation for a single channel
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 * @note	To abort a channel, the channel should first be disabled. Then wait
 *			until the channel is no longer busy by checking the corresponding
 *			bit in BUSY. Finally, abort the channel operation. This prevents the
 *			channel from restarting an incomplete operation when it is enabled
 *			again.
 */
STATIC INLINE void Chip_DMA_AbortChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	pDMA->DMACOMMON[0].ABORT = (1 << ch);
}

/**
 * @}
 */


/** @defgroup DMA_CHANNEL_8XX CHIP: LPC8xx DMA Controller driver channel specific functions
 * @{
 */

/* Support macro for DMA_CHDESC_T */
#define DMA_ADDR(addr)      ((uint32_t) (addr))

/* Support definitions for setting the configuration of a DMA channel. You
   will need to get more information on these options from the User manual. */
#define DMA_CFG_PERIPHREQEN     (1 << 0)	/*!< Enables Peripheral DMA requests */
#define DMA_CFG_HWTRIGEN        (1 << 1)	/*!< Use hardware triggering via imput mux */
#define DMA_CFG_TRIGPOL_LOW     (0 << 4)	/*!< Hardware trigger is active low or falling edge */
#define DMA_CFG_TRIGPOL_HIGH    (1 << 4)	/*!< Hardware trigger is active high or rising edge */
#define DMA_CFG_TRIGTYPE_EDGE   (0 << 5)	/*!< Hardware trigger is edge triggered */
#define DMA_CFG_TRIGTYPE_LEVEL  (1 << 5)	/*!< Hardware trigger is level triggered */
#define DMA_CFG_TRIGBURST_SNGL  (0 << 6)	/*!< Single transfer. Hardware trigger causes a single transfer */
#define DMA_CFG_TRIGBURST_BURST (1 << 6)	/*!< Burst transfer (see UM) */
#define DMA_CFG_BURSTPOWER_1    (0 << 8)	/*!< Set DMA burst size to 1 transfer */
#define DMA_CFG_BURSTPOWER_2    (1 << 8)	/*!< Set DMA burst size to 2 transfers */
#define DMA_CFG_BURSTPOWER_4    (2 << 8)	/*!< Set DMA burst size to 4 transfers */
#define DMA_CFG_BURSTPOWER_8    (3 << 8)	/*!< Set DMA burst size to 8 transfers */
#define DMA_CFG_BURSTPOWER_16   (4 << 8)	/*!< Set DMA burst size to 16 transfers */
#define DMA_CFG_BURSTPOWER_32   (5 << 8)	/*!< Set DMA burst size to 32 transfers */
#define DMA_CFG_BURSTPOWER_64   (6 << 8)	/*!< Set DMA burst size to 64 transfers */
#define DMA_CFG_BURSTPOWER_128  (7 << 8)	/*!< Set DMA burst size to 128 transfers */
#define DMA_CFG_BURSTPOWER_256  (8 << 8)	/*!< Set DMA burst size to 256 transfers */
#define DMA_CFG_BURSTPOWER_512  (9 << 8)	/*!< Set DMA burst size to 512 transfers */
#define DMA_CFG_BURSTPOWER_1024 (10 << 8)	/*!< Set DMA burst size to 1024 transfers */
#define DMA_CFG_BURSTPOWER(n)   ((n) << 8)	/*!< Set DMA burst size to 2^n transfers, max n=10 */
#define DMA_CFG_SRCBURSTWRAP    (1 << 14)	/*!< Source burst wrapping is enabled for this DMA channel */
#define DMA_CFG_DSTBURSTWRAP    (1 << 15)	/*!< Destination burst wrapping is enabled for this DMA channel */
#define DMA_CFG_CHPRIORITY(p)   ((p) << 16)	/*!< Sets DMA channel priority, min 0 (highest), max 3 (lowest) */

/**
 * @brief	Setup a DMA channel configuration
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @param	cfg		: An Or'ed value of DMA_CFG_* values that define the channel's configuration
 * @return	Nothing
 * @note	This function sets up all configurable options for the DMA channel.
 *			These options are usually set once for a channel and then unchanged.<br>
 *
 *			The following example show how to configure the channel for peripheral
 *			DMA requests, burst transfer size of 1 (in 'transfers', not bytes),
 *			continuous reading of the same source address, incrementing destination
 *			address, and highest channel priority.<br>
 *			Example: Chip_DMA_SetupChannelConfig(pDMA, SSP0_RX_DMA,
 *				(DMA_CFG_PERIPHREQEN | DMA_CFG_TRIGBURST_BURST | DMA_CFG_BURSTPOWER_1 |
 *				DMA_CFG_SRCBURSTWRAP | DMA_CFG_CHPRIORITY(0)));<br>
 *
 *			The following example show how to configure the channel for an external
 *			trigger from the imput mux with low edge polarity, a burst transfer size of 8,
 *			incrementing source and destination addresses, and lowest channel
 *			priority.<br>
 *			Example: Chip_DMA_SetupChannelConfig(pDMA, DMA_CH14,
 *				(DMA_CFG_HWTRIGEN | DMA_CFG_TRIGPOL_LOW | DMA_CFG_TRIGTYPE_EDGE |
 *				DMA_CFG_TRIGBURST_BURST | DMA_CFG_BURSTPOWER_8 |
 *				DMA_CFG_CHPRIORITY(3)));<br>
 *
 *			For non-peripheral DMA triggering (DMA_CFG_HWTRIGEN definition), use the
 *			DMA input mux functions to configure the DMA trigger source for a DMA channel.
 */
STATIC INLINE void Chip_DMA_SetupChannelConfig(LPC_DMA_T *pDMA, DMA_CHID_T ch, uint32_t cfg)
{
	pDMA->DMACH[ch].CFG = cfg;
}

/* DMA channel control and status register definitions */
#define DMA_CTLSTAT_VALIDPENDING    (1 << 0)	/*!< Valid pending flag for this channel */
#define DMA_CTLSTAT_TRIG            (1 << 2)	/*!< Trigger flag. Indicates that the trigger for this channel is currently set */

/**
 * @brief	Returns channel specific status flags
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	AN Or'ed value of DMA_CTLSTAT_VALIDPENDING and DMA_CTLSTAT_TRIG
 */
STATIC INLINE uint32_t Chip_DMA_GetChannelStatus(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	return (pDMA->DMACH[ch].XFERCFG & ~DMA_XFERCFG_RESERVED);
}

/* DMA channel transfer configuration registers definitions */
#define DMA_XFERCFG_CFGVALID        (1 << 0)	/*!< Configuration Valid flag */
#define DMA_XFERCFG_RELOAD          (1 << 1)	/*!< Indicates whether the channels control structure will be reloaded when the current descriptor is exhausted */
#define DMA_XFERCFG_SWTRIG          (1 << 2)	/*!< Software Trigger */
#define DMA_XFERCFG_CLRTRIG         (1 << 3)	/*!< Clear Trigger */
#define DMA_XFERCFG_SETINTA         (1 << 4)	/*!< Set Interrupt flag A for this channel to fire when descriptor is complete */
#define DMA_XFERCFG_SETINTB         (1 << 5)	/*!< Set Interrupt flag B for this channel to fire when descriptor is complete */
#define DMA_XFERCFG_WIDTH_8         (0 << 8)	/*!< 8-bit transfers are performed */
#define DMA_XFERCFG_WIDTH_16        (1 << 8)	/*!< 16-bit transfers are performed */
#define DMA_XFERCFG_WIDTH_32        (2 << 8)	/*!< 32-bit transfers are performed */
#define DMA_XFERCFG_SRCINC_0        (0 << 12)	/*!< DMA source address is not incremented after a transfer */
#define DMA_XFERCFG_SRCINC_1        (1 << 12)	/*!< DMA source address is incremented by 1 (width) after a transfer */
#define DMA_XFERCFG_SRCINC_2        (2 << 12)	/*!< DMA source address is incremented by 2 (width) after a transfer */
#define DMA_XFERCFG_SRCINC_4        (3 << 12)	/*!< DMA source address is incremented by 4 (width) after a transfer */
#define DMA_XFERCFG_DSTINC_0        (0 << 14)	/*!< DMA destination address is not incremented after a transfer */
#define DMA_XFERCFG_DSTINC_1        (1 << 14)	/*!< DMA destination address is incremented by 1 (width) after a transfer */
#define DMA_XFERCFG_DSTINC_2        (2 << 14)	/*!< DMA destination address is incremented by 2 (width) after a transfer */
#define DMA_XFERCFG_DSTINC_4        (3 << 14)	/*!< DMA destination address is incremented by 4 (width) after a transfer */
#define DMA_XFERCFG_XFERCOUNT(n)    ((n - 1) << 16)	/*!< DMA transfer count in 'transfers', between (0)1 and (1023)1024 */

/**
 * @brief	Setup a DMA channel transfer configuration
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @param	cfg		: An Or'ed value of DMA_XFERCFG_* values that define the channel's transfer configuration
 * @return	Nothing
 * @note	This function sets up the transfer configuration for the DMA channel.<br>
 *
 *			The following example show how to configure the channel's transfer for
 *			multiple transfer descriptors (ie, ping-pong), interrupt 'A' trigger on
 *			transfer descriptor completion, 128 byte size transfers, and source and
 *			destination address increment.<br>
 *			Example: Chip_DMA_SetupChannelTransfer(pDMA, SSP0_RX_DMA,
 *				(DMA_XFERCFG_CFGVALID | DMA_XFERCFG_RELOAD | DMA_XFERCFG_SETINTA |
 *				DMA_XFERCFG_WIDTH_8 | DMA_XFERCFG_SRCINC_1 | DMA_XFERCFG_DSTINC_1 |
 *				DMA_XFERCFG_XFERCOUNT(128)));<br>
 */
STATIC INLINE void Chip_DMA_SetupChannelTransfer(LPC_DMA_T *pDMA, DMA_CHID_T ch, uint32_t cfg)
{
	pDMA->DMACH[ch].XFERCFG = cfg;
}

/**
 * @brief	Set DMA transfer register interrupt bits (safe)
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @param	mask	: Bits to set
 * @return	Nothing
 * @note	This function safely sets bits in the DMA channel specific XFERCFG
 *			register.
 */
STATIC INLINE void Chip_DMA_SetTranBits(LPC_DMA_T *pDMA, DMA_CHID_T ch, uint32_t mask)
{
	/* Read and write values may not be the same, write 0 to
	   undefined bits */
	pDMA->DMACH[ch].XFERCFG = (pDMA->DMACH[ch].XFERCFG & ~DMA_XFERCFG_RESERVED) | mask;
}

/**
 * @brief	Clear DMA transfer register interrupt bits (safe)
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @param	mask	: Bits to clear
 * @return	Nothing
 * @note	This function safely clears bits in the DMA channel specific XFERCFG
 *			register.
 */
STATIC INLINE void Chip_DMA_ClearTranBits(LPC_DMA_T *pDMA, DMA_CHID_T ch, uint32_t mask)
{
	/* Read and write values may not be the same, write 0 to
	   undefined bits */
	pDMA->DMACH[ch].XFERCFG &= ~(DMA_XFERCFG_RESERVED | mask);
}

/**
 * @brief	Update the transfer size in an existing DMA channel transfer configuration
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @param	trans	: Number of transfers to update the transfer configuration to (1 - 1023)
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_SetupChannelTransferSize(LPC_DMA_T *pDMA, DMA_CHID_T ch, uint32_t trans)
{
	pDMA->DMACH[ch].XFERCFG = (pDMA->DMACH[ch].XFERCFG & ~(DMA_XFERCFG_RESERVED | (0x3FF << 16))) | DMA_XFERCFG_XFERCOUNT(trans);
}

/**
 * @brief	Sets a DMA channel configuration as valid
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_SetChannelValid(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	Chip_DMA_SetTranBits(pDMA, ch, DMA_XFERCFG_CFGVALID);
}

/**
 * @brief	Sets a DMA channel configuration as invalid
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_SetChannelInValid(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	Chip_DMA_ClearTranBits(pDMA, ch, DMA_XFERCFG_CFGVALID);
}

/**
 * @brief	Performs a software trigger of the DMA channel
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	Nothing
 */
STATIC INLINE void Chip_DMA_SWTriggerChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	Chip_DMA_SetTranBits(pDMA, ch, DMA_XFERCFG_SWTRIG);
}

/**
 * @brief	Checks if the given channel is active or not
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @return	1 if channel @a ch is active; 0 if channel @a ch is not active
 */
STATIC INLINE bool Chip_DMA_IsChannelActive(LPC_DMA_T *pDMA, DMA_CHID_T ch)
{
	return (pDMA->DMACOMMON[0].ACTIVE & (1 << ch)) != 0;
}

/**
 * @brief	Sets up a DMA channel with the passed DMA transfer descriptor
 * @param	pDMA	: The base of DMA controller on the chip
 * @param	ch		: DMA channel ID
 * @param	desc	: Pointer to DMA transfer descriptor
 * @return	false if the DMA channel was active, otherwise true
 * @note	This function will set the DMA descriptor in the SRAM table to the
 *			the passed descriptor. This function is only meant to be used when
 *			the DMA channel is not active and can be used to setup the
 *			initial transfer for a linked list or ping-pong buffer or just a
 *			single transfer without a next descriptor.<br>
 *
 *			If using this function to write the initial transfer descriptor in
 *			a linked list or ping-pong buffer configuration, it should contain a
 *			non-NULL 'next' field pointer.
 */
STATIC INLINE bool Chip_DMA_SetupTranChannel(LPC_DMA_T *pDMA, DMA_CHID_T ch, const DMA_CHDESC_T *desc)
{
	/* If channel is active return false */
	if (Chip_DMA_IsChannelActive(pDMA, ch))
		return false;

	/* Assign the descriptor to descriptor table */
	((DMA_CHDESC_T *) (pDMA->SRAMBASE & ~DMA_SRAMBASE_RESERVED))[ch] = *desc;
	return true;
}

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __DMA_8XX_H_ */
