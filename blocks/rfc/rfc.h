/**
 * @file    rfc.h
 * @brief   Main header for RFC (Reference Frame Control) block
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

#ifndef RFC_H__
#define RFC_H__

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

#ifdef RFC_USE_VARIANT_F32
#include "rfc_F32.h"
#endif

#include "rfc_types.h"

/**
 * \defgroup group_rfc RFC (reference frame control)
 *
 * \section sct_rfc_general Overview
 * <br>
 * The RFC (Reference Frame Control) block implements a cascaded controller for a three-phase
 * power stage. It is organized as two coordinated loops:
 * - The outer loop (RfcOuter) processes capacitor voltage measurements and computes a d-axis
 *   current reference (Idref).
 * - The inner loop (RfcInner) tracks the current reference in the synchronous reference frame
 *   and updates phase voltage commands.
 *
 * The block stores all runtime state in context structures and therefore supports
 * reentrant usage across multiple RFC instances.
 *
 * \section sct_rfc_configurable_features Configurable features
 * <br>
 * The RFC block provides the following configurable features:
 *
 * <table>
 * <tr><th>Configuration parameter</th><th>Name</th><th>Loop</th><th>Description</th></tr>
 * <tr><td>Inner LPF coefficients</td><td>lpc_voltage_alpha, lpc_voltage_beta,
 *     lpc_current_alpha, lpc_current_beta</td><td>Inner</td>
 *     <td>Coefficients for optional d/q-axis voltage and current low-pass filtering.</td></tr>
 * <tr><td>Outer LPF coefficients</td><td>lpc_vc1_alpha, lpc_vc1_beta,
 *     lpc_vc2_alpha, lpc_vc2_beta</td><td>Outer</td>
 *     <td>Coefficients for optional capacitor-voltage low-pass filtering.</td></tr>
 * <tr><td>Inner controller contexts</td><td>ctrl3p3z_ctx_d, ctrl3p3z_ctx_q</td><td>Inner</td>
 *     <td>d-axis and q-axis Filter_3P3Z controller contexts for the current control loop.</td></tr>
 * <tr><td>Outer controller context</td><td>ctrl3p3z_ctx</td><td>Outer</td>
 *     <td>Filter_3P3Z controller context for the outer voltage-control loop.</td></tr>
 * <tr><td>Filter output scaling</td><td>scale_3p3z</td><td>Both</td>
 *     <td>Scaling factor applied to Filter_3P3Z output. Present only when
 *     FILTER_3P3Z_USE_VARIANT_DF1_Q23 or FILTER_3P3Z_USE_VARIANT_HW is selected.</td></tr>
 * <tr><td>Inner current references</td><td>iq_ref</td><td>Inner</td>
 *     <td>q-axis current reference in amperes for the inner current controller.</td></tr>
 * <tr><td>Plant parameters</td><td>grid_inductance, scale_vd_vq</td><td>Inner</td>
 *     <td>Grid filter inductance and feedforward scaling factor for d/q voltage decoupling.</td></tr>
 * <tr><td>Modulation normalization</td><td>normalization_factor</td><td>Inner</td>
 *     <td>Multiplier applied after zero-sequence addition and optional common-mode injection.
 *     RFC does not clamp the resulting phase modulation references.</td></tr>
 * <tr><td>Zero-sequence gain</td><td>Kp_zerosequence</td><td>Outer</td>
 *     <td>Proportional gain for the capacitor-voltage balancing zero-sequence controller.</td></tr>
 * </table>
 *
 * \section sct_rfc_variants Variant and configuration options
 * The RFC block currently provides the F32 variant. The Filter_3P3Z implementation used by
 * its embedded voltage and current controllers is selected independently by the global
 * Filter_3P3Z configuration.
 * <br>
 * Macro name | Default | Macro purpose
 * -----------|---------|--------------
 * RFC_USE_VARIANT_F32 | Defined | Selects the F32 variant for the RFC block
 * RFC_USE_INLINE | Not defined | Enables inline execution for RfcInner and RfcOuter
 * RFC_LPF_CAP_VOLTAGE_EN | Not defined | Enables capacitor-voltage filtering in the outer loop
 * RFC_LPF_DQ_VOLTAGE_EN | Not defined | Enables d/q voltage filtering in the inner loop
 * RFC_LPF_CURRENT_EN | Not defined | Enables current-feedback filtering in the inner loop
 * RFC_USE_COMMON_MODE_INJECTION | Not defined | Enables common-mode injection before normalization
 * RFC_USE_CMSIS_DSP_LIB | Not defined | Uses CMSIS-DSP sine and cosine functions instead of C math
 * RFC_USE_LOCAL_STORAGE | Not defined | Stores intermediate calculations on the stack to reduce context size
 *
 * \subsection ssct_rfc_f32_variant F32 variant
 * <br>
 * This is the default variant. RFC-level filtering, transforms, decoupling, and modulation
 * calculations use single-precision floating-point arithmetic.
 * The design of this variant is shown in the block diagram below.
 * \image html ./images/RFC_block.svg width=60%
 * The F32 variant is selected by defining RFC_USE_VARIANT_F32 in the user config file.
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_rfc_f32
 *
 * **Performance optimization:**
 * <br>
 * To allow for optimization of execution time, the API functions can be
 * inlined by defining RFC_USE_INLINE in the user config file.
 * The default is no function inlining.
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_rfc_f32_inlined
 *
 * \subsection ssct_rfc_controller_backend Controller arithmetic and Filter_3P3Z backend
 * The F32 variant performs the measurement filtering, reference-frame transformations,
 * decoupling, common-mode calculation, and modulation normalization in float32 arithmetic.
 * The three embedded Filter_3P3Z controllers use the globally selected Filter_3P3Z variant:
 *
 * <table>
 * <tr><th>Filter_3P3Z variant</th><th>RFC controller interface</th></tr>
 * <tr><td>FILTER_3P3Z_USE_VARIANT_DF2_F32</td>
 *     <td>Controller inputs and outputs remain float32. The scale_3p3z configuration
 *     fields are not present.</td></tr>
 * <tr><td>FILTER_3P3Z_USE_VARIANT_DF1_Q23</td>
 *     <td>RFC converts controller inputs to int16_t and converts int32_t controller
 *     outputs back to float32 using scale_3p3z.</td></tr>
 * <tr><td>FILTER_3P3Z_USE_VARIANT_HW</td>
 *     <td>Uses the same RFC conversion and scaling path as DF1-Q23.</td></tr>
 * </table>
 *
 * For the DF1-Q23 and hardware variants, RFC does not automatically convert physical units
 * to a normalized Q0.15 representation. The float32 controller inputs are converted directly
 * to int16_t: positive fractional values are truncated and negative fractional values are
 * rounded down. This conversion does not saturate. The application must therefore ensure that
 * the controller inputs remain representable by int16_t and that the Filter_3P3Z coefficients,
 * gains, and scale_3p3z values are designed for the chosen signal domain.
 *
 * \note The same global Filter_3P3Z variant is used by all three RFC controller contexts.
 * \note The library default Filter_3P3Z variant is DF1-Q23 unless the application selects
 *       another variant in mtb_mw_pctrl_user_config.h.
 *
 * When RFC_USE_LOCAL_STORAGE is defined, intermediate calculations are stored on the stack
 * instead of in the context structures. Application-provided inputs, persistent filter state,
 * and outputs remain in the context and use the same application interface.
 *
 * \note Common-mode injection is calculated after ZeroSequence has been added equally to all
 *       three phase commands. The phase max/min centering therefore removes that equal
 *       zero-sequence component from the final modulation references. The application must
 *       account for this interaction when using capacitor-voltage balancing.
 *
 * **Using the API**
 * <br>
 * \snippet{doc} this snippet_rfc_using_the_API
 *
 * The following example selects the DF2-F32 Filter_3P3Z backend and enables all optional
 * RFC signal filters:
 * \snippet code_snippets snippet_mw_pctrl_user_config_rfc_f32_example
 *
 * Example usage of the RFC F32 variant. The coefficients and scaling values are application
 * specific and must be designed for the converter and the execution periods of both loops:
 * \snippet code_snippets snippet_using_rfc_f32
 *
 */
