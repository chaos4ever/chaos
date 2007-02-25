/*

 The native 8:8:8:8 graphics library extension

 This library can be used when writing software that
 must run every where.

 (c) 2000 Jonas Tärnström
 DISTRIBUTED UNDER LGPL

*/

#include <graphics/graphics.h>
#include <graphics/image.h>
#include <graphics/color.h>

void module_image_put (s32 x, s32 y, local_image_t *image);
void module_image_get (s32 x, s32 y, u32 size_x, u32 size_y, local_image_t$void (*graphics_image_convert_to_local) (native_image_t *in, local_image_t *out$void (*graphics_image_convert_to_native) (local_image_t *in, native_image_t *ou$

  /* Region related functions */
void module_region_copy (region_t dest, region_t src);
void module_region_fill (region_t region, native_color_t color);


  /* Pixel related functions */
void module_pixel_put (u32 x, u32 y, native_color_t color);
void module_pixel_get (u32 x, u32 y, native_color_t *color);


  /* Palette related functions */
void module_palette_set (u32 color, color_t color);
void module_palette_get (u32 color, color_t *color);
void module_palette_reset (void);
