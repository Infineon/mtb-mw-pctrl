/**
 * @file    mtb_mw_pctrl.h
 * @brief   Aggregated header file for the ModusToolbox Power Control Middleware library.
 *          This file works as a single-include point for the whole library.
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

#ifndef MTB_MW_PCTRL_H__
#define MTB_MW_PCTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \page page_mtb_mw_pctrl Using the Library
 *
 * \section sct_mtb_mw_pctrl_including_the_library Including the library
 * The recommended way to add the **Power Control Middleware Library** to your project is through
 * the **ModusToolbox Library Manager**. Open the Library Manager from the IDE or by running
 * 'make modlibs' from a ModusToolbox shell terminal, search for **mtb-mw-pctrl**, and add it
 * to your project. The Library Manager will handle downloading and configuring the library
 * dependency automatically.
 *
 * Alternatively, you can manually create a dependency file **deps/mtb-mw-pctrl.mtb** to include
 * the **Power Control Middleware Library** into the project, and run 'make getlibs' from a ModusToolbox
 * shell terminal in your project to install the dependency.
 *
 * The **Power Control Middleware Library** can be used in an application source file by including the header file:
 * \snippet code_snippets snippet_using_the_pctrl_mw_lib
 *
 * \section sct_mtb_mw_pctrl_blocks Functional Blocks
 * The library provides the following functional blocks and implementation variants:
 *
 * <table>
 * <tr><th>Block Name</th><th>Variant</th><th>Description</th></tr>
 * <tr><td>filter_3p3z</td><td>DF1-Q23 (default)</td>
 *     <td>Bit-accurate emulation of HWFILTER peripheral from PSoC Control
 *     Performance line MCUs for portability and simulation.</td></tr>
 * <tr><td>filter_3p3z</td><td>Hardware</td>
 *     <td>Hardware-accelerated implementation using HWFILTER peripheral
 *     on PSoC Control Performance line MCUs.</td></tr>
 * <tr><td>filter_3p3z</td><td>DF2-F32</td>
 *     <td>Software implementation in Direct Form 2 using single-precision
 *     floating-point for MCUs without HWFILTER peripheral.</td></tr>
 * <tr><td>ac_rms_pll</td><td>Three phase (default)</td>
 *     <td>Standard SRF-PLL for balanced three-phase systems using
 *     alpha-beta frame inputs.</td></tr>
 * <tr><td>ac_rms_pll</td><td>Single phase</td>
 *     <td>SRF-PLL with SOGI for quadrature signal generation in
 *     single-phase systems. Supports DC offset and harmonic rejection.</td></tr>
 * <tr><td>ac_rms_pll</td><td>Three phase double decoupling</td>
 *     <td>DDSRF-PLL for superior phase tracking under unbalanced grid
 *     conditions and voltage sags.</td></tr>
 * <tr><td>mppt</td><td>Perturb and observe (default)</td>
 *     <td>Duty-cycle based P&amp;O algorithm for maximum power point tracking
 *     in photovoltaic and other variable power sources.</td></tr>
 * </table>
 *
 * Each block provides multiple implementation variants optimized for different use cases
 * (e.g., hardware-accelerated, software-portable, precision vs. performance trade-offs).
 * See @ref sct_mtb_mw_pctrl_config for variant selection details.
 * Before including the library, the variants can be configured. Otherwise, the default variant is used.
 *
 * \section sct_mtb_mw_pctrl_config Configuration
 * \brief **Library configuration and variant selection**
 * <br>
 * Each functional block provides multiple implementation variants to suit different requirements:
 * - **Hardware-accelerated variants**: Utilize MCU-specific peripherals for optimal performance
 * - **Software variants**: Portable implementations for simulation or targets without hardware acceleration
 * - **Specialized variants**: Optimized for specific use cases (e.g., single-phase vs. three-phase PLLs)
 *
 * Unless otherwise stated in the block description, all variants of a block share the same API,
 * allowing seamless switching between variants without application code changes.
 *
 * Library configuration is done project-wide using a user configuration header file
 * (see @ref sct_mtb_mw_pctrl_user_config). This ensures consistent behavior across
 * all compilation units in your application.
 *
 * The following table lists the available configuration options:
 *
 * <table>
 * <tr><th>Block</th><th>Macro name</th><th>Default</th><th>Purpose</th></tr>
 * <tr><td>filter_3p3z</td><td>FILTER_3P3Z_USE_VARIANT_DF1_Q23</td>
 *     <td>defined</td><td>Use software variant (DF1-Q23)</td></tr>
 * <tr><td>filter_3p3z</td><td>FILTER_3P3Z_USE_VARIANT_DF2_F32</td>
 *     <td>not defined</td><td>Use software variant (DF2-float32)</td></tr>
 * <tr><td>filter_3p3z</td><td>FILTER_3P3Z_USE_VARIANT_HW</td>
 *     <td>not defined</td><td>Use hardware-accelerated variant</td></tr>
 * <tr><td>filter_3p3z</td><td>FILTER_3P3Z_USE_INLINE</td>
 *     <td>not defined</td><td>Use function inlining</td></tr>
 * <tr><td>filter_3p3z</td>
 *     <td>FILTER_3P3Z_USE_VARIANT_DF1_Q23_HW_PPCA_BEHAVIOR</td>
 *     <td>not defined</td>
 *     <td>Enable bit-accurate HW emulation (DF1-Q23 only)</td></tr>
 * <tr><td>ac_rms_pll</td><td>AC_RMS_PLL_USE_VARIANT_1PH</td>
 *     <td>not defined</td><td>Use single phase variant</td></tr>
 * <tr><td>ac_rms_pll</td><td>AC_RMS_PLL_USE_VARIANT_3PH</td>
 *     <td>defined</td><td>Use three phase variant</td></tr>
 * <tr><td>ac_rms_pll</td><td>AC_RMS_PLL_USE_VARIANT_3PHDD</td>
 *     <td>not defined</td>
 *     <td>Use three phase double-decoupling variant</td></tr>
 * <tr><td>ac_rms_pll</td><td>AC_RMS_PLL_USE_INLINE</td>
 *     <td>not defined</td><td>Use function inlining</td></tr>
 * <tr><td>ac_rms_pll</td><td>AC_RMS_PLL_FREQ_FILTER</td>
 *     <td>not defined</td><td>Enable frequency low-pass filter</td></tr>
 * <tr><td>ac_rms_pll</td><td>AC_RMS_PLL_RMS_FILTER</td>
 *     <td>not defined</td><td>Enable RMS low-pass filter</td></tr>
 * <tr><td>ac_rms_pll</td><td>AC_RMS_PLL_D_AXIS_ALIGN</td>
 *     <td>not defined</td>
 *     <td>Align PLL to d-axis instead of q-axis</td></tr>
 * <tr><td>ac_rms_pll</td><td>AC_RMS_PLL_USE_CMSIS_DSP_LIB</td>
 *     <td>not defined</td>
 *     <td>Use ARM CMSIS DSP library for math operations</td></tr>
 * <tr><td>ac_rms_pll</td><td>AC_RMS_PLL_3PHDD_USE_CTRL_3P3Z_HW</td>
 *     <td>not defined</td>
 *     <td>Use hardware 3P3Z controller (3PHDD only)</td></tr>
 * <tr><td>ac_rms_pll</td><td>ACRMSPLL_DCOFFSET_REJECT</td>
 *     <td>not defined</td>
 *     <td>Enable DC offset rejection (1PH only)</td></tr>
 * <tr><td>ac_rms_pll</td><td>ACRMSPLL_3RD_HARMONIC_REJECT</td>
 *     <td>not defined</td>
 *     <td>Enable 3rd harmonic rejection (1PH only)</td></tr>
 * <tr><td>ac_rms_pll</td><td>ACRMSPLL_5TH_HARMONIC_REJECT</td>
 *     <td>not defined</td>
 *     <td>Enable 5th harmonic rejection (1PH only)</td></tr>
 * <tr><td>mppt</td><td>MPPT_USE_VARIANT_PNO</td>
 *     <td>defined</td><td>Use Perturb and Observe variant</td></tr>
 * <tr><td>mppt</td><td>MPPT_USE_INLINE</td>
 *     <td>not defined</td><td>Use function inlining</td></tr>
 * </table>
 *
 * Without defining the macros for variant selection, the default variants are used.
 *
 * \section sct_mtb_mw_pctrl_user_config User configuration file
 * <br>
 * Implementation variants are selected project-wide in the user configuration file
 * (mtb_mw_pctrl_user_config.h) as a centralized location. This file must be placed
 * in the include path of the application project. It is included by the library to
 * apply configurations that deviate from the default values.
 *
 * For using the filter software variant as an inline function, the user config file should contain
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_df1_q23_inlined
 *
 * Alternatively, for using the filter hardware variant, the user config file should contain
 * \snippet code_snippets snippet_mw_pctrl_user_config_variant_hw
 */

/* ==============================================================================
 *   1. PCTRL MW Library configuration
 * ==============================================================================
 */

#include "mtb_mw_pctrl_config.h"

/* ==============================================================================
 *   2. Include files
 * ==============================================================================
 */

/* Header file aggregation for the Power Control Middleware library.
 */
#include "filter_3p3z/filter_3p3z.h"
#include "ac_rms_pll/ac_rms_pll.h"
#include "mppt/mppt.h"

/* ==============================================================================
 *   3. Definitions
 * ==============================================================================
 */


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

#endif // MTB_MW_PCTRL_H__


/* --- End of File ------------------------------------------------ */
