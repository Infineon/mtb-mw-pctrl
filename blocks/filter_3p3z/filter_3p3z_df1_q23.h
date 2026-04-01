/**
 * @file    filter_3p3z_df1_q23.h
 * @brief   API of a software 3-pole 3-zero filter.
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

#ifndef FILTER_3P3Z_DF1_Q23_H__
#define FILTER_3P3Z_DF1_Q23_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================================================================
 *   1. Include files
 * ==============================================================================
 */

#include "mtb_mw_pctrl_compiler.h"
#include "mtb_mw_pctrl_config.h"
#include "filter_3p3z_port.h"
#include "filter_3p3z_types.h"
#include "cy_pctrl_log.h"
#include <stddef.h>

/* ==============================================================================
 *   2. Definitions
 * ==============================================================================
 */

/**
 * TO_INT32
 * Helper macro to cast an arbitrary quantized value to int32_t.
 * This macro masks out the upper (not effective) bits from v and does sign extension
 * to cast to an int32_t.
 *
 * @param[in] v     : Arbitrary quantized value
 * @param[in] width : Number of effective bits in v, including the sign
 * @return          : Corresponding int32_t value
 */
#define TO_INT32(v, width)  ((int32_t)(uint32_t)(( \
    ((((uint32_t)(v)) & (((uint32_t)1u) << ((width) - 1u))) != 0u)? \
    ((UINT32_MAX) & (~((((uint32_t)1u) << ((width) - 1u)) - 1u))) : (uint32_t)0u \
) | (((uint32_t)(v)) & ((((uint32_t)1u) << ((width) - 1u)) - 1u))))

/**
 * TO_UINT8
 * Helper macro to cast an arbitrary quantized value to uint8_t.
 * This macro masks out the upper (not effective) bits from v.
 *
 * @param[in] v     : Arbitrary quantized value
 * @param[in] width : Number of effective bits in v
 * @return          : Corresponding uint8_t value
 */
#define TO_UINT8(v, width)  ((uint8_t)((uint8_t)(v) & ((((uint8_t)1u) << (width)) - 1u)))

/**
 * Quantization of the y-branch.
 */
#define QY      (24u)

/* ==============================================================================
 *   3. Types
 * ==============================================================================
 */


/* ==============================================================================
 *   4. Exported data
 * ==============================================================================
 */


/* ==============================================================================
 *   5. Function prototypes and inline functions
 * ==============================================================================
 */

void Filter3p3zReset_DF1_Q23(filter_3p3z_context_df1_q23_t* const context);
void Filter3p3zInit_DF1_Q23(filter_3p3z_context_df1_q23_t* const context);

/**
 * 3p3z filter
 * This filter is a bit-true firmware implementation the HWFILTER3P3Z hardware of PSC3_P8
 * and is meant to be used on other targets where no filter hardware is available
 * or on PSC3_P8 if all hardware instances are utilized. The firmware implementation
 * has the same features and uses the same parameters as the hardware
 * filter does.
 * As long as the parameters and internal values do not exceed the ranges the hardware filter
 * allows, the firmware implementation is functionally equivalent but certainly slower.
 *
 * As the firmware uses C99 data types (e.g., int32_t, uint8_t) there
 * are additional bits in internal values and parameter ranges that can be utilized
 * (e.g., gin, gout, scale_x, scale_x, ...). The behavior of the firmware implementation
 * is different if such bits are used. There is an option to exactly emulate
 * the HWFILTER3P3Z hardware of PSC3_P8. Basically, this adds masking of bits that
 * are not implemented in the HWFILTER3P3Z hardware.
 * To activate this option, define FILTER_3P3Z_USE_VARIANT_DF1_Q23_HW_PPCA_BEHAVIOR.
 *
 * Filter equation:
 * out[n] = ((b3.x[n-3] + b2.x[n-2] + b1.x[n-1] + b0.x[n])) + ((a3.y[n-3] + a2.y[n-2] + a1.y[n-1]))
 *
 * @param[in] context       : Pointer to the context structure
 * @param[in] dIn0_0q15     : Normalized setpoint (Q0.15)
 * @param[in] dIn1_0q15     : Normalized signal (Q0.15)
 * @param[out] dOut_0q23    : Pointer to output value (Q0.23)
 */
