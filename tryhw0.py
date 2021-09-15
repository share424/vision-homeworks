from uwimg import *

# 1. Getting and setting pixels
im = load_image("data/dog.jpg")
for row in range(im.h):
    for col in range(im.w):
        set_pixel(im, col, row, 0, 0)
save_image(im, "output/dog_no_red")

# 3. Grayscale image
im = load_image("data/colorbar.png")
graybar = rgb_to_grayscale(im)
save_image(graybar, "output/graybar")

# 4. Shift Image
im = load_image("data/dog.jpg")
shift_image(im, 0, .4)
shift_image(im, 1, .4)
shift_image(im, 2, .4)
save_image(im, "output/overflow")

# 5. Clamp Image
clamp_image(im)
save_image(im, "output/doglight_fixed")

# 6. Colorspace and saturation
im = load_image("data/dog.jpg")
rgb_to_hsv(im)
shift_image(im, 1, .2)
clamp_image(im)
hsv_to_rgb(im)
save_image(im, "output/dog_saturated")

# 7. scale saturation
im = load_image("data/dog.jpg")
rgb_to_hsv(im)
scale_image(im, 1, 2)
clamp_image(im)
hsv_to_rgb(im)
save_image(im, "output/dog_scale_saturated")

# 8. RGB to HCL
im = load_image("data/dog.jpg")
rgb_to_hcl(im)
shift_image(im, 2, 0.9)
shift_image(im, 1, -0.02)
clamp_image(im)
hcl_to_rgb(im)
save_image(im, "output/dog_less_chroma")