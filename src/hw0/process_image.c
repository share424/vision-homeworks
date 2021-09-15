#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include "image.h"

bool fix_pixel(image im, int *x, int *y, int *c)
{
    *x = MIN(MAX(*x, 0), im.w - 1);
    *y = MIN(MAX(*y, 0), im.h - 1);
    *c = MIN(MAX(*c, 0), im.c - 1);
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
                set_pixel(im, x, y, c, MAX(MIN(pixel, 1), 0));
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
    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            float hue = get_pixel(im, x, y, 0);
            float saturation = get_pixel(im, x, y, 1);
            float value = get_pixel(im, x, y, 2);

            float r, g, b;

            if (saturation == 0) {
                r = value;
                g = value;
                b = value;
            } else {
                float h = hue * 6;
                if (h == 6) h = 0;
                int i = floor(h);
                float x = value * (1 - saturation);
                float y = value * (1 - saturation * (h - i));
                float z = value * (1 - saturation * (1 - (h - i)));

                switch (i) {
                    case 0:  r = value; g = z;     b = x; break;
                    case 1:  r = y;     g = value; b = x; break;
                    case 2:  r = x;     g = value; b = z; break;
                    case 3:  r = x;     g = y;     b = value; break;
                    case 4:  r = z;     g = x;     b = value; break;
                    default: r = value; g = x;     b = y;
                }
            }

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

float absolute(float a)
{
    return -1 * a;
}

void rgb_to_hcl(image im)
{
    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            float r = get_pixel(im, x, y, 0);
            float g = get_pixel(im, x, y, 1);
            float b = get_pixel(im, x, y, 2);

            // convert RGB to CIE-XYZ
            float gamma = 2.2;
            r = pow(r, gamma);
            g = pow(g, gamma);
            b = pow(b, gamma);

            // sRGB (D65) transformation matrix
            float X = r * 0.4124 + g * 0.3576 + b * 0.1805;
            float Y = r * 0.2126 + g * 0.7152 + b * 0.0722;
            float Z = r * 0.0193 + g * 0.1192 + b * 0.9505;

            // convert CIE-XYZ to CIE-Lab
            // XYZ reference: Observer: 2 degree, illuminant: D65 (Daylight, sRGB, Adobe-RGB)
            X = X / 95.047;
            Y = Y / 100;
            Z = Z / 108.883;

            float fx = (X > 0.008856) ? pow(X, (float)1/3) : (903.3 * X + 16) / 116;
            float fy = (Y > 0.008856) ? pow(Y, (float)1/3) : (903.3 * Y + 16) / 116;
            float fz = (Z > 0.008856) ? pow(Z, (float)1/3) : (903.3 * Z + 16) / 116;

            float L = 116 * fy - 16;
            float ca = 500 * (fx - fy);
            float cb = 200 * (fy - fz);

            // convert CIE-Lab to CIE-Lch
            float C = sqrt(ca*ca + cb*cb);
            float H = atan2(cb, ca);
            H = (H > 0) ? (H / PI) * 180 : (360 - (absolute(H)/PI) * 180);
            
            // normalize
            H /= 360;
            C /= 100;
            L /= 100;

            set_pixel(im, x, y, 0, H);
            set_pixel(im, x, y, 1, C);
            set_pixel(im, x, y, 2, L);
        }
    }
}

float deg2rad(float deg)
{
    return deg * PI / 180;
}

void hcl_to_rgb(image im)
{
    for(int x = 0; x<im.w; x++) {
        for(int y = 0; y<im.h; y++) {
            float H = get_pixel(im, x, y, 0) * 360;
            float C = get_pixel(im, x, y, 1) * 100;
            float L = get_pixel(im, x, y, 2) * 100;

            // convert CIE-Lch to CIE-Lab
            float ca = cos(deg2rad(H)) * C;
            float cb = sin(deg2rad(H)) * C;

            // convert CIE-Lab to CIE-XYZ
            float Y = (L + 16) / 116;
            float X = (ca / 500) + Y;
            float Z = Y - (cb / 200);

            Y = (Y > 0.008856) ? pow(Y, 3) : (Y * 116 - 16) / 903.3; 
            X = (X > 0.008856) ? pow(X, 3) : (X * 116 - 16) / 903.3; 
            Z = (Z > 0.008856) ? pow(Z, 3) : (Z * 116 - 16) / 903.3; 

            // XYZ reference: Observer: 2 degree, illuminant: D65 (Daylight, sRGB, Adobe-RGB)
            X = X * 95.047;
            Y = Y * 100;
            Z = Z * 108.883;

            // convert CIE-XYZ to RGB
            // sRGB (D65) transformation inverse matrix
            float r = X * 3.2440 + Y * -1.5371 + Z * -0.4985;
            float g = X * -0.9692 + Y * 1.8760 + Z * 0.0415;
            float b = X * 0.0556 + Y * -0.2040 + Z * 1.0572;

            float gamma = 2.2;
            r = pow(r, 1/gamma);
            g = pow(g, 1/gamma);
            b = pow(b, 1/gamma);

            set_pixel(im, x, y, 0, r);
            set_pixel(im, x, y, 1, g);
            set_pixel(im, x, y, 2, b);

        }
    }
}