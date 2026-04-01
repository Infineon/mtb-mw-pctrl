/**
 * @file ac_rms_pll_types.h
 * @brief Definition of ac_rms_pll types and structures.
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

#ifndef AC_RMS_PLL_TYPES_H__
#define AC_RMS_PLL_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================================================================
 *   1. Include files
 * ==============================================================================
 */

#include "mtb_mw_pctrl_compiler.h"
#include "mtb_mw_pctrl_config.h"
#if defined(AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW) || defined(CY_DOXYGEN)
#include "filter_3p3z.h"
#endif


/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */


/* ==============================================================================
 *   3. Types
 * ============================================================================== */
/**
 * \addtogroup group_ac_rms_pll_typedefs_and_structures
 * \{
 * \brief Context structure and sub-structures for the AC RMS PLL block.
 * \details The context structure contains all the necessary data for AC RMS PLL operation.
 * It is divided into two main parts:
 * - **Configuration structure** (ac_rms_pll_config_t): Must be set by the application
 * - **Runtime variables structure** (ac_rms_pll_static_t): Managed automatically by the library
 * \snippet{doc} ac_rms_pll.h snippet_ac_rms_pll_using_the_API
 */

/**
 * \brief AC RMS PLL configuration structure
 * Structure used to keep the module configuration (sub-structure of the modules context).
 * This structure contains all configuration parameters that must be set by the application
 * before calling AcRmsPllInit().
 * Includes PLL controller parameters, filter coefficients, and
 * variant-specific settings for single-phase, three-phase, and three-phase
 * double decoupling variants.
 */
typedef struct
{
    float32_t omega_n;                /**< Nominal frequency in rad/s used as init value of the PLL output */
    float32_t lpc_rms_alpha;          /**< Coefficient of lowpass filter for the RMS: alpha/(beta+z) */
    float32_t lpc_rms_beta;           /**< Coefficient of lowpass filter for the RMS: alpha/(beta+z) */
    float32_t lpc_rms_init;           /**< Initial value of output of lowpass filter for the RMS: alpha/(z-beta) */
    float32_t lpc_omega_alpha;        /**< Coefficient of lowpass filter for the frequency: alpha/(z-beta) */
    float32_t lpc_omega_beta;         /**< Coefficient of lowpass filter for the frequency: alpha/(z-beta) */
    float32_t lpc_omega_init;         /**< Initial value of output of lowpass filter for the frequency: alpha/(z-beta)
                                       */
    float32_t Kp;                     /**< Proportional gain for PI regulator */
    float32_t Ki;                     /**< Integral gain for PI regulator */
    float32_t cterm_min;              /**< Lower limit for PI regulator output */
    float32_t cterm_max;              /**< Upper limit for PI regulator output */
    float32_t dt;                     /**< Sampling time in seconds */
    #if defined(AC_RMS_PLL_USE_VARIANT_3PHDD) || defined(CY_DOXYGEN)
    float32_t lpc_dd_alpha;           /**< Low-pass filter coefficient for sequence extraction (alpha) */
    float32_t lpc_dd_beta;            /**< Low-pass filter coefficient for sequence extraction (beta = 1-alpha) */
    float32_t lpc_dd_init;            /**< Initial value for sequence component filters */
    #if defined(AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW) || defined(CY_DOXYGEN)
    filter_3p3z_context_t ctrl3p3z_ctx; /**< Optional 3p3z HW control loop context (user allocates & initializes) */
    float32_t scale_3p3z;             /**< Scaling factor to convert Q0.23 to float for 3p3z controller output*/
    #endif
    #endif /* AC_RMS_PLL_USE_VARIANT_3PHDD */
    #if defined(AC_RMS_PLL_USE_VARIANT_1PH) || defined(CY_DOXYGEN)
    float32_t Kosg;                   /**< Damping factor for OSG (single-phase variant only) */
    float32_t Krms;                   /**< Gain for RMS calculation (single-phase variant only) */
    #if defined(ACRMSPLL_DCOFFSET_REJECT) || defined(CY_DOXYGEN)
    float32_t Kdc;                    /**< Gain for DC offset compensation (single-phase variant with DC rejection only)
                                       */
    float32_t dc_max;                 /**< Maximum limit for DC offset (single-phase variant with DC rejection only) */
    float32_t dc_min;                 /**< Minimum limit for DC offset (single-phase variant with DC rejection only) */
    #endif /* ACRMSPLL_DCOFFSET_REJECT */
    #if defined(ACRMSPLL_3RD_HARMONIC_REJECT) || defined(CY_DOXYGEN)
    float32_t Kosg3;                  /**< Damping factor 3rd-harmonic (single-phase variant with 3rd-harmonic
                                         rejection) */
    #endif /* ACRMSPLL_3RD_HARMONIC_REJECT */
    #if defined(ACRMSPLL_5TH_HARMONIC_REJECT) || defined(CY_DOXYGEN)
    float32_t Kosg5;                  /**< Damping factor 5th-harmonic (single-phase variant with 5th-harmonic
                                         rejection) */
    #endif /* ACRMSPLL_5TH_HARMONIC_REJECT */
    #endif /* AC_RMS_PLL_USE_VARIANT_1PH */
} ac_rms_pll_config_t;

