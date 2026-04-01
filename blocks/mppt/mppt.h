/**
 * @file    mppt.h
 * @brief   Main header for MPPT (Maximum Power Point Tracking) block
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

#ifndef MPPT_H__
#define MPPT_H__

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

#ifdef MPPT_USE_VARIANT_PNO
#include "mppt_pno.h"
#endif

#include "mppt_types.h"

/**
 * \defgroup group_mppt MPPT (Maximum Power Point Tracking)
 *
 * \section sct_mppt_general Overview
 * <br>
 * This block implements Maximum Power Point Tracking (MPPT) algorithms for photovoltaic
 * systems and other variable power sources. The MPPT controller continuously adjusts
 * the operating voltage to extract maximum power from the source under varying
 * environmental conditions.
 *
 * The MPPT block provides robust tracking performance for solar power applications,
 * wind energy systems, and other renewable energy sources where maximum power
 * extraction is critical for system efficiency.
 *
 * \section sct_mppt_configurable_features Configurable features
 * <br>
 * The MPPT block provides the following configurable features:
 *
 * Configuration Parameter | Name | Description
 * ------------- | ------------- | -------------
 * Perturbation step size | step_size | Voltage step size for perturbation in Volts
 * Minimum voltage limit | v_min | Lower operating voltage limit in Volts
 * Maximum voltage limit | v_max | Upper operating voltage limit in Volts
 * Minimum power threshold | p_min_threshold | Minimum power for MPPT operation in Watts
 * Sampling time | dt | Algorithm execution period in seconds
 * Settling time | settle_time | Time delay after perturbation in seconds
 *
 * \section sct_mppt_variants Variants and configuration options
 * This section describes the available variants and configuration options.
 * All variants share the same API so that changing the block interface does not require
 * further changes to the application code when switching between the variants.
 * <br>
 * Macro name | Macro purpose
 * ------------- | -------------
 * MPPT_USE_VARIANT_PNO | Use Perturb and Observe variant of MPPT (default)
 * MPPT_USE_INLINE | Use function inlining for MPPT
 *
 * \subsection ssct_mppt_pno_variant Perturb and Observe variant
 * <br>
 * This is the default variant that implements the classic Perturb and Observe
 * algorithm. The algorithm works by periodically perturbing the operating voltage
 * and observing the resulting power change. If power increases, the perturbation
 * continues in the same direction; if power decreases, the perturbation direction
 * is reversed.
 * The design of this variant is shown in the flowchart below.
 * \image html ./images/mppt_pno_flowchart.svg width=50%
 * The Perturb and Observe variant is selected by defining MPPT_USE_VARIANT_PNO
 * in the user config file. This is the default variant.
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_pno
 *
 * **Performance optimization:**
 * <br>
 * To allow for the optimization of execution time, the API functions can be
 * inlined by defining MPPT_USE_INLINE in the user config file.
 * The default is no function inlining.
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_pno_inlined
 *
 * **Using the API**
 * <br>
 * \snippet{doc} this snippet_mppt_using_the_API
 *
 * Example usage of the Perturb and Observe variant:
 * \snippet code_snippets snippet_using_mppt_pno
 */
/*
   [snippet_mppt_using_the_API]
   The application must allocate the context structure (mppt_context_t) in a memory section of choice.
   It must set the values of the configuration sub-structure (mppt_context_t::config).
   The static sub-structure (mppt_context_t::vars) contains the static memory the block needs to operate.
   It must not be initialized by the application.
   The application has to call the block reset function (MpptReset()) to clear the static sub-structure and
   the block init function (MpptInit()) to initialize the internal configuration.
   [snippet_mppt_using_the_API]
 */

/**
 * \defgroup group_mppt_macros_and_enums Macros and enumerated types
 * \ingroup group_mppt
 * \{
 * \brief Configuration macros for MPPT block
 *
 * These macros should be defined in mtb_mw_pctrl_user_config.h to configure the
 * MPPT algorithm variant and features.
 *
 * <b>Variant selection macros:</b>
 *
 * - <b>MPPT_USE_VARIANT_PNO</b> (default)<br>
 *   Use perturb and observe (P&O) variant of MPPT. This is the classic hill-climbing
 *   algorithm that periodically perturbs the operating point and observes the resulting
 *   power change. If power increases, perturbation continues in the same direction; if
 *   power decreases, the perturbation direction is reversed. This variant is suitable
 *   for photovoltaic systems, wind energy converters, and other renewable energy applications
 *   requiring maximum power extraction.
 *
 * <b>Performance optimization:</b>
 *
 * - <b>MPPT_USE_INLINE</b><br>
 *   Enable function inlining for MPPT. Define this macro to inline the main MPPT algorithm
 *   execution function for improved performance. This trades code size for faster execution time,
 *   which can be beneficial in high-frequency control loops.
 */

/** \} group_mppt_macros_and_enums */

/**
 * \defgroup group_mppt_typedefs_and_structures Typedefs and structures
 * \ingroup group_mppt
 * \defgroup group_mppt_functions Functions
 * \ingroup group_mppt
 */

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */

