#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdbool.h>

bool is_crop_valid_for_width_and_height(unsigned int width, unsigned int height,
                                        unsigned int tl_x, unsigned int tl_y,
                                        unsigned int br_x, unsigned int br_y);

#endif
