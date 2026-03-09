
#pragma once

#include "rdo_bc_encoder.h"
#include "dds_defs.h"
#include "utils.h"

#if defined(_WIN64)
#define EXPORT __declspec(dllexport)
#elif defined(__linux__)
#define EXPORT __attribute__((visibility("default")))
#elif defined(__APPLE__)
#define EXPORT
#endif

extern "C" {

	/** \struct encode_output
	* @brief Struct containing compressed image data.
	*/
	typedef struct encode_output {
		/** @brief The width of the image. */
		int width;
		/** @brief The height of the image. */
		int height;
		/** @brief The number of mipmaps the image contains. */
		int mipmap_count;
		/** @brief The \ref DXGI_FORMAT of the image data. */
		DXGI_FORMAT format;
		/** @brief The compressed image data.  */
		char* blocks;
		/** @brief The number of blocks contained in the \ref blocks pointer. Use \code{c}\ref encode_output.num_blocks * \ref encode_output.bytes_per_block\endcode to calculate the total size of \ref encode_output.blocks */
		int num_blocks;
		/** @brief The number of bytes per compressed block. */
		int bytes_per_block;
		/** @brief The number of bits per pixel for encoded data. (bytes_per_block / 16) * 8 for BC1-7. */
		int bits_per_pixel;
	} encode_output;

	/**
	* \enum bc7enc_error 
	*/
	enum bc7enc_error {
		/** @brief No error occured. */
		bc7enc_error_success = 0,
		/** @brief No source file name was provided. */
		bc7enc_error_no_source_file_name,
		/** @brief No output pointer was provided. */
		bc7enc_error_null_output_pointer,
		/** @brief Could not load the source image. */
		bc7enc_error_could_not_load_source_file,
		/** @brief Could not initialize the encoder. Unsupported format or other. */
		bc7enc_error_could_not_initialize_encoder,
		/** @brief Error occured during comression. */
		bc7enc_error_could_not_encode_image,
		/** @brief Out of memory, could not allocate. */
		bc7enc_error_out_of_memory,
		/** @brief No source memory was provided. */
		bc7enc_error_null_input_memory,
	};

	// FIXME: Compress from memory with prefiltered mipmaps...

	/**
	* @param[in] width The width of the input image
	* @param[in] height The height of the input image
	* @param[in] mip_level The number of mip levels to compress
	* @param[in] data The input image, a pointer to mip_level pointer, each pointer points to max(1, width / (1 << mip)) * max(1, height / (1 << mip)) * 4 bytes in RGBA8 format.
	* @param[in] params The compression parameters to use for the compression
	* @param[out] output The compressed image data
	* @return bc7enc_error_success if the compression succeeded, error code otherwise.
	*/
	EXPORT enum bc7enc_error bc7enc_compress_image_mips_from_memory(int width, int height, int mip_levels, const void** data, rdo_bc::rdo_bc_params params, encode_output* output);

	/**
	* @param[in] width The width of the input image
	* @param[in] height The height of the input image
	* @param[in] data The input image, a pointer to width * height * 4 bytes in RGBA8 format.
	* @param[in] params The compression parameters to use for the compression
	* @param[out] output The compressed image data
	* @return bc7enc_error_success if the compression succeeded, error code otherwise.
	*/
	EXPORT enum bc7enc_error bc7enc_compress_image_from_memory(int width, int height, void* data, rdo_bc::rdo_bc_params params, encode_output* output);

	/**
	* @param[in] image_path A path to an image file to load and compress
	* @param[in] params The compression parameters to use for the compression
	* @param[out] output The compressed image data
	* @return bc7enc_error_success if the compression succeeded, error code otherwise.
	*/
	EXPORT enum bc7enc_error bc7enc_compress_image_from_file(const char* image_path, rdo_bc::rdo_bc_params params, encode_output* output);

	/**
	* @brief Frees the compressed image data stored inside an \ref encode_output structure.
	* @param[in] output The output structure whoes data to free.
	*/
	EXPORT void bc7enc_free_encode_output(encode_output* output);

	/**
	* @brief Write compressed image data to a DirectDraw Surface (DDS) file.
	* @param[in] dds_path The location to save the dds file.
	* @param[in] output The compressed encode output.
	* @param[in] srgb If BC1, BC2, BC3, and BC7 formats should be saved in their sRGB variant.
	* @param[in] If a DX10 header should be forced in the dds file.
	* @returns true if writing the file succeeded, false otherwise.
	*/
	EXPORT bool bc7enc_write_encode_output_to_dds(const char* dds_path, const encode_output* output, bool srgb, bool force_dx10_header);

	/**
	* @brief Converts a \ref bc7enc_error to a string.
	* @param[in] error The bc7enc_error error code.
	* @returns A string describing the error code.
	*/
	EXPORT const char* bc7enc_error_to_string(bc7enc_error error);
}

