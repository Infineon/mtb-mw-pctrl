/**
 * @file    mtb_mw_pctrl_compiler.h
 * @brief   compiler abstractions for pctrl library
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

#ifndef MTB_MW_PCTRL_COMPILER_H__
#define MTB_MW_PCTRL_COMPILER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================================================================
 *   1. PCTRL MW Library configuration
 * ==============================================================================
 */


/* ==============================================================================
 *   2. Include files
 * ==============================================================================
 */
#include "stddef.h"

/* ==============================================================================
 *   3. Definitions
 * ==============================================================================
 */

/* Criterion for availability of a compiler abstraction.
 * In the ModusToolbox environment, the compiler abstraction is done by CMSIS.
 * This must be adapted if the library is used in other (non-MTB) environments.
 */
#define HAS_MTB_COMPILER_ABSTRACTION    __has_include("cmsis_compiler.h")

#if defined (HAS_MTB_COMPILER_ABSTRACTION) && (HAS_MTB_COMPILER_ABSTRACTION == 1)
    #include "cmsis_compiler.h"
#else
    #error Add abstraction of __STATIC_FORCEINLINE for your specific compiler here
#endif

/* Library uses data types from arm_math_types.h, any used types are also defined
 * here by generic typedefs. If more types are used, they should be added to
 * their respective block types headers.
 */
#define HAS_CMSIS_DSP_LIB   __has_include("arm_math.h")

#if defined (HAS_CMSIS_DSP_LIB) && (HAS_CMSIS_DSP_LIB == 1)
    #include "arm_math.h"
#else
typedef float float32_t;
#endif


//#define PWRLIB_INLINE __INLINE
//#define PWRLIB_INLINE __STATIC_INLINE
#define PWRLIB_INLINE __STATIC_FORCEINLINE
#define PWRLIB_UNUSED(x) ((void)x)

/* ==============================================================================
 *   3. Types
 * ==============================================================================
 */


/* ==============================================================================
 *   4. Exported data
 * ==============================================================================
 */


#ifdef __cplusplus
} /* Extern C */
#endif

#endif // MTB_MW_PCTRL_COMPILER_H__


/* --- End of File ------------------------------------------------ */
