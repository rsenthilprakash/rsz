#include "png_utils.h"
#include <png.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define PNG_DEBUG 3

static void abort_(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}

bool png_validate_file_and_query_size(size_t *img_width, size_t *img_height, const char *file_name)
{
    bool ret = true;
    size_t width = -1;
    size_t height = -1;
    png_byte color_type;
    png_byte bit_depth;
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned char header[8];    // 8 is the maximum size that can be checked

    FILE *fp = fopen(file_name, "rb");
    if (!fp)
        abort_("[read_png_file] File %s could not be opened for reading", file_name);

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
        abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);

    /* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
        abort_("[read_png_file] png_create_read_struct failed");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        abort_("[read_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[read_png_file] Error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (color_type != PNG_COLOR_TYPE_RGB || bit_depth != 8)
        ret = false;

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);

    *img_width = width;
    *img_height = height;

    return ret;
}

void png_read_color_image(unsigned char *img_data, size_t width, size_t height, const char *file_name)
{
    int i;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep * row_pointers;
    unsigned char header[8];    // 8 is the maximum size that can be checked

    FILE *fp = fopen(file_name, "rb");
    if (!fp)
        abort_("[read_png_color] File %s could not be opened for reading", file_name);

    if (!img_data)
        abort_("[read_png_color] NULL input buffer");
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
        abort_("[] File %s is not recognized as a PNG file", file_name);

    /* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
        abort_("[%s] png_create_read_struct failed");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        abort_("[read_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[read_png_file] Error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    png_read_update_info(png_ptr, info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[read_png_file] Error during read_image");

    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
    for (i = 0; i < height; i++)
        row_pointers[i] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

    png_read_image(png_ptr, row_pointers);

    for (i = 0; i < height; i++) {
        memcpy(img_data + (i * width * 3), row_pointers[i], width * 3);
        free(row_pointers[i]);
    }
    free(row_pointers);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
}

void png_write_color_image(unsigned char *img_data, size_t width, size_t height, const char *file_name)
{
    png_byte bit_depth = 8;
    png_byte color_type = PNG_COLOR_TYPE_RGB;

    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep * row_pointers;
    int i;

    FILE *fp = fopen(file_name, "wb");

    if (!fp)
        abort_("[write_png_file] File could not be opened for writing");

    /* initialize stuff */
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

   if (!png_ptr)
        abort_("[write_png_file] png_create_write_struct failed");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        abort_("[write_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during init_io");

    png_init_io(png_ptr, fp);

    /* write header */
    if (setjmp(png_jmpbuf(png_ptr)))
       abort_("[write_png_file] Error during writing header");

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 bit_depth, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
    for (i = 0; i < height; i++)
        row_pointers[i] = img_data + (i * width * 3);

    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during writing bytes");

    png_write_image(png_ptr, row_pointers);

    /* end write */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during end of write");

    png_write_end(png_ptr, NULL);

    free(row_pointers);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
}
