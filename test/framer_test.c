#include "checks.h"
#include <framer.h>

#include <string.h>
#include <stdbool.h>

static struct Framer * framer;
unsigned int fpp;

#define TO_FP(x) ((unsigned int)((x) * fpp))

void framer_setup(void)
{
    framer = framer_create();
    fpp = framer_get_fixed_point_precision();
}

void framer_cleanup(void)
{
    framer_destroy(framer);
}

TEST(framer, framer_full_crop_has_proper_box)
{
    unsigned int width = 1920;
    unsigned int height = 1080;
    unsigned int tl_x;
    unsigned int tl_y;
    unsigned int br_x;
    unsigned int br_y;

    framer_set_width_and_height(framer, width, height);
    framer_set_full_crop(framer);

    framer_get_current_crop_in_fixed_pt(framer, &tl_x, &tl_y, &br_x, &br_y);

    CHECK_EQUALS_UNSIGNED_INT(0, tl_x);
    CHECK_EQUALS_UNSIGNED_INT(0, tl_y);
    CHECK_EQUALS_UNSIGNED_INT(TO_FP(width - 1), br_x);
    CHECK_EQUALS_UNSIGNED_INT(TO_FP(height - 1), br_y);
}

TEST(framer, set_a_valid_current_crop)
{
    unsigned int width = 1920;
    unsigned int height = 1080;
    unsigned int tl_x;
    unsigned int tl_y;
    unsigned int br_x;
    unsigned int br_y;

    framer_set_width_and_height(framer, width, height);
    framer_set_full_crop(framer);

    CHECK(framer_validate_and_set_current_crop_in_fixed_pt(framer, TO_FP(50), TO_FP(50), TO_FP(1869), TO_FP(1029)) == true);

    framer_get_current_crop_in_fixed_pt(framer, &tl_x, &tl_y, &br_x, &br_y);

    CHECK_EQUALS_UNSIGNED_INT(TO_FP(50), tl_x);
    CHECK_EQUALS_UNSIGNED_INT(TO_FP(50), tl_y);
    CHECK_EQUALS_UNSIGNED_INT(TO_FP(1869), br_x);
    CHECK_EQUALS_UNSIGNED_INT(TO_FP(1029), br_y);
}

TEST(framer, framer_set_destination_crop_and_run_for_a_frame)
{
    unsigned int width = 1920;
    unsigned int height = 1080;
    unsigned int tl_x;
    unsigned int tl_y;
    unsigned int br_x;
    unsigned int br_y;

    framer_set_width_and_height(framer, width, height);
    framer_set_full_crop(framer);

    CHECK(framer_validate_and_set_destination_crop_in_fixed_pt(framer, TO_FP(100), TO_FP(100), TO_FP(1824), TO_FP(1069)) == true);

    framer_compute_current_crop_in_fixed_pt(framer);

    framer_get_current_crop_in_fixed_pt(framer, &tl_x, &tl_y, &br_x, &br_y);

    CHECK_EQUALS_UNSIGNED_INT(TO_FP(2), tl_x);
    CHECK_EQUALS_UNSIGNED_INT(TO_FP(2), tl_y);
    CHECK_EQUALS_UNSIGNED_INT(TO_FP(1917.1), br_x);
    CHECK_EQUALS_UNSIGNED_INT(TO_FP(1078.8), br_y);
}

TEST(framer, framer_set_destination_crop_and_destination_not_reached_in_one_frame)
{
    unsigned int width = 1920;
    unsigned int height = 1080;

    framer_set_width_and_height(framer, width, height);
    framer_set_full_crop(framer);

    CHECK(framer_validate_and_set_destination_crop_in_fixed_pt(framer, TO_FP(100), TO_FP(100), TO_FP(1819), TO_FP(979)) == true);

    framer_compute_current_crop_in_fixed_pt(framer);

    CHECK(framer_is_destination_reached(framer) == false);
}

/* not so useful test */
TEST(framer, framer_set_and_get_some_speed)
{
    unsigned int speed_fp = TO_FP(1.2);

    framer_set_speed_in_fixed_pt(framer, speed_fp);
    CHECK_EQUALS_UNSIGNED_INT(speed_fp, framer_get_speed_in_fixed_pt(framer));
}

TEST(framer, framer_set_destination_crop_and_reach_destination_in_known_number_of_steps)
{
    unsigned int num_frames = 50;
    unsigned int width = 1920;
    unsigned int height = 1080;
    unsigned int speed_fp = framer_get_speed_in_fixed_pt(framer);
    unsigned int offset_fp = num_frames * speed_fp;

    framer_set_width_and_height(framer, width, height);
    framer_set_full_crop(framer);

    CHECK(framer_validate_and_set_destination_crop_in_fixed_pt(framer, offset_fp, offset_fp, TO_FP(width) - offset_fp, TO_FP(height) - offset_fp) == true);

    for (unsigned int i = 0; i < num_frames; i++)
        framer_compute_current_crop_in_fixed_pt(framer);

    CHECK(framer_is_destination_reached(framer) == true);
}
