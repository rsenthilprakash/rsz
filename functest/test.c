#include "color_convert_utils.h"
#include "png_utils.h"

#include <stdio.h>
#include <stdlib.h>

static void read_yuv(const char * yuv_file_name, unsigned int w, unsigned int h, unsigned char * buf)
{
    FILE * fp = fopen(yuv_file_name, "rb");

    fread(in_buffer, 1, (w * h * 3) / 2, fp);
    fclose(fp);
}

int main(void)
{
    unsigned int w = 1920;
    unsigned int h = 1080;
    const char * yuv_file_name = "../_images/frame.yuv";
    const char * png_file_name = "../_images/frame.png";

    unsigned char * in_buffer = malloc((sizeof (*in_buffer) * w * h * 3) / 2);
    unsigned char * out_buffer = malloc(sizeof (*out_buffer) * w * h * 3);

    read_yuv(yuv_file_name, w, h, in_buffer);

    color_convert_yuv420_to_rgb(out_buffer, in_buffer, w, h);

    png_write_color_image(out_buffer, w, h, png_file_name);

    free(in_buffer);
    free(out_buffer);

    return 0;
}
