#include "color_convert_utils.h"
#include "png_utils.h"
#include <framer.h>
#include <resizer.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned int fpp;

#define TO_FP(x) ((unsigned int)((x) * fpp))

static void scale_an_active_region_to_full(struct Resizer * y_r, struct Resizer * uv_r,
                                           unsigned char *out, const unsigned char *in,
                                           unsigned int w, unsigned int h,
                                           unsigned int tl_x, unsigned int tl_y,
                                           unsigned int br_x, unsigned int br_y)
{
    unsigned char *out_ptr = out;
    const unsigned char *in_ptr = in;

    if (!resizer_validate_and_set_crop(y_r, tl_x, tl_y, br_x, br_y)) {
        printf("Invalid crop set in luma resizer\n");
        return;
    }

    resizer_resize_frame(y_r, out_ptr, in_ptr);

    if (!resizer_validate_and_set_crop(uv_r, tl_x / 2, tl_y / 2, br_x / 2, br_y / 2)) {
        printf("Invalid crop set in resizer\n");
        return;
    }

    out_ptr += w * h;
    in_ptr += w * h;
    resizer_resize_frame(uv_r, out_ptr, in_ptr);

    out_ptr += (w * h) / 4;
    in_ptr += (w * h) / 4;
    resizer_resize_frame(uv_r, out_ptr, in_ptr);
}

static void read_yuv(const char * yuv_file_name, unsigned int w, unsigned int h, unsigned char * buf)
{
    FILE * fp = fopen(yuv_file_name, "rb");

    fread(buf, 1, (w * h * 3) / 2, fp);
    fclose(fp);
}

static void write_yuv(const char * yuv_file_name, unsigned int w, unsigned int h, const unsigned char * buf)
{
    FILE * fp = fopen(yuv_file_name, "ab");

    fwrite(buf, 1, (w * h * 3) / 2, fp);
    fclose(fp);
}

static void clear_file(const char *file_name)
{
    FILE * fp = fopen(file_name, "wb");
    fclose(fp);
}

static void convert_and_write_png(const unsigned char * yuv_buffer, unsigned int w, unsigned int h,
                                 const char * png_name)
{
    unsigned char * rgb_buffer = malloc(sizeof (*rgb_buffer) * w * h * 3);

    color_convert_yuv420_to_rgb(rgb_buffer, yuv_buffer, w, h);
    png_write_color_image(rgb_buffer, w, h, png_name);

    free(rgb_buffer);
}

int main(void)
{
    unsigned int w = 1920;
    unsigned int h = 1080;
    const char * yuv_name_in = "../_images/frame.yuv";
    const char * yuv_name_out = "../_images/frame_out.yuv";
    const char * png_name_first_frame = "../_images/frame_0.png";
    const char * png_name_last_frame = "../_images/frame_final.png";

    unsigned char * in_buffer = malloc((sizeof (*in_buffer) * w * h * 3) / 2);
    unsigned char * int_buffer = malloc((sizeof (*in_buffer) * w * h * 3) / 2);

    struct Framer * f = framer_create();
    struct Resizer * y_r = resizer_create();
    struct Resizer * uv_r = resizer_create();

    fpp = framer_get_fixed_point_precision();

    framer_set_speed_in_fixed_pt(f, TO_FP(5));
    framer_set_width_and_height(f, w, h);
    framer_set_full_crop(f);
    framer_validate_and_set_destination_crop_in_fixed_pt(f, TO_FP(790), TO_FP(384), TO_FP(1589), TO_FP(833));

    resizer_set_input_dims(y_r, w, h, w);
    resizer_set_output_dims(y_r, w, h, w);

    resizer_set_input_dims(uv_r, w / 2, h / 2, w / 2);
    resizer_set_output_dims(uv_r, w / 2, h / 2, w / 2);

    read_yuv(yuv_name_in, w, h, in_buffer);

    convert_and_write_png(in_buffer, w, h, png_name_first_frame);

    clear_file(yuv_name_out);
    write_yuv(yuv_name_out, w, h, in_buffer);

    while (!framer_is_destination_reached(f)) {
        unsigned int tl_x;
        unsigned int tl_y;
        unsigned int br_x;
        unsigned int br_y;

        framer_compute_current_crop_in_fixed_pt(f);

        framer_get_current_crop_in_fixed_pt(f, &tl_x, &tl_y, &br_x, &br_y);

        printf("%u %u %u %u\n", tl_x / fpp, tl_y / fpp, br_x / fpp, br_y / fpp);

        scale_an_active_region_to_full(y_r, uv_r, int_buffer, in_buffer, w, h,
                                       tl_x / fpp, tl_y / fpp, br_x / fpp, br_y / fpp);

        write_yuv(yuv_name_out, w, h, int_buffer);
    }

    convert_and_write_png(int_buffer, w, h, png_name_last_frame);

    free(in_buffer);
    free(int_buffer);

    framer_destroy(f);
    resizer_destroy(y_r);
    resizer_destroy(uv_r);

    return 0;
}
