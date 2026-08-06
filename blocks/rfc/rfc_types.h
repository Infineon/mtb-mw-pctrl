/**
 * @file rfc_types.h
 * @brief Definition of rfc types and structures.
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

#ifndef RFC_TYPES_H__
#define RFC_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================================================================
 *   1. Include files
 * ==============================================================================
 */

#include "mtb_mw_pctrl_compiler.h"
#include "mtb_mw_pctrl_config.h"
#include "filter_3p3z.h"

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */

/* ==============================================================================
 *   3. Types
 * ==============================================================================
 */

/**
 * \addtogroup group_rfc_typedefs_and_structures
 * \{
 * \brief Context structure and sub-structures for RFC inner and outer blocks
 * \details
 * These structures contain the configuration, application-provided runtime inputs,
 * RFC-maintained state, and outputs used by the inner and outer control loops.
 *
 * The top-level context structures follow a common pattern:
 * - config: user-provided parameters and controller contexts
 * - vars: application-provided runtime inputs plus RFC-maintained state and outputs
 *
 * The application should populate config fields, then call reset/init APIs
 * before entering the periodic control loop.
 *
 * The application must not initialize or overwrite the complete vars structures.
 * RfcInnerReset() and RfcOuterReset() initialize all RFC-maintained state. During operation,
 * the application updates only these input members before the corresponding control step:
 * - Outer loop: Vdc. Vc1 and Vc2 are passed as RfcOuter() parameters.
 * - Inner loop: theta, omega, vd_in, vq_in, Ia, Ib, Ic, and ZeroSequence.
 *
 * The remaining vars members are owned by RFC. The application may read output members,
 * including ZeroSequence in the outer context and VaRef, VbRef, and VcRef in the inner context.
 */

/**
 * \brief Structure used to keep the inner module configuration
 * \details
 * Sub-structure of rfc_inner_context_t holding application-provided parameters
 * for the inner current-control loop.
 *
 * The fields required by the selected RFC features and Filter_3P3Z variant must be
 * configured by the application before calling RfcInnerInit().
 */
typedef struct
{
    float32_t lpc_voltage_alpha;            /**< Coefficient alpha for d/q-axis voltage low-pass filtering. */
    float32_t lpc_voltage_beta;             /**< Coefficient beta for d/q-axis voltage low-pass filtering. */
    float32_t lpc_current_alpha;            /**< Coefficient alpha for d/q-axis current low-pass filtering. */
    float32_t lpc_current_beta;             /**< Coefficient beta for d/q-axis current low-pass filtering. */
    filter_3p3z_context_t ctrl3p3z_ctx_d;   /**< d-axis control-loop context. */
    filter_3p3z_context_t ctrl3p3z_ctx_q;   /**< q-axis control-loop context. */
    #if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW) || defined(CY_DOXYGEN)
    float32_t scale_3p3z;                   /**< Converts inner-loop int32_t Filter_3P3Z output to float32 command
                                               units. */
    #endif
    float32_t iq_ref;                       /**< q-axis current reference in amperes for inner current controller. */
    float32_t grid_inductance;              /**< Grid filter inductance value. */
    float32_t normalization_factor;         /**< Multiplier applied to each final phase command. RFC does not clamp the
                                               result. */
    float32_t scale_vd_vq;                  /**< Grid positive-sequence Vd/Vq feedforward scale factor. */
} rfc_inner_config_t;

/**
 * \brief Structure used to keep the outer module configuration
 * \details
 * Sub-structure of rfc_outer_context_t holding application-provided parameters
 * for the outer voltage-control loop.
 *
 * The fields required by the selected RFC features and Filter_3P3Z variant must be
 * configured by the application before calling RfcOuterInit().
 */
typedef struct
{
    float32_t lpc_vc1_alpha;                /**< Coefficient alpha for vc1 capacitor-voltage low-pass filtering. */
    float32_t lpc_vc1_beta;                 /**< Coefficient beta for vc1 capacitor-voltage low-pass filtering. */
    float32_t lpc_vc2_alpha;                /**< Coefficient alpha for vc2 capacitor-voltage low-pass filtering. */
    float32_t lpc_vc2_beta;                 /**< Coefficient beta for vc2 capacitor-voltage low-pass filtering. */
    filter_3p3z_context_t ctrl3p3z_ctx;     /**< Outer-loop control context. */
    #if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW) || defined(CY_DOXYGEN)
    float32_t scale_3p3z;                   /**< Converts outer-loop int32_t Filter_3P3Z output to float32
                                               current-reference units. */
    #endif
    float32_t Kp_zerosequence;              /**< Proportional gain for zero-sequence controller. */
} rfc_outer_config_t;

/**
 * \brief Structure containing inner-loop runtime inputs, state, and outputs
 * \details
 * This structure combines application-updated input members with RFC-maintained state,
 * intermediate calculations, and outputs for the inner current control loop.
 */
