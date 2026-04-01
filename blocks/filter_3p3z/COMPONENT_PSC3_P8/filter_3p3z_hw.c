/**
 * @file    filter_3p3z_hw.c
 * @brief   PWRLIB MW wrapper of the hardware 3-pole 3-zero filter of the PPCA HW FILTER platform.
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


/* ==============================================================================
 *   1. Include files
 * ==============================================================================
 */

#include "filter_3p3z_hw.h"
#include <stdbool.h>

/* ==============================================================================
 *   2. Local definitions
 * ==============================================================================
 */


/* ==============================================================================
 *   3. Local types
 * ==============================================================================
 */


/* ==============================================================================
 *   4. Data
 * ==============================================================================
 */


/* ==============================================================================
 *   5. Local function prototypes
 * ==============================================================================
 */


/* ==============================================================================
 *   6. Local functions
 * ==============================================================================
 */


/* ==============================================================================
 *   7. Exported functions
 * ==============================================================================
 */


void Filter3p3zReset_HW(filter_3p3z_context_df1_q23_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    // HW filter is reset by a disable-enable cycle
    Cy_PPCA_HWFILT3P3Z_FilterEnable(context->vars.variant.base, false);
    Cy_PPCA_HWFILT3P3Z_FilterEnable(context->vars.variant.base, true);
}


/* Init calls the PDL init function to copy the configuration structure into the HW registers */
void Filter3p3zInit_HW(filter_3p3z_context_df1_q23_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    // Copy configuration to variant-specific configuration
    context->vars.variant.config.cx0 = (uint32_t)context->config.cx_q23[0];
    context->vars.variant.config.cx1 = (uint32_t)context->config.cx_q23[1];
    context->vars.variant.config.cx2 = (uint32_t)context->config.cx_q23[2];
    context->vars.variant.config.cx3 = (uint32_t)context->config.cx_q23[3];
    context->vars.variant.config.cy1 = (uint32_t)context->config.cy_q23[0];
    context->vars.variant.config.cy2 = (uint32_t)context->config.cy_q23[1];
    context->vars.variant.config.cy3 = (uint32_t)context->config.cy_q23[2];
    context->vars.variant.config.dataOutOffset = (uint32_t)context->config.dOutOffset_q23;
    context->vars.variant.config.gIn = (uint8_t)context->config.gIn;
    context->vars.variant.config.gOut = (uint8_t)context->config.gOut;
    context->vars.variant.config.limMin = (uint32_t)context->config.limMin_q23;
    context->vars.variant.config.limMax = (uint32_t)context->config.limMax_q23;
    /* Misra rule 10.5 forbids casting from any type to enum. Consequently, this would mean
     * to define scaleCx and scaleCy with the same enum typedef from PDL, which would make
     * the generic API of the filter dependent on a specific variant. Locally suppressing rule 10.5
     * introduces the risk of truncation if scaleCx/Cy have larger values than the enum can represent.
     * This risk is not larger than the truncations of all other configuration parameters that
     * have a lower bit width in the hardware registers.
     */
    // coverity[misra_c_2012_rule_10_5_violation]
    context->vars.variant.config.scaleCX = (cy_en_ppca_filter_scale_factor_t)context->config.scaleCx;
    // coverity[misra_c_2012_rule_10_5_violation]
    context->vars.variant.config.scaleCY = (cy_en_ppca_filter_scale_factor_t)context->config.scaleCy;

    // Firmware write is the source for DATA_IN1
    context->vars.variant.config.srcSel = CY_AHB_IF;

    // If data source of DATA_IN1 is bus write by firmware:
    // The filter function writes DATA_IN0 first, then DATA_IN1,
    // the HW filter evaluation is triggered by writing to DATA_IN1
    context->vars.variant.config.enTrig0 = false;
    context->vars.variant.config.enTrig1 = true;

    // Copy configuration structure to HW registers
    Cy_PPCA_HWFILT3P3Z_InitFilterConfig(context->vars.variant.base, &(context->vars.variant.config));
}


void Filter3p3z_HW_noinline(
    filter_3p3z_context_df1_q23_t* const context,
    int16_t const dIn0_0q15, int16_t const dIn1_0q15, int32_t* dOut_0q23)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);

    Filter3p3z_HW_inline(context, dIn0_0q15, dIn1_0q15, dOut_0q23);
}


/* --- End of File ------------------------------------------------ */
