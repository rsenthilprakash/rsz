#include "resizer.h"
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

static struct Resizer resizer;

static void setup(void)
{
    memset(&resizer, 0, sizeof (resizer));
    resizer_init(&resizer);
}

static void cleanup(void)
{
}

static bool compare_images_fn(const unsigned char * a, const unsigned char * b, unsigned int w, unsigned int h)
{
    unsigned int i;
    unsigned int j;
    const unsigned char * a_ptr = a;
    const unsigned char * b_ptr = b;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            if (*a_ptr++ != *b_ptr++)
                return false;
        }
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

TEST(dummy, set_a_valid_input_crop)
{
    resizer_set_input_dims(&resizer, 4, 4);
    CHECK(resizer_validate_and_set_crop(&resizer, 0, 0, 2, 2) == true);
}

TEST(dummy, set_input_crop_same_as_input_dim)
{
    resizer_set_input_dims(&resizer, 4, 4);
    CHECK(resizer_validate_and_set_crop(&resizer, 0, 0, 4, 4) == true);
}

TEST(dummy, invalid_input_crop_top_left_x)
{
    resizer_set_input_dims(&resizer, 4, 4);
    CHECK(resizer_validate_and_set_crop(&resizer, -1, 0, 2, 2) == false);
}

TEST(dummy, invalid_input_crop_top_left_y)
{
    resizer_set_input_dims(&resizer, 4, 4);
    CHECK(resizer_validate_and_set_crop(&resizer, 0, -1, 2, 2) == false);
}

TEST(dummy, invalid_input_crop_bottom_left_x)
{
    resizer_set_input_dims(&resizer, 4, 4);
    CHECK(resizer_validate_and_set_crop(&resizer, 0, 0, 5, 2) == false);
}

TEST(dummy, invalid_input_crop_bottom_left_y)
{
    resizer_set_input_dims(&resizer, 4, 4);
    CHECK(resizer_validate_and_set_crop(&resizer, 0, 0, 2, 5) == false);
}

TEST(dummy, invalid_input_crop_top_left_x_less_than_or_equal_to_bottom_left_x)
{
    resizer_set_input_dims(&resizer, 4, 4);
    CHECK(resizer_validate_and_set_crop(&resizer, 2, 0, 2, 2) == false);
}

TEST(dummy, invalid_input_crop_top_left_y_less_than_or_equal_to_bottom_left_y)
{
    resizer_set_input_dims(&resizer, 4, 4);
    CHECK(resizer_validate_and_set_crop(&resizer, 0, 2, 2, 2) == false);
}

TEST(dummy, invalid_scale_factor_for_invalid_input_crop)
{
    resizer_set_input_dims(&resizer, 1920, 1080);
    resizer_set_output_dims(&resizer, 1280, 720);

    CHECK(resizer_get_scale_factor(&resizer) == -1);
}

TEST(dummy, test_scale_factor_for_input_output_dimensions)
{
    resizer_set_input_dims(&resizer, 1920, 1080);
    resizer_set_output_dims(&resizer, 1280, 720);
    resizer_set_full_input_crop(&resizer);

    CHECK(resizer_get_scale_factor(&resizer) == 1.5);
}

TEST(dummy, resizing_without_valid_crop_fails)
{
    unsigned char in_image[] = {1, 1, 1,
                                1, 1, 1,
                                1, 1, 1};
    unsigned char out_image[9];

    resizer_set_input_dims(&resizer, 3, 3);
    resizer_set_output_dims(&resizer, 3, 3);

    CHECK(resizer_resize_frame(&resizer, out_image, in_image) == RESIZER_FAILURE);
}

TEST(dummy, same_in_out_dimensions_does_not_alter_the_image)
{
    unsigned char in_image[] = {1, 1, 1,
                                1, 1, 1,
                                1, 1, 1};
    unsigned char exp_out[] = {1, 1, 1,
                               1, 1, 1,
                               1, 1, 1};
    unsigned char out_image[9];

    resizer_set_input_dims(&resizer, 3, 3);
    resizer_set_output_dims(&resizer, 3, 3);
    resizer_set_full_input_crop(&resizer);

    CHECK(resizer_resize_frame(&resizer, out_image, in_image) == RESIZER_SUCCESS);

    COMPARE_IMAGES(out_image, exp_out, 3, 3);
}

TEST(dummy, dc_in_gives_same_dc_out_for_scale_of_3_by_2)
{
    unsigned char in_image[] = {1, 1, 1,
                                1, 1, 1,
                                1, 1, 1};
    unsigned char exp_out[] = {1, 1,
                               1, 1};
    unsigned char out_image[4];

    resizer_set_input_dims(&resizer, 3, 3);
    resizer_set_output_dims(&resizer, 2, 2);
    resizer_set_full_input_crop(&resizer);

    CHECK(resizer_resize_frame(&resizer, out_image, in_image) == RESIZER_SUCCESS);

    COMPARE_IMAGES(out_image, exp_out, 2, 2);
}


static void run_all_tests(void)
{
    size_t total = sizeof test_functions / sizeof test_functions[0];

    for (size_t i = 0; i < total; ++i) {
        setup();
        test_functions[i]();
        cleanup();
        printf(".");
    }

    printf("\nTotal tests: %zu, checks: %zu, failed: %zu \n", total, check_count, failure_count);

    if (!failure_count)
        printf("All tests passed\n");
}

int main(void)
{
    run_all_tests();

    return 0;
}
