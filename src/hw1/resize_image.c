#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    return get_pixel(im, round(x), round(y), c);
}

image nn_resize(image im, int w, int h)
{
    image resized_image = make_image(w, h, im.c);
    float x_step = (float)im.w / (float)w;
    float y_step = (float)im.h / (float)h;

    float x_offset = (x_step/2) - 0.5;
    float y_offset = (y_step/2) - 0.5;

    for(int x = 0; x<w; x++) {
        for(int y = 0; y<h; y++) {
            for(int c = 0; c<im.c; c++) {
                set_pixel(resized_image, x, y, c, nn_interpolate(im, x*x_step + x_offset, y*y_step + y_offset, c));
            }
        }
    }

    return resized_image;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    float lower_x = floor(x);
    float upper_x = ceil(x);
    float lower_y = floor(y);
    float upper_y = ceil(y);

    float v1 = get_pixel(im, lower_x, lower_y, c);
    float v2 = get_pixel(im, upper_x, lower_y, c);
    float v3 = get_pixel(im, lower_x, upper_y, c);
    float v4 = get_pixel(im, upper_x, upper_y, c);

    float a1 = (upper_x - x) * (upper_y - y);
    float a2 = (x - lower_x) * (upper_y - y);
    float a3 = (upper_x - x) * (y - lower_y);
    float a4 = (x - lower_x) * (y - lower_y);

    return v1*a1 + v2*a2 + v3*a3 + v4*a4;
}

image bilinear_resize(image im, int w, int h)
{
    image resized_image = make_image(w, h, im.c);
    float x_step = (float)im.w / (float)w;
    float y_step = (float)im.h / (float)h;

    float x_offset = (x_step/2) - 0.5;
    float y_offset = (y_step/2) - 0.5;

    for(int x = 0; x<w; x++) {
        for(int y = 0; y<h; y++) {
            for(int c = 0; c<im.c; c++) {
                set_pixel(resized_image, x, y, c, bilinear_interpolate(im, x*x_step + x_offset, y*y_step + y_offset, c));
            }
        }
    }

    return resized_image;
}

