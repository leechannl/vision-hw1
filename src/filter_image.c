#include "image.h"
#include "log.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TWOPI 6.2831853

image make_ones_image(int w, int h, int c) {
  image im = make_image(w, h, c);
  for (int i = 0; i < im.w * im.h * im.c; i++) {
    im.data[i] = 1;
  }
  return im;
}

void l1_normalize(image im) {
  float sum = 0;
  for (int i = 0; i < (im.w * im.h * im.c); i++) {
    sum += im.data[i];
  }

  for (int i = 0; i < im.w; i++) {
    for (int j = 0; j < im.h; j++) {
      for (int k = 0; k < im.c; k++) {
        set_pixel(im, i, j, k, get_pixel(im, i, j, k) / sum);
      }
    }
  }
}

image make_box_filter(int w) {
  image im = make_ones_image(w, w, 1);
  l1_normalize(im);
  return im;
}

float get_conv(image im, int col, int row, int chn, image f, int f_chn) {
  assert(f_chn < f.c || f_chn >= 0);
  float v = 0;

  for (int i = 0; i < f.w; i++) {
    for (int j = 0; j < f.h; j++) {
      v += get_pixel(f, i, j, f_chn) * get_pixel(im, col - f.w / 2 + i, row - f.h / 2 + j, chn);
    }
  }

  return v;
}

image convolve_image(image im, image filter, int preserve) {
  assert(filter.c == 1 || filter.c == im.c);
  image new_im;
  if (filter.c == im.c) {
    if (preserve != 1) { // normal convolution, generate a 1 channel image
      new_im = make_image(im.w, im.h, 1);
      for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
          float v = 0;
          for (int k = 0; k < im.c; k++) {
            v += get_conv(im, i, j, k, filter, k);
          }
          set_pixel(new_im, i, j, 0, v);
        }
      }
    } else { // preserve channels, keep original channel number
      float v = 0;
      new_im = make_image(im.w, im.h, im.c);
      for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
          for (int k = 0; k < im.c; k++) {
            v = get_conv(im, i, j, k, filter, k);
            set_pixel(new_im, i, j, k, v);
          }
        }
      }
    }
  } else {               // filter.c == 1
    if (preserve != 1) { // generate a 1 channel image
      new_im = make_image(im.w, im.h, 1);
      for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
          float v = 0;
          for (int k = 0; k < im.c; k++) {
            v += get_conv(im, i, j, k, filter, 0);
          }
          set_pixel(new_im, i, j, 0, v);
        }
      }
    } else { // keep orginal channel number
      float v = 0;
      new_im = make_image(im.w, im.h, im.c);
      for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
          for (int k = 0; k < im.c; k++) {
            v = get_conv(im, i, j, k, filter, 0);
            set_pixel(new_im, i, j, k, v);
          }
        }
      }
    }
  }

  return new_im;
}

image make_highpass_filter() {
  //  0 -1  0
  // -1  4 -1
  //  0 -1  0
  image f = make_image(3, 3, 1);
  float values[] = {0, -1, 0, -1, 4, -1, 0, -1, 0};
  set_pixels(f, values, ARRAY_SIZE(values));
  return f;
}

image make_sharpen_filter() {
  //  0 -1  0
  // -1  5 -1
  //  0 -1  0
  image f = make_image(3, 3, 1);
  float values[] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
  set_pixels(f, values, ARRAY_SIZE(values));
  return f;
}

image make_emboss_filter() {
  // -2 -1  0
  // -1  1  1
  //  0  1  2
  image f = make_image(3, 3, 1);
  float values[] = {-2, -1, 0, -1, 1, 1, 0, 1, 2};
  set_pixels(f, values, ARRAY_SIZE(values));
  return f;
}

image make_vemboss_filter() {
  //  0  1  0
  //  0  1  0
  //  0 -1  0
  image f = make_image(3, 3, 1);
  float values[] = {0, 1, 0, 0, 1, 0, 0, -1, 0};
  set_pixels(f, values, ARRAY_SIZE(values));
  return f;
}

image make_hemboss_filter() {
  //  0  0  0
  // -1  1  1
  //  0  0  0
  image f = make_image(3, 3, 1);
  float values[] = {0, 0, 0, -1, 1, 1, 0, 0, 0};
  set_pixels(f, values, ARRAY_SIZE(values));
  return f;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we
// not? Why?
// Answer:
//    Kernel sum to 1, then the original color will be preseved, so `preserve` should be 1, otherwise be 0.
//    So for highpass filter `preserve` be 0, others be 1.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer:
//    Need to do clamp image. Because after doing convolution, the pixel value is likely greater than 1.

image make_gaussian_filter(float sigma) {
  // g(x,y) = 1/(TWO_PI*pow(sigma,2)) * exp(-( (pow(x-mean,2)+pow(y-mean,2)) / (2*pow(sigma,2)) ))
  // g(x,y) = multiplier * exp(exponent)
  // multiplier = 1/(TWO_PI*pow(sigma,2))
  // exponent = -( (pow(x-mean,2)+pow(y-mean,2)) / (2*pow(sigma,2)) )

  int six_sigma = (int)ceilf(sigma * 6);
  int size = six_sigma % 2 ? six_sigma : six_sigma + 1;

  image f = make_image(size, size, 1);

  float multiplier = 1 / (TWOPI * sigma * sigma);
  int mean = size / 2;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      float exponent = -(pow(i - mean, 2) + pow(j - mean, 2)) / (2 * sigma * sigma);
      float v = multiplier * exp(exponent);
      set_pixel(f, i, j, 0, v);
    }
  }
  l1_normalize(f);
  return f;
}

image add_image(image a, image b) {
  // TODO
  return make_image(1, 1, 1);
}

image sub_image(image a, image b) {
  // TODO
  return make_image(1, 1, 1);
}

image make_gx_filter() {
  // TODO
  return make_image(1, 1, 1);
}

image make_gy_filter() {
  // TODO
  return make_image(1, 1, 1);
}

void feature_normalize(image im) {
  // TODO
}

image *sobel_image(image im) {
  // TODO
  return calloc(2, sizeof(image));
}

image colorize_sobel(image im) {
  // TODO
  return make_image(1, 1, 1);
}
