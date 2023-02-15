/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2023 Marvell.
 */

#include <tvmdp.hpp>

#include <iostream>

namespace tvmdp
{

int
tvmdp_hello(void)
{
	std::cout << "Hello TVMDP!!!" << std::endl;

	return 0;
}

} // namespace tvmdp
