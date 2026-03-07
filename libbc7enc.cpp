
#include "libbc7enc.h"

#include <stdio.h>

enum bc7enc_error bc7enc_compress_image_from_memory(int width, int height, void* data, rdo_bc::rdo_bc_params params, encode_output* output)
{
	// If we don't 
	if (output == NULL)
		return bc7enc_error_null_output_pointer;

	if (data == NULL)
	{
		return bc7enc_error_null_input_memory;
	}

	// FIXME: Avoid making a complete copy of the input image...
	utils::image_u8 source_image(width, height, (utils::color_quad_u8*)data);

	// FIXME: Make the encoder write directly to a specified memory location
	// We can't take ownership of the data inside the vector so as long as we use it
	// we need to do a copy of the data...
	rdo_bc::rdo_bc_encoder encoder;
	if (!encoder.init(source_image, params))
	{
		return bc7enc_error_could_not_initialize_encoder;
	}

	if (!encoder.encode())
	{
		return bc7enc_error_could_not_encode_image;
	}

	output->width = encoder.get_orig_width();
	output->height = encoder.get_orig_height();
	output->mipmap_count = encoder.get_mip_levels();
	output->format = params.m_dxgi_format;
	output->blocks = (char*)malloc(encoder.get_total_blocks_all_mips() * encoder.get_bytes_per_block());
	if (output->blocks == NULL)
	{
		return bc7enc_error_out_of_memory;
	}
	const void* blocks2 = encoder.get_blocks();
	memcpy(output->blocks, blocks2, encoder.get_total_blocks_all_mips_size_in_bytes());
	output->num_blocks = encoder.get_total_blocks_all_mips();
	output->bytes_per_block = encoder.get_bytes_per_block();
	output->bits_per_pixel = encoder.get_pixel_format_bpp();

	return bc7enc_error_success;
}

enum bc7enc_error bc7enc_compress_image_from_file(const char* image_path, rdo_bc::rdo_bc_params params, encode_output* output)
{
	// If we don't 
	if (output == NULL)
		return bc7enc_error_null_output_pointer;

	if (image_path == NULL || strlen(image_path) == 0)
	{
		return bc7enc_error_no_source_file_name;
	}

	utils::image_u8 source_image;
	if (!load_png(image_path, source_image))
		return bc7enc_error_could_not_load_source_file;

	// FIXME: Make the encoder write directly to a specified memory location
	// We can't take ownership of the data inside the vector so as long as we use it
	// we need to do a copy of the data...
	rdo_bc::rdo_bc_encoder encoder;
	if (!encoder.init(source_image, params))
	{
		return bc7enc_error_could_not_initialize_encoder;
	}

	if (!encoder.encode())
	{
		return bc7enc_error_could_not_encode_image;
	}

	output->width = encoder.get_orig_width();
	output->height = encoder.get_orig_height();
	output->mipmap_count = encoder.get_mip_levels();
	output->format = params.m_dxgi_format;
	output->blocks = (char*)malloc(encoder.get_total_blocks_all_mips() * encoder.get_bytes_per_block());
	if (output->blocks == NULL)
	{
		return bc7enc_error_out_of_memory;
	}
	const void* blocks2 = encoder.get_blocks();
	memcpy(output->blocks, blocks2, encoder.get_total_blocks_all_mips_size_in_bytes());
	output->num_blocks = encoder.get_total_blocks_all_mips();
	output->bytes_per_block = encoder.get_bytes_per_block();
	output->bits_per_pixel = encoder.get_pixel_format_bpp();

	return bc7enc_error_success;
}

void bc7enc_free_encode_output(encode_output* output)
{
	free(output->blocks);
	output->blocks = NULL;
}

bool bc7enc_write_encode_output_to_dds(const char* dds_path, const encode_output* output, bool srgb, bool force_dx10_header)
{
	return utils::save_dds(dds_path, output->width, output->height, output->mipmap_count, output->blocks, output->bits_per_pixel, output->format, srgb, force_dx10_header);
}

const char* bc7enc_error_to_string(bc7enc_error error) {
	switch (error) {
		case bc7enc_error_success:
			return "Success";
		case bc7enc_error_no_source_file_name:
			return "No source file name";
		case bc7enc_error_null_output_pointer:
			return "No output pointer";
		case bc7enc_error_could_not_load_source_file:
			return "Could not load source file";
		case bc7enc_error_could_not_initialize_encoder:
			return "Could not initialize encoder";
		case bc7enc_error_could_not_encode_image:
			return "Could not encode image";
		case bc7enc_error_out_of_memory:
			return "Out of memory";
		case bc7enc_error_null_input_memory:
			return "No input memory";
	}
}