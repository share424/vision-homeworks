#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include "image.h"

bool fix_pixel(image im, int *x, int *y, int *c)
{
    *x = min(max(*x, 0), im.w - 1);
    *y = min(max(*y, 0), im.h - 1);
    *c = min(max(*c, 0), im.c - 1);
}

int coord_to_index(image im, int x, int y, int c)
{
    return ((y * im.w) + x) + (c * im.w * im.h);
}

float get_pixel(image im, int x, int y, int c)
{
    fix_pixel(im, &x, &y, &c);
    return im.data[coord_to_index(im, x, y, c)];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    fix_pixel(im, &x, &y, &c);
    im.data[coord_to_index(im, x, y, c)] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // pop quiz: if the image is 256x256x3, how many total pixels are there?
    // answer: 65536 pixels

    // method 0 (fastest)
    memcpy(copy.data, im.data, sizeof(float)*im.w*im.h*im.c);

    // method 1
    // for(int i = 0; i<im.w*im.h*im.c; i++) copy.data[i] = im.data[i];

    // method 2
    // for(int x = 0; x<im.w; x++) {
    //     for(int y = 0; y<im.h; y++) {
    //         for(int c = 0; c<im.c; c++) {
    //             set_pixel(copy, x, y, c, get_pixel(im, x, y, c));
    //         }
    //     }
    // }
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            float r = get_pixel(im, x, y, 0);
            float g = get_pixel(im, x, y, 1);
            float b = get_pixel(im, x, y, 2);
            set_pixel(gray, x, y, 0, (0.299 * r + 0.587 * g + 0.114 * b));
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            float pixel = get_pixel(im, x, y, c);
            set_pixel(im, x, y, c, pixel + v);
        }
    }
}

void clamp_image(image im)
{
    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            for(int c = 0; c<im.c; c++) {
                float pixel = get_pixel(im, x, y, c);
                set_pixel(im, x, y, c, max(min(pixel, 1), 0));
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    float r, g, b, value, saturation, hue, m, ha, c;
    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            r = get_pixel(im, x, y, 0);
            g = get_pixel(im, x, y, 1);
            b = get_pixel(im, x, y, 2);
            value = three_way_max(r, g, b);
            m = three_way_min(r, g, b);
            c = value - m;
            saturation = (value == 0 ? 0 : c / value);

            if (c == 0) {
                ha = 0;
            } else if (value == r) {
                ha = (g - b) / c;
            } else if (value == g) {
                ha = ((b - r) / c) + 2;
            } else {
                ha = ((r - g) / c) + 4;
            }

            hue = ha / 6;
            if (ha < 0) {
                hue += 1;
            }

            set_pixel(im, x, y, 0, hue);
            set_pixel(im, x, y, 1, saturation);
            set_pixel(im, x, y, 2, value);
        }
    }
}


void hsv_to_rgb(image im)
{
    float r, g, b, value, saturation, hue, m, ha, c, v;
    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            hue = get_pixel(im, x, y, 0);
            saturation = get_pixel(im, x, y, 1);
            value = get_pixel(im, x, y, 2);

            int h = ceil(hue * 360);

            c = value * saturation;
            m = value - c;
            ha = h/60;
            v = c * (1 - abs((int)floor(ha)%2 - 1));

            if (ha < 1) {
                r = c;
                g = v;
                b = 0;
            } else if (ha < 2) {
                r = v;
                g = c;
                b = 0;
            } else if (ha < 3) {
                r = 0;
                g = c;
                b = v;
            } else if (ha < 4) {
                r = 0;
                g = v;
                b = c;
            } else if (ha < 5) {
                r = v;
                g = 0;
                b = c;
            } else {
                r = c;
                g = 0;
                b = v;
            }

            r += m;
            g += m;
            b += m;

            set_pixel(im, x, y, 0, r);
            set_pixel(im, x, y, 1, g);
            set_pixel(im, x, y, 2, b);
        }
    }
}

void scale_image(image im, int c, float v)
{
    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            float pixel = get_pixel(im, x, y, c);
            set_pixel(im, x, y, c, pixel * v);
       }
    }
}