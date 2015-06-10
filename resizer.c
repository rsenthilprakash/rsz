#include "resizer.h"
#include "resizer_core.h"

#include <string.h>

void resizer_set_input_dims(struct Resizer * r, unsigned int width, unsigned int height)
{
    r->in_width = width;
    r->in_height = height;
}

void resizer_set_output_dims(struct Resizer * r, unsigned int width, unsigned int height)
{
    r->out_width = width;
    r->out_height = height;
}

double resizer_get_scale_factor(const struct Resizer * r)
{
    return (double)r->in_width / r->out_width;
}

bool resizer_validate_and_set_crop(struct Resizer * r, unsigned int tl_x, unsigned int tl_y,
                                   unsigned int br_x, unsigned int br_y)
{
    bool crop_valid = true;

    if (tl_x > r->in_width)
        crop_valid = false;
    else if (br_x > r->in_width)
        crop_valid = false;
    else if (br_x <= tl_x)
        crop_valid = false;
    else if (tl_y > r->in_height)
        crop_valid = false;
    else if (br_y > r->in_height)
        crop_valid = false;
    else if (br_y <= tl_y)
        crop_valid = false;
    else {
        r->in_crop_tl_x = tl_x;
        r->in_crop_tl_y = tl_y;
        r->in_crop_br_x = br_x;
        r->in_crop_br_y = br_y;
    }

    return crop_valid;
}

static void copy_input_to_output(unsigned char *output, const unsigned char *input,
                                 unsigned int width, unsigned int height)
{
    unsigned int i;
    const unsigned char * in_ptr = input;
    unsigned char * out_ptr = output;

    for (i = 0; i < height; i++) {
        memcpy(out_ptr, in_ptr, width);
        in_ptr += width;
        out_ptr += width;
    }
}

void resizer_resize_frame(const struct Resizer * r, unsigned char * output,
                          const unsigned char * input)
{
    if((r->out_width == r->in_width) &&
       (r->out_height == r->in_height)) {
        copy_input_to_output(output, input, r->in_width, r->in_height);
    }
    else {
        scale_planar_fixed(output, input, r->out_width, r->out_height, r->out_width,
                           r->in_width, r->in_height, r->in_width);
    }
}
