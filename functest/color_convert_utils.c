#include "color_convert_utils.h"

static double YR = 1.0;
static double UR = 0.0;
static double VR = 1.399;
static double YG = 1.0;
static double UG = -0.344;
static double VG = -0.714;
static double YB = 1.0;
static double UB = 1.772;
static double VB = 0.0;

void color_convert_yuv420_to_rgb(unsigned char * output, const unsigned char * input,
                                 unsigned int width, unsigned int height)
{
    const unsigned char * y_ptr = input;
    const unsigned char * u_ptr = y_ptr + (width * height);
    const unsigned char * v_ptr = u_ptr + (width * height) / 4;
    unsigned char * out_ptr = output;
    unsigned int i;
    unsigned int j;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            int y = (int) (*y_ptr++);
            int u = (int) (*u_ptr - 128);
            int v = (int) (*v_ptr - 128);

            int r = YR * y + UR * u + VR * v;
            int g = YG * y + UG * u + VG * v;
            int b = YB * y + UB * u + VB * v;

            if (r > 255) r = 255;
            if (r < 0) r = 0;
            if (g > 255) g = 255;
            if (g < 0) g = 0;
            if (b > 255) b = 255;
            if (b < 0) b = 0;

            *out_ptr++ = (unsigned char) r;
            *out_ptr++ = (unsigned char) g;
            *out_ptr++ = (unsigned char) b;

            if (j % 2) {
                u_ptr++;
                v_ptr++;
            }
        }

        if (!(i % 2)) {
            u_ptr -= width / 2;
            v_ptr -= width / 2;
        }
    }
}