#ifdef MPPT_USE_VARIANT_PNO
#define MpptReset MpptReset_pno
#define MpptInit MpptInit_pno
#ifdef MPPT_USE_INLINE
#define Mppt Mppt_pno_inline
#else
#define Mppt Mppt_pno_noinline
#endif
#endif /* ifdef MPPT_USE_VARIANT_PNO */

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
 * \brief API functions for MPPT block
 * \details
 * Each function of the API accepts a pointer to an MPPT context instance as first parameter.
 * The context is self-contained. No additional local or global static data is
 * needed for MPPT operation. This makes the implementation reentrant.
 *
 * Before using the MPPT, it must be reset (@ref MpptReset) and initialized (@ref MpptInit).
 * Further, the mppt_context_t::config part of the context must be correctly set by the application.
 *
 * The @ref Mppt is called on each tracking step.
 * It processes voltage and current inputs and returns the normalized reference by reference.
 */

/**
 * \brief Reset MPPT module state.
 *
 * \details
 * Clears (zeros) all internal runtime variables stored in mppt_context_t::vars:
 *  - Previous voltage sample (v_prev)
 *  - Previous power sample (p_prev)
 *  - Current power and intermediate deltas (p_current, dp, dv, dp_dv)
 *  - Reference output (d_ref) is set to a defined initial value
 *
 * Call sequence requirement:
 *  1. Application fills context->config
 *  2. MpptReset()
 *  3. MpptInit()
 *  4. Repeated Mppt() calls
 *
 * Safe to call again to discard accumulated tracking history.
 *
 * \pre context != NULL
 * \post Internal state cleared; NOT initialized for tracking until MpptInit() runs.
 * \param[in] context MPPT context
 * \note Does not alter user configuration (mppt_context_t::config).
 * \note Safe to call multiple times to discard accumulated tracking history.
 */
void MpptReset(mppt_context_t* const context);

/**
 * \brief Initialize internal working variables from configuration.
 *
 * \details
 * Consumes user-provided limits and step settings (e.g., config.d_min,
 * config.d_max, config.delta_d) and prepares the internal state for the
 * first tracking iteration:
 *  - Validates and (if necessary) normalizes limit ordering (implementation-specific)
 *  - Clamps the reference into [d_min, d_max]
 *  - Ensures step size (delta_d) is non-negative and within a sane bound
 *  - Leaves historical samples (p_prev, v_prev) at their reset values (0) so the
 *    first Mppt() call establishes a baseline
 *.
 *
 * \pre context != NULL
 * \pre MpptReset() already executed on this context
 * \post Context ready for first Mppt() iteration
 * \param[in] context MPPT context
 * \warning Undefined behavior if called without a prior MpptReset().
 */
void MpptInit(mppt_context_t* const context);

/**
 * \brief Execute one Perturb-and-Observe (P&O) tracking step.
 *
 * \details
 * Implements a Perturb and Observe algorithm that outputs a normalized reference
 * value in the range [0.0, 1.0]. The algorithm is topology-agnostic: the output
 * can be used as a duty cycle reference, a normalized voltage reference, or any
 * other control variable depending on the power converter topology (e.g., buck,
 * boost, buck-boost, full-bridge).
 *
 * Algorithm steps:
 *  1. Compute instantaneous power: p_current = voltage * current
 *  2. Compute deltas: dp = p_current - p_prev, dv = voltage - v_prev
 *  3. Form product: dp_dv = dp * dv
 *  4. If dp_dv >= 0: decrement reference (d_ref -= delta_d); else increment
 *  5. Clamp reference into [d_min, d_max]
 *  6. Update historical samples (p_prev, v_prev)
 *  7. Output updated normalized reference
 *
 * The application is responsible for scaling the normalized output to the
 * appropriate physical range for the target converter. For example:
 *  - Direct use as PWM duty cycle
 *  - Multiply by V_bus to obtain a voltage setpoint
 *  - Map to a current reference for a current-controlled topology
 *
 * Characteristics/limitations of current variant:
 *  - No adaptive step modulation
 *  - No explicit settling/debounce timing
 *  - No minimum power threshold gating
 *
 * \pre Context initialized via MpptReset() then MpptInit()
 * \pre d_ref != NULL
 * \post *d_ref contains new normalized reference within [config.d_min, config.d_max]
 *
 * \param[in]  context MPPT context
 * \param[in]  voltage Latest input voltage (Volts)
 * \param[in]  current Latest input current (Amperes)
 * \param[out] d_ref   Pointer to store normalized reference output (0.0 .. 1.0).
 *                     Interpretation depends on converter topology (duty cycle,
 *                     normalized voltage, etc.)
 *
 * \warning Voltage/current samples must be time-coherent.
 * \note First call (after init) uses zeroed history; behavior of initial step is
 * deterministic but may not represent a true perturb decision until second call.
 */
#ifdef MPPT_USE_INLINE
PWRLIB_INLINE void Mppt(mppt_context_t* const context, float32_t voltage, float32_t current, float32_t* d_ref);
#else
void Mppt(mppt_context_t* const context, float32_t voltage, float32_t current, float32_t* d_ref);
#endif

/** \} group_mppt_functions */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif // ifndef MPPT_H__

/* --- End of File ------------------------------------------------ */
