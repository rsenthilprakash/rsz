#ifndef FRAMER_H_INCLUDED
#define FRAMER_H_INCLUDED

#include <stdbool.h>

struct Framer;

struct Framer * framer_create(void);
void framer_destroy(struct Framer *f);

unsigned int framer_get_fixed_point_precision(void);

void framer_set_width_and_height(struct Framer *f, unsigned int width, unsigned int height);
void framer_set_full_crop(struct Framer *f);

bool framer_validate_and_set_current_crop_in_fixed_pt(struct Framer *f, unsigned int tl_x, unsigned int tl_y,
                                                      unsigned int br_x, unsigned int br_y);
void framer_get_current_crop_in_fixed_pt(const struct Framer *f, unsigned int *tl_x, unsigned int *tl_y,
                                         unsigned int *br_x, unsigned int *br_y);

bool framer_validate_and_set_destination_crop_in_fixed_pt(struct Framer *f, unsigned int tl_x, unsigned int tl_y,
                                                          unsigned int br_x, unsigned int br_y);

void framer_compute_current_crop_in_fixed_pt(struct Framer * f);

bool framer_is_destination_reached(const struct Framer * f);

#endif