/*
   [snippet_rfc_using_the_API]
   The application must allocate the context structures (rfc_inner_context_t and rfc_outer_context_t)
   in a memory section of choice.
   It must set the values of the configuration sub-structures
   (rfc_inner_context_t::config and rfc_outer_context_t::config).
   The vars sub-structures combine application-provided runtime inputs, RFC-maintained state,
   and outputs. The application must not initialize or modify the complete vars structures.
   The reset and init functions initialize the RFC-maintained members.
   Before each outer-loop iteration, the application updates rfc_outer_context_t::vars.Vdc and
   passes the capacitor voltages to RfcOuter().
   Before each inner-loop iteration, the application updates theta, omega, vd_in, vq_in,
   Ia, Ib, and Ic in rfc_inner_context_t::vars. It also copies the latest ZeroSequence output
   from the outer context into rfc_inner_context_t::vars.ZeroSequence.
   All other vars members are owned by RFC and must not be modified during operation.
   The application has to call the block reset functions (@ref RfcInnerReset, @ref RfcOuterReset)
   to clear the vars sub-structures and the block init functions
   (@ref RfcInnerInit, @ref RfcOuterInit) to initialize the internal filter states.
   [snippet_rfc_using_the_API]
 */

/**
 * \defgroup group_rfc_macros_and_enums Macros and enumerated types
 * \ingroup group_rfc
 * \{
 * \brief Configuration macros for RFC block
 *
 * These macros can be defined in mtb_mw_pctrl_user_config.h to configure the
 * RFC variant and optional features.
 *
 * <b>Variant selection macros:</b>
 *  - <b>RFC_USE_VARIANT_F32</b> (default)<br>
 *    Selects the F32 variant for RFC block.
 *
 * <b>Performance optimization:</b>
 *  - <b>RFC_USE_INLINE</b><br>
 *    Enable function inlining for RFC. Define this macro to inline the main processing functions
 *    for improved execution time. This trades code size for performance.
 *
 * <b>Optional features (disabled by default):</b>
 *  - <b>RFC_LPF_CAP_VOLTAGE_EN</b><br>
 *    Enables low-pass filtering on outer-loop capacitor voltage measurements (Vc1, Vc2).
 *  - <b>RFC_LPF_DQ_VOLTAGE_EN</b><br>
 *    Enables low-pass filtering on inner-loop d/q-axis voltage inputs.
 *  - <b>RFC_LPF_CURRENT_EN</b><br>
 *    Enables low-pass filtering on inner-loop d/q-axis current feedback.
 *  - <b>RFC_USE_COMMON_MODE_INJECTION</b><br>
 *    Enables common-mode voltage injection in the modulation stage for improved DC-link utilization.
 *  - <b>RFC_USE_CMSIS_DSP_LIB</b><br>
 *    Uses CMSIS-DSP optimized trigonometric functions (sine/cosine) via rfc_port.h instead of standard math library.
 *  - <b>RFC_USE_LOCAL_STORAGE</b><br>
 *    Stores intermediate calculations on the stack instead of in the context structure, reducing context size.
 */

