/**
 * @file    mppt_pno.c
 * @brief   MPPT Perturb and Observe algorithm implementation
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

#include "mppt_pno.h"

#ifdef MPPT_USE_VARIANT_PNO

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

void MpptReset_pno(mppt_context_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    /* Resets all the state variables */
    context->vars.v_prev = 0.0f;
    context->vars.p_prev = 0.0f;
    context->vars.p_current = 0.0f;
    context->vars.d_ref = 0.0f;
    context->vars.dp = 0.0f;
    context->vars.dv = 0.0f;
}


void MpptInit_pno(mppt_context_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    /* Initializes the reference output to the configured initial value */
    context->vars.d_ref = context->config.d_initial;
}


void Mppt_pno_noinline(mppt_context_t* const context, float32_t voltage, float32_t current, float32_t* d_ref)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    Mppt_pno_inline(context, voltage, current, d_ref);
}


#endif /* ifdef MPPT_USE_VARIANT_PNO */

/* --- End of File ------------------------------------------------ */
