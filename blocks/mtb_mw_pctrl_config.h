/**
 * @file    mtb_mw_pctrl_config.h
 * @brief   configuration header for pctrl library
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

#ifndef MTB_MW_PCTRL_CONFIG_H__
#define MTB_MW_PCTRL_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================================================================
 *   1. PCTRL MW Library configuration
 * ==============================================================================
 */
#include "mtb_mw_pctrl_compiler.h"
#include "mtb_mw_pctrl_user_config.h"

#if !defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) && \
    !defined(FILTER_3P3Z_USE_VARIANT_HW) && \
    !defined(FILTER_3P3Z_USE_VARIANT_DF2_F32)
/* Uncomment one option to select the global default for the 3P3Z filter
 */
#define FILTER_3P3Z_USE_VARIANT_DF1_Q23
//#define FILTER_3P3Z_USE_VARIANT_HW
#endif

#ifndef FILTER_3P3Z_USE_INLINE
/* Uncomment for using function inlining as global default for the 3P3Z filter
 */
//#define FILTER_3P3Z_USE_INLINE
#endif


#ifdef FILTER_3P3Z_USE_VARIANT_DF1_Q23
/* Emulate the HWFILTER3P3Z hardware of PPCA HW
 * The filter_3p3z library implementation uses C99 data types (e.g., int32_t, uint8_t).
 * As such types have more bits than the HWFILTER3P3Z supports for some parameters
 * (e.g., gin, gout, scale_x, scale_x, ...) the behavior of the firmware implementation
 * is different if such bits are used. Uncomment this macro to exactly emulate
 * the HWFILTER3P3Z hardware of PPCA. Basically, this adds masking of bits that
 * are not implement in the HWFILTER3P3Z hardware.
 * Note that this might slow down the execution.
 */
#ifndef FILTER_3P3Z_USE_VARIANT_DF1_Q23_HW_PPCA_BEHAVIOR
/* Uncomment for using exact PPCA HW FILTER behavior as global default for the 3P3Z filter DF1 Q23 variant
 */
//#define FILTER_3P3Z_USE_VARIANT_DF1_Q23_HW_PPCA_BEHAVIOR
#endif
#endif

/* Check for invalid combinations
 */
#if !defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) && \
    !defined(FILTER_3P3Z_USE_VARIANT_HW) && \
    !defined(FILTER_3P3Z_USE_VARIANT_DF2_F32)
#error "Either FILTER_3P3Z_USE_VARIANT_DF1_Q23 or \
    FILTER_3P3Z_USE_VARIANT_HW or \
    FILTER_3P3Z_USE_VARIANT_DF2_F32 must be defined."
#endif
#if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) && defined(FILTER_3P3Z_USE_VARIANT_HW)
#error "FILTER_3P3Z_USE_VARIANT_DF1_Q23 and FILTER_3P3Z_USE_VARIANT_HW must not be defined at the same time."
#endif
#if defined(FILTER_3P3Z_USE_VARIANT_DF1_Q23) && defined(FILTER_3P3Z_USE_VARIANT_DF2_F32)
#error "FILTER_3P3Z_USE_VARIANT_DF1_Q23 and FILTER_3P3Z_USE_VARIANT_DF2_F32 must not be defined at the same time."
#endif
#if defined(FILTER_3P3Z_USE_VARIANT_DF2_F32) && defined(FILTER_3P3Z_USE_VARIANT_HW)
#error "FILTER_3P3Z_USE_VARIANT_DF2_F32 and FILTER_3P3Z_USE_VARIANT_HW must not be defined at the same time."
#endif

#if !defined(AC_RMS_PLL_USE_VARIANT_3PH) && !defined(AC_RMS_PLL_USE_VARIANT_1PH)
/* Uncomment one option to select the global default for the 3P3Z filter
 */
#define AC_RMS_PLL_USE_VARIANT_3PHDD
//#define AC_RMS_PLL_USE_VARIANT_1PH
#endif

