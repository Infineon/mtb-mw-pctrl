/**
 * @file    filter_3p3z_hw.h
 * @brief   API of the hardware 3-pole 3-zero filter of the PPCA HW FILTER platform.
 *
 */

/* ===========================================================================
** Copyright (C) 2025 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
**
** Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Infineon
** Technologies AG reserves the right to make changes to the Software without
** notice. Infineon Technologies AG does not assume any liability arising out
** of the application or use of the Software or any product or circuit
** described in the Software. Infineon Technologies AG not authorize its
** products for use in any products where a malfunction or failure of the
** Infineon Technologies AG product may reasonably be expected to result in
** significant property damage, injury or death ("High Risk Product"). By
** including Infineon Technologies AG's product in a High Risk Product, the
** manufacturer of such system or application assumes all risk of such use and
** in doing so agrees to indemnify Infineon Technologies AG against all
** liability.
** ===========================================================================
*/

#ifndef FILTER_3P3Z_HW_H__
#define FILTER_3P3Z_HW_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================================================================
 *   1. Include files
 * ==============================================================================
 */

#include "mtb_mw_pctrl_compiler.h"
#include "mtb_mw_pctrl_config.h"
#include "filter_3p3z_port.h"
#include "cy_ppca_hwfilt3p3z.h"
#include "cy_pctrl_log.h"
#include <stddef.h>

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */

/**
 * If the variant needs a different configuration structure (because of
 * a different layout or additional members)
 * FILTER_3P3Z_USE_VARIANT_CONFIG is defined and the internal structure
 * of the configuration is provided in the static context of the block.
 */
#define FILTER_3P3Z_USE_VARIANT_CONFIG

/* ==============================================================================
 *   3. Types
 * ==============================================================================
 */

/**
 * \addtogroup group_filter_3p3z_typedefs_and_structures
 * \{
 */

/**
 * Structure used to keep the module configuration.
 *
 * @note: This is a sub-structure of the modules context.
 */
typedef struct
{
    PPCA_HWFILT3P3Z_SS_HWFILT3P3Z_Type* base;
    cy_stc_ppca_hwfilt3p3z_config_t config;
} filter_3p3z_variant_config_t;

/** \} group_filter_3p3z_structures */

// Include types only after defining the variant-specific config type
// coverity[misra_c_2012_rule_20_1_violation]
#include "filter_3p3z_types.h"

/* ==============================================================================
 *   4. Exported data
 * ==============================================================================
 */


/* ==============================================================================
 *   5. Function prototypes and inline functions
 * ==============================================================================
 */

void Filter3p3zReset_HW(filter_3p3z_context_df1_q23_t* const context);
void Filter3p3zInit_HW(filter_3p3z_context_df1_q23_t* const context);

PWRLIB_INLINE void Filter3p3z_HW_inline(
    filter_3p3z_context_df1_q23_t* const context,
    int16_t const dIn0_0q15, int16_t const dIn1_0q15, int32_t* dOut_0q23)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);
    cy_pctrl_assert_msg((dOut_0q23 != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Output pointer is NULL in %s!\r\n", __func__);

    Cy_PPCA_HWFILT3P3Z_Write_DATA_IN0(context->vars.variant.base, (uint16_t)dIn0_0q15);
    // writing to DATA_IN1 triggers the hardware filter
    Cy_PPCA_HWFILT3P3Z_Write_DATA_IN1(context->vars.variant.base, (uint16_t)dIn1_0q15);
    // Add some delay such that reading the filter status for the first time gives CY_FILTER_IS_FREE.
    // This gives the minimum execution time and is still robust if starting the filter was delayed
    // due to conflicts on the bus.
    // On main core, 10 NOPs are needed.
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    do
    {
    } while (Cy_PPCA_HWFILT3P3Z_ReadFilterStatus(context->vars.variant.base) != CY_FILTER_IS_FREE);

    // Read filter output
    *dOut_0q23 = (int32_t)Cy_PPCA_HWFILT3P3Z_ReadFilterDataOutput(context->vars.variant.base);
}


void Filter3p3z_HW_noinline(
    filter_3p3z_context_df1_q23_t* const context,
    int16_t const dIn0_0q15, int16_t const dIn1_0q15, int32_t* dOut_0q23);

#ifdef __cplusplus
} /* Extern C */
#endif

#endif // ifndef FILTER_3P3Z_HW_H__


/* --- End of File ------------------------------------------------ */
