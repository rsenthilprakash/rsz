#include "utils.h"

bool is_crop_valid_for_width_and_height(unsigned int width, unsigned int height,
                                        unsigned int tl_x, unsigned int tl_y,
                                        unsigned int br_x, unsigned int br_y)
{
    bool crop_valid = true;

    if (tl_x > width)
        crop_valid = false;
    else if (br_x > width)
        crop_valid = false;
    else if (br_x <= tl_x)
        crop_valid = false;
    else if (tl_y > height)
        crop_valid = false;
    else if (br_y > height)
        crop_valid = false;
    else if (br_y <= tl_y)
        crop_valid = false;

    return crop_valid;
}
