/**
 * @file    rfc_F32.c
 * @brief   Software implementation of a reference frame control block.
 *
 */

/* ===========================================================================
** Copyright (C) 2026 Infineon Technologies AG
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

#include "rfc_F32.h"
#include <string.h>     // for memset()

#ifdef RFC_USE_VARIANT_F32

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
 * \brief Initialize outer-loop runtime/filter states for RFC F32 variant.
 * \param[in] context RFC outer context.
 */
void RfcOuterInit_F32(rfc_outer_context_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    /* Initialize LPF states used by the outer voltage loop filters. */
    context->vars.FilteredVc1 = 0.0f;
    context->vars.FilteredVc2 = 0.0f;

    Filter3p3zInit(&context->config.ctrl3p3z_ctx);
}


/**
 * \brief Reset outer-loop runtime/filter states for RFC F32 variant.
 * \param[in] context RFC outer context.
 */
void RfcOuterReset_F32(rfc_outer_context_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    /* Reset all variables in the context. */
    (void)memset(&context->vars, 0, sizeof(context->vars));

    Filter3p3zReset(&context->config.ctrl3p3z_ctx);
}


/**
 * \brief Initialize inner-loop runtime/filter states for RFC F32 variant.
 * \param[in] context RFC inner context.
 */
void RfcInnerInit_F32(rfc_inner_context_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    /* Initialize LPF states used by the inner d-q voltage/current filters. */
    context->vars.VdF = 0.0f;
    context->vars.VqF = 0.0f;
    context->vars.IdF = 0.0f;
    context->vars.IqF = 0.0f;

    Filter3p3zInit(&context->config.ctrl3p3z_ctx_d);
    Filter3p3zInit(&context->config.ctrl3p3z_ctx_q);
}


/**
 * \brief Reset inner-loop runtime/filter states for RFC F32 variant.
 * \param[in] context RFC inner context.
 */
void RfcInnerReset_F32(rfc_inner_context_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    /* Reset all variables in the context. */
    (void)memset(&context->vars, 0, sizeof(context->vars));

    Filter3p3zReset(&context->config.ctrl3p3z_ctx_d);
    Filter3p3zReset(&context->config.ctrl3p3z_ctx_q);
}


/**
 * \brief Non-inlined wrapper for RFC outer-loop processing.
 * \param[in] context RFC outer context.
 * \param[in] Vc1 Top capacitor voltage.
 * \param[in] Vc2 Bottom capacitor voltage.
 * \param[out] Idref Reference current generated by the outer loop.
 */
void RfcOuter_F32_noinline(rfc_outer_context_t* const context, float32_t Vc1, float32_t Vc2, float32_t* Idref)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    RfcOuter_F32_inline(context, Vc1, Vc2, Idref);
}


/**
 * \brief Non-inlined wrapper for RFC inner-loop processing.
 * \param[in] context RFC inner context.
 * \param[in] Idref d-axis current reference in amperes.
 */
void RfcInner_F32_noinline(rfc_inner_context_t* const context, float32_t Idref)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    RfcInner_F32_inline(context, Idref);
}


#endif /* RFC_USE_VARIANT_F32 */

/* --- End of File ------------------------------------------------ */
