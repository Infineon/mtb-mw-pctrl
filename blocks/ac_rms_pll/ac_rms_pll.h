/**
 * @file ac_rms_pll.h
 * @brief API for a grid phase tracking PLL with RMS output.
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

#ifndef AC_RMS_PLL_H__
#define AC_RMS_PLL_H__

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

#ifdef AC_RMS_PLL_USE_VARIANT_3PH
#include "ac_rms_pll_3ph.h"
#endif /* ifdef AC_RMS_PLL_USE_VARIANT_3PH */

#ifdef AC_RMS_PLL_USE_VARIANT_3PHDD
#include "ac_rms_pll_3phdd.h"
#endif /* ifdef AC_RMS_PLL_USE_VARIANT_3PHDD */

#ifdef AC_RMS_PLL_USE_VARIANT_1PH
#include "ac_rms_pll_1ph.h"
#endif /* ifdef AC_RMS_PLL_USE_VARIANT_1PH */

#include "ac_rms_pll_types.h"

/**
 * \defgroup group_ac_rms_pll AC RMS PLL
 *
 * \section sct_ac_rms_pll_general Overview
 * <br>
 * This block implements Phase-Locked Loops (PLL) for tracking the phase angle
 * of AC signals and calculating real-time RMS values along with the frequency information.
 * The block provides robust phase tracking capabilities for grid-tied power converters
 * and AC measurement applications. All variants are based on the Synchronous Reference
 * Frame Phase-Locked Loop (SRF-PLL) architecture, which uses the Park transformation
 * to project the input signals onto a rotating reference frame and drives the
 * quadrature (or direct) axis component to zero via a PI controller to lock onto
 * the grid phase angle.
 *
 * \section sct_ac_rms_pll_configurable_features Configurable features
 * <br>
 * The AC RMS PLL provides the following configurable features:
 *
 * <table>
 * <tr><th>Configuration parameter</th><th>Name</th>
 *     <th>Applicable variant</th><th>Description</th></tr>
 * <tr><td>Nominal frequency</td><td>omega_n</td>
 *     <td>All</td>
 *     <td>Nominal frequency in rad/s used as init value of the PLL output</td></tr>
 * <tr><td>RMS lowpass alpha</td><td>lpc_rms_alpha</td>
 *     <td>All</td>
 *     <td>Coefficient alpha for low pass filter for RMS value
 *     (\f$\frac{\alpha \cdot z}{z - \beta}\f$)</td></tr>
 * <tr><td>RMS lowpass beta</td><td>lpc_rms_beta</td>
 *     <td>All</td>
 *     <td>Coefficient beta for low pass filter for RMS value
 *     (\f$\frac{\alpha \cdot z}{z - \beta}\f$)</td></tr>
 * <tr><td>RMS lowpass initial value</td><td>lpc_rms_init</td>
 *     <td>All</td>
 *     <td>Initial value of output of low pass filter for RMS value
 *     (\f$\frac{\alpha \cdot z}{z - \beta}\f$)</td></tr>
 * <tr><td>Frequency lowpass alpha</td><td>lpc_omega_alpha</td>
 *     <td>All</td>
 *     <td>Coefficient alpha for low pass filter for frequency
 *     (\f$\frac{\alpha \cdot z}{z - \beta}\f$)</td></tr>
 * <tr><td>Frequency lowpass beta</td><td>lpc_omega_beta</td>
 *     <td>All</td>
 *     <td>Coefficient beta for low pass filter for frequency
 *     (\f$\frac{\alpha \cdot z}{z - \beta}\f$)</td></tr>
 * <tr><td>Frequency lowpass initial value</td><td>lpc_omega_init</td>
 *     <td>All</td>
 *     <td>Initial value of output of low pass filter for frequency
 *     (\f$\frac{\alpha \cdot z}{z - \beta}\f$)</td></tr>
 * <tr><td>PI output lower limit</td><td>cterm_min</td>
 *     <td>All</td><td>Lower limit for PI regulator output</td></tr>
 * <tr><td>PI output upper limit</td><td>cterm_max</td>
 *     <td>All</td><td>Upper limit for PI regulator output</td></tr>
 * <tr><td>Proportional gain</td><td>Kp</td>
 *     <td>All</td>
 *     <td>Proportional gain for PI regulator
 *     (not used in 3PHDD with HW controller)</td></tr>
 * <tr><td>Integral gain</td><td>Ki</td>
 *     <td>All</td>
 *     <td>Integral gain for PI regulator
 *     (not used in 3PHDD with HW controller)</td></tr>
 * <tr><td>Sampling time</td><td>dt</td>
 *     <td>All</td><td>Sampling time or loop period in seconds</td></tr>
 * <tr><td>Sequence filter LPC alpha</td><td>lpc_dd_alpha</td>
 *     <td>3PHDD</td>
 *     <td>Coefficient alpha for low pass filter for sequence extraction
 *     (\f$\frac{\alpha \cdot z}{z - \beta}\f$)</td></tr>
 * <tr><td>Sequence filter LPC beta</td><td>lpc_dd_beta</td>
 *     <td>3PHDD</td>
 *     <td>Coefficient beta for low pass filter for sequence extraction
 *     (\f$\frac{\alpha \cdot z}{z - \beta}\f$)</td></tr>
 * <tr><td>Sequence filter initial value</td><td>lpc_dd_init</td>
 *     <td>3PHDD</td>
 *     <td>Initial value of output of low pass filter for sequence extraction
 *     (\f$\frac{\alpha \cdot z}{z - \beta}\f$)</td></tr>
 * <tr><td>3P3Z controller context</td><td>ctrl3p3z_ctx</td>
 *     <td>3PHDD (optional)</td>
 *     <td>Hardware 3P3Z controller context
 *     (when AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW is defined).
 *     See \ref group_filter_3p3z for details.</td></tr>
 * <tr><td>3P3Z scaling factor</td><td>scale_3p3z</td>
 *     <td>3PHDD (optional)</td>
 *     <td>Scaling factor to convert Q0.23 to float for 3P3Z output</td></tr>
 * <tr><td>OSG damping factor</td><td>Kosg</td>
 *     <td>1PH</td>
 *     <td>Damping factor for orthogonal signal generator</td></tr>
 * <tr><td>RMS calculation gain</td><td>Krms</td>
 *     <td>1PH</td><td>Gain for RMS calculation</td></tr>
 * <tr><td>DC offset compensation gain</td><td>Kdc</td>
 *     <td>1PH (optional)</td>
 *     <td>Gain for DC offset compensation
 *     (when ACRMSPLL_DCOFFSET_REJECT is defined)</td></tr>
 * <tr><td>DC offset maximum limit</td><td>dc_max</td>
 *     <td>1PH (optional)</td>
 *     <td>Maximum limit for DC offset
 *     (when ACRMSPLL_DCOFFSET_REJECT is defined)</td></tr>
 * <tr><td>DC offset minimum limit</td><td>dc_min</td>
 *     <td>1PH (optional)</td>
 *     <td>Minimum limit for DC offset
 *     (when ACRMSPLL_DCOFFSET_REJECT is defined)</td></tr>
 * <tr><td>3rd-harmonic OSG damping factor</td><td>Kosg3</td>
 *     <td>1PH (optional)</td>
 *     <td>Damping factor for 3rd-harmonic OSG
 *     (when ACRMSPLL_3RD_HARMONIC_REJECT is defined)</td></tr>
 * <tr><td>5th-harmonic OSG damping factor</td><td>Kosg5</td>
 *     <td>1PH (optional)</td>
 *     <td>Damping factor for 5th-harmonic OSG
 *     (when ACRMSPLL_5TH_HARMONIC_REJECT is defined)</td></tr>
 * </table>
 *
 * \section sct_ac_rms_pll_variants Variants and configuration options
 * This section describes the available variants and configuration options.
 * All variants are based on the SRF-PLL technique and share the same API so that
 * changing the block interface does not require further changes to the application
 * code when switching between the variants.
 * <br>
 * <table>
 * <tr><th>Macro name</th><th>Macro purpose</th></tr>
 * <tr><td>AC_RMS_PLL_USE_VARIANT_3PH</td>
 *     <td>Use the 3-phase variant of ac_rms_pll (default)</td></tr>
 * <tr><td>AC_RMS_PLL_USE_VARIANT_3PHDD</td>
 *     <td>Use the 3-phase double-decoupling variant of ac_rms_pll</td></tr>
 * <tr><td>AC_RMS_PLL_USE_VARIANT_1PH</td>
 *     <td>Use the 1-phase variant of ac_rms_pll</td></tr>
 * <tr><td>AC_RMS_PLL_USE_INLINE</td>
 *     <td>Use function inlining for ac_rms_pll</td></tr>
 * <tr><td>AC_RMS_PLL_FREQ_FILTER</td>
 *     <td>Enable the frequency low-pass filter</td></tr>
 * <tr><td>AC_RMS_PLL_RMS_FILTER</td>
 *     <td>Enable the RMS low-pass filter</td></tr>
 * <tr><td>ACRMSPLL_DCOFFSET_REJECT</td>
 *     <td>Enable DC offset rejection (1-phase only)</td></tr>
 * <tr><td>ACRMSPLL_3RD_HARMONIC_REJECT</td>
 *     <td>Enable 3rd-harmonic rejection (1-phase only)</td></tr>
 * <tr><td>ACRMSPLL_5TH_HARMONIC_REJECT</td>
 *     <td>Enable 5th-harmonic rejection (1-phase only)</td></tr>
 * <tr><td>AC_RMS_PLL_USE_CMSIS_DSP_LIB</td>
 *     <td>Use ARM CMSIS DSP library for mathematical operations</td></tr>
 * <tr><td>AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW</td>
 *     <td>Use hardware 3P3Z controller (3PHDD only)</td></tr>
 * </table>
 * <br>
 * **Three-phase variant**
 * <br>
 * This is the default variant that implements the SRF-PLL for three-phase systems
 * using balanced alpha-beta frame inputs. It utilizes the Park transformation for
 * phase detection by minimizing the voltage projected on the quadrature axis.
 * This variant is optimized for three-phase grid applications and provides excellent
 * tracking performance under balanced conditions.
 * The design of this variant is shown in the block diagram below.
 *
 * \image html ./images/srf_pll.svg width=60%
 *
 * The three-phase variant is selected by defining AC_RMS_PLL_USE_VARIANT_3PH
 * in the user config file. This is the default variant.
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_3ph
 *
 * **Three-phase double decoupling variant:**
 * <br>
 * This variant extends the SRF-PLL with a Double-Decoupled Synchronous Reference
 * Frame (DDSRF) decoupling network that eliminates double-frequency oscillations
 * during unbalanced grid conditions. This provides superior phase tracking accuracy
 * under voltage sags, single-phase faults, and other grid imbalance conditions.
 *
 * The DDSRF variant uses two synchronous reference frames (positive and negative sequence)
 * and a cross-decoupling mechanism to separate sequence components, allowing the PLL to
 * track only the positive sequence component even under severe unbalance.
 * The design of this variant is shown in the block diagram below.
 * \image html ./images/3phasedd.svg width=60%
 *
 * Optionally, the PI controller can be replaced with a hardware-accelerated 3P3Z filter
 * by defining AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW. In this case, the ctrl3p3z_ctx field
 * in the configuration must point to an initialized \ref group_filter_3p3z instance.
 * See @ref group_filter_3p3z for details on configuring the 3P3Z filter. This feature
 * is only available for 3PHDD variant. Support for other variants shall be added in the
 * future.
 *
 * The three-phase double-decoupling variant is selected by defining AC_RMS_PLL_USE_VARIANT_3PHDD
 * in the user config file.
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_3phdd
 *
 * **Single-phase variant**
 * <br>
 * This variant implements the SRF-PLL for single-phase systems. Since only a
 * single-phase signal is available, a Second Order Generalized Integrator (SOGI)
 * is used to generate the orthogonal component (beta) from the input signal (alpha).
 * The SOGI acts as an Orthogonal Signal Generator (OSG) that creates the missing
 * quadrature signal required for SRF-PLL operation.
 * Once both alpha and beta components are available, the single-phase variant uses
 * the same SRF-PLL architecture as the three-phase variant, providing robust phase
 * tracking capabilities for single-phase grid applications.
 * The design of this variant is shown in the block diagram below.
 * \image html ./images/single_phase_srf_pll.svg width=60%
 * The single-phase variant is selected by defining AC_RMS_PLL_USE_VARIANT_1PH
 * in the user config file.
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_1ph
 *
 * Additional features available for the single-phase variant
 * - DC offset rejection for tracking and removing DC offsets in the input signal
 * - 3rd-harmonic rejection for tracking and removing 3rd harmonic distortions
 * - 5th-harmonic rejection for tracking and removing 5th harmonic distortions
 *
 * All single-phase features can be enabled together:
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_1ph_with_features
 *
 * **Performance optimization**
 * <br>
 * To allow for optimization of execution time, the API functions can be
 * inlined by defining AC_RMS_PLL_USE_INLINE in the user config file.
 * The default is no function inlining.
 *
 * **Using the API**
 * <br>
 * \snippet{doc} this snippet_ac_rms_pll_using_the_API
 *
 * Example usage of the three-phase variant:
 * \snippet code_snippets snippet_using_ac_rms_pll_3ph
 */
