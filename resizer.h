#ifndef RESIZER_H_INCLUDED
#define RESIZER_H_INCLUDED

struct Resizer {
    unsigned int in_width;
    unsigned int in_height;
    unsigned int out_width;
    unsigned int out_height;
};

void resizer_set_input_dims(struct Resizer * r, unsigned int width, unsigned int height);
void resizer_set_output_dims(struct Resizer * r, unsigned int width, unsigned int height);

double resizer_get_scale_factor(const struct Resizer * r);

void resizer_resize_frame(const struct Resizer * r, unsigned char * output,
                          const unsigned char * input);

#endif