/** \} group_rfc_macros_and_enums */

/**
 * \defgroup group_rfc_typedefs_and_structures Typedefs and structures
 * \ingroup group_rfc
 * \defgroup group_rfc_functions Functions
 * \ingroup group_rfc
 */

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */

#ifdef RFC_USE_VARIANT_F32
#define RfcInnerReset RfcInnerReset_F32
#define RfcInnerInit RfcInnerInit_F32
#define RfcOuterReset RfcOuterReset_F32
#define RfcOuterInit RfcOuterInit_F32
#ifdef RFC_USE_INLINE
#define RfcOuter RfcOuter_F32_inline
#define RfcInner RfcInner_F32_inline
#else
#define RfcOuter RfcOuter_F32_noinline
#define RfcInner RfcInner_F32_noinline
#endif
#endif /* ifdef RFC_USE_VARIANT_F32 */

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
 * \brief API functions for RFC block
 * \details
 * Each function of the API accepts a pointer to a RFC context instance as first parameter.
 * The context is self-contained. No additional local or global static data is needed for RFC operation.
 * This makes the implementation reentrant.
 *
 * Before using the RFC, the application must follow this sequence:
 * 1. Populate the rfc_inner_context_t::config and rfc_outer_context_t::config fields, including
 *    the filter_3p3z_context_t members embedded inside config.
 * 2. Call @ref RfcInnerReset and @ref RfcOuterReset to zero all runtime state.
 * 3. Call @ref RfcInnerInit and @ref RfcOuterInit to initialize filter states from the populated config.
 *
 * The @ref RfcInner and @ref RfcOuter functions are the main processing functions of the RFC block.
 * They execute one iteration of the inner and outer control loops, respectively.
 *
 * Before each @ref RfcOuter call, the application updates rfc_outer_context_t::vars.Vdc.
 * Before each @ref RfcInner call, the application updates the inner-context input members
 * theta, omega, vd_in, vq_in, Ia, Ib, and Ic, and copies the latest outer-loop
 * ZeroSequence output into the inner context. The remaining vars members are maintained by RFC.
 *
 */


