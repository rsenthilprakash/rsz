#ifndef PNG_UTILS_H_INCLUDED
#define PNG_UTILS_H_INCLUDED

#include <stddef.h>
#include <stdbool.h>

bool png_validate_file_and_query_size(size_t *img_width, size_t *img_height, const char *file_name);
void png_read_color_image(unsigned char *img_data, size_t out_width, size_t out_height, const char *file_name);
void png_write_color_image(unsigned char *img_data, size_t width, size_t height, const char *file_name);

#endif
