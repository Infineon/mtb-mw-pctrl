/**
 * @file    filter_3p3z_df1_q23.c
 * @brief   Software implementation of a 3-pole 3-zero filter.
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


/* ==============================================================================
 *   1. Include files
 * ==============================================================================
 */

#include "filter_3p3z_df1_q23.h"

/* ==============================================================================
 *   2. Local definitions
 * ==============================================================================
 */


/* ==============================================================================
 *   3. Local types
 * ==============================================================================
 */


/* ==============================================================================
 *   4. Data
 * ==============================================================================
 */


/* ==============================================================================
 *   5. Local function prototypes
 * ==============================================================================
 */


/* ==============================================================================
 *   6. Local functions
 * ==============================================================================
 */


/* ==============================================================================
 *   7. Exported functions
 * ==============================================================================
 */


void Filter3p3zReset_DF1_Q23(filter_3p3z_context_df1_q23_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    /* Resets the internal context */
    context->vars.x_q23[0] = 0;
    context->vars.x_q23[1] = 0;
    context->vars.x_q23[2] = 0;
    context->vars.y_q23[0] = 0;
    context->vars.y_q23[1] = 0;
    context->vars.y_q23[2] = 0;
}


void Filter3p3zInit_DF1_Q23(filter_3p3z_context_df1_q23_t* const context)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);
    cy_pctrl_assert_msg((context != NULL), CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_ERROR,
                        "Context is NULL (uninitialized) in %s!\r\n", __func__);

    // Silences the warning/error about unused variable
    (void)context;

    // Do nothing else in software variant of filter_3p3z
}


void Filter3p3z_DF1_Q23_noinline(
    filter_3p3z_context_df1_q23_t* const context,
    int16_t const dIn0_0q15, int16_t const dIn1_0q15, int32_t* dOut_0q23)
{
    cy_pctrl_log_msg(CYLF_PCTRL_MIDDLEWARE, CY_PCTRL_LOG_DEBUG, "Entered function %s!\r\n", __func__);

    Filter3p3z_DF1_Q23_inline(context, dIn0_0q15, dIn1_0q15, dOut_0q23);
}


/* --- End of File ------------------------------------------------ */