/**
 * \brief Initialize RFC outer module working state.
 *
 * \details
 * Initializes the runtime state used by the outer voltage-control loop.
 * This function clears the low-pass filter states for the capacitor-voltage
 * measurements and initializes the embedded 3p3z controller context stored in
 * rfc_outer_context_t::config.
 *
 * \param[in] context RFC outer context
 *
 */
void RfcOuterInit(rfc_outer_context_t* const context);

/**
 * \brief Initialize RFC inner module working state.
 *
 * \details
 * Initializes the runtime state used by the inner current-control loop.
 * This function clears the low-pass filter states for the d-axis and q-axis
 * voltage/current signals and initializes the embedded d-axis and q-axis 3p3z
 * controller contexts stored in rfc_inner_context_t::config.
 *
 * \param[in] context RFC inner context
 *
 */
void RfcInnerInit(rfc_inner_context_t* const context);

/**
 * \brief Reset RFC inner module state.
 *
 * \details
 * Clears (zeros) all internal runtime variables stored in rfc_inner_context_t::vars:
 *  - Grid-voltage angle and frequency (theta, omega)
 *  - Three-phase currents (Ia, Ib, Ic)
 *  - d/q-axis grid voltage inputs (vd_in, vq_in)
 *  - Filtered d/q-axis currents (IdF, IqF)
 *  - Filtered d/q-axis voltages (VdF, VqF)
 *  - Phase-voltage references (VaRef, VbRef, VcRef)
 *  - Zero-sequence term from outer loop (ZeroSequence) *
 *
 * Additionally, when RFC_USE_LOCAL_STORAGE is not defined, the following
 * intermediate variables are also reset:
 *  - Pre-computed trig values (sin_theta, cos_theta)
 *  - Scaled d/q-axis grid voltages (vd_scaled, vq_scaled)
 *  - alpha/beta-axis currents (i_alpha, i_beta)
 *  - d/q-axis currents (i_d, i_q)
 *  - d/q-axis controller outputs (ctrl3p3z_d_out, ctrl3p3z_q_out)
 *  - d/q-axis command voltages (v_d_cmd, v_q_cmd)
 *  - Phase min/max for common-mode centering (v_min, v_max)
 *  - Common-mode injection total (common_mode_total) *
 * \param[in] context RFC inner context
 *
 */
void RfcInnerReset(rfc_inner_context_t* const context);

/**
 * \brief Reset RFC outer module state.
 *
 * \details
 * Clears the complete rfc_outer_context_t::vars structure and resets the embedded
 * Filter_3P3Z controller state. This also clears the application-provided Vdc input.
 * The application must update Vdc before the next RfcOuter() call.
 *
 * \note Does not modify scalar RFC configuration values or embedded Filter_3P3Z
 *       coefficients. Runtime state inside the embedded controller context is reset.
 *
 * Additionally, when RFC_USE_LOCAL_STORAGE is not defined, the outer-loop
 * controller output (ctrl3p3z_out) is also reset.
 *
 * \param[in] context RFC outer context
 */
void RfcOuterReset(rfc_outer_context_t* const context);


/**
 * \brief Execute RFC outer control step.
 * \details
 * Executes one iteration of the outer voltage-control loop.
 * This function uses the runtime signals stored in the RFC outer context and
 * the provided capacitor voltage measurements to compute an updated d-axis current reference.
 * The application must update rfc_outer_context_t::vars.Vdc before the call.
 *
 * The function writes two outputs into the context:
 * - The d-axis current reference (*Idref) via the output pointer.
 * - The zero-sequence balancing voltage (context->vars.ZeroSequence), computed from
 *   the filtered capacitor-voltage difference scaled by Kp_zerosequence.
 *
 * Processing sequence:
 * 1. Update capacitor-voltage signals:
 *    - If RFC_LPF_CAP_VOLTAGE_EN is defined, first-order LPFs are applied to
 *      Vc1 and Vc2 using lpc_vc1_alpha/lpc_vc1_beta and
 *      lpc_vc2_alpha/lpc_vc2_beta, producing FilteredVc1 and FilteredVc2.
 *    - Otherwise, FilteredVc1 = Vc1 and FilteredVc2 = Vc2.
 * 2. Run the outer Filter_3P3Z controller using:
 *    - reference input: context->vars.Vdc
 *    - feedback input:  FilteredVc1 + FilteredVc2
 *    and write the resulting d-axis current reference to *Idref.
 *    For fixed-point / HW Filter_3P3Z variants, conversion/scaling is applied
 *    using RFC_F32_F32TOI16, RFC_F32_I32TOF32, and context->config.scale_3p3z.
 * 3. Compute zero-sequence balancing term:
 *    context->vars.ZeroSequence = Kp_zerosequence * (FilteredVc1 - FilteredVc2).
 *
 *
 * \note Before calling RfcInner(), the application must copy the computed zero-sequence
 * value from the outer context to the inner context:
 * \code
 * inner_ctx.vars.ZeroSequence = outer_ctx.vars.ZeroSequence;
 * \endcode
 *
 * \param[in]  context RFC outer context.
 * \param[in]  Vc1    Top capacitor voltage.
 * \param[in]  Vc2    Bottom capacitor voltage.
 * \param[out] Idref  d-axis current reference generated by the outer voltage-control loop.
 */
