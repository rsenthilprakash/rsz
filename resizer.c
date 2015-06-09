#include "resizer.h"

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

void resizer_resize_frame(const struct Resizer * r, unsigned char * output,
                          const unsigned char * input)
{
    unsigned int i;
    unsigned int j;
    const unsigned char * in_ptr = input;
    unsigned char * out_ptr = output;

    for (i = 0; i < r->out_height; i++) {
        for (j = 0; j < r->out_width; j++) {
            *out_ptr++ = *in_ptr++;
        }
    }
}
