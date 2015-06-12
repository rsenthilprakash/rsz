#include "resizer_core.h"

#include <stdlib.h>
#include <string.h>

static const unsigned int FIXED_PT_PRECISION_NUM_BITS = 8;
static unsigned int get_fixed_point_precision(void)
{
    return FIXED_PT_PRECISION_NUM_BITS;
}

static inline unsigned char get_data_y(const unsigned char *ptr, unsigned int y)
{
    return *(ptr + y);
}

void scale_planar_fixed(unsigned char *out_ptr, const unsigned char *in,
                        unsigned int out_width, unsigned int out_height, unsigned int out_stride,
                        unsigned int in_width, unsigned int in_height, unsigned int in_stride)
{
    /* TODO: Find out the correct way of defining these */
    const unsigned int FIX_SHIFT = get_fixed_point_precision();
    const unsigned int FIX_UNITY = 1 << FIX_SHIFT;
    const unsigned int FIX_MASK = FIX_UNITY - 1;
    const unsigned int FIX_UNITY_2 = FIX_UNITY / 2;

    unsigned int scale_x = (in_height << FIX_SHIFT) / out_height;
    unsigned int scale_y = (in_width << FIX_SHIFT) / out_width;
    unsigned int i;
    unsigned char *out;
    unsigned int x = 0;
    unsigned char *line_in = calloc(1, (in_width + 1) * 2);
    unsigned char *line_in_0 = line_in;
    unsigned char *line_in_1 = line_in + in_width + 1;

    for (i = 0; i < out_height; i++, x += scale_x) {
        unsigned int fx = x >> FIX_SHIFT;
        unsigned int rx = x & FIX_MASK; /* equal to x - (fx << FIX_SHIFT). i.e residual after rounding */
        unsigned int rx_1 = FIX_UNITY - rx;
        unsigned int j;
        unsigned int y = 0;

        memcpy(line_in_0, in + (fx * in_stride), in_width);
        line_in_0[in_width] = *(in + (fx * in_stride) + in_width - 1);

        if (fx < (in_height - 1)) {
            memcpy(line_in_1, in + ((fx + 1) * in_stride), in_width);
            line_in_1[in_width] = *(in + ((fx + 1) * in_stride) + in_width - 1);
        }
        else
            memcpy(line_in_1, line_in_0, in_width + 1);

        out = out_ptr + (i * out_stride);

        for (j = 0; j < out_width; j++, y += scale_y) {
            unsigned int fy = y >> FIX_SHIFT;
            unsigned int ry = y & FIX_MASK;
            unsigned int ry_1 = FIX_UNITY - ry;

            unsigned char data_11 = get_data_y(line_in_0, fy);
            unsigned char data_21 = get_data_y(line_in_1, fy);
            unsigned int out_data = (data_11 * rx_1 + data_21 * rx) * ry_1;
            unsigned int round_out;

            if (ry) {
                unsigned char data_12 = get_data_y(line_in_0, fy + 1);
                unsigned char data_22 = get_data_y(line_in_1, fy + 1);

                out_data += (data_12 * rx_1 + data_22 * rx) * ry;
            }

            /* Since out_data is product of two fix numbers, need to downshift twice.
             * In the process, first down shift once. Then try to find residual.
             * If residual ir more than (FIX_UNITY / 2) i.e 0.5, add 1 to the second shifted output.
             * In effect try to round off instead of floor
             */
            out_data >>= FIX_SHIFT;
            round_out = ((out_data & FIX_MASK) > FIX_UNITY_2) ? (out_data >> FIX_SHIFT) + 1 : (out_data >> FIX_SHIFT);

            *out++ = (unsigned char)round_out;
        }
    }

    free(line_in);
}
