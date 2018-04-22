/*
 * @brief LPC8xx Pin Interrupt and Pattern Match driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licenser disclaim any and
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

#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Set source for pattern match engine */
void Chip_PININT_SetPatternMatchSrc(LPC_PIN_INT_T *pPININT, uint8_t chan, Chip_PININT_BITSLICE_T slice)
{
    uint32_t pmsrc_reg;
    
    /* Source source for pattern matching */ 
    pmsrc_reg = pPININT->PMSRC & ~((PININT_SRC_BITSOURCE_MASK << (PININT_SRC_BITSOURCE_START + (slice * 3)))
		| PININT_PMSRC_RESERVED);
	pPININT->PMSRC = pmsrc_reg | (chan << (PININT_SRC_BITSOURCE_START + (slice * 3)));
}

/* Configure Pattern match engine */
void Chip_PININT_SetPatternMatchConfig(LPC_PIN_INT_T *pPININT, Chip_PININT_BITSLICE_T slice, 
        Chip_PININT_BITSLICE_CFG_T slice_cfg, bool end_point)
{
    uint32_t pmcfg_reg;
    
    /* Configure bit slice configuration */
    pmcfg_reg = pPININT->PMCFG & ~((PININT_SRC_BITCFG_MASK << (PININT_SRC_BITCFG_START + (slice * 3)))
		| PININT_PMCFG_RESERVED);
    pPININT->PMCFG = pmcfg_reg | (slice_cfg << (PININT_SRC_BITCFG_START + (slice * 3)));

    /* If end point is true, enable the bits */
	if (end_point == true)
	{
        /* By default slice 7 is final component */
		if (slice != PININTBITSLICE7)
        {
			pPININT->PMCFG = (0x1 << slice) | (pPININT->PMCFG & ~PININT_PMCFG_RESERVED);
		}
	}
}