/**
 * \brief AC RMS PLL runtime variables structure
 * Structure used to keep the module's runtime state variables (sub-structure of the modules context).
 * This structure contains all the internal state variables, filter memories, and intermediate
 * calculation results. It is managed automatically by the library and should not be
 * modified directly by the application.
 *
 * **Common variables (all variants):**
 * - PLL tracking: theta, omega_raw, omega_filtered
 * - Park transform: sin_theta, cos_theta, d, q, q_n
 * - PI controller: iterm, cterm
 * - RMS calculation: rms_raw, rms_filtered
 *
 * **Variant-Specific Variables:**
 * - **Single-phase**: SOGI filters (alpha_filtered, beta_filtered, osg_integral_term) and optional
 *   DC offset rejection, 3rd/5th harmonic rejection states
 * - **Three-phase dual decoupling**: Dual reference frame states (d_pos, q_pos, d_neg, q_neg), decoupling network
 *   outputs (dd_pos, dq_pos, dd_neg, dq_neg) and their filtered values
 */
typedef struct
{
    float32_t theta;                   /**< Phase angle of the input AC waveform in radians */
    float32_t omega_raw;               /**< Angular frequency of the input AC waveform in rad/s (un-filtered) */
    float32_t omega_filtered;          /**< Angular frequency of the input AC waveform in rad/s (filtered) */
    float32_t sin_theta;               /**< Sine of the phase angle of the input AC waveform */
    float32_t cos_theta;               /**< Cosine of the phase angle of the input AC waveform */
    float32_t d;                       /**< Direct component of the Park Transform */
    float32_t q;                       /**< Quadrature component of the Park Transform */
    float32_t q_n;                     /**< Normalized quadrature component of the Park Transform */
    float32_t rms_raw;                 /**< RMS value of the input AC waveform (un-filtered) */
    float32_t rms_filtered;            /**< RMS value of the input AC waveform (filtered) */
    float32_t iterm;                   /**< PI regulator integrator term */
    float32_t cterm;                   /**< PI regulator control output */
    float32_t integrator_sum;          /**< Angular frequency integrator sum */

    #if defined(AC_RMS_PLL_USE_VARIANT_3PHDD) || defined(CY_DOXYGEN)
    /* DDSRF-specific variables */
    float32_t sin_2theta;               /**< Sine of the 2x phase angle of the input AC waveform */
    float32_t cos_2theta;               /**< Cosine of the 2x phase angle of the input AC waveform */
    float32_t d_pos;                    /**< Raw d-axis voltage in +ve sequence frame (before filtering)*/
    float32_t q_pos;                    /**< Raw q-axis voltage in +ve sequence frame (before filtering)*/
    float32_t d_neg;                    /**< Raw d-axis voltage in -ve sequence frame */
    float32_t q_neg;                    /**< Raw q-axis voltage in -ve sequence frame */
    float32_t dd_pos;                   /**< Raw decoupled d-axis voltage in +ve sequence frame (before filtering)*/
    float32_t dq_pos;                   /**< Raw decoupled q-axis voltage in +ve sequence frame (before filtering)*/
    float32_t dd_neg;                   /**< Raw decoupled d-axis voltage in -ve sequence frame */
    float32_t dq_neg;                   /**< Raw decoupled q-axis voltage in -ve sequence frame */
    float32_t dd_pos_filt;              /**< Filtered d-axis voltage in +ve sequence frame */
    float32_t dq_pos_filt;              /**< Filtered q-axis voltage in +ve sequence frame */
    float32_t dd_neg_filt;              /**< Filtered d-axis voltage in -ve sequence frame */
    float32_t dq_neg_filt;              /**< Filtered q-axis voltage in -ve sequence frame */
    #if defined(AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW) || defined(CY_DOXYGEN)
    int32_t ctrl3p3z_out;           /**< Last 3p3z controller output mapped to cterm */
    #endif
    #endif /* AC_RMS_PLL_USE_VARIANT_3PHDD */

    /*SOGI Variables*/
    #if defined(AC_RMS_PLL_USE_VARIANT_1PH) || defined(CY_DOXYGEN)
    float32_t alpha_error;             /**< SOGI alpha input error */
    float32_t alpha_filtered;          /**< SOGI alpha term output */
    float32_t beta_filtered;           /**< SOGI beta term output */
    float32_t alpha_filtered_I;        /**< Integrated alpha filtered */
    float32_t alpha_filtered_last;     /**< Last value of alpha filtered */
    float32_t osg_integral_term;       /**< SOGI integrator term */
    float32_t osg_integral_term_last;  /**< SOGI integrator term last value */
    #if defined(ACRMSPLL_DCOFFSET_REJECT) || defined(CY_DOXYGEN)
    float32_t osg_integral_dc_term;    /**< DC offset integrator term */
    float32_t alpha_compensated;       /**< alpha term after DC compensation */
    float32_t alpha_compensated_last;  /**< alpha term after DC compensation last value */
    #endif /*ACRMSPLL_DCOFFSET_REJECT*/
    #if defined(ACRMSPLL_3RD_HARMONIC_REJECT) || defined(CY_DOXYGEN)
    float32_t alpha3;                  /**< 3rd-harmonic alpha input error */
    float32_t omega3;                  /**< 3rd-harmonic angular frequency */
    float32_t alpha3_filtered;         /**< 3rd-harmonic alpha term output */
    float32_t beta3_filtered;          /**< 3rd-harmonic beta term output */
    float32_t alpha3_filtered_I;       /**< 3rd-harmonic integrated alpha filtered */
    float32_t alpha3_filtered_last;    /**< 3rd-harmonic integrated alpha filtered last value */
    float32_t osg3_integral_term;      /**< 3rd-harmonic integrator term */
    float32_t osg3_integral_term_last; /**< 3rd-harmonic integrator term last value */
    #endif /* ACRMSPLL_3RD_HARMONIC_REJECT*/
    #if defined(ACRMSPLL_5TH_HARMONIC_REJECT) || defined(CY_DOXYGEN)
    float32_t alpha5;                  /**< 5th-harmonic alpha input error */
    float32_t omega5;                  /**< 5th-harmonic angular frequency */
    float32_t alpha5_filtered;         /**< 5th-harmonic alpha term output */
    float32_t beta5_filtered;          /**< 5th-harmonic beta term output */
    float32_t alpha5_filtered_I;       /**< 5th-harmonic integrated alpha filtered */
    float32_t alpha5_filtered_last;    /**< 5th-harmonic integrated alpha filtered last value */
    float32_t osg5_integral_term;      /**< 5th-harmonic integrator term */
    float32_t osg5_integral_term_last; /**< 5th-harmonic integrator term last value */
    #endif /* ACRMSPLL_5TH_HARMONIC_REJECT*/
    #endif /* AC_RMS_PLL_USE_VARIANT_1PH*/
} ac_rms_pll_static_t;

/**
 * Top-level structure of the AC RMS PLL module.
 * This structure must be instantiated by the application and contains both
 * configuration parameters and runtime state variables.
 */
typedef struct
{
    ac_rms_pll_static_t vars;       /**< Structure containing the static context variables (managed automatically) */
    ac_rms_pll_config_t config;     /**< Configuration structure (must be set by application) */
} ac_rms_pll_context_t;

/** \} group_ac_rms_pll_typedefs_and_structures */
#ifdef __cplusplus
} /* Extern C */
#endif
#endif /*AC_RMS_PLL_TYPES_H__*/