/*
   [snippet_ac_rms_pll_using_the_API]
   The application must allocate the context structure (ac_rms_pll_context_t) in a memory section of choice.
   It must set the values of the configuration sub-structure (ac_rms_pll_context_t::config).
   The static sub-structure (ac_rms_pll_context_t::vars) contains the static memory the block needs to operate.
   It must not be initialized by the application.
   The application has to call the block reset function (@ref AcRmsPllReset) to clear the static sub-structure and
   the block init function (@ref AcRmsPllInit) to initialize the internal configuration.
   [snippet_ac_rms_pll_using_the_API]
 */

/**
 * \defgroup group_ac_rms_pll_macros_and_enums Macros and enumerated types
 * \ingroup group_ac_rms_pll
 * \{
 * \brief Configuration macros for AC RMS PLL block
 *
 * These macros should be defined in mtb_mw_pctrl_user_config.h to configure the
 * AC RMS PLL variant and features. Only one variant selection macro should be
 * defined at a time.
 *
 * <b>Variant selection macros (define ONE):</b>
 *
 * - <b>AC_RMS_PLL_USE_VARIANT_3PH</b> (default)<br>
 *   Use three-phase variant of AC RMS PLL. Select the standard three-phase SRF-PLL variant.
 *   This variant uses balanced alpha-beta frame inputs and is optimized for three-phase grid
 *   applications under balanced conditions.
 *
 * - <b>AC_RMS_PLL_USE_VARIANT_3PHDD</b><br>
 *   Use three-phase double-decoupling variant of AC RMS PLL. Select the DDSRF-PLL variant
 *   for superior performance under unbalanced grid conditions. This variant implements a
 *   decoupling network that eliminates double-frequency oscillations during voltage sags
 *   and single-phase faults.
 *
 * - <b>AC_RMS_PLL_USE_VARIANT_1PH</b><br>
 *   Use single-phase variant of AC RMS PLL. Select the single-phase SRF-PLL variant with
 *   SOGI for quadrature signal generation. This variant supports optional DC offset rejection
 *   and harmonic rejection features.
 *
 * <b>Performance optimization:</b>
 *
 * - <b>AC_RMS_PLL_USE_INLINE</b><br>
 *   Enable function inlining for AC RMS PLL. Define this macro to inline the main processing
 *   function for improved execution time. This trades code size for performance.
 *
 * <b>Common feature enable macros (all variants):</b>
 *
 * - <b>AC_RMS_PLL_FREQ_FILTER</b><br>
 *   Enable frequency low-pass filter. Define this macro to enable low-pass filtering of the
 *   PLL frequency output. This reduces noise in the frequency measurement at the cost of
 *   slower transient response. Filter coefficients are configured via lpc_omega_alpha and
 *   lpc_omega_beta parameters.
 *
 * - <b>AC_RMS_PLL_RMS_FILTER</b><br>
 *   Enable RMS low-pass filter. Define this macro to enable low-pass filtering of the RMS
 *   output. This reduces noise in the RMS measurement at the cost of slower transient response.
 *   Filter coefficients are configured via lpc_rms_alpha and lpc_rms_beta parameters.
 *
 * - <b>AC_RMS_PLL_D_AXIS_ALIGN</b><br>
 *   Align PLL to d-axis instead of q-axis. Define this macro to change PLL alignment from
 *   q-axis (default) to d-axis. This affects which component is driven to zero by the PI
 *   controller. Most applications use q-axis alignment (default).
 *
 * - <b>AC_RMS_PLL_USE_CMSIS_DSP_LIB</b><br>
 *   Use ARM CMSIS DSP library for math operations. Define this macro to use ARM CMSIS DSP
 *   library functions for sine, cosine, and square root operations. This can improve
 *   performance on ARM Cortex-M processors with DSP extensions.
 *
 * <b>Single-phase variant features (AC_RMS_PLL_USE_VARIANT_1PH only):</b>
 *
 * - <b>ACRMSPLL_DCOFFSET_REJECT</b><br>
 *   Enable DC offset rejection. Define this macro to enable DC offset tracking and
 *   compensation in the single-phase variant. This feature uses an additional integrator
 *   to track and remove DC offsets in the input signal. Requires configuration of Kdc,
 *   dc_max, and dc_min parameters.
 *
 * - <b>ACRMSPLL_3RD_HARMONIC_REJECT</b><br>
 *   Enable 3rd harmonic rejection. Define this macro to enable 3rd harmonic tracking and
 *   rejection in the single-phase variant. This feature uses an additional SOGI tuned to
 *   3x fundamental frequency to track and remove 3rd harmonic distortion. Requires
 *   configuration of Kosg3 parameter.
 *
 * - <b>ACRMSPLL_5TH_HARMONIC_REJECT</b><br>
 *   Enable 5th harmonic rejection. Define this macro to enable 5th harmonic tracking and
 *   rejection in the single-phase variant. This feature uses an additional SOGI tuned to
 *   5x fundamental frequency to track and remove 5th harmonic distortion. Requires
 *   configuration of Kosg5 parameter.
 *
 * <b>Three-phase double-decoupling variant features (AC_RMS_PLL_USE_VARIANT_3PHDD only):</b>
 *
 * - <b>AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW</b><br>
 *   Use hardware 3P3Z filter for PLL controller. Define this macro to replace the PI
 *   controller with a hardware-accelerated 3P3Z filter in the three-phase double-decoupling
 *   variant. This provides more flexibility in controller design and can improve performance
 *   on devices with hardware filter support. Requires configuration of ctrl3p3z_ctx and
 *   scale_3p3z parameters.
 */

