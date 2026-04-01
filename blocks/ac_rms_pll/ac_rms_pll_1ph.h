/**
 * @file    ac_rms_pll_1ph.h
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

#ifndef AC_RMS_PLL_1PH_H__
#define AC_RMS_PLL_1PH_H__

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

#ifdef AC_RMS_PLL_USE_VARIANT_1PH

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

void AcRmsPllReset_1ph(ac_rms_pll_context_t* const context);
void AcRmsPllInit_1ph(ac_rms_pll_context_t* const context);

/**
 * Single-phase AC PLL with RMS output
 * This function implements a Synchronous Reference Frame Phase-Locked Loop (SRF-PLL) for phase tracking.
 * It is intended for grid-tied power converters. By utilizing the Park transformation for phase detection,
 * it tracks the input phase by minimizing the voltage projected on the quadrature axis. As Park transformation
 * requires both a sinusoidal signal and its quadrature phase, this function implements an orthogonal signal
 * generator using a Second-Order Generalized Integrator (SOGI). Additionally, there are optional features
 * for DC offset rejection, 3rd-harmonic rejection and 5th-harmonic rejection which may be activated using
 * configuration macros. The value of the phase angle can be obtained from the passed context variable within
 * vars.theta, whereas the frequency of the input can be obtained from vars.omega or vars.omega_filtered.
 *
 * @param[in] context   : Pointer to the context structure
 * @param[in] alpha     : AC signal for tracking
 * @param[in] beta      : Unused
 * @param[in] rms       : Pointer to the RMS output
 */
