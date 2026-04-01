/**
 * @file filter_3p3z.h
 * @brief API of a 3-pole 3-zero filter.
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

#ifndef FILTER_3P3Z_H__
#define FILTER_3P3Z_H__

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

#ifdef FILTER_3P3Z_USE_VARIANT_HW
#include "filter_3p3z_hw.h"
#endif

#ifdef FILTER_3P3Z_USE_VARIANT_DF1_Q23
#include "filter_3p3z_df1_q23.h"
#endif

#ifdef FILTER_3P3Z_USE_VARIANT_DF2_F32
#include "filter_3p3z_df2_f32.h"
#endif

#include "filter_3p3z_types.h"

/**
 * \defgroup group_filter_3p3z Filter 3p3z
 *
 * \section sct_filter_3p3z_general Overview
 * <br>
 * This block implements a 3-pole 3-zero (3p3z) digital filter, commonly used as a
 * compensator in power converter control loops. The 3p3z structure provides three poles
 * and three zeros, enabling flexible shaping of the loop transfer function to achieve
 * desired stability margins and transient response characteristics.
 *
 * The block supports two filter architectures:
 * - **Direct Form 1 (DF1):** Used by the DF1-Q23 software variant and the hardware-accelerated
 *   (HW) variant. DF1 maintains separate delay lines for the input (\f$x\f$) and output
 *   (\f$y\f$) histories, and includes the full fixed-point signal processing chain (input gain,
 *   coefficient scaling, output gain, output offset, and output limiting) to match the
 *   HWFILTER3P3Z peripheral on PSoC Control MCUs.
 * - **Direct Form 2 (DF2):** Used by the DF2-F32 software variant. DF2 uses a single shared
 *   delay line, reducing memory usage. This variant operates in single-precision floating-point
 *   arithmetic, simplifying coefficient design and adding support for back-calculation
 *   anti-windup on the output limiter.
 *
 * The general 3p3z transfer function implemented by all variants is:
 *
 * \f$ H(z) = \frac{cx_0 + cx_1 z^{-1} + cx_2 z^{-2} + cx_3 z^{-3}}
 *                 {1    - cy_1 z^{-1} - cy_2 z^{-2} - cy_3 z^{-3}} \f$
 *
 * The Direct Form 1 and the hardware-accelerated variants share the same top-level API
 * (@ref Filter3p3zReset, @ref Filter3p3zInit, * @ref Filter3p3z), allowing seamless switching
 * between software and hardware implementations without application code changes. The Direct
 * Form 2 variant deviates from this API for the data types (single-precision floating-point).
 * \section sct_filter_3p3z_configurable_features Configurable features
 * <br>
 * The filter provides the following configurable features. Note that the available
 * parameters depend on the selected variant.
 *
 * <table>
 * <tr><th>Parameter</th>
 *     <th>Name</th><th>DF1-Q23 / HW</th><th>DF2-F32</th><th>Description</th></tr>
 * <tr><td>Filter input</td><td>dIn0/dIn1</td>
 *     <td>Q0.15</td><td>float32</td>
 *     <td>The input of the filter is the difference of
 *     \f$dIn0-dIn1\f$.</td></tr>
 * <tr><td>Filter output</td><td>dOut</td>
 *     <td>Q0.23</td><td>float32</td>
 *     <td>The output of the filter, returned by reference.</td></tr>
 * <tr><td>Filter coefficients</td><td>cx/cy</td>
 *     <td>Q0.23</td><td>float32</td>
 *     <td>Arrays \f$[cx_0, cx_1, cx_2, cx_3]\f$ and
 *     \f$[cy_1, cy_2, cy_3]\f$.</td></tr>
 * <tr><td>Input gain</td><td>gIn</td>
 *     <td>Yes (0..3)</td><td>N/A</td>
 *     <td>Left shift by 0..3 applied to the input difference.
 *     DF1-Q23 and HW variants only.</td></tr>
 * <tr><td>Coefficient scaling</td><td>scaleCx/scaleCy</td>
 *     <td>Yes (0..7)</td><td>N/A</td>
 *     <td>Left shift by 0..7 applied to accumulated cx/cy branches.
 *     DF1-Q23 and HW variants only.</td></tr>
 * <tr><td>Output gain</td><td>gOut</td>
 *     <td>Yes (0..7)</td><td>N/A</td>
 *     <td>Right shift by 0..7 applied to the filter output.
 *     DF1-Q23 and HW variants only.</td></tr>
 * <tr><td>Output offset</td><td>dOutOffset</td>
 *     <td>Yes (Q0.23)</td><td>N/A</td>
 *     <td>Constant value added to the output after gain.
 *     DF1-Q23 and HW variants only.</td></tr>
 * <tr><td>Output limiter</td><td>limMin/limMax</td>
 *     <td>Yes (Q0.23)</td><td>Yes (float32, as min/max)</td>
 *     <td>Output clamping limits.</td></tr>
 * <tr><td>Anti-windup</td><td>anti-windup_gain</td>
 *     <td>N/A</td><td>Yes (float32)</td>
 *     <td>Back-calculation anti-windup gain applied to the DF2
 *     delay element when output is clamped.
 *     DF2-F32 variant only.</td></tr>
 * </table>
 * \section sct_filter_3p3z_hardware_variant Using the API
 * \snippet{doc} this snippet_filter_3p3z_using_the_API
 * \section sct_filter_3p3z_variants Variants and configuration options
 * This section describes the available variants and configuration options.
 * The Direct Form 1 and the hardware-accelerated variants share the same API so that
 * changing the block interface does not require further changes to the application
 * code when switching between the variants. The Direct Form 2 variant deviates from
 * this API for the data types (single-precision floating-point).
 * <br>
 * <table>
 * <tr><th>Macro name</th><th>Macro purpose</th></tr>
 * <tr><td>FILTER_3P3Z_USE_VARIANT_DF1_Q23</td>
 *     <td>Use the DF1-Q23 software variant of the filter_3p3z (default)</td></tr>
 * <tr><td>FILTER_3P3Z_USE_VARIANT_DF2_F32</td>
 *     <td>Use the DF2-F32 software variant of the filter_3p3z</td></tr>
 * <tr><td>FILTER_3P3Z_USE_VARIANT_HW</td>
 *     <td>Use the hardware-accelerated variant of the filter_3p3z
 *     (using HWFILTER peripheral from HW_PPCA)</td></tr>
 * <tr><td>FILTER_3P3Z_USE_VARIANT_DF1_Q23_HW_PPCA_BEHAVIOR</td>
 *     <td>Emulate bit-true HW filter of HW_PPCA</td></tr>
 * <tr><td>FILTER_3P3Z_USE_INLINE</td>
 *     <td>Use function inlining for the filter_3p3z</td></tr>
 * </table>
 * \subsection ssct_filter_3p3z_software_variant Software variants
 * The library provides two software variants that implement the 3p3z filter without
 * hardware acceleration, suitable for simulation, portability, or when hardware resources
 * are unavailable.
 *
 * **DF1-Q23 variant:**
 * This variant implements the 3p3z filter in Direct Form 1 using fixed-point Q23 arithmetic.
 * The filter equation is
 *
 * \f$dOut[n] = \Big(cx_3 * x[n-3] + cx_2 * x[n-2] + cx_1 * x[n-1] + cx_0 * x[n]\Big) +
 * \Big(cy_3 * y[n-3] + cy_2 * y[n-2] + cy_1 * y[n-1]\Big)\f$
 *
 * The filter works in the normalized range of +-1. The filter architecture is shown in the block diagram below:
 *
 * \image html ./images/filter_3p3z.svg width=80%
 * It mimics the implementation of the PPCA HW 3P3Z FILTER IP block, retaining the same
 * configuration parameters and data types. This enables three use cases:
 *
 * - Switch the application to a target without HWFILTER
 * - Switch to a software implementation if the HW resource is used up
 * - Use the software variant for system simulation
 *
 * Because this variant mirrors the hardware filter, it includes the full hardware signal
 * processing chain: input gain (gIn), coefficient scaling (scaleCx/scaleCy), rounding and
 * saturation, output gain (gOut), output offset (dOutOffset), and output limiting
 * (limMin/limMax). These stages are specific to the fixed-point arithmetic used by the
 * HWFILTER peripheral and are not applicable to the DF2-F32 variant.
 * Anti-windup is not supported by this variant.
 *
 * The DF1-Q23 software variant is selected by defining FILTER_3P3Z_USE_VARIANT_DF1_Q23
 * in the user config file. This is the default variant. Anti-windup feature is not supported
 * by this variant.
 *
 * **DF2-F32 variant:**
 * This variant implements the 3p3z filter in Direct Form 2 using single-precision
 * floating-point arithmetic. It is a simplified variant compared to DF1-Q23, as
 * floating-point arithmetic eliminates the need for the fixed-point scaling stages
 * (input gain, coefficient scaling, output gain, output offset) that are required
 * by the DF1-Q23 and HW variants.
 *
 * The DF2-F32 variant uses only the filter coefficients (cx, cy), output clamping limits
 * (min, max), and an anti-windup gain. The anti-windup uses back-calculation: when the output
 * is clamped, the excess is fed back into the DF2 delay element, scaled by the
 * antiwindup_gain parameter, to prevent integrator windup.
 *
 * This variant is suitable for microcontrollers with floating-point units when hardware
 * acceleration is not available. It provides easier coefficient calculation since the
 * coefficients are specified directly as float32 values without the need for fixed-point
 * quantization.
 *
 * The DF2-F32 software variant is selected by defining FILTER_3P3Z_USE_VARIANT_DF2_F32
 * in the user config file.
 *
 * **Performance optimization:**
 * To allow for an optimization of the execution time, the API functions can be
 * inlined by defining FILTER_3P3Z_USE_INLINE in the user config file.
 * The default is no function inlining.
 *
 * The functional behavior of the DF1 Q23 variant is bit-accurate compared to
 * the 3p3z hardware filter of PPCA HW FILTER as long as the internal data
 * of the filter remains within the bit width of the hardware implementation.
 * In case of an overflow, the hardware filter would show a change of the sign bit
 * while the software variant would eventually be able to represent the number
 * correctly because of its int32 operations. Additional masking of internal data
 * would fully match the hardware behavior but is not used because it would
 * affect the execution time.
 * Optionally, full bit-accuracy can be enabled (e.g., for simulation purposes) by
 * defining FILTER_3P3Z_USE_VARIANT_DF1_Q23_HW_PPCA_BEHAVIOR in the user config file.
 *
 * **Code snippet for using the filter_3p3z software variant**
 * <br>
 * The user config file should contain the
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_df1_q23_inlined
 * The filter configuration and execution is done by
 * <br>
 * \snippet code_snippets snippet_using_filter_3p3z_df1_q23
 * \subsection ssct_filter_3p3z_hardware_variant Hardware variant
 *
 * If the compile target is PPCA HW, the 3p3z hardware filter can be used.
 *
 * The hardware variant is selected by defining FILTER_3P3Z_USE_VARIANT_HW in the user config file.
 * This variant also does not support the anti-windup feature.
 * To allow for an optimization of the execution time, the API functions can be
 * inlined by defining FILTER_3P3Z_USE_INLINE in the user config file.
 * The default is no function inlining.
 *
 * **Code snippet for using the filter_3p3z hardware variant**
 * <br>
 * The user config file should contain the
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_hw
 * Open the MTB Device Configurator and assign a HWFILT3P3Z block
 * to the correct PPCA CPU in the PPCA configuration block (CNFG). Initialize the
 * CNFG block before using the hardware filter:
 * \snippet code_snippets snippet_initialize_hw_cnfg_block
 * Enable the hardware filter instance
 * \snippet code_snippets snippet_enable_hwfilt3p3z_instance
 * The filter configuration and execution is done by
 * <br>
 * \snippet code_snippets snippet_using_filter_3p3z_hw
 */