#ifdef RFC_USE_INLINE
PWRLIB_INLINE void RfcOuter(rfc_outer_context_t* const context, float32_t Vc1, float32_t Vc2, float32_t* Idref);
#else
void RfcOuter(rfc_outer_context_t* const context, float32_t Vc1, float32_t Vc2, float32_t* Idref);
#endif

/**
 * \brief Execute RFC inner control step.
 * \details
 * Executes one iteration of the inner current-control loop in the synchronous
 * reference frame.
 *
 * This function reads context->vars.ZeroSequence and adds it to the reconstructed
 * phase-voltage references (VaRef, VbRef, VcRef) for capacitor-voltage balancing.
 * The zero-sequence value is produced by RfcOuter() and written to
 * rfc_outer_context_t::vars.ZeroSequence.
 *
 * Before the call, the application must update context->vars.theta,
 * context->vars.omega, context->vars.vd_in, context->vars.vq_in,
 * context->vars.Ia, context->vars.Ib, and context->vars.Ic.
 *
 * \pre Before calling this function the application must copy the zero-sequence
 * output from the outer context into the inner context:
 * \code
 * inner_ctx.vars.ZeroSequence = outer_ctx.vars.ZeroSequence;
 * \endcode
 *
 * Processing sequence:
 * 1. Apply feedforward scaling to precomputed dq voltages:
 *    vd_scaled = vd_in * scale_vd_vq and vq_scaled = vq_in * scale_vd_vq.
 * 2. Optionally LPF the dq voltages (RFC_LPF_DQ_VOLTAGE_EN) to update VdF/VqF.
 * 3. Transform measured phase currents (Ia, Ib, Ic) to i_alpha/i_beta, then to i_d/i_q.
 * 4. Optionally LPF the dq currents (RFC_LPF_CURRENT_EN) to update IdF/IqF.
 * 5. Execute d-axis and q-axis Filter_3P3Z controllers:
 *    - d-axis reference is Idref
 *    - q-axis reference is context->config.iq_ref
 *    For fixed-point / HW Filter_3P3Z variants, conversion/scaling is applied
 *    using RFC_F32_F32TOI16, RFC_F32_I32TOF32, and context->config.scale_3p3z.
 * 6. Apply dq decoupling to form voltage commands v_d_cmd and v_q_cmd using
 *    omega, grid_inductance, IdF/IqF, and filtered dq voltages.
 * 7. Perform dq->abc inverse transform to generate VaRef, VbRef, and VcRef.
 * 8. Add context->vars.ZeroSequence to each phase command for capacitor balancing.
 * 9. If RFC_USE_COMMON_MODE_INJECTION is enabled, compute phase max/min,
 *     inject common-mode offset, and recenter phase commands.
 * 10. Normalize VaRef/VbRef/VcRef using normalization_factor.
 *
 * The resulting phase commands are optionally common-mode centered and then multiplied by
 * rfc_inner_config_t::normalization_factor. The final modulation references are written to
 * rfc_inner_context_t::vars (VaRef, VbRef, and VcRef).
 *
 * \note RFC does not clamp VaRef, VbRef, or VcRef. The application must select
 * normalization_factor and any downstream limiting so that the references remain within
 * the range accepted by the modulation stage.
 *
 * \param[in] context RFC inner context.
 * \param[in] Idref   d-axis current reference in amperes, produced by RfcOuter().
 */
#ifdef RFC_USE_INLINE
PWRLIB_INLINE void RfcInner(rfc_inner_context_t* const context, float32_t Idref);
#else
void RfcInner(rfc_inner_context_t* const context, float32_t Idref);
#endif

/** \} group_rfc_functions */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif // ifndef RFC_H__

/* --- End of File ------------------------------------------------ */
