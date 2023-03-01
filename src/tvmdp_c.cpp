/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2023 Marvell.
 */

#include "tvmdp.h"
#include "tvmdp.hpp"

#ifdef __cplusplus
extern "C" {
#endif

int
tvmdp_hello(void)
{
	return tvmdp::tvmdp_hello();
}

#ifdef __cplusplus
}
#endif
