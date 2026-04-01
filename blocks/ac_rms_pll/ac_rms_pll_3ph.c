/**
 * @file    ac_rms_pll_3ph.c
 * @brief   Implementation of block functionality.
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

#include "ac_rms_pll_3ph.h"

#ifdef AC_RMS_PLL_USE_VARIANT_3PH

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
 * AC RMS PLL reset three-phase
 * Resets the internal module context for the three-phase variant
 *
 * @param[in] context   : Pointer to the context structure
 */
void AcRmsPllReset_3ph(ac_rms_pll_context_t* const context)
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
    context->vars.rms_filtered = 0.0F;
}


/**
 * AC RMS PLL init three-phase
 * Initializes the internal module context for the three-phase variant
 *
 * @param[in] context   : Pointer to the context structure
 */
void AcRmsPllInit_3ph(ac_rms_pll_context_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    /*Initializes the internal context*/
    context->vars.omega_filtered = context->config.lpc_omega_init;
    context->vars.rms_filtered = context->config.lpc_rms_init;
}


/**
 * AC tracking PLL with RMS output, three-phase non-inline
 *
 * Calls the inline implementation to provide a non-inline version
 *
 * @param[in] context   : Pointer to the context structure
 * @param[in] alpha     : AC signal for tracking
 * @param[in] beta      : Quadrature of alpha
 * @param[in] rms       : Pointer to the RMS output
 */
void AcRmsPll_3ph_noinline(ac_rms_pll_context_t* const context, float32_t alpha, float32_t beta,
                           float32_t* rms)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);

    AcRmsPll_3ph_inline(context, alpha, beta, rms);
}


#endif /* ifdef AC_RMS_PLL_USE_VARIANT_3PH */

/* --- End of File ------------------------------------------------ */