PWRLIB_INLINE void Filter3p3z_DF1_Q23_inline(
    filter_3p3z_context_df1_q23_t* const context,
    int16_t const dIn0_0q15, int16_t const dIn1_0q15, int32_t* dOut_0q23)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);
    cy_pctrl_assert_msg((dOut_0q23 != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Output pointer is NULL in %s!\r\n", __func__);

    #ifdef FILTER_3P3Z_USE_VARIANT_DF1_Q23_HW_PPCA_BEHAVIOR
    // bit mask all parameters and input values to exactly mimic behavior of PSC3_P8 HWFILTER3P3Z
    // note that this slows down execution
    // din0_q15 is int16_t, no need to mask
    // din1_q15 is int16_t, no need to mask
    context->vars.x_q23[0u] = TO_INT32(context->vars.x_q23[0u], 25u);
    context->vars.x_q23[1u] = TO_INT32(context->vars.x_q23[1u], 25u);
    context->vars.x_q23[2u] = TO_INT32(context->vars.x_q23[2u], 25u);
    context->vars.y_q23[0u] = TO_INT32(context->vars.y_q23[0u], 24u);
    context->vars.y_q23[1u] = TO_INT32(context->vars.y_q23[1u], 24u);
    context->vars.y_q23[2u] = TO_INT32(context->vars.y_q23[2u], 24u);
    context->config.dOutOffset_q23 = TO_INT32(context->config.dOutOffset_q23, 24u);
    context->config.limMax_q23 = TO_INT32(context->config.limMax_q23, 24u);
    context->config.limMin_q23 = TO_INT32(context->config.limMin_q23, 24u);
    context->config.scaleCx = TO_UINT8(context->config.scaleCx, 3u);
    context->config.scaleCy = TO_UINT8(context->config.scaleCy, 3u);
    context->config.gIn = TO_UINT8(context->config.gIn, 2u);
    context->config.gOut = TO_UINT8(context->config.gOut, 3u);
    #endif // if FILTER_3P3Z_USE_VARIANT_DF1_Q23_HW_PPCA_BEHAVIOR

    // Input deviation from reference
    int32_t dIn_1q15 = (int32_t)dIn0_0q15 - dIn1_0q15;

    // Input gain, max. 2 bit
    // Note: applying gain does not move the decimal point
    dIn_1q15 = (int32_t)(uint32_t)((uint32_t)dIn_1q15 << context->config.gIn);

    #ifdef FILTER_3P3Z_USE_VARIANT_DF1_Q23_HW_PPCA_BEHAVIOR
    dIn_1q15 = TO_INT32(dIn_1q15, 17u);
    #endif

    // Normalize to from Q1.15 to Q1.23
    // This is basically just adding 8 LSBs
    int32_t dIn_1q23 = (int32_t)(uint32_t)((uint32_t)dIn_1q15 << 8u);

    // a Biquad (3p3z) filter consists of two LPF filters added together:
    // One LPF over the input values and one LPF over the output values
    // Input LPF
    // Each input multiplication is q23xq23=q46bit.
    int64_t inputLpf_3q46;
    inputLpf_3q46 = (int64_t)(context->config.cx_q23[0u] * (int64_t)dIn_1q23);                     // cx0 * x[n]
    inputLpf_3q46 += (int64_t)(context->config.cx_q23[1u] * (int64_t)context->vars.x_q23[0u]);     // cx1 * x[n-1]
    inputLpf_3q46 += (int64_t)(context->config.cx_q23[2u] * (int64_t)context->vars.x_q23[1u]);     // cx2 * x[n-2]
    inputLpf_3q46 += (int64_t)(context->config.cx_q23[3u] * (int64_t)context->vars.x_q23[2u]);     // cx3 * x[n-3]

    // Output LPF
    // Each output multiplication is q23xq23=q46.
    int64_t outputLpf_2q46;
    outputLpf_2q46 = (int64_t)(context->config.cy_q23[0u] * (int64_t)context->vars.y_q23[0u]);     // cy1 * y[n-1]
    outputLpf_2q46 += (int64_t)(context->config.cy_q23[1u] * (int64_t)context->vars.y_q23[1u]);    // cy2 * y[n-2]
    outputLpf_2q46 += (int64_t)(context->config.cy_q23[2u] * (int64_t)context->vars.y_q23[2u]);    // cy3 * y[n-3]

    // Scale accumulated output, left shift by max. 7 bit
    // Note: scaling does not move the decimal point
    int64_t acc_11q46 = (int64_t)(uint64_t)((uint64_t)inputLpf_3q46 << context->config.scaleCx) +
                        (int64_t)(uint64_t)((uint64_t)outputLpf_2q46 << context->config.scaleCy);


    // Round and saturate
    // Add half lsb for rounding
    // If value is greater than +/-1, it saturates
    // If less than +/-1 it takes the value as it is
    const int64_t halfLsb_q46 = (int64_t)(uint64_t)((uint32_t)1u << ((2u * (QY - 1u)) - QY));
    acc_11q46 += halfLsb_q46;

    const int64_t one_q46 = (int64_t)(uint64_t)((uint64_t)1u << (2u * (QY - 1u)));
    if (acc_11q46 >= one_q46)
    {
        acc_11q46 = one_q46-1;
    }
    else if (acc_11q46 < -one_q46)
    {
        acc_11q46 = -one_q46;
    }
    else
    {
        // Keep as is
    }

    // Convert to Q0.23
    // Note: Misra 10.1 forbids bitwise right shifts of signed values because
    // it is not defined by the C-standard whether to fill with zeros
    // or to extend the sign bit. In this case we do not care because
    // the upper bits are cut off anyway when casting to int32_t.
    // So, we cast to uint64_t, shift left by 23,
    // keep the lower 32 bit and cast back to int32_t.
    int32_t y_0q23 = (int32_t)(uint32_t)((uint64_t)acc_11q46 >> (QY - 1u));

    // Updating input array
    context->vars.x_q23[2u] = context->vars.x_q23[1u];
    context->vars.x_q23[1u] = context->vars.x_q23[0u];
    context->vars.x_q23[0u] = dIn_1q23;

    // Updating output array
    context->vars.y_q23[2u] = context->vars.y_q23[1u];
    context->vars.y_q23[1u] = context->vars.y_q23[0u];
    context->vars.y_q23[0u] = (int32_t)y_0q23;

    // Filter output

    // Output gain, max. 3 bit
    // Note: applying gain does not move the decimal point
    // Note: Misra 10.1 forbids bitwise right shifts of signed values because
    // it is not defined by the C-standard whether to fill with zeros
    // or to extend the sign bit.
    if (y_0q23 < 0)
    {
        y_0q23 = (int32_t)(uint32_t)(~((~(uint32_t)y_0q23) >> context->config.gOut));
    }
    else
    {
        y_0q23 = (int32_t)(uint32_t)((uint32_t)y_0q23 >> context->config.gOut);
    }

    // Output offset
    // Note: it is expected that both values have the same number representation.
    int32_t y_1q23 = y_0q23 + context->config.dOutOffset_q23;

    // Output limiter
    if (y_1q23 > context->config.limMax_q23)
    {
        y_1q23 = context->config.limMax_q23;
    }
    else if (y_1q23 < context->config.limMin_q23)
    {
        y_1q23 = context->config.limMin_q23;
    }
    else
    {
        // Keep as is
    }

    *dOut_0q23 = y_1q23;
}


void Filter3p3z_DF1_Q23_noinline(
    filter_3p3z_context_df1_q23_t* const context,
    int16_t const dIn0_0q15, int16_t const dIn1_0q15, int32_t* dOut_0q23);

#ifdef __cplusplus
} /* Extern C */
#endif

#endif // ifndef FILTER_3P3Z_DF1_Q23_H__


/* --- End of File ------------------------------------------------ */
