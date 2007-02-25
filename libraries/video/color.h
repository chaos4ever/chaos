#ifndef __GRAPHICS_COLOR_H__
#define __GRAPHICS_COLOR_H__

#include <graphics/config.h>

/*

  This type describes the chaos native color type.

*/

typedef struct
{

  native_channel_type red,
                      green,
                      blue;

#ifdef __GRAPHICS_NATIVE_LUMINANCE__
  native_channel_type luminance;
#endif

  native_channel_type alpha;


} native_color_type;

/* This struct shouldn't be located here */

typedef struct
{

  u8 pal_index;

} local_color_type;

#endif