/*
   [snippet_filter_3p3z_using_the_API]
   The application must allocate the context structure (filter_3p3z_context_t) in a memory section of choice.
   It must set the values of the configuration sub-structure (filter_3p3z_context_t::config).
   The static sub-structure (filter_3p3z_context_t::vars) contains the static memory the block needs to operate.
   It must not be initialized by the application.
   The application has to call the block reset function (@ref Filter3p3zReset) to clear the static sub-structure and
   the block init function (@ref Filter3p3zInit) to initialize the internal configuration.
   [snippet_filter_3p3z_using_the_API]
 */

/**
 * \defgroup group_filter_3p3z_macros_and_enums Macros and enumerated types
 * \ingroup group_filter_3p3z
 * \{
 * \brief Configuration macros for Filter 3p3z block
 *
 * These macros should be defined in mtb_mw_pctrl_user_config.h to configure the
 * Filter 3p3z variant and features. Only one variant selection macro should be
 * defined at a time.
 *
 * <b>Variant selection macros (define ONE):</b>
 *
 * - <b>FILTER_3P3Z_USE_VARIANT_DF1_Q23</b> (default)<br>
 *   Use software variant in Direct Form 1 with Q23 fixed-point arithmetic. This variant
 *   provides bit-accurate emulation of the HWFILTER3P3Z peripheral, enabling portability
 *   and system simulation when hardware resources are unavailable or exhausted.
 *
 * - <b>FILTER_3P3Z_USE_VARIANT_HW</b><br>
 *   Use hardware-accelerated variant leveraging the HWFILTER3P3Z peripheral on PSoC Control
 *   MCUs. This variant offers optimal performance by offloading filter computations to
 *   dedicated hardware, reducing CPU load and execution time.
 *
 * - <b>FILTER_3P3Z_USE_VARIANT_DF2_F32</b><br>
 *   Use software variant in Direct Form 2 with single-precision floating-point arithmetic.
 *   This variant simplifies coefficient calculation and may provide better numerical stability.
 *   Recommended for microcontrollers with floating-point units when hardware acceleration
 *   is not available.
 *
 * <b>Performance optimization:</b>
 *
 * - <b>FILTER_3P3Z_USE_INLINE</b><br>
 *   Enable function inlining for Filter 3p3z. Define this macro to inline the main filter
 *   execution function for improved performance. This trades code size for faster execution time.
 *
 * <b>DF1-Q23 Variant features (FILTER_3P3Z_USE_VARIANT_DF1_Q23 only):</b>
 *
 * - <b>FILTER_3P3Z_USE_VARIANT_DF1_Q23_HW_PPCA_BEHAVIOR</b><br>
 *   Enable bit-accurate hardware emulation. Define this macro to exactly mimic the behavior
 *   of the PSC3_P8 HWFILTER3P3Z peripheral by masking internal values to match hardware bit widths.
 *   This ensures identical behavior for simulation and verification purposes, but may reduce
 *   execution speed. Use this for bit-exact validation against hardware filter results.
 */

