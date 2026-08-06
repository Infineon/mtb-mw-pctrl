/**
 * @file    rfc_F32.h
 * @brief   Variant F32 declarations for the reference frame control block.
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

#ifndef RFC_F32_H__
#define RFC_F32_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================================================================
 *   1. Include files
 * ==============================================================================
 */

#include "mtb_mw_pctrl_compiler.h"
#include "mtb_mw_pctrl_config.h"
#include "rfc_port.h"
#include "rfc_types.h"
#include "cy_pctrl_log.h"
#include "filter_3p3z.h"

#ifdef RFC_USE_VARIANT_F32

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */

#define RFC_F32_F32TOI16(x)     ((int16_t)( \
    (float32_t)(x) >= 0.0f? (int32_t)(float32_t)(x) : \
    (float32_t)(int16_t)(float32_t)(x) > (float32_t)(x)? (int32_t)(float32_t)((float32_t)(x) - 1.0f) : (int32_t)(float32_t)(x)))
#define RFC_F32_I32TOF32(x)     ((float32_t)(x))

#define RFC_F32_COS120               (-0.5f)           /* cos(2/3*PI rad) */
#define RFC_F32_SIN120               (0.866025403784f) /* sin(2/3*PI rad) */
#define RFC_F32_TWO_OVER_THREE       (2.0f / 3.0f)
#define RFC_F32_HALF                 (0.5f)
#define RFC_F32_TWO                  (2.0f)
#define RFC_F32_ONE_OVER_SQRT3       (0.577350269189f)

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
 * \addtogroup group_rfc_functions
 * \{
 * \brief API functions for RFC F32 variant
 * \details
 * This header provides declarations and inline implementations for the
 * single-precision floating-point RFC variant.
 *
 * The expected call sequence is:
 * 1. Populate context->config.
 * 2. Call reset/init APIs.
 * 3. Call RfcOuter_F32_inline/RfcOuter_F32_noinline and
 *    RfcInner_F32_inline/RfcInner_F32_noinline periodically.
 */

/**
 * \brief Reset RFC outer module state.
 * \details
 * Clears runtime variables in rfc_outer_context_t::vars and resets the
 * outer-loop Filter_3P3Z controller state.
 *
 * \param[in] context RFC outer context.
 */
void RfcOuterReset_F32(rfc_outer_context_t* const context);

/**
 * \brief Initialize RFC outer module state.
 * \details
 * Initializes outer-loop LPF states and initializes the embedded
 * Filter_3P3Z controller context from rfc_outer_context_t::config.
 *
 * \param[in] context RFC outer context.
 */
void RfcOuterInit_F32(rfc_outer_context_t* const context);

/**
 * \brief Reset RFC inner module state.
 * \details
 * Clears runtime variables in rfc_inner_context_t::vars and resets the d-axis
 * and q-axis Filter_3P3Z controller states.
 *
 * \param[in] context RFC inner context.
 */
void RfcInnerReset_F32(rfc_inner_context_t* const context);

/**
 * \brief Initialize RFC inner module state.
 * \details
 * Initializes inner-loop LPF states and initializes d-axis and q-axis
 * Filter_3P3Z controller contexts from rfc_inner_context_t::config.
 *
 * \param[in] context RFC inner context.
 */
void RfcInnerInit_F32(rfc_inner_context_t* const context);

/**
 * \brief Outer voltage control and capacitor balancing
 * \details
 * Executes one outer-loop control iteration.
 *
 * The function updates outer-loop internal states and writes:
 * - Idref through the output pointer
 * - Zero-sequence term into context->vars.ZeroSequence
 *
 * \param[in] context Pointer to the RFC context structure.
 * \param[in] Vc1 Top capacitor voltage.
 * \param[in] Vc2 Bottom capacitor voltage.
 * \param[out] Idref Reference current generated by the outer loop.
 */
PWRLIB_INLINE void RfcOuter_F32_inline(rfc_outer_context_t* const context, float32_t Vc1, float32_t Vc2,
                                       float32_t* Idref)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);
    cy_pctrl_assert_msg((Idref != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Output pointer is NULL in %s!\r\n", __func__);

    /* 1. LPF for vc1 and vc2
       2. sum and difference vc1 and vc2 filtered outputs
       3. 3P3Z for Idref
       4. Generating zero sequence using proportional controller
     */

    /* LPF for Vc1 and Vc2 */
    #ifdef RFC_LPF_CAP_VOLTAGE_EN
    context->vars.FilteredVc1 = (context->config.lpc_vc1_alpha * Vc1) +
                                (context->config.lpc_vc1_beta * context->vars.FilteredVc1);
    context->vars.FilteredVc2 = (context->config.lpc_vc2_alpha * Vc2) +
                                (context->config.lpc_vc2_beta * context->vars.FilteredVc2);
    #else
    context->vars.FilteredVc1 = Vc1;
    context->vars.FilteredVc2 = Vc2;
    #endif

    /* 3P3Z for Idref */
    float32_t dout_f32;
    #if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW) || defined(CY_DOXYGEN)
    {
        int16_t din0_i16 = RFC_F32_F32TOI16(context->vars.Vdc);
        int16_t din1_i16 = RFC_F32_F32TOI16(context->vars.FilteredVc1 + context->vars.FilteredVc2);
        int32_t dout_i32;
        Filter3p3z(&context->config.ctrl3p3z_ctx,
                   din0_i16,
                   din1_i16,
                   &dout_i32);
        #ifndef RFC_USE_LOCAL_STORAGE
        context->vars.ctrl3p3z_out = dout_i32;
        #endif
        dout_f32 = RFC_F32_I32TOF32(dout_i32);
        *Idref = context->config.scale_3p3z * dout_f32;
    }
    #else // if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW) || defined(CY_DOXYGEN)
    {
        Filter3p3z(&context->config.ctrl3p3z_ctx,
                   (context->vars.Vdc),
                   (context->vars.FilteredVc1 + context->vars.FilteredVc2),
                   &dout_f32);
        *Idref = dout_f32;
        #ifndef RFC_USE_LOCAL_STORAGE
        context->vars.ctrl3p3z_out = dout_f32;
        #endif
    }
    #endif //if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW) || defined(CY_DOXYGEN)

    /* Generating zero sequence using proportional controller */
    context->vars.ZeroSequence = context->config.Kp_zerosequence *
                                 (context->vars.FilteredVc1 - context->vars.FilteredVc2);
}


/**
 * \brief Current control block and modulation block
 * \details
 * Executes one inner-loop control iteration in the synchronous frame and
 * updates phase-voltage references (VaRef, VbRef, VcRef) in the context.
 *
 * This function reads context->vars.ZeroSequence and injects it into each
 * reconstructed phase-voltage reference for capacitor-voltage balancing.
 * The zero-sequence value must be set by the caller from the outer context
 * before this function is called:
 * \code
 * inner_ctx.vars.ZeroSequence = outer_ctx.vars.ZeroSequence;
 * \endcode
 *
 * \param[in] context Pointer to the RFC inner context structure.
 * \param[in] Idref   d-axis current reference in amperes, produced by RfcOuter_F32_inline().
 */
PWRLIB_INLINE void RfcInner_F32_inline(rfc_inner_context_t* const context, float32_t Idref)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    /* 1. Pre-computed dq voltage from upstream Park transform (ac_rms_pll)
       2. Optionally filter the dq frame voltages using lpf
       3. Convert 3 phase current to dq frame
       4. Optionally filter the dq frame currents using lpf
       5. Call Id and Iq 3P3Z controllers
       6. dq decoupling
       7. Inverse clarke and park transformations
       8. Optionally do common mode injection
     */

    float32_t sin_theta = PCTRL_RFC_SINF(context->vars.theta);
    float32_t cos_theta = PCTRL_RFC_COSF(context->vars.theta);
    #ifndef RFC_USE_LOCAL_STORAGE
    context->vars.sin_theta = sin_theta;
    context->vars.cos_theta = cos_theta;
    #endif

    /* Apply post-build scaling factor to grid positive sequence voltage components (feedforward) */
    float32_t vd_scaled = context->vars.vd_in * context->config.scale_vd_vq;
    float32_t vq_scaled = context->vars.vq_in * context->config.scale_vd_vq;
    #ifndef RFC_USE_LOCAL_STORAGE
    context->vars.vd_scaled = vd_scaled;
    context->vars.vq_scaled = vq_scaled;
    #endif

    /* Optionally filter the dq frame voltages using lpf */
    #if defined(RFC_LPF_DQ_VOLTAGE_EN)
    context->vars.VdF = (context->config.lpc_voltage_alpha * vd_scaled) +
                        (context->config.lpc_voltage_beta * context->vars.VdF);
    context->vars.VqF = (context->config.lpc_voltage_alpha * vq_scaled) +
                        (context->config.lpc_voltage_beta * context->vars.VqF);
    #else
    context->vars.VdF = vd_scaled;
    context->vars.VqF = vq_scaled;
    #endif

    /* Convert 3 phase current to dq frame */
    float32_t i_alpha = RFC_F32_TWO_OVER_THREE *
                        (context->vars.Ia - (RFC_F32_HALF * (context->vars.Ib + context->vars.Ic)));
    float32_t i_beta = RFC_F32_ONE_OVER_SQRT3 * (context->vars.Ib - context->vars.Ic);
    #ifndef RFC_USE_LOCAL_STORAGE
    context->vars.i_alpha = i_alpha;
    context->vars.i_beta = i_beta;
    #endif

    float32_t i_d = (i_alpha * sin_theta) - (i_beta * cos_theta);
    float32_t i_q = (i_alpha * cos_theta) + (i_beta * sin_theta);
    #ifndef RFC_USE_LOCAL_STORAGE
    context->vars.i_d = i_d;
    context->vars.i_q = i_q;
    #endif

    /* Optionally filter the dq frame currents using lpf */
    #ifdef RFC_LPF_CURRENT_EN
    context->vars.IdF = (context->config.lpc_current_alpha * i_d) +
                        (context->config.lpc_current_beta * context->vars.IdF);
    context->vars.IqF = (context->config.lpc_current_alpha * i_q) +
                        (context->config.lpc_current_beta * context->vars.IqF);
    #else
    context->vars.IdF = i_d;
    context->vars.IqF = i_q;
    #endif

    /* Call Id and Iq 3P3Z controllers */
    float32_t dout_d;
    float32_t dout_q;
    #if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW) || defined(CY_DOXYGEN)
    {
        int16_t din0_d_i16 = RFC_F32_F32TOI16(Idref);
        int16_t din1_d_i16 = RFC_F32_F32TOI16(context->vars.IdF);
        int16_t din0_q_i16 = RFC_F32_F32TOI16(context->config.iq_ref);
        int16_t din1_q_i16 = RFC_F32_F32TOI16(context->vars.IqF);
        int32_t dout_d_i32;
        int32_t dout_q_i32;
        Filter3p3z(&context->config.ctrl3p3z_ctx_d,
                   din0_d_i16,
                   din1_d_i16,
                   &dout_d_i32);
        Filter3p3z(&context->config.ctrl3p3z_ctx_q,
                   din0_q_i16,
                   din1_q_i16,
                   &dout_q_i32);

        #ifndef RFC_USE_LOCAL_STORAGE
        context->vars.ctrl3p3z_d_out = dout_d_i32;
        context->vars.ctrl3p3z_q_out = dout_q_i32;
        #endif
        dout_d = context->config.scale_3p3z * RFC_F32_I32TOF32(dout_d_i32);
        dout_q = context->config.scale_3p3z * RFC_F32_I32TOF32(dout_q_i32);
    }
    #else // if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW) || defined(CY_DOXYGEN)
    {
        Filter3p3z(&context->config.ctrl3p3z_ctx_d,
                   Idref,
                   context->vars.IdF,
                   &dout_d);
        Filter3p3z(&context->config.ctrl3p3z_ctx_q,
                   context->config.iq_ref,
                   context->vars.IqF,
                   &dout_q);
        #ifndef RFC_USE_LOCAL_STORAGE
        context->vars.ctrl3p3z_d_out = dout_d;
        context->vars.ctrl3p3z_q_out = dout_q;
        #endif
    }
    #endif // if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW) || defined(CY_DOXYGEN)

    /* dq decoupling using current-loop controller outputs */
    float32_t v_d_cmd = -(dout_d) +
                        (context->vars.VdF +
                         ((context->vars.omega * context->config.grid_inductance) * context->vars.IqF));
    float32_t v_q_cmd = -(dout_q) +
                        (context->vars.VqF -
                         ((context->vars.omega * context->config.grid_inductance) * context->vars.IdF));
    #ifndef RFC_USE_LOCAL_STORAGE
    context->vars.v_d_cmd = v_d_cmd;
    context->vars.v_q_cmd = v_q_cmd;
    #endif

    /* dq->abc inverse transform
     * sin/cos at theta+/-120deg are expanded via angle addition formula using
     * sin_theta and cos_theta (computed once above),
     * avoiding any additional trig function calls:
     *   sin(theta +/- 120) = -0.5*sin + sqrt(3)/2 * (+/-)cos
     *   cos(theta +/- 120) = -0.5*cos - sqrt(3)/2 * (+/-)sin
     */
    context->vars.VaRef = (v_d_cmd * sin_theta) +
                          (v_q_cmd * cos_theta) +
                          context->vars.ZeroSequence;

    context->vars.VbRef = (v_d_cmd * ((RFC_F32_COS120 * sin_theta) - (RFC_F32_SIN120 * cos_theta))) +
                          (v_q_cmd * ((RFC_F32_COS120 * cos_theta) + (RFC_F32_SIN120 * sin_theta))) +
                          context->vars.ZeroSequence;

    context->vars.VcRef = (v_d_cmd * ((RFC_F32_COS120 * sin_theta) + (RFC_F32_SIN120 * cos_theta))) +
                          (v_q_cmd * ((RFC_F32_COS120 * cos_theta) - (RFC_F32_SIN120 * sin_theta))) +
                          context->vars.ZeroSequence;

    /* Common-mode injection in modulation block */
    #ifdef RFC_USE_COMMON_MODE_INJECTION
    /* Common-mode injection using phase max/min centering */
    float32_t v_max = context->vars.VaRef;
    if (context->vars.VbRef > v_max)
    {
        v_max = context->vars.VbRef;
    }
    if (context->vars.VcRef > v_max)
    {
        v_max = context->vars.VcRef;
    }

    float32_t v_min = context->vars.VaRef;
    if (context->vars.VbRef < v_min)
    {
        v_min = context->vars.VbRef;
    }
    if (context->vars.VcRef < v_min)
    {
        v_min = context->vars.VcRef;
    }
    #ifndef RFC_USE_LOCAL_STORAGE
    context->vars.v_max = v_max;
    context->vars.v_min = v_min;
    #endif

    float32_t common_mode_total = -RFC_F32_HALF * (v_max + v_min);
    #ifndef RFC_USE_LOCAL_STORAGE
    context->vars.common_mode_total = common_mode_total;
    #endif
    context->vars.VaRef += common_mode_total;
    context->vars.VbRef += common_mode_total;
    context->vars.VcRef += common_mode_total;
    #endif // ifdef RFC_USE_COMMON_MODE_INJECTION

    /* Normalize phase references for modulation stage. */
    context->vars.VaRef = context->vars.VaRef * context->config.normalization_factor;
    context->vars.VbRef = context->vars.VbRef * context->config.normalization_factor;
    context->vars.VcRef = context->vars.VcRef * context->config.normalization_factor;
}


/**
 * \brief Execute the non-inlined RFC outer control step.
 * \details
 * Wrapper that calls RfcOuter_F32_inline().
 *
 * \param[in] context RFC outer context.
 * \param[in] Vc1 Top capacitor voltage.
 * \param[in] Vc2 Bottom capacitor voltage.
 * \param[out] Idref Reference current generated by the outer loop.
 */
void RfcOuter_F32_noinline(rfc_outer_context_t* const context, float32_t Vc1, float32_t Vc2, float32_t* Idref);

/**
 * \brief Execute the non-inlined RFC inner control step.
 * \details
 * Wrapper that calls RfcInner_F32_inline().
 *
 * \param[in] context RFC inner context.
 * \param[in] Idref d-axis current reference in amperes.
 */
void RfcInner_F32_noinline(rfc_inner_context_t* const context, float32_t Idref);


/** \} group_rfc_functions */

#endif /* ifdef RFC_USE_VARIANT_F32 */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif // ifndef RFC_F32_H__

/* --- End of File ------------------------------------------------ */