/** \} group_ac_rms_pll_macros_and_enums */

/**
 * \defgroup group_ac_rms_pll_typedefs_and_structures Typedefs and structures
 * \ingroup group_ac_rms_pll
 * \defgroup group_ac_rms_pll_functions Functions
 * \ingroup group_ac_rms_pll
 */


/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */

#ifdef AC_RMS_PLL_USE_VARIANT_3PH
#define AcRmsPllReset AcRmsPllReset_3ph
#define AcRmsPllInit AcRmsPllInit_3ph
#ifdef AC_RMS_PLL_USE_INLINE
#define AcRmsPll AcRmsPll_3ph_inline
#else
#define AcRmsPll AcRmsPll_3ph_noinline
#endif
#endif /* ifdef AC_RMS_PLL_USE_VARIANT_3PH */

#ifdef AC_RMS_PLL_USE_VARIANT_3PHDD
#define AcRmsPllReset AcRmsPllReset_3phdd
#define AcRmsPllInit AcRmsPllInit_3phdd
#ifdef AC_RMS_PLL_USE_INLINE
#define AcRmsPll AcRmsPll_3phdd_inline
#else
#define AcRmsPll AcRmsPll_3phdd_noinline
#endif
#endif /* ifdef AC_RMS_PLL_USE_VARIANT_3PHDD */

