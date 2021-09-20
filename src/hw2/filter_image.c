#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    for(int c = 0; c<im.c; c++) {
        float sum = 0;
        for(int i = 0; i<im.w*im.h; i++) {
            sum += im.data[c*im.w*im.h + i];
        }
        for(int i = 0; i<im.w*im.h; i++) {
            im.data[c*im.w*im.h + i] /= (sum == 0 ? 0 : sum);
        }
    }
}

image make_box_filter(int w)
{
    image filter = make_image(w, w, 1);
    float v = (float)1.0/(float)(w*w);
    for(int i = 0; i<w*w; i++) filter.data[i] = v;
    return filter;
}

image convolve_image(image im, image filter, int preserve)
{
    // filter can only have 1 channel or same as im
    assert((filter.c == 1) || (filter.c == im.c));

    image out = make_image(im.w, im.h, (preserve == 1 ? im.c : 1));

    int x_pivot = filter.w/2;
    int y_pivot = filter.h/2;

    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            float sum[im.c];
            memset(sum, 0, sizeof(float) * im.c);
            for(int i = 0; i<filter.w; i++) {
                for(int j = 0; j<filter.h; j++) {
                    int px = x - (x_pivot - i);
                    int py = y - (y_pivot - j);
                    for(int z = 0; z<im.c; z++) {
                        sum[z] += get_pixel(im, px, py, z) * 
                                    get_pixel(filter, i, j, (filter.c == im.c ? z : 0));
                    }
                }
            }
            for(int z = 0; z<im.c; z++) {
                float value = sum[z];
                if (preserve != 1) {
                    value += get_pixel(out, x, y, 0);
                }
                set_pixel(out, x, y, (preserve == 1 ? z : 0), value);
            }
        }
    }

    return out;
}

image make_highpass_filter()
{
    image filter = make_image(3, 3, 1);
    float value[9] = {  0, -1,  0, 
                       -1,  4, -1, 
                        0, -1,  0 };
    memcpy(filter.data, value, sizeof(value));
    
    return filter;
}

image make_sharpen_filter()
{
    image filter = make_image(3, 3, 1);
    float value[9] = {  0, -1,  0, 
                       -1,  5, -1, 
                        0, -1,  0 };
    memcpy(filter.data, value, sizeof(value));
    
    return filter;
}

image make_emboss_filter()
{
    image filter = make_image(3, 3, 1);
    float value[9] = { -2, -1,  0, 
                       -1,  1,  1, 
                        0,  1,  2 };
    memcpy(filter.data, value, sizeof(value));
    return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: 
// highpass_filter (laplacian): we should not use preserve on this filter because we only need the gradient information of 
//                                     the image, so better use 1 channel for the output image
// sharpen_filter: we should use preserve on this filter, because sharpen filter will increase the image sharp, so we need
//                 the color information
// emboss_filter: we should not use preserve on this filter, because we only need the color gradient information of the image

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: yes, we need do clamping after do convolution on all filters since there are value > 1 and < 0. this value will make
//         the calculation result above 1 or below 0

image make_gaussian_filter(float sigma)
{
    int w = (int)ceil(sigma * 6);
    // if the calculation result is even, add one
    if (w%2 == 0) ++w;

    int offset = (int)w/2;

    image filter = make_image(w, w, 1);

    for(int x = -offset; x<w - offset; x++) {
        for(int y = -offset; y<w - offset; y++) {
            float value = (1.0/(TWOPI * sigma * sigma)) * exp(-(x*x + y*y)/(2 * sigma * sigma));
            set_pixel(filter, x + offset, y + offset, 0, value);
        }
    }
    l1_normalize(filter);

    return filter;
}

image add_image(image a, image b)
{
    assert(a.w == b.w || a.h == b.h || a.c == b.c);
    
    image out = make_image(a.w, a.h, a.c);

    for(int x = 0; x<a.w; x++) {
        for(int y = 0; y<a.h; y++) {
            for(int z = 0; z<a.c; z++) {
                set_pixel(out, x, y, z, get_pixel(a, x, y, z) + get_pixel(b, x, y, z));
            }
        }
    }

    return out;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w || a.h == b.h || a.c == b.c);
    
    image out = make_image(a.w, a.h, a.c);

    for(int x = 0; x<a.w; x++) {
        for(int y = 0; y<a.h; y++) {
            for(int z = 0; z<a.c; z++) {
                set_pixel(out, x, y, z, get_pixel(a, x, y, z) - get_pixel(b, x, y, z));
            }
        }
    }
    
    return out;
}

image make_gx_filter()
{
    image filter = make_image(3, 3, 1);
    float value[9] = { -1,  0,  1, 
                       -2,  0,  2, 
                       -1,  0,  1 };
    memcpy(filter.data, value, sizeof(value));
    
    return filter;
}

image make_gy_filter()
{
    image filter = make_image(3, 3, 1);
    float value[9] = { -1, -2, -1, 
                        0,  0,  0, 
                        1,  2,  1 };
    memcpy(filter.data, value, sizeof(value));
    
    return filter;
}

void feature_normalize(image im)
{
    float max = im.data[0];
    float min = im.data[0];

    // find maximum and minimum pixel
    for(int i = 1; i<im.w*im.h*im.c; i++) {
        max = MAX(max, im.data[i]);
        min = MIN(min, im.data[i]);
    }

    for(int i = 0; i<im.w*im.h*im.c; i++) {
        im.data[i] = (im.data[i] - min) / (max - min);
    }
}

image sobel_magnitude(image im)
{
    image gx = make_gx_filter();
    image gy = make_gy_filter();

    image out_x = convolve_image(im, gx, 0);
    image out_y = convolve_image(im, gy, 0);

    image out = make_image(im.w, im.h, 1);

    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            float p = get_pixel(out_x, x, y, 0);
            float q = get_pixel(out_y, x, y, 0);
            set_pixel(out, x, y, 0, sqrt(p*p + q*q));
        }
    }

    free_image(gx);
    free_image(gy);
    free_image(out_x);
    free_image(out_y);

    return out;
}

image sobel_orientation(image im)
{
    image gx = make_gx_filter();
    image gy = make_gy_filter();

    image out_x = convolve_image(im, gx, 0);
    image out_y = convolve_image(im, gy, 0);

    image out = make_image(im.w, im.h, 1);

    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            float p = get_pixel(out_x, x, y, 0);
            float q = get_pixel(out_y, x, y, 0);
            set_pixel(out, x, y, 0, atan2(q, p));
        }
    }

    free_image(gx);
    free_image(gy);
    free_image(out_x);
    free_image(out_y);

    return out;
}

image *sobel_image(image im)
{
    image *outs = calloc(2, sizeof(image));
    outs[0] = sobel_magnitude(im);
    outs[1] = sobel_orientation(im);

    return outs;
}

image colorize_sobel(image im)
{
    image *sobel = sobel_image(im);
    feature_normalize(sobel[0]);
    feature_normalize(sobel[1]);
    image out = make_image(im.w, im.h, 3);
    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            // orientation as hue
            set_pixel(out, x, y, 0, get_pixel(sobel[1], x, y, 0));
            // orientation as saturation
            set_pixel(out, x, y, 1, get_pixel(sobel[1], x, y, 0));
            // magnitude as value
            set_pixel(out, x, y, 2, get_pixel(sobel[0], x, y, 0));
        }
    }

    // convert to rgb
    hsv_to_rgb(out);

    free_image(sobel[0]);
    free_image(sobel[1]);
    return out;
}
