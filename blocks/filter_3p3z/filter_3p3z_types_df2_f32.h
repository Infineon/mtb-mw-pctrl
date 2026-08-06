/**
 * @file filter_3p3z_types_df2_f32.h
 * @brief Definition of filter_3p3z DF2 F32 variant types and structures.
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

#ifndef FILTER_3P3Z_DF2_F32_TYPES_H__
#define FILTER_3P3Z_DF2_F32_TYPES_H__

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
 * \brief Context structure and sub-structures for the 3p3z filter block, DF2 F32 variant.
 */

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
 * Structure used to keep the static module variables, DF2 F32 (sub-structure of the modules context).
 */
typedef struct
{
    float32_t v_n[3];
} filter_3p3z_static_df2_f32_t;


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


#endif // ifndef FILTER_3P3Z_DF2_F32_TYPES_H__


/* --- End of File ------------------------------------------------ */
