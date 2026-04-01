/**
 * @file    filter_3p3z_df2_f32.h
 * @brief   API of a software 3-pole 3-zero filter in DF2 F32 form.
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

#ifndef FILTER_3P3Z_DF2_F32_H__
#define FILTER_3P3Z_DF2_F32_H__

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
#include "filter_3p3z_types.h"
#include "cy_pctrl_log.h"
#include <stddef.h>

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */


/* ==============================================================================
 *   3. Types
 * ==============================================================================
 */

/**
 * \addtogroup group_filter_3p3z_typedefs_and_structures
 * \{
 */


/** \} group_filter_3p3z_structures */


/* ==============================================================================
 *   4. Exported data
 * ==============================================================================
 */


/* ==============================================================================
 *   5. Function prototypes and inline functions
 * ==============================================================================
 */

void Filter3p3zReset_DF2_F32(filter_3p3z_context_df2_f32_t* const context);
void Filter3p3zInit_DF2_F32(filter_3p3z_context_df2_f32_t* const context);


/**
 * 3p3z direct form two clamped float version (clamping using backward calculation)
 *
 * @param[in] context  : Pointer to the context structure
 * @param[in] dIn0     : Normalized setpoint (float32)
 * @param[in] dIn1     : Normalized signal (float32)
 * @param[out] dOut    : Pointer to output value (float32)
 */
PWRLIB_INLINE void Filter3p3z_DF2_F32_inline(
    filter_3p3z_context_df2_f32_t* const context,
    float32_t const dIn0, float32_t const dIn1, float32_t* dOut)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);
    cy_pctrl_assert_msg((dOut != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Output pointer is NULL in %s!\r\n", __func__);

    /* Input error: deviation of signal from setpoint */
    float32_t  error = dIn0 - dIn1;

    /* DF2 intermediate state: feedback path */
    float32_t v_n_0 = error
                      + (context->config.cy[0] * context->vars.v_n[0])
                      + (context->config.cy[1] * context->vars.v_n[1])
                      + (context->config.cy[2] * context->vars.v_n[2]);

    /* DF2 output: feedforward path */
    float32_t y = (context->config.cx[0] * v_n_0)
                  + (context->config.cx[1] * context->vars.v_n[0])
                  + (context->config.cx[2] * context->vars.v_n[1])
                  + (context->config.cx[3] * context->vars.v_n[2]);

    /* Output clamping with back-calculation antiwindup */
    float32_t excess_upper = y - context->config.max;
    float32_t excess_lower = y - context->config.min;

    if (excess_upper > 0.0f)
    {
        y -= excess_upper;
        v_n_0 -= excess_upper * context->config.antiwindup_gain;
    }
    else if (excess_lower < 0.0f)
    {
        y -= excess_lower;
        v_n_0 -= excess_lower * context->config.antiwindup_gain;
    }
    else
    {
        /* Output within limits, no clamping needed */
    }

    /* Shift delay line */
    context->vars.v_n[2] = context->vars.v_n[1];
    context->vars.v_n[1] = context->vars.v_n[0];
    context->vars.v_n[0] = v_n_0;

    *dOut = y;
}


void Filter3p3z_DF2_F32_noinline(
    filter_3p3z_context_df2_f32_t* const context,
    float32_t const dIn0, float32_t const dIn1, float32_t* dOut);


#ifdef __cplusplus
} /* Extern C */
#endif

#endif // ifndef FILTER_3P3Z_DF2_F32_H__


/* --- End of File ------------------------------------------------ */
