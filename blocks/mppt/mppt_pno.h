/**
 * @file    mppt_pno.h
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

#ifndef MPPT_PNO_H__
#define MPPT_PNO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================================================================
 *   1. Include files
 * ==============================================================================
 */

#include "mtb_mw_pctrl_compiler.h"
#include "mtb_mw_pctrl_config.h"
#include "mppt_port.h"
#include "mppt_types.h"

#include "cy_pctrl_log.h"
#include <stddef.h>

#ifdef MPPT_USE_VARIANT_PNO

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */

#define MPPT_PNO_SMALL_NUM  (1e-6f)    /**< Small number to avoid division by zero */

/* ==============================================================================
 *   3. Types
 * ==============================================================================
 */

/* ==============================================================================
 *   4. Exported data
 * ==============================================================================
 */

/* ==============================================================================
 *   5. Function prototypes and inline functions
 * ==============================================================================
 */

/**
 * \addtogroup group_mppt_functions
 * \{
 */

void MpptReset_pno(mppt_context_t* const context);
void MpptInit_pno(mppt_context_t* const context);

/**
 * \brief Inline MPPT Perturb and Observe algorithm execution
 * \param context Pointer to the MPPT context structure
 * \param voltage Current measured voltage in Volts
 * \param current Current measured current in Amps
 * \param d_ref Pointer to store the normalized reference output (0.0 to 1.0)
 */
PWRLIB_INLINE void Mppt_pno_inline(mppt_context_t* const context, float32_t voltage, float32_t current,
                                   float32_t* d_ref)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);
    cy_pctrl_assert_msg((d_ref != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Output pointer is NULL in %s!\r\n", __func__);

    /* Calculate and store current power in context */
    context->vars.p_current = voltage * current;

    /* Calculate power and voltage differences */
    context->vars.dp = context->vars.p_current - context->vars.p_prev;
    context->vars.dv = voltage - context->vars.v_prev;

    if (context->vars.dp > context->config.deadband)
    {
        if (context->vars.dv > 0.0f)
        {
            context->vars.d_ref += context->config.delta_d;
        }
        else
        {
            context->vars.d_ref -= context->config.delta_d;
        }
    }
    else if (context->vars.dp < (-1.0f * context->config.deadband))
    {
        if (context->vars.dv > 0.0f)
        {
            context->vars.d_ref -= context->config.delta_d;
        }
        else
        {
            context->vars.d_ref += context->config.delta_d;
        }
    }
    else
    {
        /*Do nothing*/
    }

    /* Apply reference output limits */
    if (context->vars.d_ref > context->config.d_max)
    {
        context->vars.d_ref = context->config.d_max;
    }
    else if (context->vars.d_ref < context->config.d_min)
    {
        context->vars.d_ref = context->config.d_min;
    }
    else
    {
        /* Keep as is */
    }

    /* Update the previous values */
    context->vars.v_prev = voltage;
    context->vars.p_prev = context->vars.p_current;

    /* Output the reference value */
    *d_ref = context->vars.d_ref;
}


void Mppt_pno_noinline(mppt_context_t* const context, float32_t voltage, float32_t current, float32_t* d_ref);


/** \} group_mppt_functions */

#endif /* ifdef MPPT_USE_VARIANT_PNO */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif // ifndef MPPT_PNO_H__

/* --- End of File ------------------------------------------------ */
