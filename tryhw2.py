from uwimg import *
im = load_image("data/dog.jpg")

# # 1. bluring
# f = make_box_filter(7)
# blur = convolve_image(im, f, 1)
# thumb = nn_resize(blur, blur.w//7, blur.h//7)
# thumb_nn = nn_resize(im, im.w//7, im.h//7)
# save_image(thumb, "output/dogthumb")
# save_image(thumb_nn, "output/dogthumb_nn")
# save_image(blur, "output/dog-blur")

# # 2. higpass filter
# f = make_highpass_filter()
# edge = convolve_image(im, f, 0)
# clamp_image(edge)
# save_image(edge, "output/dog-edge")

# # 3. sharpen filter
# f = make_sharpen_filter()
# sharpen = convolve_image(im, f, 1)
# clamp_image(sharpen)
# save_image(sharpen, "output/dog-sharpen")

# # 4. emboss filter
# f = make_emboss_filter()
# emboss = convolve_image(im, f, 0)
# clamp_image(emboss)
# save_image(emboss, "output/dog-emboss")

# # 5. gaussian filter
# f = make_gaussian_filter(2)
# gaussian_blur = convolve_image(im, f, 1)
# clamp_image(gaussian_blur)
# save_image(gaussian_blur, "output/dog-gaussian-blur")

# # 6. Hybrid images
# a = load_image("data/melisa.png")
# b = load_image("data/aria.png")
# low_freq_a = convolve_image(a, f, 1)
# low_freq_b = convolve_image(b, f, 1)
# high_freq_a = sub_image(a, low_freq_a)
# combined = add_image(low_freq_b, high_freq_a)
# clamp_image(combined)
# save_image(combined, "output/combined")

# # 7. Image frequency
# low_freq = convolve_image(im, f, 1);
# high_freq = sub_image(im, low_freq)
# reconstructed = add_image(low_freq, high_freq)
# save_image(low_freq, "output/dog-low-frequency")
# save_image(high_freq, "output/dog-high-frequency")
# save_image(reconstructed, "output/dog-reconstructed-frequency")

# # 8. Sobel filters
# out = sobel_image(im)
# magnitude = out[0]
# orientatation = out[1]
# feature_normalize(magnitude)
# feature_normalize(orientatation)
# save_image(magnitude, "output/dog-sobel-magnitude")
# save_image(orientatation, "output/dog-sobel-orientation")

# 9. Sobel colorization
colorized_sobel = colorize_sobel(im)
save_image(colorized_sobel, "output/dog-sobel-colorized")




