/**
 * @file    ac_rms_pll_3ph.h
 * @brief   API of a three-phase SRF PLL with RMS output.
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

#ifndef AC_RMS_PLL_3PH_H__
#define AC_RMS_PLL_3PH_H__

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
#include <stddef.h>

#ifdef AC_RMS_PLL_USE_VARIANT_3PH

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */


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

void AcRmsPllReset_3ph(ac_rms_pll_context_t* const context);
void AcRmsPllInit_3ph(ac_rms_pll_context_t* const context);

/**
 * Three-phase AC SRF PLL with the RMS output
 * This function implements a Synchronous Reference Frame Phase-Locked Loop (SRF-PLL) for phase tracking.
 * It is intended for grid-tied power converters. By utilizing the Park transformation for phase detection,
 * it tracks the input phase by minimizing the voltage projected on the quadrature axis. It accepts the alpha
 * and beta components from an alpha-beta-zero transformation of three-phase signals. The value of phase
 * angle can be obtained from the passed context variable within vars.theta, whereas the frequency of the
 * input can be obtained from vars.omega or vars.omega_filtered.
 *
 * @param[in] context   : Pointer to the context structure
 * @param[in] alpha     : Alpha component of 3-phase signals
 * @param[in] beta      : Beta component of 3-phase signals
 * @param[in] rms       : Pointer to the RMS output
 */
PWRLIB_INLINE void AcRmsPll_3ph_inline(ac_rms_pll_context_t* const context, float32_t alpha, float32_t beta,
                                       float32_t* rms)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);
    cy_pctrl_assert_msg((rms != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Output pointer is NULL in %s!\r\n", __func__);

    /*Trigonometric calculations*/
    context->vars.sin_theta = PCTRL_SINF(context->vars.theta);
    context->vars.cos_theta = PCTRL_COSF(context->vars.theta);
    /*Park Transform*/
    #ifndef AC_RMS_PLL_D_AXIS_ALIGN
    /*Q-axis alignment*/
    context->vars.q = (alpha * context->vars.cos_theta) + (beta * context->vars.sin_theta);
    context->vars.d = (alpha * context->vars.sin_theta) + ((-1.0f) * beta * context->vars.cos_theta);
    #else
    /*D-axis alignment*/
    context->vars.d = (alpha * context->vars.cos_theta) + (beta * context->vars.sin_theta);
    context->vars.q = ((-1.0f) * alpha * context->vars.sin_theta) + (beta * context->vars.cos_theta);
    #endif
    /*RMS calculation*/
    #ifdef AC_RMS_PLL_USE_CMSIS_DSP_LIB
    PCTRL_SQRTF(((context->vars.d * context->vars.d) + (context->vars.q * context->vars.q)), &context->vars.rms_raw);
    #else
    context->vars.rms_raw = PCTRL_SQRTF(((context->vars.d * context->vars.d) + (context->vars.q * context->vars.q)));
    #endif
    #ifndef AC_RMS_PLL_RMS_FILTER
    *rms = context->vars.rms_raw;
    #else
    context->vars.rms_filtered = (context->config.lpc_rms_alpha * context->vars.rms_raw) +
                                 (context->config.lpc_rms_beta *
                                  context->vars.rms_filtered);
    *rms = context->vars.rms_filtered;
    #endif
    /*Normalize the input*/
    context->vars.q_n = context->vars.q/(context->vars.rms_raw + AC_RMS_PLL_SMALL_NUM);

    /*PI regulator to drive q_n to zero*/
    /*Integral term*/
    context->vars.iterm += context->config.Ki * context->vars.q_n * context->config.dt;

    /*Saturate iterm for anti-windup*/
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

    /*Clamp the PI output*/
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
    /*End the PI regulator*/

    /*Add the regulator effort to the nominal frequency*/
    context->vars.omega_raw = context->vars.cterm + context->config.omega_n;

    #ifndef AC_RMS_PLL_FREQ_FILTER
    context->vars.omega_filtered = context->vars.omega_raw;
    #else
    context->vars.omega_filtered = (context->config.lpc_omega_alpha * context->vars.omega_raw) +
                                   (context->config.lpc_omega_beta * context->vars.omega_filtered);
    #endif
    /*Integrator stage*/

    /*Forward euler*/
    context->vars.integrator_sum += context->vars.omega_raw * context->config.dt;
    /*2PI wrap around*/
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


void AcRmsPll_3ph_noinline(ac_rms_pll_context_t* const context, float32_t alpha, float32_t beta,
                           float32_t* rms);


#endif /* AC_RMS_PLL_USE_VARIANT_3PH */

#ifdef __cplusplus
} /* Extern C */
#endif


#endif /* AC_RMS_PLL_1PH_H__ */


/* --- End of File ------------------------------------------------ */
