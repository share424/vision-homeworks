from uwimg import *

a = load_image("data/dog_a.jpg")
b = load_image("data/dog_b.jpg")
flow = optical_flow_images(b, a, 15, 8)
draw_flow(a, flow, 8)
save_image(a, "output/lines")

#optical_flow_webcam(15,4,8)

# im = load_image("data/dog.jpg")
# im = box_filter_image(im, 15)
# save_image(im, "output/dog")
