/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2023 Marvell.
 */

#include <tvmdp.h>

#include <cstdlib>
#include <iostream>

#include <linux/limits.h>
#include <sys/mman.h>
#include <unistd.h>

#include <jansson.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/registry.h>

/* Shared memory file descriptor name */
#define ML_MODEL_SHMFD_NAME "tvmdp_shmfd"

/* Shared memory file descriptor path */
#define ML_MODEL_SHMFD_PATH "/proc/%d/fd/%d"

namespace tvmdp
{

/* TVMDP data per each model */
struct tvmdp_model_data {
	/* Device handle */
	uint8_t *device;

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

	/* Clock callback function */
	tvmdp_clock_cb_t clock;
} data;

/* Inference stats
 *
 * Units of start and end are dependent on the tvmdp_clock_cb_t.
 */
struct tvmdp_ml_stats {
	/* Start */
	uint64_t start;

	/* Start */
	uint64_t end;
};

/* Inference Result structure
 *
 * Structure to store inference results.
 * A structure with same fields is defined in dataplane library / driver.
 */
struct tvmdp_ml_result {
	/* Job error code */
	uint64_t error_code;

	/* Inference stats */
	struct tvmdp_ml_stats stats;

	/* User context pointer */
	void *user_ptr;
};

int
tvmdp_hello(void)
{
	std::cout << "Hello TVMDP!!!" << std::endl;

	return 0;
}

int
tvmdp_configure(uint16_t nb_models, tvmdp_clock_cb_t clock)
{
	/* Check for configuration status */
	if (data.model != nullptr) {
		std::cout << "Device already configured" << std::endl;
		return 0;
	}

	/* Check for nb_models value */
	if (nb_models == 0) {
		std::cerr << "Invalid arguments, nb_models=" << nb_models << std::endl;
		return -EINVAL;
	}

	/* Allocate memory for model data */
	data.model = reinterpret_cast<struct tvmdp_model_data *>(
		malloc(nb_models * sizeof(struct tvmdp_model_data)));
	if (data.model == nullptr) {
		std::cerr << "Memory allocation failed for data" << std::endl;
		return -ENOMEM;
	}
	data.nb_models = nb_models;
	data.clock = clock;

	/* Reset memory */
	memset(data.model, 0, sizeof(struct tvmdp_model_data) * nb_models);

	/* Initialize the device */
	data.device = DLDevice{kDLCPU, 0};

	return 0;
}

int
tvmdp_close(void)
{
	uint16_t i;

	/* Check configuration status */
	if (data.model == nullptr) {
		std::cerr << "TVMDP not configured" << std::endl;
		return -EINVAL;
	}

	/* Check for active models */
	for (i = 0; i < data.nb_models; i++) {
		if (data.model[i].module != NULL) {
			std::cerr << "Model still in use, model_id = " << i << std::endl;
			return -EINVAL;
		}
	}

	/* Free memory */
	free(data.model);

	/* Reset the pointer */
	data.model = NULL;

	return 0;
}

int
tvmdp_model_load(void *device, uint16_t model_id, void *model_object,
		 struct tvmrt_glow_callback *tvmrt_glow_cb)
{
	struct tvmdp_model_object *object;
	tvm::runtime::Module module_so;
	tvm::runtime::Module module_ge;

	char str[PATH_MAX];
	int fd;

	(void)tvmrt_glow_cb;

	/* Check configuration status */
	if (data.model == nullptr) {
		std::cerr << "TVMDP not configured" << std::endl;
		return -EINVAL;
	}

	/* Check if model is loaded */
	if (data.model[model_id].module != NULL) {
		std::cerr << "Model is already loaded, model_id = " << model_id << std::endl;
		return -EINVAL;
	}

	/* Check for arguments */
	if (model_object == nullptr) {
		std::cerr << "Invalid arguments, model_object = NULL" << std::endl;
		return -EINVAL;
	}

	/* Create Shared memory file descriptor */
	snprintf(str, PATH_MAX, "%s_%d_%d", ML_MODEL_SHMFD_NAME, getpid(), model_id);
	fd = memfd_create(str, 0);
	if (fd < 0) {
		std::cerr << "Failed to create shared memory descriptor object" << std::endl;
		return -errno;
	}

