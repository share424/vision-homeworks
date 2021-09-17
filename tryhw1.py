from uwimg import *
im = load_image("data/dogsmall.jpg")
a = nn_resize(im, im.w*4, im.h*4)
save_image(a, "output/dog4x-nn")

b = bilinear_resize(im, im.w*4, im.h*4)
save_image(a, "output/dog4x-bilinear")