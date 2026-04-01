/**
 * @file    ac_rms_pll_3phdd.h
 * @brief   Decoupled Double Synchronous Reference Frame (DDSRF) PLL implementation
 *
 * This file implements a DDSRF PLL for three-phase systems. The DDSRF PLL extends the
 * standard SRF PLL by implementing a decoupling network that eliminates double-frequency
 * oscillations during unbalanced grid conditions, improving phase tracking accuracy.
 *
 * The DDSRF approach uses two synchronous reference frames (positive and negative sequence)
 * and a cross-decoupling mechanism to separate sequence components, allowing the PLL to
 * track only the positive sequence component even under severe unbalance conditions.
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
** described in the Software. Infineon Technologies AG does not authorize its
** products for use in any products where a malfunction or failure of the
** Infineon Technologies AG product may reasonably be expected to result in
** significant property damage, injury or death ("High Risk Product"). By
** including Infineon Technologies AG's product in a High Risk Product, the
** manufacturer of such system or application assumes all risk of such use and
** in doing so agrees to indemnify Infineon Technologies AG against all
** liability.
** ===========================================================================
*/

#ifndef AC_RMS_PLL_3PHDD_H__
#define AC_RMS_PLL_3PHDD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================================================================
 *   1. Include files
 * ==============================================================================
 */

#include "mtb_mw_pctrl_compiler.h"
#include "mtb_mw_pctrl_config.h"
#include "ac_rms_pll_port.h"
#include "ac_rms_pll_types.h"
#include "cy_pctrl_log.h"
#ifdef AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW
#include "filter_3p3z.h"
#endif

#ifdef AC_RMS_PLL_USE_VARIANT_3PHDD

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */
#define ACRMSPLL_F32TOQ16(x)    ((int32_t)(x * (float32_t)(1 << 15)))
#define ACRMSPLL_Q24TOF32(x)    (((float32_t)x) / (float32_t)(1 << 23))

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

void AcRmsPllReset_3phdd(ac_rms_pll_context_t* const context);
void AcRmsPllInit_3phdd(ac_rms_pll_context_t* const context);

/**
 * Decoupled double synchronous reference frame PLL
 *
 * This function implements a DDSRF-PLL for accurate phase tracking under unbalanced
 * grid conditions. The algorithm:
 * 1. Performs Park transforms to both positive and negative rotating frames
 * 2. Applies low-pass filtering to extract fundamental sequence components
 * 3. Uses a cross-decoupling network to remove oscillations caused by sequence interaction
 * 4. Tracks the positive sequence component with a standard PI controller
 *
 * This variant significantly improves phase tracking performance compared to standard
 * SRF PLL when negative sequence components are present in the grid voltage.
 *
 * @param[in] context   : Pointer to the context structure
 * @param[in] alpha     : Alpha component of three-phase signals (stationary frame)
 * @param[in] beta      : Beta component of three-phase signals (stationary frame)
 * @param[out] rms      : Pointer to store the calculated RMS value
 */
