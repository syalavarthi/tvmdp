/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2023 Marvell.
 */

#include <tvmdp.h>

#include <cstdint>
#include <iostream>

#include <tvm/runtime/module.h>

namespace tvmdp
{

/* TVMDP data per each model */
struct tvmdp_model_data {
	/* Model ID */
	uint16_t model_id;

	/* TVM model objects */
	struct tvmdp_model_object object;

	/* Handle to TVM module */
	tvm::runtime::Module *module;
};

struct tvmdp_data {
	/* Pointer to array of model data structures */
	struct tvmdp_model_data *model;

	/* Number of models configured */
	uint16_t nb_models;

	/* TVM Device */
	DLDevice device;
} data;

int
tvmdp_hello(void)
{
	std::cout << "Hello TVMDP!!!" << std::endl;

	return 0;
}

int
tvmdp_configure(uint16_t nb_models)
{
	(void)nb_models;

	return 0;
}

int
tvmdp_close(void)
{

	return 0;
}

int
tvmdp_model_load(uint16_t model_id, void *model_object)
{
	(void)model_id;
	(void)model_object;

	return 0;
}

int
tvmdp_model_unload(uint16_t model_id)
{
	(void)model_id;

	return 0;
}

int
tvmdp_model_metadata_get(uint16_t model_id, void *metadata_addr)
{
	(void)model_id;
	(void)metadata_addr;

	return 0;
}

int
tvmdp_model_run(uint16_t model_id, int32_t num_input, DLTensor *input_tensor, int32_t num_output,
		DLTensor *output_tensor)
{
	(void)model_id;
	(void)num_input;
	(void)input_tensor;
	(void)num_output;
	(void)output_tensor;

	return 0;
}

} // namespace tvmdp
