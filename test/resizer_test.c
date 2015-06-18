#include <resizer.h>
#include <framer.h>
#include "test_functions.h"
#include "AllTests.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define TEST(a, b) void b(void)
#define IGNORE_TEST(a, b) void b(void)

static size_t failure_count = 0;
static size_t check_count = 0;

#define CHECK(cond)                                                     \
    do {                                                                \
        check_count++;                                                  \
        if (!(cond)) {                                                  \
            printf("\n%s: %d \n",  __FUNCTION__, __LINE__);             \
            printf(#cond " failed\n");                                  \
            failure_count++;                                            \
        }                                                               \
    } while(0)

#define COMPARE_IMAGES(out, exp, w, h)                  \
    do {                                                \
        if (!compare_images_fn(out, exp, w, h)) {       \
            CHECK(false);                               \
            print_image(out, w, h, "Output");           \
            print_image(exp, w, h, "Expected");         \
        }                                               \
        else                                            \
            CHECK(true);                                \
    } while(0)

static struct Resizer * resizer;

void resizer_setup(void)
{
    resizer = resizer_create();
}

void resizer_cleanup(void)
{
    resizer_destroy(resizer);
}

static bool compare_images_fn(const unsigned char * a, const unsigned char * b, unsigned int w, unsigned int h)
{
    unsigned int i;
    const unsigned char * a_ptr = a;
    const unsigned char * b_ptr = b;

    for (i = 0; i < (w * h); i++) {
        if (*a_ptr++ != *b_ptr++)
            return false;
    }

    return true;
}

static void print_image(const unsigned char * img, unsigned int w, unsigned int h, const char *header)
{
    unsigned int i;
    unsigned int j;
    const unsigned char * img_ptr = img;

    printf("\n %s \n", header);
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            printf("%4u ", *img_ptr++);
        }
        printf("\n");
    }
}

TEST(resizer, set_a_valid_input_crop)
{
    resizer_set_input_dims(resizer, 4, 4, 4);
    CHECK(resizer_validate_and_set_crop(resizer, 0, 0, 2, 2) == true);
}

TEST(resizer, set_input_crop_same_as_input_dim)
{
    resizer_set_input_dims(resizer, 4, 4, 4);
    CHECK(resizer_validate_and_set_crop(resizer, 0, 0, 4, 4) == true);
}

TEST(resizer, invalid_input_crop_top_left_x)
{
    resizer_set_input_dims(resizer, 4, 4, 4);
    CHECK(resizer_validate_and_set_crop(resizer, -1, 0, 2, 2) == false);
}

TEST(resizer, invalid_input_crop_top_left_y)
{
    resizer_set_input_dims(resizer, 4, 4, 4);
    CHECK(resizer_validate_and_set_crop(resizer, 0, -1, 2, 2) == false);
}

TEST(resizer, invalid_input_crop_bottom_left_x)
{
    resizer_set_input_dims(resizer, 4, 4, 4);
    CHECK(resizer_validate_and_set_crop(resizer, 0, 0, 5, 2) == false);
}

TEST(resizer, invalid_input_crop_bottom_left_y)
{
    resizer_set_input_dims(resizer, 4, 4, 4);
    CHECK(resizer_validate_and_set_crop(resizer, 0, 0, 2, 5) == false);
}

TEST(resizer, invalid_input_crop_top_left_x_less_than_or_equal_to_bottom_left_x)
{
    resizer_set_input_dims(resizer, 4, 4, 4);
    CHECK(resizer_validate_and_set_crop(resizer, 2, 0, 2, 2) == false);
}

TEST(resizer, invalid_input_crop_top_left_y_less_than_or_equal_to_bottom_left_y)
{
    resizer_set_input_dims(resizer, 4, 4, 4);
    CHECK(resizer_validate_and_set_crop(resizer, 0, 2, 2, 2) == false);
}

TEST(resizer, invalid_scale_factor_for_invalid_input_crop)
{
    resizer_set_input_dims(resizer, 1920, 1080, 1920);
    resizer_set_output_dims(resizer, 1280, 720, 1280);

    CHECK(resizer_get_scale_factor(resizer) == -1);
}

TEST(resizer, test_scale_factor_for_input_output_dimensions)
{
    resizer_set_input_dims(resizer, 1920, 1080, 1920);
    resizer_set_output_dims(resizer, 1280, 720, 1280);
    resizer_set_full_input_crop(resizer);

    CHECK(resizer_get_scale_factor(resizer) == 1.5);
}

TEST(resizer, resizing_without_valid_crop_fails)
{
    unsigned char in_image[] = {1, 1, 1,
                                1, 1, 1,
                                1, 1, 1};
    unsigned char out_image[9];

    resizer_set_input_dims(resizer, 3, 3, 3);
    resizer_set_output_dims(resizer, 3, 3, 3);

    CHECK(resizer_resize_frame(resizer, out_image, in_image) == RESIZER_FAILURE);
}

TEST(resizer, same_in_out_dimensions_does_not_alter_the_image)
{
    unsigned char in_image[] = {1, 2, 3,
                                4, 5, 6,
                                7, 8, 9};
    unsigned char exp_out[] = {1, 2, 3,
                               4, 5, 6,
                               7, 8, 9};
    unsigned char out_image[9];

    resizer_set_input_dims(resizer, 3, 3, 3);
    resizer_set_output_dims(resizer, 3, 3, 3);
    resizer_set_full_input_crop(resizer);

    CHECK(resizer_resize_frame(resizer, out_image, in_image) == RESIZER_SUCCESS);

    COMPARE_IMAGES(out_image, exp_out, 3, 3);
}