	/* Writing buffer data to shared memory fd */
	object = reinterpret_cast<struct tvmdp_model_object *>(model_object);
	if (write(fd, object->so.addr, object->so.size) < 0) {
		std::cerr << "Failed to write to shared memory object" << std::endl;
		close(fd);
		return -errno;
	}

	lseek(fd, SEEK_SET, 0);

	/* Loading *.so file */
	snprintf(str, PATH_MAX, ML_MODEL_SHMFD_PATH, getpid(), fd);
	module_so = tvm::runtime::Module::LoadFromFile(str, "so");

	/* Calling Graph executor */
	module_ge = (*tvm::runtime::Registry::Get("tvm.graph_executor.create"))(
		(char *)object->json.addr, module_so, (int)data.device.device_type,
		data.device.device_id);

	data.model[model_id].module = new tvm::runtime::Module(module_ge);

	/* Copy model object structure */
	memcpy(&data.model[model_id].object, object, sizeof(struct tvmdp_model_object));
	data.model[model_id].model_id = model_id;
	data.model[model_id].device = (uint8_t *)device;

	return 0;
}

int
tvmdp_model_unload(uint16_t model_id)
{
	/* Check configuration status */
	if (data.model == nullptr) {
		std::cerr << "TVMDP not configured" << std::endl;
		return -EINVAL;
	}

	/* Check if model is loaded */
	if (data.model[model_id].module == NULL) {
		std::cerr << "Model is not loaded, model_id = " << model_id << std::endl;
		return -EINVAL;
	}

	/* Delete module */
	delete data.model[model_id].module;

	/* Reset handle */
	data.model[model_id].module = NULL;

	return 0;
}

int
tvmdp_model_metadata_get(uint16_t model_id, void *metadata_addr)
{
	tvm::runtime::Map<tvm::runtime::String, tvm::runtime::NDArray> Map;
	tvm::runtime::ShapeTuple shape;
	tvm::runtime::NDArray io_array;
	tvm::runtime::Module *module;
	TVMByteArray params;

	json_error_t json_error;
	json_t *json_parsed;
	json_t *json_nodes;
	json_t *json_array;
	json_t *json_name;
	json_t *json_op;

	struct tvmdp_model_metadata *metadata;
	struct tvmdp_model_object object;
	DLDataType dtype;
	int flag = 0;
	int i;

	/* Check configuration status */
	if (data.model == nullptr) {
		std::cerr << "TVMDP not configured" << std::endl;
		return -EINVAL;
	}

	/* Check if model is loaded */
	if (data.model[model_id].module == NULL) {
		std::cerr << "Model is not loaded, model_id = " << model_id << std::endl;
		return -EINVAL;
	}

	object = data.model[model_id].object;
	module = data.model[model_id].module;

	/* Load JSON */
	json_parsed = json_loadb((const char *)object.json.addr, object.json.size, 0, &json_error);
	if (!json_parsed) {
		fprintf(stderr, "error: on line %d: %s\n", json_error.line, json_error.text);
		return -1;
	}

	/* Loading params data */
	params.data = reinterpret_cast<const char *>(object.params.addr);
	params.size = object.params.size;
	module->GetFunction("load_params")(params);

	Map = (*tvm::runtime::Registry::Get("runtime.LoadParams"))(params);

	json_nodes = json_object_get(json_parsed, "nodes");
	if (json_nodes == NULL) {
		std::cerr << "Failed parsing JSON nodes" << std::endl;
		return -errno;
	}

	/* Get number of inputs */
	metadata = reinterpret_cast<struct tvmdp_model_metadata *>(metadata_addr);
	for (i = 0; i < ((int)json_array_size(json_nodes)); i++) {
		json_array = json_array_get(json_nodes, i);
		json_op = json_object_get(json_array, "op");

		if (strcmp(json_string_value(json_op), "null") == 0) {
			json_name = json_object_get(json_array, "name");
			flag = 0;

			for (auto &p : Map) {
				if (strcmp(p.first.c_str(), json_string_value(json_name)) == 0) {
					flag = 1;
					break;
				}
			}

			if (flag == 0) {
				strcpy(metadata->input[metadata->model.num_input].name,
				       json_string_value(json_name));
				metadata->model.num_input++;
			}
		}
	}

	/* Get number of outputs */
	metadata->model.num_output = module->GetFunction("get_num_outputs")();

	/* Set model name */
	snprintf(metadata->model.name, TVMDP_NAME_STRLEN, "%s_%u", "Model", model_id);

	/* Set Model datatype */
	dtype = DLDataType{kDLFloat, 32, 1};

	/* Get input shape */
	for (i = 0; i < metadata->model.num_input; i++) {
		io_array = module->GetFunction("get_input")(metadata->input[i].name);
		shape = io_array.Shape();
		metadata->input[i].ndim = 0;

		for (auto &p : shape) {
			metadata->input[i].shape[metadata->input[i].ndim] = p;
			metadata->input[i].ndim++;
		}
		/* Set device type and device id */
		metadata->input[i].device = data.device;
		metadata->input[i].datatype = dtype;
		metadata->input[i].model_datatype = dtype;
		metadata->input[i].scale = 1.0;
		metadata->input[i].format = 0;
	}

	/* Get output shape */
	for (i = 0; i < metadata->model.num_output; i++) {
		io_array = module->GetFunction("get_output")(i);
		shape = io_array.Shape();
		metadata->output[i].ndim = 0;

		for (auto &p : shape) {
			metadata->output[i].shape[metadata->output[i].ndim] = p;
			metadata->output[i].ndim++;
		}
		/* Set device type and device id */
		metadata->output[i].device = data.device;
		metadata->output[i].datatype = dtype;
		metadata->output[i].model_datatype = dtype;
		metadata->output[i].scale = 1.0;
		metadata->output[i].format = 0;
	}

	return 0;
}

int
tvmdp_model_metadata_get_stage1(void *buffer, size_t size, void *metadata_addr)
{
	struct tvmdp_model_metadata *metadata = (struct tvmdp_model_metadata *)metadata_addr;

	json_error_t json_error;
	json_t *json_parsed;
	json_t *json_nodes;
	json_t *json_array;
	json_t *json_obj;

	int i;

	/* Load JSON */
	json_parsed = json_loadb((const char *)buffer, size, 0, &json_error);
	if (!json_parsed) {
		std::cerr << "error: on line" << json_error.line << json_error.text;
		return -1;
	}

	json_nodes = json_object_get(json_parsed, "nodes");
	if (json_nodes == NULL) {
		std::cerr << "Failed parsing JSON nodes" << std::endl;
		return -errno;
	}

	for (i = 0; i < ((int)json_array_size(json_nodes)); i++) {
		json_array = json_array_get(json_nodes, i);
		json_obj = json_object_get(json_array, "op");

		if (strcmp(json_string_value(json_obj), "tvm_op") == 0) {
			/* Get the Layer Name */
			json_obj = json_object_get(json_array, "name");
			strcpy(metadata->model.layer[metadata->model.nb_layers].name,
			       json_string_value(json_obj));

			/* capture layer type */
			json_obj = json_object_get(json_array, "attrs");
			json_obj = json_object_get(json_obj, "Compiler");
			if (json_obj == NULL) {
				/* For LLVM models */
				strcpy(metadata->model.layer[metadata->model.nb_layers].type,
				       "LLVM");
			} else {
				strcpy(metadata->model.layer[metadata->model.nb_layers].type,
				       json_string_value(json_obj));
			}

			metadata->model.nb_layers++;
		}
	}

	return 0;
}

int
tvmdp_model_metadata_get_stage2(uint16_t model_id, void *metadata_addr)
{

	return tvmdp_model_metadata_get(model_id, metadata_addr);
}

void
tvmdp_model_run(uint16_t model_id, int32_t num_input, DLTensor *input_tensor, int32_t num_output,
		DLTensor *output_tensor, void *result, volatile uint64_t *status)
{
	struct tvmdp_ml_result *ml_result;
	tvm::runtime::Module *module;
	int i = 0;

	module = data.model[model_id].module;
	ml_result = (struct tvmdp_ml_result *)result;

	ml_result->stats.start = data.clock();
	for (i = 0; i < num_input; i++)
		module->GetFunction("set_input_zero_copy")(i, &input_tensor[i]);

	for (i = 0; i < num_output; i++)
		module->GetFunction("set_output_zero_copy")(i, &output_tensor[i]);

	module->GetFunction("run")();
	ml_result->error_code = 0x0;
	ml_result->stats.end = data.clock();
	*(volatile uint64_t *)status = 0x1;
}

} // namespace tvmdp
