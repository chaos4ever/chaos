#ifndef __GRAPHICS_IMAGE_H__
#define __GRAPHICS_IMAGE_H__

#include <graphics/color.h>

/*

 This type describes the native chaos image format.

*/

typedef struct
{

  u32 size_x, size_y;
  color_t *data;

} native_image_type;

#endif


