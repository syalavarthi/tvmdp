/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2023 Marvell.
 */

#ifndef _TVMDP_H_
#define _TVMDP_H_

#include <dlpack/dlpack.h>

#ifdef __cplusplus
#define TVMDP_EXPORT_C extern "C"

#include <cstddef>
#include <cstdint>

/**
 * TVMDP name space
 */
namespace tvmdp
{

#else
#define TVMDP_EXPORT_C

#include <stddef.h>
#include <stdint.h>
#endif

/* Name string length */
#define TVMDP_NAME_STRLEN 32

/* Number of input/outputs per model */
#define TVMDP_INPUT_OUTPUT_MAX 32

/* Maximum number of dimensions of tensor / shape */
#define TVMDP_SHAPE_DIM_MAX 8

/* TVM object / artifact info structure */
struct tvmdp_object {
	/* Name */
	char name[TVMDP_NAME_STRLEN];

	/* Size */
	uint32_t size;

	/* Offset in tar file */
	uint32_t offset;

	/* Address */
	uint8_t *addr;
};

/* TVM Model objects */
struct tvmdp_model_object {
	/* Shared libary object */
	struct tvmdp_object so;

	/* JSON file object */
	struct tvmdp_object json;

	/* Params binary object */
	struct tvmdp_object params;
};

/* TVM metadata: Model section */
struct tvmdp_metadata_model_section {
	/* Model name */
	char name[TVMDP_NAME_STRLEN];

	/* Model version string */
	uint8_t version[TVMDP_NAME_STRLEN];

	/* Number of input tensors */
	int32_t num_input;

	/* Number of output tensors */
	int32_t num_output;
};

/* TVM metadata: I/O section */
struct tvmdp_metadata_io_section {
	/* Name of IO data */
	char name[TVMDP_NAME_STRLEN];

	/* IO format */
	uint8_t format;

	/* Number of dimensions */
	int ndim;

	/* IO shape */
	int64_t shape[TVMDP_SHAPE_DIM_MAX];

	/* Type of incoming / outgoing data */
	DLDataType datatype;

	/* Type of data required by model */
	DLDataType model_datatype;

	/* float_32 scale value
	 * quantized = non-quantized * scale
	 */
	float scale;

	/* TVM device */
	DLDevice device;
};

/* TVM ML Model metadata */
struct tvmdp_model_metadata {
	/* Model section */
	struct tvmdp_metadata_model_section model;

	/* Input section */
	struct tvmdp_metadata_io_section input[TVMDP_INPUT_OUTPUT_MAX];

	/* Output section */
	struct tvmdp_metadata_io_section output[TVMDP_INPUT_OUTPUT_MAX];
};

/* Callback for clock function */
typedef uint64_t (*tvmdp_clock_cb_t)(void);

/** TVMDP Op structure */
struct tvmdp_ml_op {
	/** Number of inputs */
	int32_t num_input;

	/** Input tensor */
	DLTensor *input_tensor;

	/** Number of outputs */
	int32_t num_output;

	/** Output tensor */
	DLTensor *output_tensor;

	/** Pointer to result structure */
	void *result;

	/** Status / poll pointer */
	volatile uint64_t *status;
};

/**
 * TVMDP Hello World!!!
 *
 * @return
 *   0 on success, < 0 on error
 */
TVMDP_EXPORT_C int tvmdp_hello(void);

/**
 * Configure and initialize TVMDP library resources
 *
 * @param[in] nb_models
 *   Numbers of models to be supported by TVMDP
 * @param[in] clock
 *   Callback function for clock
 *
 * @return
 *   0 on success, < 0 on error
 */
TVMDP_EXPORT_C int tvmdp_configure(uint16_t nb_models, tvmdp_clock_cb_t clock);

/**
 * Release TVMDP resources
 *
 * @return
 *   0 on success, < 0 on error
 */
TVMDP_EXPORT_C int tvmdp_close(void);

/**
 * Load a TVM model
 *
 * Allocate TVMDP library internal resources and handles for the model.
 *
 * @param[in] model_id
 *   Model ID assigned by dataplane library
 * @param[in] model_object
 *   Model object structure pointer
 *
 * @return
 *   0 on success, < 0 on error
 */
TVMDP_EXPORT_C int tvmdp_model_load(uint16_t model_id, void *model_object);

/**
 * Unload a TVM model
 *
 * Release resources allocated by library for a model.
 *
 * @param[in] model_id
 *   Model ID assigned by dataplane library
 *
 * @return
 *   0 on success, < 0 on error
 */
TVMDP_EXPORT_C int tvmdp_model_unload(uint16_t model_id);

/**
 * Get metadata information of the model
 *
 * @param[in] model_id
 *   Model ID assigned by dataplane library
 * @param[out] metadata_addr
 *   Pointer to model metadata structure
 *
 * @return
 *   0 on success, < 0 on error
 */
TVMDP_EXPORT_C int tvmdp_model_metadata_get(uint16_t model_id, void *metadata_addr);

/**
 * Run inference for a model
 *
 * @param[in] model_id
 *   Model ID assigned by dataplane library
 * @param[in] op
 *   TVMDP Op handle
 *
 * @return
 *   0 on success, < 0 on error
 */
TVMDP_EXPORT_C void tvmdp_model_run(uint16_t model_id, struct tvmdp_ml_op *op);

#ifdef __cplusplus
}
#endif

#endif /* _TVMDP_H_ */
