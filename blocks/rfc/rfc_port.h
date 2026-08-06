/**
 * @file rfc_port.h
 * @brief Portability layer for reference frame control.
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

#ifndef RFC_PORT_H__
#define RFC_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mtb_mw_pctrl_config.h"

/**
 * \addtogroup group_rfc_macros_and_enums
 * \{
 * \brief Portability macros for math operations used by RFC.
 *
 * These macros abstract the math backend used by RFC. By default, the C math
 * library is used. If RFC_USE_CMSIS_DSP_LIB is defined, CMSIS-DSP functions
 * are used instead.
 */

#ifdef RFC_USE_CMSIS_DSP_LIB
#include "arm_math.h"
/* CMSIS DSP library */
#define PCTRL_RFC_SINF(x)       ((float32_t) arm_sin_f32(x))        /**< RFC sine wrapper */
#define PCTRL_RFC_COSF(x)       ((float32_t) arm_cos_f32(x))        /**< RFC cosine wrapper */
#define PCTRL_RFC_SQRTF(x, y)   ((void) arm_sqrt_f32(x, y))         /**< RFC square-root wrapper (CMSIS signature) */

#else /* RFC_USE_CMSIS_DSP_LIB */
#include "math.h"
/* C math library */
#define PCTRL_RFC_SINF(x)       ((float32_t)sinf(x))                /**< RFC sine wrapper */
#define PCTRL_RFC_COSF(x)       ((float32_t)cosf(x))                /**< RFC cosine wrapper */
#define PCTRL_RFC_SQRTF(x)      ((float32_t)sqrtf(x))               /**< RFC square-root wrapper (C math signature) */

#endif /* RFC_USE_CMSIS_DSP_LIB */

#ifndef PI
#define PI                3.14159265358979f     /**< Mathematical constant pi used by RFC calculations. */
#endif

/** \} group_rfc_macros_and_enums */

#ifdef __cplusplus
} /* Extern C */
#endif

#endif // ifndef RFC_PORT_H__

/* --- End of File ------------------------------------------------ */
