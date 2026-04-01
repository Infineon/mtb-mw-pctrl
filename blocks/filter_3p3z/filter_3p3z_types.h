/**
 * @file filter_3p3z_types.h
 * @brief Definition of filter_3p3z types and structures.
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

#ifndef FILTER_3P3Z_TYPES_H__
#define FILTER_3P3Z_TYPES_H__

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
 * ==============================================================================
 */

/**
 * \addtogroup group_filter_3p3z_typedefs_and_structures
 * \{
 * \brief Context structure and sub-structures for the 3p3z filter block.
 * <br>
 * \snippet{doc} filter_3p3z.h snippet_filter_3p3z_using_the_API
 */

/**
 * Structure used to keep the module configuration, DF1 Q23 (sub-structure of the modules context).
 */
typedef struct
{
    int32_t cx_q23[4];          /**< Filter coefficients CX0, CX1, CX2, CX3 of input LPF (Q0.23) */
    int32_t cy_q23[3];          /**< Filter coefficients CY1, CY2, CY3 of output LPF (Q0.23) */
    int32_t dOutOffset_q23;     /**< Output offset: value (Q0.23) added to the output */
    int32_t limMax_q23;         /**< Output limiter max value (Q0.23) */
    int32_t limMin_q23;         /**< Output limiter min value (Q0.23) */
    uint8_t scaleCx;            /**< Input CX scaling factor: left shift by 0..7 */
    uint8_t scaleCy;            /**< Input CY scaling factor: left shift by 0..7 */
    uint8_t gIn;                /**< Input gain: left shift by 0..3 */
    uint8_t gOut;               /**< Output gain: right shift by 0..7 */
} filter_3p3z_config_df1_q23_t;


/**
 * Structure used to keep the module configuration, DF2 F32 (sub-structure of the modules context).
 */
typedef struct
{
    float32_t cx[4];
    float32_t cy[3];
    float32_t antiwindup_gain;
    float32_t max;          // Max value to clamp the regulator output.
    float32_t min;          // Min value to clamp the regulator output.
} filter_3p3z_config_df2_f32_t;


/**
 * Structure used to keep the static module variables, DF1 Q23 (sub-structure of the modules context).
 */
typedef struct
{
    int32_t x_q23[3];                       /**< History of the input signal (Q0.23) */
    int32_t y_q23[3];                       /**< History of the output signal (Q0.23) */
    #ifdef FILTER_3P3Z_USE_VARIANT_CONFIG
    filter_3p3z_variant_config_t variant;   /**< variant configuration */
    #endif
} filter_3p3z_static_df1_q23_t;


/**
 * Structure used to keep the static module variables, DF2 F32 (sub-structure of the modules context).
 */
typedef struct
{
    float32_t v_n[3];
} filter_3p3z_static_df2_f32_t;


/**
 * Top-level structure of the module, DF1 Q23
 */
typedef struct
{
    filter_3p3z_static_df1_q23_t vars;      /**< Structure containing the static context variables */
    filter_3p3z_config_df1_q23_t config;    /**< Configuration structure */
} filter_3p3z_context_df1_q23_t;


/**
 * Top-level structure of the module, DF2 F32
 */
typedef struct
{
    filter_3p3z_static_df2_f32_t vars;      /**< Structure containing the static context variables */
    filter_3p3z_config_df2_f32_t config;    /**< Configuration structure */
} filter_3p3z_context_df2_f32_t;


/** \} group_filter_3p3z_structures */

#ifdef __cplusplus
} /* Extern C */
#endif


#endif // ifndef FILTER_3P3Z_TYPES_H__


/* --- End of File ------------------------------------------------ */
