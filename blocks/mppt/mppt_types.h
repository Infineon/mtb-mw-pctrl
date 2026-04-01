/**
 * @file    mppt_types.h
 * @brief   Type definitions for MPPT Perturb and Observe block
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

#ifndef MPPT_TYPES_H__
#define MPPT_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================================================================
 *   1. Include files
 * ==============================================================================
 */

#include "mtb_mw_pctrl_compiler.h"
#include "mtb_mw_pctrl_config.h"

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */

/* ==============================================================================
 *   3. Types
 * ============================================================================== */

/**
 * \addtogroup group_mppt_typedefs_and_structures
 * \{
 * \brief Context structure and sub-structures for the MPPT block.
 * <br>
 * The context structure contains all the necessary data for MPPT operation.
 * It is divided into two main parts:
 * - **Configuration structure** (mppt_config_t): Must be set by the application
 * - **Runtime variables structure** (mppt_static_t): Managed automatically by the library
 * \snippet{doc} mppt.h snippet_mppt_using_the_API
 */

/**
 * \brief MPPT configuration structure
 * Structure used to keep the module configuration (sub-structure of the modules context).
 * This structure contains all the configuration parameters that must be set by the application
 * before calling MpptInit().
 * The configuration includes reference perturbation step size, operating limits, and deadband.
 */
typedef struct
{
    float32_t delta_d;         /**< Reference perturbation step size (0.0 to 1.0) */
    float32_t d_min;           /**< Minimum reference limit (0.0 to 1.0) */
    float32_t d_max;           /**< Maximum reference limit (0.0 to 1.0) */
    float32_t d_initial;       /**< Initial reference value (0.0 to 1.0) */
    float32_t deadband;        /**< Deadband for power to stop oscillation around max power point */
} mppt_config_t;

/**
 * Structure used to keep the module's runtime state variables (sub-structure of the modules context).
 * This structure contains all internal state variables, previous measurements, and intermediate
 * calculation results. It is managed automatically by the library and should not be
 * modified directly by the application.
 */
typedef struct
{
    float32_t v_prev;          /**< Previous voltage measurement */
    float32_t p_prev;          /**< Previous power measurement */
    float32_t p_current;       /**< Current power measurement */
    float32_t d_ref;           /**< Current normalized reference output (0.0 to 1.0) */
    float32_t dp;              /**< Power difference (P_current - P_previous) */
    float32_t dv;              /**< Voltage difference (V_current - V_previous) */
} mppt_static_t;

/**
 * Top-level structure of the MPPT module.
 * This structure must be instantiated by the application and contains both
 * configuration parameters and runtime state variables.
 */
typedef struct
{
    mppt_static_t vars;    /**< Structure containing the static context variables (managed automatically) */
    mppt_config_t config;  /**< Configuration structure (must be set by application) */
} mppt_context_t;

/** \} group_mppt_typedefs_and_structures */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif /*MPPT_TYPES_H__*/

/* --- End of File ------------------------------------------------ */