/** \} group_filter_3p3z_macros_and_enums */

/**
 * \defgroup group_filter_3p3z_typedefs_and_structures Typedefs and structures
 * \ingroup group_filter_3p3z
 * \defgroup group_filter_3p3z_functions Functions
 * \ingroup group_filter_3p3z
 */

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */

#ifdef FILTER_3P3Z_USE_VARIANT_HW
typedef filter_3p3z_config_df1_q23_t filter_3p3z_config_t;
typedef filter_3p3z_static_df1_q23_t filter_3p3z_static_t;
typedef filter_3p3z_context_df1_q23_t filter_3p3z_context_t;
#define Filter3p3zReset Filter3p3zReset_HW
#define Filter3p3zInit Filter3p3zInit_HW
#ifdef FILTER_3P3Z_USE_INLINE
#define Filter3p3z Filter3p3z_HW_inline
#else
#define Filter3p3z Filter3p3z_HW_noinline
#endif
#endif // ifdef FILTER_3P3Z_USE_VARIANT_HW

#ifdef FILTER_3P3Z_USE_VARIANT_DF1_Q23
typedef filter_3p3z_config_df1_q23_t filter_3p3z_config_t;
typedef filter_3p3z_static_df1_q23_t filter_3p3z_static_t;
typedef filter_3p3z_context_df1_q23_t filter_3p3z_context_t;
#define Filter3p3zReset Filter3p3zReset_DF1_Q23
#define Filter3p3zInit Filter3p3zInit_DF1_Q23
#ifdef FILTER_3P3Z_USE_INLINE
#define Filter3p3z Filter3p3z_DF1_Q23_inline
#else
#define Filter3p3z Filter3p3z_DF1_Q23_noinline
#endif
#endif // ifdef FILTER_3P3Z_USE_VARIANT_DF1_Q23