PWRLIB_INLINE void AcRmsPll_3phdd_inline(ac_rms_pll_context_t* const context,
                                         float32_t alpha, float32_t beta,
                                         float32_t* rms)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);
    cy_pctrl_assert_msg((rms != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Output pointer is NULL in %s!\r\n", __func__);

    /* Trigonometric calculations for both reference frames */
    context->vars.sin_theta = PCTRL_SINF(context->vars.theta);
    context->vars.cos_theta = PCTRL_COSF(context->vars.theta);

    context->vars.sin_2theta = 2.0f * context->vars.sin_theta*context->vars.cos_theta;
    context->vars.cos_2theta = 1.0f - (2.0f * context->vars.sin_theta*context->vars.sin_theta);
    /* ---------- Park Transform to Dual Reference Frames ---------- */
    // Positive sequence frame (rotating at +omega)
    #ifndef AC_RMS_PLL_D_AXIS_ALIGN
    context->vars.d_pos = (alpha * context->vars.sin_theta) + ((-1.0f) * beta * context->vars.cos_theta);
    context->vars.q_pos = (alpha * context->vars.cos_theta) + (beta * context->vars.sin_theta);


    /* Park Transform for negative sequence frame (using -theta) */
    context->vars.d_neg = ((-1.0f) * alpha * context->vars.sin_theta) + ((-1.0f) * beta * context->vars.cos_theta);
    context->vars.q_neg = (alpha * context->vars.cos_theta) - (beta * context->vars.sin_theta);

    #else // ifndef AC_RMS_PLL_D_AXIS_ALIGN
    context->vars.d_pos = (alpha * context->vars.cos_theta) + (beta * context->vars.sin_theta);
    context->vars.q_pos = (-alpha * context->vars.sin_theta) + (beta * context->vars.cos_theta);

    /* Park Transform for negative sequence frame (using -theta) */
    context->vars.d_neg = (alpha * context->vars.cos_theta) - (beta * context->vars.sin_theta);
    context->vars.q_neg = (alpha * context->vars.sin_theta) + (beta * context->vars.cos_theta);
    #endif // ifndef AC_RMS_PLL_D_AXIS_ALIGN

    /*Positive decoupling network*/
    context->vars.dd_pos = (context->vars.d_pos - (context->vars.cos_2theta *  context->vars.dd_neg_filt)) -
                           (context->vars.sin_2theta * context->vars.dq_neg_filt);
    context->vars.dq_pos = (context->vars.q_pos + (context->vars.sin_2theta *  context->vars.dd_neg_filt)) -
                           (context->vars.cos_2theta * context->vars.dq_neg_filt);



    /*Negative decoupling network*/
    context->vars.dd_neg = (context->vars.d_neg - (context->vars.cos_2theta *  context->vars.dd_pos_filt)) +
                           (context->vars.sin_2theta * context->vars.dq_pos_filt);
    context->vars.dq_neg = (context->vars.q_neg - (context->vars.sin_2theta *  context->vars.dd_pos_filt)) -
                           (context->vars.cos_2theta * context->vars.dq_pos_filt);

    /* Filter positive sequence */
    context->vars.dd_pos_filt = (context->config.lpc_dd_alpha * context->vars.dd_pos) +
                                (context->config.lpc_dd_beta * context->vars.dd_pos_filt);
    context->vars.dq_pos_filt = (context->config.lpc_dd_alpha * context->vars.dq_pos) +
                                (context->config.lpc_dd_beta * context->vars.dq_pos_filt);

    /* Filter negative sequence */
    context->vars.dd_neg_filt = (context->config.lpc_dd_alpha * context->vars.dd_neg) +
                                (context->config.lpc_dd_beta * context->vars.dd_neg_filt);
    context->vars.dq_neg_filt = (context->config.lpc_dd_alpha * context->vars.dq_neg) +
                                (context->config.lpc_dd_beta * context->vars.dq_neg_filt);

    /* Decoupled positive sequence (removing the negative sequence influence) */
    context->vars.d = context->vars.dd_pos;
    context->vars.q = context->vars.dq_pos;

    /* RMS calculation */
    #ifdef AC_RMS_PLL_USE_CMSIS_DSP_LIB
    PCTRL_SQRTF(((context->vars.d * context->vars.d) + (context->vars.q * context->vars.q)), &context->vars.rms_raw);
    #else
    context->vars.rms_raw = PCTRL_SQRTF(((context->vars.d * context->vars.d) + (context->vars.q * context->vars.q)));
    #endif

    /* Rest of the code remains the same as the original */
    #ifndef AC_RMS_PLL_RMS_FILTER
    *rms = context->vars.rms_raw;
    #else
    context->vars.rms_filtered = (context->config.lpc_rms_alpha * context->vars.rms_raw) +
                                 (context->config.lpc_rms_beta *
                                  context->vars.rms_filtered);
    *rms = context->vars.rms_filtered;
    #endif

    /* Normalize the input using decoupled q component */
    context->vars.q_n = context->vars.q/(context->vars.rms_raw + AC_RMS_PLL_SMALL_NUM);

    #ifndef AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW
    /* PI Regulator to drive q_n to zero (using decoupled q component) */
    /* Integral term */
    context->vars.iterm += context->config.Ki * context->vars.q_n * context->config.dt;

    /* Saturate iterm for anti-windup */
    if (context->vars.iterm > context->config.cterm_max)
    {
        context->vars.iterm = context->config.cterm_max;
    }
    else if (context->vars.iterm < context->config.cterm_min)
    {
        context->vars.iterm = context->config.cterm_min;
    }
    else
    {
        /*Do nothing*/
    }

    context->vars.cterm = (context->config.Kp * context->vars.q_n) + context->vars.iterm;

    /* Clamp PI output */
    if (context->vars.cterm > context->config.cterm_max)
    {
        context->vars.cterm = context->config.cterm_max;
    }
    else if (context->vars.cterm < context->config.cterm_min)
    {
        context->vars.cterm = context->config.cterm_min;
    }
    else
    {
        /*Do nothing*/
    }
    /* End PI regulator */
    #else // ifndef AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW
    /* 3p3z HW control path: controller expects Q0.15 fixed-point inputs and Q0.23 fixed-point outputs */
    Filter3p3z(&context->config.ctrl3p3z_ctx, 0, ACRMSPLL_F32TOQ16(context->vars.q_n), &context->vars.ctrl3p3z_out);
    /* Back to float and scaled */
    context->vars.cterm = context->config.scale_3p3z*ACRMSPLL_Q24TOF32(context->vars.ctrl3p3z_out);
    #endif // ifndef AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW

    /* Add regulator effort to nominal frequency */
    context->vars.omega_raw = context->vars.cterm + context->config.omega_n;

    #ifndef AC_RMS_PLL_FREQ_FILTER
    context->vars.omega_filtered = context->vars.omega_raw;
    #else
    context->vars.omega_filtered = (context->config.lpc_omega_alpha * context->vars.omega_raw) +
                                   (context->config.lpc_omega_beta * context->vars.omega_filtered);
    #endif

    /* Integrator stage */
    /* Forward euler */
    context->vars.integrator_sum += context->vars.omega_raw * context->config.dt;

    /* 2PI Wrap around */
    if ((context->vars.integrator_sum < 0.0f) || (context->vars.integrator_sum > AC_RMS_PLL_TAU))
    {
        context->vars.integrator_sum = context->vars.integrator_sum - (((float32_t)PCTRL_FLOORF(
                                                                            AC_RMS_PLL_TAU_INV *
                                                                            context->vars.integrator_sum)) *
                                                                       AC_RMS_PLL_TAU);
    }
    else
    {
        /*Do nothing*/
    }
    context->vars.theta = context->vars.integrator_sum;
}


void AcRmsPll_3phdd_noinline(ac_rms_pll_context_t* const context, float32_t alpha, float32_t beta,
                             float32_t* rms);


#endif /* AC_RMS_PLL_USE_VARIANT_3PHDD */

#ifdef __cplusplus
} /* Extern C */
#endif


#endif /* AC_RMS_PLL_1PH_H__ */


/* --- End of File ------------------------------------------------ */
