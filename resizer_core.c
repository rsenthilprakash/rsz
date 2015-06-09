#include "resizer_core.h"

#include <stdlib.h>
#include <string.h>

static inline unsigned char get_data_y(const unsigned char *ptr, int y)
{
    return *(ptr + y);
}

void scale_planar_fixed(unsigned char *out_ptr, const unsigned char *in,
                        unsigned int out_width, unsigned int out_height, unsigned int out_stride,
                        unsigned int in_width, unsigned int in_height, unsigned int in_stride)
{
    unsigned int scale_x = (in_height << 8) / out_height;
    unsigned int scale_y = (in_width << 8) / out_width;
    unsigned int i;
    unsigned int j;
    unsigned char *out;
    unsigned int x = 0;
    unsigned int y = 0;
    unsigned char *line_in = calloc(1, (in_width + 1) * 2);
    unsigned char *line_in_0 = line_in;
    unsigned char *line_in_1 = line_in + in_width + 1;
    unsigned char data_11, data_12, data_21, data_22;
    unsigned int out_data;

    for (i = 0; i < out_height; i++) {
        int fx = x >> 8;
        int rx = x - (fx << 8);
        int rx_1 = 256 - rx;

        memcpy(line_in_0, in + (fx * in_stride), in_width);
        line_in_0[in_width] = *(in + (fx * in_stride) + in_width - 1);
        if (fx < (in_height - 1)) {
            memcpy(line_in_1, in + ((fx + 1) * in_stride), in_width);
            line_in_1[in_width] = *(in + ((fx + 1) * in_stride) + in_width - 1);
        }
        else {
            memcpy(line_in_1, line_in_0, in_width + 1);
        }

        out = out_ptr + (i * out_stride);

        y = 0;
        for (j = 0; j < out_width; j++) {
            int fy = y >> 8;
            int ry = y & 0xff;
            int ry_1 = 256 - ry;

            data_11 = get_data_y(line_in_0, fy);
            data_21 = get_data_y(line_in_1, fy);
            out_data = (data_11 * rx_1 + data_21 * rx) * ry_1;
            if (ry) {
                data_12 = get_data_y(line_in_0, fy + 1);
                data_22 = get_data_y(line_in_1, fy + 1);
                out_data += (data_12 * rx_1 + data_22 * rx) * ry;
            }
            *out++ = (unsigned char) (out_data >> 16);

            y += scale_y;
        }
        x += scale_x;
    }
    free(line_in);
}
