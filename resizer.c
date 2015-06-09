#include "resizer.h"

double resizer_get_scale_factor(unsigned int in_width, unsigned int out_width)
{
    return (double)in_width / out_width;
}
