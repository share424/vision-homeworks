#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#include "matrix.h"
#include <time.h>

// Frees an array of descriptors.
// descriptor *d: the array.
// int n: number of elements in array.
void free_descriptors(descriptor *d, int n)
{
    int i;
    for(i = 0; i < n; ++i){
        free(d[i].data);
    }
    free(d);
}

// Create a feature descriptor for an index in an image.
// image im: source image.
// int i: index in image for the pixel we want to describe.
// returns: descriptor for that index.
descriptor describe_index(image im, int i)
{
    int w = 5;
    descriptor d;
    d.p.x = i%im.w;
    d.p.y = i/im.w;
    d.data = calloc(w*w*im.c, sizeof(float));
    d.n = w*w*im.c;
    int c, dx, dy;
    int count = 0;
    // If you want you can experiment with other descriptors
    // This subtracts the central value from neighbors
    // to compensate some for exposure/lighting changes.
    for(c = 0; c < im.c; ++c){
        float cval = im.data[c*im.w*im.h + i];
        for(dx = -w/2; dx < (w+1)/2; ++dx){
            for(dy = -w/2; dy < (w+1)/2; ++dy){
                float val = get_pixel(im, i%im.w+dx, i/im.w+dy, c);
                d.data[count++] = cval - val;
            }
        }
    }
    return d;
}

// Marks the spot of a point in an image.
// image im: image to mark.
// ponit p: spot to mark in the image.
void mark_spot(image im, point p)
{
    int x = p.x;
    int y = p.y;
    int i;
    for(i = -9; i < 10; ++i){
        set_pixel(im, x+i, y, 0, 1);
        set_pixel(im, x, y+i, 0, 1);
        set_pixel(im, x+i, y, 1, 0);
        set_pixel(im, x, y+i, 1, 0);
        set_pixel(im, x+i, y, 2, 1);
        set_pixel(im, x, y+i, 2, 1);
    }
}

// Marks corners denoted by an array of descriptors.
// image im: image to mark.
// descriptor *d: corners in the image.
// int n: number of descriptors to mark.
void mark_corners(image im, descriptor *d, int n)
{
    int i;
    for(i = 0; i < n; ++i){
        mark_spot(im, d[i].p);
    }
}

// Creates a 1d Gaussian filter.
// float sigma: standard deviation of Gaussian.
// returns: single row image of the filter.
image make_1d_gaussian(float sigma)
{
    int w = (int)ceil(sigma * 6);
    // if the calculation result is even, add one
    if (w%2 == 0) ++w;
    int offset = (int)w/2;
    image filter = make_image(w, 1, 1);

    for(int x = -offset; x<w - offset; x++) {
        float value = (1.0/(TWOPI * sigma * sigma)) * exp(-(x*x)/(2 * sigma * sigma));
        filter.data[x + offset] = value;
    }

    return filter;
}

// Smooths an image using separable Gaussian filter.
// image im: image to smooth.
// float sigma: std dev. for Gaussian.
// returns: smoothed image.
image smooth_image(image im, float sigma)
{
    // if(1){
    //     image g = make_gaussian_filter(sigma);
    //     image s = convolve_image(im, g, 1);
    //     free_image(g);
    //     return s;
    // } else {
        // TODO: optional, use two convolutions with 1d gaussian filter.
        // If you implement, disable the above if check.
        image filter = make_1d_gaussian(sigma);
        image out = convolve_image(im, filter, 1);
        // flip the filter
        image h_filter = make_image(1, filter.w, 1);
        for(int y = 0; y<filter.w; y++) {
            set_pixel(h_filter, 0, y, 0, get_pixel(filter, y, 0, 0));
        }
        out = convolve_image(out, h_filter, 1);
        free_image(filter);
        free_image(h_filter);
        return out;
    // }
}

