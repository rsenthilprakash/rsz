#ifndef RESIZER_H_INCLUDED
#define RESIZER_H_INCLUDED

#include <stdbool.h>

struct Resizer;

enum ResizerStatus {
    RESIZER_SUCCESS,
    RESIZER_FAILURE,
};

struct Resizer * resizer_create(void);
void resizer_destroy(struct Resizer * r);

void resizer_set_input_dims(struct Resizer * r, unsigned int width, unsigned int height,
                            unsigned int stride);
void resizer_set_output_dims(struct Resizer * r, unsigned int width, unsigned int height,
                             unsigned int stride);

/* scale factor: for dowlscale is > 1 and for upscale is < 1, invalid: -1 */
double resizer_get_scale_factor(const struct Resizer * r);

bool resizer_validate_and_set_crop(struct Resizer * r, unsigned int tl_x, unsigned int tl_y,
                                   unsigned int br_x, unsigned int br_y);
void resizer_set_full_input_crop(struct Resizer * r);

enum ResizerStatus resizer_resize_frame(const struct Resizer * r, unsigned char * output,
                                        const unsigned char * input);

#endif