PWRLIB_INLINE void AcRmsPll_1ph_inline(ac_rms_pll_context_t* const context, float32_t alpha, float32_t beta,
                                       float32_t* rms)
{
    PWRLIB_UNUSED(beta);

    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);
    cy_pctrl_assert_msg((rms != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Output pointer is NULL in %s!\r\n", __func__);

    /*Orthogonal signal generator*/
    /*SOGI Tustin*/
    context->vars.alpha_error = alpha - context->vars.alpha_filtered;
    #ifdef ACRMSPLL_3RD_HARMONIC_REJECT
    context->vars.alpha_error = context->vars.alpha_error - context->vars.alpha3_filtered;
    #endif /* ACRMSPLL_3RD_HARMONIC_REJECT*/
    #ifdef ACRMSPLL_5TH_HARMONIC_REJECT
    context->vars.alpha_error = context->vars.alpha_error - context->vars.alpha5_filtered;
    #endif /* ACRMSPLL_3RD_HARMONIC_REJECT*/
    #ifdef ACRMSPLL_DCOFFSET_REJECT
    context->vars.alpha_compensated = context->vars.alpha_error - context->vars.osg_integral_dc_term;
    context->vars.osg_integral_term =
        ((context->config.Kosg * (context->vars.alpha_compensated)) - context->vars.beta_filtered) *
        context->vars.omega_filtered;
    #else /*ACRMSPLL_DCOFFSET_REJECT*/
    context->vars.osg_integral_term =
        ((context->config.Kosg * context->vars.alpha_error) - context->vars.beta_filtered) *
        context->vars.omega_filtered;
    #endif /*ACRMSPLL_DCOFFSET_REJECT*/
    /*Tustin integrator for input filtering */
    context->vars.alpha_filtered = context->vars.alpha_filtered +
                                   ((context->config.dt *
                                     (context->vars.osg_integral_term + context->vars.osg_integral_term_last))/2.0f);
    context->vars.osg_integral_term_last = context->vars.osg_integral_term;
    /*Tustin integrator for input quadrature filtering */
    context->vars.alpha_filtered_I = context->vars.alpha_filtered_I +
                                     ((context->config.dt *
                                       (context->vars.alpha_filtered + context->vars.alpha_filtered_last))/2.0f);
    context->vars.alpha_filtered_last = context->vars.alpha_filtered;
    /*Calculate the orthogonal signal*/
    context->vars.beta_filtered = context->vars.alpha_filtered_I * context->vars.omega_filtered;
    #ifdef ACRMSPLL_DCOFFSET_REJECT
    if (context->vars.alpha_compensated > context->config.dc_max)
    {
        context->vars.alpha_compensated = context->config.dc_max;
    }
    else if (context->vars.alpha_compensated < context->config.dc_min)
    {
        context->vars.alpha_compensated = context->config.dc_min;
    }
    else
    {
        /*Do nothing*/
    }
    context->vars.alpha_compensated = context->config.Kdc * context->vars.alpha_compensated *
                                      context->vars.omega_filtered;
    /*Tustin integrator for DC offset compensation*/
    context->vars.osg_integral_dc_term = context->vars.osg_integral_dc_term +
                                         ((context->config.dt *
                                           (context->vars.alpha_compensated + context->vars.alpha_compensated_last))/
                                          2.0f);
    context->vars.alpha_compensated_last = context->vars.alpha_compensated;
    #endif /*ACRMSPLL_DCOFFSET_REJECT*/

    #ifdef ACRMSPLL_3RD_HARMONIC_REJECT
    context->vars.alpha3 = context->vars.alpha_error;
    context->vars.omega3 = context->vars.omega_filtered * 3.0f;
    context->vars.osg3_integral_term =
        ((context->config.Kosg3 *(context->vars.alpha3)) -
         context->vars.beta3_filtered) *
        context->vars.omega3;
    /*Tustin integrator for input filtering */
    context->vars.alpha3_filtered = context->vars.alpha3_filtered +
                                    ((context->config.dt *
                                      (context->vars.osg3_integral_term + context->vars.osg3_integral_term_last))/2.0f);
    context->vars.osg3_integral_term_last = context->vars.osg3_integral_term;
    /*Tustin integrator for input quadrature filtering */
    context->vars.alpha3_filtered_I = context->vars.alpha3_filtered_I +
                                      ((context->config.dt *
                                        (context->vars.alpha3_filtered + context->vars.alpha3_filtered_last))/2.0f);
    /*Update the last value*/
    context->vars.alpha3_filtered_last = context->vars.alpha3_filtered;
    /*Calculate orthogonal signal*/
    context->vars.beta3_filtered = context->vars.alpha3_filtered_I * context->vars.omega3;
    #endif /* ACRMSPLL_3RD_HARMONIC_REJECT*/

    #ifdef ACRMSPLL_5TH_HARMONIC_REJECT
    context->vars.alpha5 = context->vars.alpha_error;
    context->vars.omega5 = context->vars.omega_filtered * 5.0f;
    context->vars.osg5_integral_term =
        ((context->config.Kosg5 *(context->vars.alpha5)) -
         context->vars.beta5_filtered) *
        context->vars.omega5;
    /*Tustin integrator for input filtering */
    context->vars.alpha5_filtered = context->vars.alpha5_filtered +
                                    ((context->config.dt *
                                      (context->vars.osg5_integral_term + context->vars.osg5_integral_term_last))/2.0f);
    context->vars.osg5_integral_term_last = context->vars.osg5_integral_term;
    /*Tustin integrator for input quadrature filtering */
    context->vars.alpha5_filtered_I = context->vars.alpha5_filtered_I +
                                      ((context->config.dt *
                                        (context->vars.alpha5_filtered + context->vars.alpha5_filtered_last))/2.0f);
    context->vars.alpha5_filtered_last = context->vars.alpha5_filtered;
    /*Calculate the orthogonal signal*/
    context->vars.beta5_filtered = context->vars.alpha5_filtered_I * context->vars.omega5;
    #endif /* ACRMSPLL_5TH_HARMONIC_REJECT*/

    /*Trigonometric calculations*/
    context->vars.sin_theta = PCTRL_SINF(context->vars.theta);
    context->vars.cos_theta = PCTRL_COSF(context->vars.theta);
    /*Park transform*/
    #ifndef AC_RMS_PLL_D_AXIS_ALIGN
    /*Q-axis alignment*/
    context->vars.q = (context->vars.alpha_filtered * context->vars.cos_theta) + (context->vars.beta_filtered *
                                                                                  context->vars.sin_theta);
    context->vars.d = (context->vars.alpha_filtered * context->vars.sin_theta) +
                      ((-1.0f) * context->vars.beta_filtered *
                       context->vars.cos_theta);
    #else
    /*D-axis alignment*/
    context->vars.d = (context->vars.alpha_filtered * context->vars.cos_theta) + (context->vars.beta_filtered *
                                                                                  context->vars.sin_theta);
    context->vars.q = ((-1.0f) * context->vars.alpha_filtered * context->vars.sin_theta) +
                      (context->vars.beta_filtered *
                       context->vars.cos_theta);
    #endif /* ifndef AC_RMS_PLL_D_AXIS_ALIGN */
    /*RMS calculation*/
    #ifdef AC_RMS_PLL_USE_CMSIS_DSP_LIB
    PCTRL_SQRTF(((context->vars.d * context->vars.d) + (context->vars.q * context->vars.q)), &context->vars.rms_raw);
    context->vars.rms_raw *= context->config.Krms;
    #else
    context->vars.rms_raw = context->config.Krms * PCTRL_SQRTF(
        ((context->vars.d * context->vars.d) + (context->vars.q * context->vars.q)));
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
    /*Clamp PI output*/
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
    /*Add regulator effort to the nominal frequency*/
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
    /*2PI Wrap around*/
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


void AcRmsPll_1ph_noinline(ac_rms_pll_context_t* const context, float32_t alpha, float32_t beta, float32_t* rms);


#endif /* AC_RMS_PLL_USE_VARIANT_1PH */

#ifdef __cplusplus
} /* Extern C */
#endif


#endif /* AC_RMS_PLL_1PH_H__ */


/* --- End of File ------------------------------------------------ */