// Calculate the structure matrix of an image.
// image im: the input image.
// float sigma: std dev. to use for weighted sum.
// returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
//          third channel is IxIy.
image structure_matrix(image im, float sigma)
{
    image S = make_image(im.w, im.h, 3);

    image x_filter = make_gx_filter();
    image y_filter = make_gy_filter();

    image im_x = convolve_image(im, x_filter, 0);
    image im_y = convolve_image(im, y_filter, 0);

    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            float px = get_pixel(im_x, x, y, 0);
            float py = get_pixel(im_y, x, y, 0);
            set_pixel(S, x, y, 0, px*px);
            set_pixel(S, x, y, 1, py*py);
            set_pixel(S, x, y, 2, px*py);
        }
    }

    image filter = make_gaussian_filter(sigma);

    S = convolve_image(S, filter, 1);

    return S;
}

// Estimate the cornerness of each pixel given a structure matrix S.
// image S: structure matrix for an image.
// returns: a response map of cornerness calculations.
image cornerness_response(image S)
{
    image R = make_image(S.w, S.h, 1);
    // TODO: fill in R, "cornerness" for each pixel using the structure matrix.
    // We'll use formulation det(S) - alpha * trace(S)^2, alpha = .06.
    for(int x = 0; x<S.w; x++) {
        for(int y = 0; y<S.h; y++) {
            // first channel is Ix^2
            // second channel is Iy^2
            // third channel is IxIy
            float a = get_pixel(S, x, y, 0);
            float b = get_pixel(S, x, y, 2);
            float c = get_pixel(S, x, y, 2);
            float d = get_pixel(S, x, y, 1);

            // calculate matrix 2x2 determinant
            float det = a*d - b*c;
            // calculate matrix 2x2 trace
            float trace = a + d;

            set_pixel(R, x, y, 0, det - 0.06 * trace * trace);
        }
    }
    return R;
}

// Perform non-max supression on an image of feature responses.
// image im: 1-channel image of feature responses.
// int w: distance to look for larger responses.
// returns: image with only local-maxima responses within w pixels.
image nms_image(image im, int w)
{
    image r = copy_image(im);
    // TODO: perform NMS on the response map.
    // for every pixel in the image:
    //     for neighbors within w:
    //         if neighbor response greater than pixel response:
    //             set response to be very low (I use -999999 [why not 0??])
    int wf = 2*w + 1;

    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            for(int i = 0; i<wf; i++) {
                for(int j = 0; j<wf; j++) {
                    float neighbor = get_pixel(im, x - (w - i), y - (w - j), 0);
                    float pixel = get_pixel(im, x, y, 0);
                    if (neighbor > pixel) {
                        set_pixel(r, x, y, 0, -999999);
                    }
                }
            }
        }
    }
    return r;
}

// Perform harris corner detection and extract features from the corners.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
// int *n: pointer to number of corners detected, should fill in.
// returns: array of descriptors of the corners in the image.
descriptor *harris_corner_detector(image im, float sigma, float thresh, int nms, int *n)
{
    // Calculate structure matrix
    image S = structure_matrix(im, sigma);

    // Estimate cornerness
    image R = cornerness_response(S);

    // Run NMS on the responses
    image Rnms = nms_image(R, nms);


    //TODO: count number of responses over threshold
    int count = 0; // change this
    for(int i = 0; i<Rnms.w*Rnms.h; i++) {
        if (Rnms.data[i] >= thresh) {
            count++;
        }
    }

    
    *n = count; // <- set *n equal to number of corners in image.
    descriptor *d = calloc(count, sizeof(descriptor));
    //TODO: fill in array *d with descriptors of corners, use describe_index.
    int idx = 0;
    for(int i = 0; i<Rnms.w*Rnms.h; i++) {
        if (Rnms.data[i] >= thresh) {
            d[idx++] = describe_index(im, i);
        }
    }


    free_image(S);
    free_image(R);
    free_image(Rnms);
    return d;
}

// Find and draw corners on an image.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
void detect_and_draw_corners(image im, float sigma, float thresh, int nms)
{
    int n = 0;
    descriptor *d = harris_corner_detector(im, sigma, thresh, nms, &n);
    mark_corners(im, d, n);
}
