#ifndef COLOR_CONVERT_UTILS_H_INCLUDED
#define COLOR_CONVERT_UTILS_H_INCLUDED

void color_convert_yuv420_to_rgb(unsigned char * output, const unsigned char * input,
                                 unsigned int width, unsigned int height);

#endif
