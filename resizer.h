#ifndef RESIZER_H_INCLUDED
#define RESIZER_H_INCLUDED

#include <stdbool.h>

struct Resizer {
    unsigned int in_width;
    unsigned int in_height;
    unsigned int out_width;
    unsigned int out_height;
    unsigned int in_crop_tl_x;
    unsigned int in_crop_tl_y;
    unsigned int in_crop_br_x;
    unsigned int in_crop_br_y;
    bool crop_valid;
};

enum ResizerStatus {
    RESIZER_SUCCESS,
    RESIZER_FAILURE,
};

void resizer_init(struct Resizer * r);

void resizer_set_input_dims(struct Resizer * r, unsigned int width, unsigned int height);
void resizer_set_output_dims(struct Resizer * r, unsigned int width, unsigned int height);

/* scale factor: for dowlscale is > 1 and for upscale is < 1 */
double resizer_get_scale_factor(const struct Resizer * r);

bool resizer_validate_and_set_crop(struct Resizer * r, unsigned int tl_x, unsigned int tl_y,
                                   unsigned int br_x, unsigned int br_y);
void resizer_set_full_input_crop(struct Resizer * r);

enum ResizerStatus resizer_resize_frame(const struct Resizer * r, unsigned char * output,
                                        const unsigned char * input);

#endif
