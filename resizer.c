#include "resizer.h"
#include "resizer_core.h"

#include <string.h>

void resizer_init(struct Resizer * r)
{
    r->crop_valid = false;
}

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

    r->crop_valid = true;

    return crop_valid;
}

void resizer_set_full_input_crop(struct Resizer * r)
{
    r->in_crop_tl_x = 0;
    r->in_crop_tl_y = 0;
    r->in_crop_br_x = r->in_width;
    r->in_crop_br_y = r->in_height;
    r->crop_valid = true;
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

enum ResizerStatus resizer_resize_frame(const struct Resizer * r, unsigned char * output,
                                        const unsigned char * input)
{
    unsigned int in_crop_width;
    unsigned int in_crop_height;

    if (!output || !input)
        return RESIZER_FAILURE;

    if (!r->crop_valid)
        return RESIZER_FAILURE;

    in_crop_width = r->in_crop_br_x - r->in_crop_tl_x;
    in_crop_height = r->in_crop_br_y - r->in_crop_tl_y;

    if((r->out_width == in_crop_width) &&
       (r->out_height == in_crop_height)) {
        copy_input_to_output(output, input, in_crop_width, in_crop_height);
    }
    else {
        scale_planar_fixed(output, input, r->out_width, r->out_height, r->out_width,
                           in_crop_width, in_crop_height, r->in_width);
    }

    return RESIZER_SUCCESS;
}
