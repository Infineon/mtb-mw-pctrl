/**
 * @file ac_rms_pll_port.h
 * @brief Portability layer for AC RMS PLL
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
#ifndef AC_RMS_PLL_PORT_H_
#define AC_RMS_PLL_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mtb_mw_pctrl_config.h"

#ifdef AC_RMS_PLL_USE_CMSIS_DSP_LIB
#include "arm_math.h"
/*CMSIS DSP Lib*/
#define PCTRL_SINF(x)           ((float32_t) arm_sin_f32(x))
#define PCTRL_COSF(x)           ((float32_t) arm_cos_f32(x))
#define PCTRL_SQRTF(x, y)       ((void) arm_sqrt_f32(x, y))
#define PCTRL_FLOORF(x)         ((float32_t) floorf(x))

#else /*AC_RMS_PLL_USE_CMSIS_DSP_LIB*/
#include "math.h"
/* Math lib*/
#define PCTRL_SINF(x)       ((float32_t) sinf(x))
#define PCTRL_COSF(x)       ((float32_t) cosf(x))
#define PCTRL_SQRTF(x)      ((float32_t) sqrtf(x))
#define PCTRL_FLOORF(x)     ((float32_t) floorf(x))

#endif /*AC_RMS_PLL_USE_CMSIS_DSP_LIB*/

#ifndef PI
#define PI                3.14159265358979f
#endif
#ifndef TAU
#define TAU               6.283185307179586f
#endif

#define AC_RMS_PLL_SMALL_NUM        0.000001f                // Small number to avoid division by zero
#define AC_RMS_PLL_TAU              6.283185307179586f
#define AC_RMS_PLL_TAU_INV          0.159154943091895f
#define AC_RMS_PLL_SQRT2_INV        0.707106781186547f

#ifdef __cplusplus
} /* Extern C */
#endif

#endif /* AC_RMS_PLL_PORT_H_ */

/* --- End of File ------------------------------------------------ */
