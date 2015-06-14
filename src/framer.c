#include <framer.h>

#include <stdlib.h>

struct Framer {
    unsigned int width;
    unsigned int height;
    unsigned int current_tl_x;
    unsigned int current_tl_y;
    unsigned int current_br_x;
    unsigned int current_br_y;
};

/* TODO: Error handling in the create and destroy functions */
struct Framer * framer_create(void)
{
    struct Framer * f = calloc(1, sizeof(* f));

    return f;
}

void framer_destroy(struct Framer * f)
{
    if (f)
        free(f);
}

void framer_set_width_and_height(struct Framer *f, unsigned int width, unsigned int height)
{
    f->width = width;
    f->height = height;
}

void framer_set_full_crop(struct Framer *f)
{
    f->current_tl_x = 0;
    f->current_tl_y = 0;
    f->current_br_x = f->width - 1;
    f->current_br_y = f->height - 1;
}

void framer_get_current_crop(const struct Framer *f, unsigned int *tl_x, unsigned int *tl_y,
                             unsigned int *br_x, unsigned int *br_y)
{
    *tl_x = f->current_tl_x;
    *tl_y = f->current_tl_y;
    *br_x = f->current_br_x;
    *br_y = f->current_br_y;
}
