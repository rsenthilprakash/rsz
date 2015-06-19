#include <framer.h>

#include <stdlib.h>
#include <stdio.h>

static const unsigned int FIX_FAC = 1000;

static const double DEFAULT_SPEED = 2.0;

struct Framer {
    unsigned int width;
    unsigned int height;
    unsigned int current_tl_x;
    unsigned int current_tl_y;
    unsigned int current_br_x;
    unsigned int current_br_y;
    unsigned int dest_tl_x;
    unsigned int dest_tl_y;
    unsigned int dest_br_x;
    unsigned int dest_br_y;
    int tl_x_jump;
    int tl_y_jump;
    int br_x_jump;
    int br_y_jump;
    unsigned int speed;
};

/* TODO: Error handling in the create and destroy functions */
struct Framer * framer_create(void)
{
    struct Framer * f = calloc(1, sizeof(* f));

    f->speed = (unsigned int)(DEFAULT_SPEED * FIX_FAC);
    return f;
}

void framer_destroy(struct Framer * f)
{
    if (f)
        free(f);
}

unsigned int framer_get_fixed_point_precision(void)
{
    return FIX_FAC;
}

void framer_set_width_and_height(struct Framer * f, unsigned int width, unsigned int height)
{
    f->width = width;
    f->height = height;
}

void framer_set_speed_in_fixed_pt(struct Framer * f, unsigned int speed)
{
    f->speed = speed;
}

unsigned int framer_get_speed_in_fixed_pt(const struct Framer * f)
{
    return f->speed;
}

void framer_set_full_crop(struct Framer * f)
{
    f->current_tl_x = 0;
    f->current_tl_y = 0;
    f->current_br_x = (f->width - 1) * FIX_FAC;
    f->current_br_y = (f->height - 1) * FIX_FAC;
}

/* TODO: similar logic sits in resizer.c. Try to consolidate */
static bool is_crop_valid_for_width_and_height(unsigned int width, unsigned int height,
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

bool framer_validate_and_set_current_crop_in_fixed_pt(struct Framer * f, unsigned int tl_x, unsigned int tl_y,
                                                      unsigned int br_x, unsigned int br_y)
{
    bool crop_valid = is_crop_valid_for_width_and_height(f->width * FIX_FAC, f->height * FIX_FAC,
                                                         tl_x, tl_y, br_x, br_y);

    if (crop_valid) {
        f->current_tl_x = tl_x;
        f->current_tl_y = tl_y;
        f->current_br_x = br_x;
        f->current_br_y = br_y;
    }

    return crop_valid;
}

void framer_get_current_crop_in_fixed_pt(const struct Framer * f, unsigned int * tl_x, unsigned int * tl_y,
                                         unsigned int * br_x, unsigned int * br_y)
{
    *tl_x = f->current_tl_x;
    *tl_y = f->current_tl_y;
    *br_x = f->current_br_x;
    *br_y = f->current_br_y;
}

static unsigned int max2(unsigned int a, unsigned int b)
{
    return (a > b) ? a : b;
}

static unsigned int max4(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
    return max2(max2(a, b), max2(c, d));
}

static unsigned int get_max_num_frames_for_speed(int diff_1, int diff_2,
                                                 int diff_3, int diff_4,
                                                 unsigned int speed)
{
    unsigned int max_diff = max4(abs(diff_1), abs(diff_2), abs(diff_3), abs(diff_4));
    /* abs() returns int which will be cast to unsigned while calling max4.
     * Since both int and unsigned int have same rank, conversion to unsigned is implicit.
     * TODO:
     * Does it apply to get_jump_dir_and_compute_diff function below?
     * Without the explicit cast of num_frames to int, would the answer have been unsigned?
     */

    return max_diff / speed;
}

static int get_jump_from_max_num_frames(int diff, unsigned int num_frames)
{
    return diff / (int)num_frames;
}


bool framer_validate_and_set_destination_crop_in_fixed_pt(struct Framer * f, unsigned int tl_x, unsigned int tl_y,
                                                          unsigned int br_x, unsigned int br_y)
{
    bool crop_valid = is_crop_valid_for_width_and_height(f->width * FIX_FAC, f->height * FIX_FAC, tl_x, tl_y, br_x, br_y);

    if (crop_valid) {
        int tl_x_diff = tl_x - f->current_tl_x;
        int tl_y_diff = tl_y - f->current_tl_y;
        int br_x_diff = br_x - f->current_br_x;
        int br_y_diff = br_y - f->current_br_y;
        unsigned int max_num_frames = get_max_num_frames_for_speed(tl_x_diff, tl_y_diff,
                                                                   br_x_diff, br_y_diff,
                                                                   f->speed);

        f->tl_x_jump = get_jump_from_max_num_frames(tl_x_diff, max_num_frames);
        f->tl_y_jump = get_jump_from_max_num_frames(tl_y_diff, max_num_frames);
        f->br_x_jump = get_jump_from_max_num_frames(br_x_diff, max_num_frames);
        f->br_y_jump = get_jump_from_max_num_frames(br_y_diff, max_num_frames);

        f->dest_tl_x = tl_x;
        f->dest_tl_y = tl_y;
        f->dest_br_x = br_x;
        f->dest_br_y = br_y;
    }

    return crop_valid;
}

void framer_compute_current_crop_in_fixed_pt(struct Framer * f)
{
    f->current_tl_x += f->tl_x_jump;
    f->current_tl_y += f->tl_y_jump;
    f->current_br_x += f->br_x_jump;
    f->current_br_y += f->br_y_jump;
}

bool framer_is_destination_reached(const struct Framer * f)
{
    return ((f->current_tl_x == f->dest_tl_x) &&
            (f->current_tl_y == f->dest_tl_y) &&
            (f->current_br_x == f->dest_br_x) &&
            (f->current_br_y == f->dest_br_y));
}
