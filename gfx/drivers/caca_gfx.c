/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2016 - Daniel De Matteis
 *  Copyright (C) 2016 - Brad Parker
 *
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../driver.h"
#include "../../configuration.h"
#include "../../verbosity.h"
#include "../video_context_driver.h"

#include <retro_miscellaneous.h>
#include <caca.h>

static caca_canvas_t *caca_cv = NULL;
static caca_dither_t *caca_dither = NULL;
static caca_display_t *caca_display = NULL;
static unsigned caca_video_width = 0;
static unsigned caca_video_height = 0;
static unsigned caca_video_pitch = 0;
static bool caca_rgb32 = 0;

static void caca_gfx_free(void *data);

static void caca_gfx_create()
{
   caca_cv = caca_create_canvas(0, 0);

   if (caca_rgb32)
      caca_dither = caca_create_dither(32, caca_video_width, caca_video_height, caca_video_pitch,
                            0x00ff0000, 0xff00, 0xff, 0x0);
   else
      caca_dither = caca_create_dither(16, caca_video_width, caca_video_height, caca_video_pitch,
                            0xf800, 0x7e0, 0x1f, 0x0);

   caca_display = caca_create_display(caca_cv);

   printf("caca dither %d x %d pitch: %d rgb32: %d\n", caca_video_width, caca_video_height, caca_video_pitch, caca_rgb32);
}

static void *caca_gfx_init(const video_info_t *video,
      const input_driver_t **input, void **input_data)
{
   *input = NULL;
   *input_data = NULL;

   caca_video_width = video->width;
   caca_video_height = video->height;
   caca_rgb32 = video->rgb32;

   if (video->width && video->height)
   {
      if (video->rgb32)
         caca_video_pitch = video->width * 4;
      else
         caca_video_pitch = video->width * 2;

      printf("caca init %d x %d rgb32: %d\n", video->width, video->height, video->rgb32);

      caca_gfx_create();

      if (!caca_cv || !caca_dither || !caca_display)
      {
         /* TODO: handle errors */
      }

      //fwrite("\033[?25l", 6, 1, stdout);
   }

   return (void*)-1;
}

static bool caca_gfx_frame(void *data, const void *frame,
      unsigned frame_width, unsigned frame_height, uint64_t frame_count,
      unsigned pitch, const char *msg)
{
   size_t len = 0;
   void *buffer = NULL;

   (void)data;
   (void)frame;
   (void)frame_width;
   (void)frame_height;
   (void)pitch;
   (void)msg;

   if (!caca_cv || !frame)
      return true;

   if (caca_video_width != frame_width || caca_video_height != frame_height || caca_video_pitch != pitch)
   {
      caca_video_width = frame_width;
      caca_video_height = frame_height;
      caca_video_pitch = pitch;
      caca_gfx_free(NULL);
      caca_gfx_create();
   }

   caca_dither_bitmap(caca_cv, 0, 0, caca_video_width, caca_video_height,
                      caca_dither, frame);

   buffer = caca_export_canvas_to_memory(caca_cv, "ansi", &len);

   if (buffer)
   {
      if (len)
      {
         //fwrite("\033[0;0H", 6, 1, stdout);
         //fwrite(buffer, len, 1, stdout);
         /* How does this know where our image is?? */
         caca_refresh_display(caca_display);
      }

      free(buffer);
   }

   return true;
}

static void caca_gfx_set_nonblock_state(void *data, bool toggle)
{
   (void)data;
   (void)toggle;
}

static bool caca_gfx_alive(void *data)
{
   (void)data;
   return true;
}

static bool caca_gfx_focus(void *data)
{
   (void)data;
   return true;
}

static bool caca_gfx_suppress_screensaver(void *data, bool enable)
{
   (void)data;
   (void)enable;
   return false;
}

static bool caca_gfx_has_windowed(void *data)
{
   (void)data;
   return true;
}

static void caca_gfx_free(void *data)
{
   (void)data;

   if (caca_display)
      caca_free_display(caca_display);

   if (caca_dither)
      caca_free_dither(caca_dither);

   if (caca_cv)
      caca_free_canvas(caca_cv);
}

static bool caca_gfx_set_shader(void *data,
      enum rarch_shader_type type, const char *path)
{
   (void)data;
   (void)type;
   (void)path;

   return false;
}

static void caca_gfx_set_rotation(void *data,
      unsigned rotation)
{
   (void)data;
   (void)rotation;
}

static void caca_gfx_viewport_info(void *data,
      struct video_viewport *vp)
{
   (void)data;
   (void)vp;
}

static bool caca_gfx_read_viewport(void *data, uint8_t *buffer)
{
   (void)data;
   (void)buffer;

   return true;
}

static void caca_gfx_get_poke_interface(void *data,
      const video_poke_interface_t **iface)
{
   (void)data;
   (void)iface;
}

void caca_gfx_set_viewport(void *data, unsigned viewport_width,
      unsigned viewport_height, bool force_full, bool allow_rotate)
{
}

video_driver_t video_caca = {
   caca_gfx_init,
   caca_gfx_frame,
   caca_gfx_set_nonblock_state,
   caca_gfx_alive,
   caca_gfx_focus,
   caca_gfx_suppress_screensaver,
   caca_gfx_has_windowed,
   caca_gfx_set_shader,
   caca_gfx_free,
   "caca",
   caca_gfx_set_viewport,
   caca_gfx_set_rotation,
   caca_gfx_viewport_info,
   caca_gfx_read_viewport,
   NULL, /* read_frame_raw */

#ifdef HAVE_OVERLAY
  NULL, /* overlay_interface */
#endif
  caca_gfx_get_poke_interface,
};
