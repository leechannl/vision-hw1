from uwimg import (bilinear_resize, l1_normalize, load_image, make_box_filter,
                   make_ones_image, nn_resize, save_image)

im = load_image("data/dogsmall.jpg")
a = nn_resize(im, im.w * 4, im.h * 4)
save_image(a, "dog4x-nn")


im = load_image("data/dogsmall.jpg")
a = bilinear_resize(im, im.w * 4, im.h * 4)
save_image(a, "dog4x-bl")

im = load_image("data/dog.jpg")
a = nn_resize(im, im.w // 7, im.h // 7)
save_image(a, "dog7th-nn")

im = load_image("data/dog.jpg")
a = bilinear_resize(im, im.w // 7, im.h // 7)
save_image(a, "dog7th-bl")

im = make_ones_image(2, 5, 1)
l1_normalize(im)

im = make_box_filter(3)
