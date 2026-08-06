/**
 * @file filter_3p3z_types.h
 * @brief Definition of filter_3p3z types and structures.
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

#if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) || defined(FILTER_3P3Z_USE_VARIANT_HW)

#include "filter_3p3z_types_df1_q23.h"

#elif defined(FILTER_3P3Z_USE_VARIANT_DF2_F32)

#include "filter_3p3z_types_df2_f32.h"

#endif

/** \} group_filter_3p3z_structures */

#ifdef __cplusplus
} /* Extern C */
#endif


#endif // ifndef FILTER_3P3Z_TYPES_H__


/* --- End of File ------------------------------------------------ */
