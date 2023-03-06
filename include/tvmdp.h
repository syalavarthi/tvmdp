/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2023 Marvell.
 */

#ifndef _TVMDP_H_
#define _TVMDP_H_

#include <dlpack/dlpack.h>

#ifdef __cplusplus
#define TVMDP_EXPORT_C extern "C"

/**
 * TVMDP name space
 */
namespace tvmdp
{
#else
#define TVMDP_EXPORT_C
#endif

/**
 * TVMDP Hello World!!!
 *
 * @return
 *   0 on success, < 0 on error
 */
TVMDP_EXPORT_C int tvmdp_hello(void);

#ifdef __cplusplus
}
#endif

#endif /* _TVMDP_H_ */