#ifdef FILTER_3P3Z_USE_VARIANT_DF2_F32
typedef filter_3p3z_config_df2_f32_t filter_3p3z_config_t;
typedef filter_3p3z_static_df2_f32_t filter_3p3z_static_t;
typedef filter_3p3z_context_df2_f32_t filter_3p3z_context_t;
#define Filter3p3zReset Filter3p3zReset_DF2_F32
#define Filter3p3zInit Filter3p3zInit_DF2_F32
#ifdef FILTER_3P3Z_USE_INLINE
#define Filter3p3z Filter3p3z_DF2_F32_inline
#else
#define Filter3p3z Filter3p3z_DF2_F32_noinline
#endif
#endif // ifdef FILTER_3P3Z_USE_VARIANT_DF2_F32

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
 * \addtogroup group_filter_3p3z_functions
 * \{
 * \brief API functions for Filter 3p3z block
 * \details
 * Each function of the API accepts a pointer to a filter context instance as first parameter.
 * The filter context index is self-contained. No additional local or global static data is
 * needed for filter operation. This makes the implementation reentrant.
 *
 * Before using the filter, it must be initialized (@ref Filter3p3zInit) and reset (@ref Filter3p3zReset).
 * Further, the filter_3p3z_context_t::config part of the context must be correctly set by the application.
 *
 * The @ref Filter3p3z is called on each input data sample.
 * It has two inputs in (Q0.15) for setpoint and signal and it returns a pointer to the result value (Q0.23).
 */

/**
 * 3p3z filter reset
 * Resets the internal module context
 *
 * @param[in] context   : Pointer to the context structure
 */
void Filter3p3zReset(filter_3p3z_context_t* const context);

/**
 * 3p3z filter init
 * Initializes the 3p3z filter
 *
 * @param[in] context   : Pointer to the context structure
 */
void Filter3p3zInit(filter_3p3z_context_t* const context);


/**
 * 3p3z filter
 * Executes the 3p3z filter equation:
 *
 * @param[in] context   : Pointer to the context structure
 * @param[in] dIn0_0q15 : Normalized setpoint (Q0.15)
 * @param[in] dIn1_0q15 : Normalized signal (Q0.15)
 * @param[in] dOut_0q23 : Pointer to output value (Q0.23)
 */
#ifdef FILTER_3P3Z_USE_VARIANT_DF2_F32
    #ifdef FILTER_3P3Z_USE_INLINE
PWRLIB_INLINE void Filter3p3z(
    filter_3p3z_context_t* const context,
    float32_t const dIn0, float32_t const dIn1, float32_t* dOut);
    #else
void Filter3p3z(
    filter_3p3z_context_t* const context,
    float32_t const dIn0, float32_t const dIn1, float32_t* dOut);
    #endif
#else
    #ifdef FILTER_3P3Z_USE_INLINE
PWRLIB_INLINE void Filter3p3z(
    filter_3p3z_context_t* const context,
    int16_t const dIn0_0q15, int16_t const dIn1_0q15, int32_t* dOut_0q23);
    #else
void Filter3p3z(
    filter_3p3z_context_t* const context,
    int16_t const dIn0_0q15, int16_t const dIn1_0q15, int32_t* dOut_0q23);
    #endif
#endif // ifdef FILTER_3P3Z_USE_VARIANT_DF2_F32

/** \} group_filter_3p3z_functions */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif // ifndef FILTER_3P3Z_H__

/* --- End of File ------------------------------------------------ */