TEST(resizer, dc_in_gives_same_dc_out_for_scale_of_3_by_2)
{
    unsigned char in_image[] = {1, 1, 1,
                                1, 1, 1,
                                1, 1, 1};
    unsigned char exp_out[] = {1, 1,
                               1, 1};
    unsigned char out_image[4];

    resizer_set_input_dims(resizer, 3, 3, 3);
    resizer_set_output_dims(resizer, 2, 2, 2);
    resizer_set_full_input_crop(resizer);

    CHECK(resizer_resize_frame(resizer, out_image, in_image) == RESIZER_SUCCESS);

    COMPARE_IMAGES(out_image, exp_out, 2, 2);
}

TEST(resizer, upscale_with_crop_for_scale_of_2_by_3)
{
    unsigned char in_image[] = {1, 1, 1,
                                1, 2, 2,
                                1, 2, 2};
    unsigned char exp_out[] = {2, 2, 2,
                               2, 2, 2,
                               2, 2, 2};
    unsigned char out_image[9];

    resizer_set_input_dims(resizer, 3, 3, 3);
    resizer_set_output_dims(resizer, 3, 3, 3);

    CHECK(resizer_validate_and_set_crop(resizer, 1, 1, 2, 2) == true);
    CHECK(resizer_resize_frame(resizer, out_image, in_image) == RESIZER_SUCCESS);

    COMPARE_IMAGES(out_image, exp_out, 3, 3);
}

TEST(resizer, only_crop_implies_a_copy)
{
    unsigned char in_image[] = {1, 1, 1,
                                1, 2, 3,
                                1, 4, 5};
    unsigned char exp_out[] = {2, 3,
                               4, 5};
    unsigned char out_image[4];

    resizer_set_input_dims(resizer, 3, 3, 3);
    resizer_set_output_dims(resizer, 2, 2, 2);

    CHECK(resizer_validate_and_set_crop(resizer, 1, 1, 2, 2) == true);
    CHECK(resizer_resize_frame(resizer, out_image, in_image) == RESIZER_SUCCESS);

    COMPARE_IMAGES(out_image, exp_out, 2, 2);
}

TEST(resizer, very_simple_downscale_for_scale_of_5_by_3)
{
    unsigned char in_image[] = {1, 3, 3, 4, 4,
                                6, 9, 9, 5, 5,
                                6, 9, 9, 5, 5,
                                4, 7, 7, 8, 8,
                                4, 7, 7, 8, 8};
    unsigned char exp_out[] = {1, 3, 4,
                               6, 9, 5,
                               4, 7, 8};
    unsigned char out_image[9];

    resizer_set_input_dims(resizer, 5, 5, 5);
    resizer_set_output_dims(resizer, 3, 3, 3);
    resizer_set_full_input_crop(resizer);

    CHECK(resizer_resize_frame(resizer, out_image, in_image) == RESIZER_SUCCESS);

    COMPARE_IMAGES(out_image, exp_out, 3, 3);
}

TEST(resizer, not_so_simple_downscale_for_scale_of_5_by_3)
{
    unsigned char in_image[] = {1, 7, 10, 12, 6,
                                2, 9, 2, 2, 4,
                                11, 3, 2, 10, 3,
                                10, 3, 8, 11, 8,
                                1, 1, 3, 5, 2};
    unsigned char exp_out[] = {1, 9, 10,
                               8, 3, 6,
                               7, 5, 8};
    unsigned char out_image[9];

    resizer_set_input_dims(resizer, 5, 5, 5);
    resizer_set_output_dims(resizer, 3, 3, 3);
    resizer_set_full_input_crop(resizer);

    CHECK(resizer_resize_frame(resizer, out_image, in_image) == RESIZER_SUCCESS);

    COMPARE_IMAGES(out_image, exp_out, 3, 3);
}

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

    CHECK(tl_x == 0);
    CHECK(tl_y == 0);
    CHECK(br_x == TO_FP(width - 1));
    CHECK(br_y == TO_FP(height - 1));
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

    CHECK(tl_x == TO_FP(50));
    CHECK(tl_y == TO_FP(50));
    CHECK(br_x == TO_FP(1869));
    CHECK(br_y == TO_FP(1029));
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

    CHECK(tl_x == TO_FP(2));
    CHECK(tl_y == TO_FP(2));
    CHECK(br_x == TO_FP(1917.1));
    CHECK(br_y == TO_FP(1078.8));
}

static void run_all_tests(void)
{
    size_t num_groups = sizeof wrappers / sizeof wrappers[0];
    size_t num_tests = 0;

    for (size_t j = 0; j < num_groups; ++j) {
        struct TestsWrapper *wrapper = wrappers[j];

        for (size_t i = 0; i < wrapper->num_tests; ++i) {
            wrapper->setup();
            wrapper->tests[i]();
            wrapper->cleanup();
            printf(".");
            ++num_tests;
        }
    }

    printf("\nTotal tests: %zu, checks: %zu, failed: %zu \n", num_tests, check_count, failure_count);

    if (!failure_count)
        printf("All tests passed\n");
}

int main(void)
{
    run_all_tests();

    return 0;
}
