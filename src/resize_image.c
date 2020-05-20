#include "image.h"
#include "log.h"
#include <math.h>

float nn_interpolate(image im, float x, float y, int c) {
  if (x > im.w) { x = im.w - 1; }
  if (x < 0) { x = 0; }
  if (y > im.h) { y = im.h - 1; }
  if (y < 0) { y = 0; }

  float v = get_pixel(im, round(x), round(y), c);
  return v;
}

// im1 with w1, h1 => im2 with w2, h2
// for (X2, Y2) in im2, coresponding coordinate in im1 is:
// a * X2 + b = X1
// a = w1/w2,
// b = - 0.5 + 0.5 * a
image nn_resize(image im, int w, int h) {
  image_info(im);
  image new_image = make_image(w, h, im.c);
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      for (int k = 0; k < im.c; k++) {
        float a = (float)im.w / (float)w;
        float b = -0.5 + 0.5 * a;
        float new_x = i * a + b;
        float new_y = j * a + b;
        float v = nn_interpolate(im, new_x, new_y, k);
        set_pixel(new_image, i, j, k, v);
      }
    }
  }

  image_info(new_image);
  return new_image;
}

float bilinear_interpolate(image im, float x, float y, int c) {
  //
  // (x1,y1)-----*-----(x2, y1)
  //    |        |        |
  //    a        |        |
  //    |        |        |
  //    q1--m--(x,y)--p---q2
  //    |        |        |
  //    b        |        |
  //    |        |        |
  // (x1,y2)-----*-----(x2,y2)
  //

  float x1 = floorf(x);
  float y1 = floorf(y);
  float x2 = ceilf(x);
  float y2 = ceilf(y);

  float a = y - floorf(y);
  float b = ceilf(y) - y;
  float m = x - floorf(x);
  float p = ceilf(x) - x;

  float q1 = get_pixel(im, x1, y1, c) * b + get_pixel(im, x1, y2, c) * a;
  float q2 = get_pixel(im, x2, y1, c) * b + get_pixel(im, x2, y2, c) * a;

  return q1 * p + q2 * m;
}

image bilinear_resize(image im, int w, int h) {
  image_info(im);
  image new_image = make_image(w, h, im.c);

  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      for (int k = 0; k < im.c; k++) {
        float a = (float)im.w / (float)w;
        float b = -0.5 + 0.5 * a;
        float new_x = i * a + b;
        float new_y = j * a + b;
        float v = bilinear_interpolate(im, new_x, new_y, k);
        set_pixel(new_image, i, j, k, v);
      }
    }
  }

  image_info(new_image);
  return new_image;
}
