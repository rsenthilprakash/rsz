#include <resizer.h>
#include "resizer_core.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

struct Resizer {
    unsigned int in_width;
    unsigned int in_height;
    unsigned int in_stride;
    unsigned int out_width;
    unsigned int out_height;
    unsigned int out_stride;
    unsigned int in_crop_tl_x;
    unsigned int in_crop_tl_y;
    unsigned int in_crop_br_x;
    unsigned int in_crop_br_y;
    bool crop_valid;
};

/* TODO: Error handling in the create and destroy functions */
struct Resizer * resizer_create(void)
{
    struct Resizer * r = calloc(1, sizeof(* r));
    r->crop_valid = false;

    return r;
}

void resizer_destroy(struct Resizer * r)
{
    if (r)
        free(r);
}

void resizer_set_input_dims(struct Resizer * r, unsigned int width, unsigned int height,
                            unsigned int stride)
{
    r->in_width = width;
    r->in_height = height;
    r->in_stride = stride;
}

void resizer_set_output_dims(struct Resizer * r, unsigned int width, unsigned int height,
                             unsigned int stride)
{
    r->out_width = width;
    r->out_height = height;
    r->out_stride = stride;
}

bool resizer_validate_and_set_crop(struct Resizer * r, unsigned int tl_x, unsigned int tl_y,
                                   unsigned int br_x, unsigned int br_y)
{
    bool crop_valid = is_crop_valid_for_width_and_height(r->in_width, r->in_height,
                                                         tl_x, tl_y, br_x, br_y);
    if (crop_valid) {
        r->in_crop_tl_x = tl_x;
        r->in_crop_tl_y = tl_y;
        r->in_crop_br_x = br_x;
        r->in_crop_br_y = br_y;
    }

    r->crop_valid = crop_valid;

    return crop_valid;
}

void resizer_set_full_input_crop(struct Resizer * r)
{
    r->in_crop_tl_x = 0;
    r->in_crop_tl_y = 0;
    r->in_crop_br_x = r->in_width - 1;
    r->in_crop_br_y = r->in_height - 1;
    r->crop_valid = true;
}

double resizer_get_scale_factor(const struct Resizer * r)
{
    double scale_factor;
    unsigned int in_crop_width;

    if (!r->crop_valid)
        scale_factor = -1;
    else {
        in_crop_width = r->in_crop_br_x - r->in_crop_tl_x + 1;
        scale_factor = (double)in_crop_width / r->out_width;
    }

    return scale_factor;
}

static void copy_input_to_output(unsigned char * output, const unsigned char * input,
                                 unsigned int width, unsigned int height,
                                 unsigned int out_stride, unsigned int in_stride)
{
    unsigned int i;
    const unsigned char * in_ptr = input;
    unsigned char * out_ptr = output;

    for (i = 0; i < height; i++) {
        memcpy(out_ptr, in_ptr, width);
        in_ptr += in_stride;
        out_ptr += out_stride;
    }
}

enum ResizerStatus resizer_resize_frame(const struct Resizer * r, unsigned char * output,
                                        const unsigned char * input)
{
    unsigned int in_crop_width;
    unsigned int in_crop_height;
    const unsigned char *in_crop_buf;

    if (!output || !input)
        return RESIZER_FAILURE;

    if (!r->crop_valid)
        return RESIZER_FAILURE;

    in_crop_width = r->in_crop_br_x - r->in_crop_tl_x + 1;
    in_crop_height = r->in_crop_br_y - r->in_crop_tl_y + 1;
    in_crop_buf = input + (r->in_crop_tl_y * r->in_width) + r->in_crop_tl_x;

    if((r->out_width == in_crop_width) &&
       (r->out_height == in_crop_height)) {
        copy_input_to_output(output, in_crop_buf, in_crop_width, in_crop_height, r->out_stride, r->in_stride);
    }
    else {
        scale_planar_fixed(output, in_crop_buf, r->out_width, r->out_height, r->out_stride,
                           in_crop_width, in_crop_height, r->in_stride);
    }

    return RESIZER_SUCCESS;
}
