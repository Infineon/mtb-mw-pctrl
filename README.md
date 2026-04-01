# ModusToolbox&trade; Power Control Middleware Library

## Overview

**Power Control Middleware Library** provides the optimized and reusable functional blocks for building power topology applications in ModusToolbox&trade;. The library offers both software and hardware-accelerated implementations of common power control algorithms, enabling developers to create efficient systems with minimal code changes when switching between variants.

## Quick start

1. Include the library in your ModusToolbox&trade; project
2. Add `#include "mtb_mw_pctrl.h"` to your source files
3. Create `mtb_mw_pctrl_user_config.h` and add configuration options (e.g., variant selection)
4. Initialize block contexts for the functional blocks to be used

For a detailed usage guide, refer to [Using Power Control Middleware Library](https://infineon.github.io/mtb-mw-pctrl/html/page_mtb_mw_pctrl.html).

## API documentation

[API documentation](https://infineon.github.io/mtb-mw-pctrl/html/index.html)


## Functional blocks

The following table shows the functional blocks and implementation variants included in this library.

| Block name     | Variant | Description 
| :------------- | :------ | :---------- 
| **filter_3p3z** | DF1-Q23 (default) | Software implementation in Direct Form 1 using Q0.23 fixed-point arithmetic. Bit-accurate emulation of HWFILTER peripheral from PSOC Control Performance line MCUs for portability.
| | Hardware | Hardware-accelerated implementation using HWFILTER peripheral on PSOC Control Performance line MCUs for optimal performance.
| | DF2-F32 | Software implementation in Direct Form 2 using single-precision floating-point for ease of use on MCUs that do not support HWFILTER peripheral.
| **ac_rms_pll** | Three phase (default) | Standard SRF-PLL for balanced three-phase systems using alpha-beta frame inputs.
| | Single phase | SRF-PLL with SOGI (Second Order Generalized Integrator) for quadrature signal generation in single phase systems. Supports DC offset and harmonic rejection.
| | Three phase double decoupling | SRF-PLL with double decoupling network for superior phase tracking in three phase systems under unbalanced grid conditions and voltage sags.
| **mppt** | Perturb and observe (default)| Maximum Power Point Tracking controller implementing a duty-cycle based perturb and observe algorithm for photovoltaic and other variable power sources.
<br>

## Architecture

### Block context

Each functional block uses a context structure that contains:
- **Configuration parameters:** Block-specific settings and coefficients  
- **Internal state:** Runtime memory for filters, accumulators, and intermediate values
- **Variant configuration:** Hardware-specific settings when applicable

The application is responsible for:
- Allocating context structures
- Initializing contexts before first use
- Managing context lifetime and cleanup

**Example:**
```c
// Declare context
filter_3p3z_context_t filter_ctx;

// Initialize context
Filter3p3zInit(&filter_ctx);

// Use in application
Filter3p3z(&filter_ctx, input0, input1, &output);
```

### Multi-instantiation

Library modules are reentrant and thread-safe. They do not use any local static or global non-constant data and do not call any other non-reentrant function. Unless otherwise stated in the block documentation, the context structure of the same library block may be instantiated multiple times. The execution of two instances of the same library block on the same core may interrupt each other (preemption). Additionally, parallel execution of two instances of the same library block on different cores (multi-thread) is allowed.

### Variant management

Most of the library blocks implement more than one variant. Variants may offer different functionality options, may be pure software implementations, or may use available hardware accelerators.

**Variant selection options**
Variants are selected by defining certain configuration macros by one of the options:

1. **Global selection** (in Makefile):
   ```Makefile
   DEFINES += FILTER_3P3Z_USE_VARIANT_HW
   ```

2. **User configuration file** (`mtb_mw_pctrl_user_config.h`):
   ```c
   #define FILTER_3P3Z_USE_VARIANT_HW
   ```

The names of such macros and the block variant that is default can be found in the block documentation.

### Resource management

Besides the available memory, there is no limitation on the number of instantiations of context for a given library block. However, some library blocks provide variants that make use of specific hardware functionality. Such variants are tied to MCU targets that implement this functionality in hardware. Multi-instantiation of such block variants is possible as long as there are enough hardware resources available. The Power Control Middleware Library does not implement any resource management: no measures are taken to prevent the same hardware from being used by multiple instances of the library blocks. Resource management is up to the application.
Context structures hold both the configuration and runtime data for a block. Application using the library initiates, manages and owns block data. 

## Troubleshooting

Issue | Possible cause | Solution 
:---- | :------------- | :-------
Compilation errors with hardware variants | Target does not support hardware acceleration | Use software variant or check device capabilities 
Unexpected results | Uninitialized context | Ensure init function is called after initializing block context 
Resource conflicts | Multiple instances using same context or hardware instance | Implement application-level resource management 

<br>

## More information

For more information, refer to the following documents:

* [ModusToolbox&trade; software environment, quick start guide, documentation, and videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software)
* [Infineon Technologies AG](https://www.infineon.com)


---
© 2026, Infineon Technologies AG, or an affiliate of Infineon Technologies AG. All rights reserved.
This software, associated documentation and materials ("Software") is owned by Infineon Technologies AG or one of its affiliates ("Infineon") and is protected by and subject to worldwide patent protection, worldwide copyright laws, and international treaty provisions. Therefore, you may use this Software only as provided in the license agreement accompanying the software package from which you obtained this Software. If no license agreement applies, then any use, reproduction, modification, translation, or compilation of this Software is prohibited without the express written permission of Infineon.
Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A SPECIFIC USE/PURPOSE OR MERCHANTABILITY. Infineon reserves the right to make changes to the Software without notice. You are responsible for properly designing, programming, and testing the functionality and safety of your intended application of the Software, as well as complying with any legal requirements related to its use. Infineon does not guarantee that the Software will be free from intrusion, data theft or loss, or other breaches ("Security Breaches"), and Infineon shall have no liability arising out of any Security Breaches. Unless otherwise explicitly approved by Infineon, the Software may not be used in any application where a failure of the Product or any consequences of the use thereof can reasonably be expected to result in personal injury.
