#ifndef FRAMER_H_INCLUDED
#define FRAMER_H_INCLUDED

struct Framer;

struct Framer * framer_create(void);
void framer_destroy(struct Framer *f);

void framer_set_width_and_height(struct Framer *f, unsigned int width, unsigned int height);
void framer_set_full_crop(struct Framer *f);

void framer_get_current_crop(const struct Framer *f, unsigned int *tl_x, unsigned int *tl_y,
                             unsigned int *br_x, unsigned int *br_y);
#endif
