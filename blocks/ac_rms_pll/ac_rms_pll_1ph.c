/**
 * @file    ac_rms_pll_1ph.c
 * @brief   API of a single-phase SRF PLL with RMS output.
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

#include "ac_rms_pll_1ph.h"

#ifdef AC_RMS_PLL_USE_VARIANT_1PH

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

/**
 * AC RMS PLL reset single-phase
 * Resets the internal module context for the single-phase variant
 *
 * @param[in] context   : Pointer to the context structure
 */
void AcRmsPllReset_1ph(ac_rms_pll_context_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    /* Resets the internal context */
    context->vars.theta = 0.0F;
    context->vars.omega_raw = 0.0F;
    context->vars.omega_filtered = 0.0F;
    context->vars.iterm = 0.0F;
    context->vars.cterm = 0.0F;
    context->vars.integrator_sum = 0.0F;
    context->vars.alpha_filtered = 0.0F;
    context->vars.beta_filtered = 0.0F;
    context->vars.alpha_filtered_I = 0.0F;
    context->vars.alpha_filtered_last = 0.0F;
    context->vars.osg_integral_term = 0.0F;
    context->vars.osg_integral_term_last = 0.0F;
    context->vars.rms_filtered = 0.0F;
    #ifdef ACRMSPLL_DCOFFSET_REJECT
    context->vars.osg_integral_dc_term = 0.0F;
    context->vars.alpha_compensated_last = 0.0F;
    context->vars.alpha_compensated = 0.0F;
    #endif /*ACRMSPLL_DCOFFSET_REJECT*/

    #ifdef ACRMSPLL_3RD_HARMONIC_REJECT
    context->vars.alpha3_filtered = 0.0F;
    context->vars.beta3_filtered = 0.0F;
    context->vars.alpha3_filtered_I = 0.0F;
    context->vars.alpha3_filtered_last = 0.0F;
    context->vars.osg3_integral_term = 0.0F;
    context->vars.osg3_integral_term_last = 0.0F;
    #endif /* ACRMSPLL_3RD_HARMONIC_REJECT*/

    #ifdef ACRMSPLL_5TH_HARMONIC_REJECT
    context->vars.alpha5_filtered = 0.0F;
    context->vars.beta5_filtered = 0.0F;
    context->vars.alpha5_filtered_I = 0.0F;
    context->vars.alpha5_filtered_last = 0.0F;
    context->vars.osg5_integral_term = 0.0F;
    context->vars.osg5_integral_term_last = 0.0F;
    #endif /* ACRMSPLL_5TH_HARMONIC_REJECT*/
}


/**
 * AC RMS PLL init single-phase
 * Initializes the internal module context for the single-phase variant
 *
 * @param[in] context   : Pointer to the context structure
 */
void AcRmsPllInit_1ph(ac_rms_pll_context_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    /*Initializes the internal context*/
    context->vars.omega_filtered = context->config.lpc_omega_init;
    context->vars.rms_filtered = context->config.lpc_rms_init;
}


/**
 * AC tracking PLL with RMS output, single-phase non-inline
 *
 * Calls the inline implementation to provide a non-inline version
 *
 * @param[in] context   : Pointer to the context structure
 * @param[in] alpha     : AC signal for tracking
 * @param[in] beta      : Unused
 * @param[in] rms       : Pointer to the RMS output
 */
void AcRmsPll_1ph_noinline(ac_rms_pll_context_t* const context, float32_t alpha, float32_t beta, float32_t* rms)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);

    AcRmsPll_1ph_inline(context, alpha, beta, rms);
}


#endif /* AC_RMS_PLL_1PH_H__ */

/* --- End of File ------------------------------------------------ */