#ifdef AC_RMS_PLL_USE_VARIANT_1PH
#define AcRmsPllReset AcRmsPllReset_1ph
#define AcRmsPllInit AcRmsPllInit_1ph
#ifdef AC_RMS_PLL_USE_INLINE
#define AcRmsPll AcRmsPll_1ph_inline
#else
#define AcRmsPll AcRmsPll_1ph_noinline
#endif
#endif /* ifdef AC_RMS_PLL_USE_VARIANT_1PH */


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
 * \addtogroup group_ac_rms_pll_functions
 * \{
 * \brief API functions for AC RMS PLL block
 * \details
 * Each function of the API accepts a pointer to an AC RMS PLL context instance as first parameter.
 * The context is self-contained. No additional local or global static data is
 * needed for PLL operation. This makes the implementation reentrant.
 *
 * Before using the PLL, it must be initialized (@ref AcRmsPllInit) and reset (@ref AcRmsPllReset).
 * Further, the ac_rms_pll_context_t::config part of the context must be correctly set by the application.
 *
 * The @ref AcRmsPll is called on each input data sample.
 * It processes alpha and beta inputs and returns the RMS value by reference.
 * Phase angle and frequency information are available in the context variables.
 */

/**
 * \brief AC RMS PLL reset function
 * \details This function resets the internal module context, clearing all state variables and filters.
 * This function should be called before starting PLL operation or when restarting
 * after a disturbance.
 *
 * @param[in] context   : Pointer to the \ref ac_rms_pll_context_t structure
 * \pre \p context must point to a valid \ref ac_rms_pll_context_t instance allocated by the application.
 * \post Internal state cleared; PLL lock lost, context ready for AcRmsPll() iteration.
 * \note Does not alter user configuration (ac_rms_pll_context_t::config).
 */
void AcRmsPllReset(ac_rms_pll_context_t* const context);

/**
 * \brief AC RMS PLL initialization function
 * \details Initializes the internal module context with configuration parameters.
 * This function must be called after reset and before the first process call.
 * It sets up internal variables based on the configuration structure.
 *
 * @param[in] context   : Pointer to the \ref ac_rms_pll_context_t structure
 * \pre \p context must point to a valid \ref ac_rms_pll_context_t instance allocated by the application.
 * \pre ac_rms_pll_context_t::config must be fully populated by the application.
 * \post Context ready for first AcRmsPll() iteration.
 */
void AcRmsPllInit(ac_rms_pll_context_t* const context);

/**
 * \brief AC RMS PLL main processing function
 * \details This function performs the main PLL processing for each input sample.
 * It tracks the phase angle of the input AC signal and calculates
 * the real-time RMS value along with frequency information. Phase tracking
 * is performed using the Synchronous Reference Frame Phase-Locked Loop (SRF-PLL)
 * technique with Park transformation for phase detection.
 * For three-phase systems, both alpha and beta inputs are used directly.
 * For single-phase systems, only alpha is used and beta is generated internally
 * using the Second Order Generalized Integrator (SOGI).
 * The tracked phase angle is available in ac_rms_pll_static_t::theta, frequency in
 * ac_rms_pll_static_t::omega_raw (or ac_rms_pll_static_t::omega_filtered if filtering is enabled),
 * and RMS value is returned by reference. This function should be called at a
 * regular interval corresponding to the sampling time ac_rms_pll_config_t::dt configured in the context.
 *
 * @param[in] context   : Pointer to the \ref ac_rms_pll_context_t structure
 * @param[in] alpha     : AC signal for tracking (primary input)
 * @param[in] beta      : Quadrature of alpha (for three-phase systems, unused for single-phase)
 * @param[out] rms      : Pointer to the root square values of Park components
 * \pre \p context must point to a valid \ref ac_rms_pll_context_t instance allocated by the application.
 * \pre \p context initialized via \ref AcRmsPllInit.
 * \pre \p rms != NULL
 * \post ac_rms_pll_static_t::theta contains the tracked phase angle.
 * \post ac_rms_pll_static_t::omega_raw contains the unfiltered angular frequency.
 * \post \p rms contains the root square of the Park components.
 * \warning Input samples must be time-coherent and called at a regular interval matching ac_rms_pll_config_t::dt.
 * \note For single-phase variant the beta parameter is ignored; the quadrature signal is generated internally by the
 * SOGI.
 * \note First call after init uses zeroed history; PLL output may require several cycles to converge.
 * \note \p rms output need to be scaled by the user depending on Valpha/Vbeta convention.
 *
 * The \p rms output is computed from the synchronous-reference-frame (SRF) components as:
 *   \f$ rms = \sqrt{V_d^2 + V_q^2} \f$
 * where (\f$V_d\f$, \f$V_q\f$) are obtained by Park-transforming the input vector (\f$V_\alpha\f$, \f$V_\beta\f$) using
 * the PLL angle.
 *
 * IMPORTANT:
 * - Despite the name, \p rms is not inherently a true RMS value.
 * - \p rms is the magnitude of the input alpha/beta vector expressed in the rotating dq frame.
 * - The numerical meaning of \p rms (peak vs RMS, phase vs line-line) depends entirely on how
 *   (\f$V_\alpha\f$, \f$V_\beta\f$) are generated (Clarke scaling or orthogonal-signal generator scaling) and on the
 *   measurement type (phase L-N vs line-line L-L).
 * - Users must apply any desired scaling to obtain phase-RMS or line-line-RMS according to their
 *   selected conventions.
 *
 * Typical conversions for balanced sinusoidal fundamental (PLL locked, positive sequence):
 *
 * 1) 3-phase, power-invariant Clarke, inputs are phase voltages (L-N):
 *
 *    \f$ V_{phase,rms} = rms / \sqrt{2} \f$
 *
 *    \f$ V_{LL,rms}    = \sqrt{3/2} \cdot rms \f$
 *
 * 2) Single-phase with orthogonal signal generation (using SOGI):
 *    \p rms corresponds to the fundamental peak of the input voltage.
 *    \f$ V_{rms} = rms / \sqrt{2} \f$. For single phase variant, a scaling factor can be applied in the configuration
 * (ac_rms_pll_config_t::Krms)
 *    to directly obtain the RMS value from \p rms.
 *
 * Notes:
 * - If (\f$V_\alpha\f$, \f$V_\beta\f$) are produced with a different Clarke scaling (e.g. amplitude-invariant) or are
 *   derived from line-line measurements, the mapping to RMS changes accordingly.
 * - With harmonics/unbalance, \p rms should be interpreted as a PLL-tracked fundamental magnitude
 *   estimate, not a time-domain RMS over all frequency components.
 */

#ifdef AC_RMS_PLL_USE_INLINE
PWRLIB_INLINE void AcRmsPll(ac_rms_pll_context_t* const context, float32_t alpha, float32_t beta, float32_t* rms);
#else
void AcRmsPll(ac_rms_pll_context_t* const context, float32_t alpha, float32_t beta, float32_t* rms);
#endif

/** \} group_ac_rms_pll_functions */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif // ifndef AC_RMS_PLL_H__

/* --- End of File ------------------------------------------------ */
