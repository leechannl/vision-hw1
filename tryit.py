from uwimg import (bilinear_resize, clamp_image, colorize_sobel, convolve_image, l1_normalize, load_image,
                   make_box_filter, make_emboss_filter, make_gaussian_filter, make_hemboss_filter, make_highpass_filter,
                   make_ones_image, make_sharpen_filter, make_vemboss_filter, nn_resize, save_image)

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

im = load_image("data/dog.jpg")
f = make_highpass_filter()
a = convolve_image(im, f, 0)
clamp_image(a)
save_image(a, "dog-highpass")

im = load_image("data/dog.jpg")
f = make_sharpen_filter()
a = convolve_image(im, f, 1)
clamp_image(a)
save_image(a, "dog-sharpen")

im = load_image("data/dog.jpg")
f = make_emboss_filter()
a = convolve_image(im, f, 1)
clamp_image(a)
save_image(a, "dog-emboss")

im = load_image("data/dog.jpg")
f = make_vemboss_filter()
a = convolve_image(im, f, 1)
clamp_image(a)
save_image(a, "dog-vemboss")

im = load_image("data/dog.jpg")
f = make_hemboss_filter()
a = convolve_image(im, f, 1)
clamp_image(a)
save_image(a, "dog-hemboss")

im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-gauss2")

im = load_image("data/dog.jpg")
color_sobel = colorize_sobel(im)
save_image(color_sobel, "dog-color-sobel")