typedef struct
{
    float32_t theta;                        /**< Application input: grid-voltage angle theta in radians. */
    float32_t omega;                        /**< Application input: grid-voltage angular frequency omega in radians per
                                               second. */
    float32_t Ia;                           /**< Application input: phase-a current in amperes. */
    float32_t Ib;                           /**< Application input: phase-b current in amperes. */
    float32_t Ic;                           /**< Application input: phase-c current in amperes. */
    float32_t vd_in;                        /**< Application input: d-axis component of grid positive-sequence voltage
                                             */
    float32_t vq_in;                        /**< Application input: q-axis component of grid positive-sequence voltage
                                             */
    float32_t IdF;                          /**< Filtered d-axis current in amperes. */
    float32_t IqF;                          /**< Filtered q-axis current in amperes. */
    float32_t VdF;                          /**< Filtered d-axis voltage in volts. */
    float32_t VqF;                          /**< Filtered q-axis voltage in volts. */
    float32_t VaRef;                        /**< Output: final phase-a modulation reference after normalization. */
    float32_t VbRef;                        /**< Output: final phase-b modulation reference after normalization. */
    float32_t VcRef;                        /**< Output: final phase-c modulation reference after normalization. */
    float32_t ZeroSequence;                 /**< Application input: latest outer-loop zero-sequence output
                                             */
    #ifndef RFC_USE_LOCAL_STORAGE
    float32_t sin_theta;                    /**< Sine of theta for per-step transform reuse */
    float32_t cos_theta;                    /**< Cosine of theta for per-step transform reuse */
    float32_t vd_scaled;                    /**< d-axis grid voltage after scale factor applied */
    float32_t vq_scaled;                    /**< q-axis grid voltage after scale factor applied */
    float32_t i_d;                          /**< d-axis current in amperes. */
    float32_t i_q;                          /**< q-axis current in amperes. */
    float32_t i_alpha;                      /**< alpha-axis current in amperes. */
    float32_t i_beta;                       /**< beta-axis current in amperes. */
    float32_t v_d_cmd;                      /**< d-axis command voltage after current control and decoupling */
    float32_t v_q_cmd;                      /**< q-axis command voltage after current control and decoupling */
    #if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW) || defined(CY_DOXYGEN)
    int32_t ctrl3p3z_d_out;                 /**< d-axis controller output (DF1-Q23 or HW variant) */
    int32_t ctrl3p3z_q_out;                 /**< q-axis controller output (DF1-Q23 or HW variant) */
    #else
    float32_t ctrl3p3z_d_out;               /**< d-axis controller output (DF2 float variant) */
    float32_t ctrl3p3z_q_out;               /**< q-axis controller output (DF2 float variant) */
    #endif
    float32_t v_max;                        /**< Maximum phase reference used for common-mode centering */
    float32_t v_min;                        /**< Minimum phase reference used for common-mode centering */
    float32_t common_mode_total;            /**< Total common mode voltage calculated for injection */
    #endif // ifndef RFC_USE_LOCAL_STORAGE
} rfc_inner_static_t;

/**
 * \brief Structure containing outer-loop runtime inputs, state, and outputs
 * \details
 * This structure combines the application-updated Vdc input with RFC-maintained state,
 * intermediate calculations, and outputs for the outer voltage control loop.
 */
typedef struct
{
    float32_t Vdc;                          /**< Application input: DC-link controller setpoint signal. */
    float32_t Vc1;                          /**< Top capacitor voltage in volts. */
    float32_t Vc2;                          /**< Bottom capacitor voltage in volts. */
    float32_t FilteredVc1;                  /**< Filtered top capacitor voltage in volts. */
    float32_t FilteredVc2;                  /**< Filtered bottom capacitor voltage in volts. */
    float32_t ZeroSequence;                 /**< Output: zero-sequence balancing value computed from the
                                               capacitor-voltage
                                                 difference (Kp_zerosequence * (FilteredVc1 - FilteredVc2)).
                                                 Must be copied to rfc_inner_context_t::vars.ZeroSequence
                                                 by the application before calling RfcInner(). */
    #ifndef RFC_USE_LOCAL_STORAGE
    #if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW) || defined(CY_DOXYGEN)
    int32_t ctrl3p3z_out;                   /**< Outer-loop controller output (DF1-Q23 or HW variant) */
    #else
    float32_t ctrl3p3z_out;                 /**< Outer-loop controller output (DF2 float variant) */
    #endif // if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW) || defined(CY_DOXYGEN)
    #endif // ifndef RFC_USE_LOCAL_STORAGE
} rfc_outer_static_t;

/**
 * \brief Top-level structure of the RFC inner module
 * \details
 * This structure must be instantiated by the application and contains both
 * configuration parameters, application-provided runtime inputs, RFC-maintained state,
 * and outputs for the inner current control loop.
 */
typedef struct
{
    rfc_inner_static_t vars;                /**< Application runtime inputs plus RFC-maintained state and outputs. */
    rfc_inner_config_t config;              /**< Configuration structure. */
} rfc_inner_context_t;

/**
 * \brief Top-level structure of the RFC outer module
 * \details
 * This structure must be instantiated by the application and contains both
 * configuration parameters, application-provided runtime inputs, RFC-maintained state,
 * and outputs for the outer voltage control loop.
 */
typedef struct
{
    rfc_outer_static_t vars;                /**< Application runtime inputs plus RFC-maintained state and outputs. */
    rfc_outer_config_t config;              /**< Configuration structure. */
} rfc_outer_context_t;

/** \} group_rfc_typedefs_and_structures */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif // ifndef RFC_TYPES_H__

/* --- End of File ------------------------------------------------ */
