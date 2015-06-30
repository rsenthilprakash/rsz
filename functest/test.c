#include "color_convert_utils.h"
#include "png_utils.h"
#include <framer.h>
#include <resizer.h>
#include <stdio.h>
#include <stdlib.h>

static void scale_an_active_region_to_full(unsigned char *out, const unsigned char *in,
                                           unsigned int w, unsigned int h,
                                           unsigned int tl_x, unsigned int tl_y,
                                           unsigned int br_x, unsigned int br_y)
{
    struct Resizer * r = resizer_create();
    unsigned char *out_ptr = out;
    const unsigned char *in_ptr = in;

    resizer_set_input_dims(r, w, h, w);
    resizer_set_output_dims(r, w, h, w);

    if (!resizer_validate_and_set_crop(r, tl_x, tl_y, br_x, br_y)) {
        printf("Invalid crop set in resizer\n");
        return;
    }

    resizer_resize_frame(r, out_ptr, in_ptr);

    out_ptr += w * h;
    in_ptr += w * h;

    resizer_set_input_dims(r, w / 2, h / 2, w / 2);
    resizer_set_output_dims(r, w / 2, h / 2, w / 2);

    if (!resizer_validate_and_set_crop(r, tl_x / 2, tl_y / 2, br_x / 2, br_y / 2)) {
        printf("Invalid crop set in resizer\n");
        return;
    }

    resizer_resize_frame(r, out_ptr, in_ptr);

    out_ptr += (w * h) / 4;
    in_ptr += (w * h) / 4;

    resizer_resize_frame(r, out_ptr, in_ptr);

    resizer_destroy(r);
}

static void read_yuv(const char * yuv_file_name, unsigned int w, unsigned int h, unsigned char * buf)
{
    FILE * fp = fopen(yuv_file_name, "rb");

    fread(buf, 1, (w * h * 3) / 2, fp);
    fclose(fp);
}

int main(void)
{
    unsigned int w = 1920;
    unsigned int h = 1080;
    const char * yuv_file_name = "../_images/frame.yuv";
    const char * png_name_first_frame = "../_images/frame_0.png";
    const char * png_name_last_frame = "../_images/frame_final.png";

    unsigned char * in_buffer = malloc((sizeof (*in_buffer) * w * h * 3) / 2);
    unsigned char * int_buffer = malloc((sizeof (*in_buffer) * w * h * 3) / 2);
    unsigned char * out_buffer = malloc(sizeof (*out_buffer) * w * h * 3);

    read_yuv(yuv_file_name, w, h, in_buffer);

    color_convert_yuv420_to_rgb(out_buffer, in_buffer, w, h);

    png_write_color_image(out_buffer, w, h, png_name_first_frame);

    scale_an_active_region_to_full(int_buffer, in_buffer, w, h, 790, 384, 1589, 833);

    color_convert_yuv420_to_rgb(out_buffer, int_buffer, w, h);

    png_write_color_image(out_buffer, w, h, png_name_last_frame);

    free(in_buffer);
    free(int_buffer);
    free(out_buffer);

    return 0;
}