#ifndef AC_RMS_PLL_USE_INLINE
/* Uncomment for using function inlining as global default for the 3P3Z filter
 */
//#define AC_RMS_PLL_USE_INLINE
#endif


#ifdef AC_RMS_PLL_USE_VARIANT_1PH

#ifndef AC_RMS_PLL_FREQ_FILTER
/* Uncomment for using frequency lp filter as global default for the 1Ph AC RMS PLL
 */
//#define AC_RMS_PLL_FREQ_FILTER
#endif

#ifndef ACRMSPLL_DCOFFSET_REJECT
/* Uncomment for using DC offset rejection as global default for the 1Ph AC RMS PLL
 */
//#define ACRMSPLL_DCOFFSET_REJECT
#endif

#ifndef ACRMSPLL_3RD_HARMONIC_REJECT
/* Uncomment for using exact 3rd-harmonic rejection as global default for the 1Ph AC RMS PLL
 */
//#define ACRMSPLL_3RD_HARMONIC_REJECT
#endif

#ifndef ACRMSPLL_5TH_HARMONIC_REJECT
/* Uncomment for using exact 5th-harmonic rejection as global default for the 1Ph AC RMS PLL
 */
//#define ACRMSPLL_5TH_HARMONIC_REJECT
#endif

#endif /* AC_RMS_PLL_USE_VARIANT_1PH */

/* Check for invalid combinations
 */
#if !defined(AC_RMS_PLL_USE_VARIANT_1PH) && defined(ACRMSPLL_DCOFFSET_REJECT)
#error "ACRMSPLL_DCOFFSET_REJECT is only available if AC_RMS_PLL_USE_VARIANT_1PH is defined."
#endif
#if !defined(AC_RMS_PLL_USE_VARIANT_1PH) && defined(ACRMSPLL_3RD_HARMONIC_REJECT)
#error "ACRMSPLL_3RD_HARMONIC_REJECT is only available if AC_RMS_PLL_USE_VARIANT_1PH is defined."
#endif
#if !defined(AC_RMS_PLL_USE_VARIANT_1PH) && defined(ACRMSPLL_5TH_HARMONIC_REJECT)
#error "ACRMSPLL_5TH_HARMONIC_REJECT is only available if AC_RMS_PLL_USE_VARIANT_1PH is defined."
#endif
#if defined(AC_RMS_PLL_USE_VARIANT_3PH) && defined(ACRMSPLL_DCOFFSET_REJECT)
#error "AC_RMS_PLL_USE_VARIANT_3PH and ACRMSPLL_DCOFFSET_REJECT must not be defined at the same time."
#endif
#if defined(AC_RMS_PLL_USE_CMSIS_DSP_LIB) && !defined(HAS_CMSIS_DSP_LIB)
#error "AC_RMS_PLL_USE_CMSIS_DSP_LIB can only be used if CMSIS DSP library is available"
#endif

#if !defined(MPPT_USE_VARIANT_PNO)
/* Uncomment one option to select the global default for MPPT
 */
#define MPPT_USE_VARIANT_PNO
#endif

#ifndef MPPT_USE_INLINE
/* Uncomment for using function inlining as global default for MPPT
 */
//#define MPPT_USE_INLINE
#endif

/* Check for invalid combinations
 */
#if !defined(MPPT_USE_VARIANT_PNO)
#error "MPPT_USE_VARIANT_PNO must be defined."
#endif

/* Uncomment to enable logging on PCTRL library level.
 * Note that this might slow down the execution.
 */
//#ifndef PCTRL_ENABLE_LOGS
//#define PCTRL_ENABLE_LOGS
//#endif

#ifdef __cplusplus
} /* Extern C */
#endif

#endif // #ifndef MTB_MW_PCTRL_CONFIG_H__


/* --- End of File ------------------------------------------------ */
