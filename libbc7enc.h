
#pragma once

#include "rdo_bc_encoder.h"
#include "dds_defs.h"

#if defined(_WIN64)
#define EXPORT __declspec(dllexport)
#elif defined(__linux__)
#define EXPORT __attribute__((visibility("default")))
#elif defined(__APPLE__)
#define EXPORT
#endif

extern "C" {

	typedef struct encode_output {
		int width;
		int height;
		int mipmap_count;
		DXGI_FORMAT format;
		char* blocks;
		int num_blocks;
		int bytes_per_block;
		int bits_per_pixel;
	} encode_output;

	enum bc7enc_error {
		bc7enc_error_success = 0,
		bc7enc_error_no_source_file_name,
		bc7enc_error_null_output_pointer,
		bc7enc_error_could_not_load_source_file,
		bc7enc_error_could_not_initialize_encoder,
		bc7enc_error_could_not_encode_image,
		bc7enc_error_out_of_memory,
		bc7enc_error_null_input_memory,
	};

	EXPORT enum bc7enc_error bc7enc_compress_image_from_memory(int width, int height, void* data, rdo_bc::rdo_bc_params params, encode_output* output);

	EXPORT enum bc7enc_error bc7enc_compress_image_from_file(const char* image_path, rdo_bc::rdo_bc_params params, encode_output* output);

	EXPORT void bc7enc_free_encode_output(encode_output* output);

	EXPORT bool bc7enc_write_encode_output_to_dds(const char* dds_path, const encode_output* output, bool srgb, bool force_dx10_header);

	EXPORT const char* bc7enc_error_to_string(bc7enc_error error);

}

