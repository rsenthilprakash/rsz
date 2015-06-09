#ifndef RESIZER_CORE_H_INCLUDED
#define RESIZER_CORE_H_INCLUDED

void scale_planar_fixed(unsigned char *out_ptr, const unsigned char *in,
                        unsigned int out_width, unsigned int out_height, unsigned int out_stride,
                        unsigned int in_width, unsigned int in_height, unsigned int in_stride);

#endif
