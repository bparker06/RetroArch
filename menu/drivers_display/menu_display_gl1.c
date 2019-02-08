/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2011-2017 - Daniel De Matteis
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

#include <retro_miscellaneous.h>

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#include "../../retroarch.h"
#include "../../gfx/font_driver.h"
#include "../../gfx/video_driver.h"
#include "../../gfx/common/gl1_common.h"

#include "../menu_driver.h"

static const GLfloat gl1_vertexes[] = {
   0, 0,
   1, 0,
   0, 1,
   1, 1
};

static const GLfloat gl1_tex_coords[] = {
   0, 1,
   1, 1,
   0, 0,
   1, 0
};

static const float *menu_display_gl1_get_default_vertices(void)
{
   return &gl1_vertexes[0];
}

static const float *menu_display_gl1_get_default_tex_coords(void)
{
   return &gl1_tex_coords[0];
}

/*static GLenum menu_display_prim_to_gl1_enum(
      enum menu_display_prim_type type)
{
   switch (type)
   {
      case MENU_DISPLAY_PRIM_TRIANGLESTRIP:
         return GL_TRIANGLE_STRIP;
      case MENU_DISPLAY_PRIM_TRIANGLES:
         return GL_TRIANGLES;
      case MENU_DISPLAY_PRIM_NONE:
      default:
         break;
   }

   return 0;
}*/

static void menu_display_gl1_blend_begin(video_frame_info_t *video_info)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void menu_display_gl1_blend_end(video_frame_info_t *video_info)
{
   glDisable(GL_BLEND);
}

static void menu_display_gl1_viewport(menu_display_ctx_draw_t *draw,
      video_frame_info_t *video_info)
{
   if (draw)
      glViewport(draw->x, draw->y, draw->width, draw->height);
}

static void menu_display_gl1_draw(menu_display_ctx_draw_t *draw,
      video_frame_info_t *video_info)
{
   gl1_t             *gl1          = video_info ?
      (gl1_t*)video_info->userdata : NULL;

   if (!gl1 || !draw)
      return;

   if (!draw->coords->vertex)
      draw->coords->vertex = menu_display_gl1_get_default_vertices();
   if (!draw->coords->tex_coord)
      draw->coords->tex_coord = menu_display_gl1_get_default_tex_coords();
   if (!draw->coords->lut_tex_coord)
      draw->coords->lut_tex_coord = menu_display_gl1_get_default_tex_coords();

   menu_display_gl1_viewport(draw, video_info);
/*
   glBindTexture(GL_TEXTURE_2D, (GLuint)draw->texture);

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glVertexPointer(3, GL_FLOAT, 0, draw->coords->vertex);
   glTexCoordPointer(3, GL_FLOAT, 0, draw->coords->tex_coord);

   glDrawArrays(menu_display_prim_to_gl1_enum(
            draw->prim_type), 0, draw->coords->vertices);

   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
*/
}

static void menu_display_gl1_restore_clear_color(void)
{
   glClearColor(0.0f, 0.0f, 0.0f, 0.00f);
}

static void menu_display_gl1_clear_color(
      menu_display_ctx_clearcolor_t *clearcolor,
      video_frame_info_t *video_info)
{
   if (!clearcolor)
      return;

   glClearColor(clearcolor->r,
         clearcolor->g, clearcolor->b, clearcolor->a);
   glClear(GL_COLOR_BUFFER_BIT);
}

static bool menu_display_gl1_font_init_first(
      void **font_handle, void *video_data,
      const char *font_path, float menu_font_size,
      bool is_threaded)
{
   /*font_data_t **handle = (font_data_t**)font_handle;
   if (!(*handle = font_driver_init_first(video_data,
         font_path, menu_font_size, true,
         is_threaded,
         FONT_DRIVER_RENDER_OPENGL1_API)))*/
       return false;
   return true;
}

static void menu_display_gl1_scissor_begin(video_frame_info_t *video_info, int x, int y,
      unsigned width, unsigned height)
{
   glScissor(x, video_info->height - y - height, width, height);
   glEnable(GL_SCISSOR_TEST);
}

static void menu_display_gl1_scissor_end(video_frame_info_t *video_info)
{
   glDisable(GL_SCISSOR_TEST);
}

menu_display_ctx_driver_t menu_display_ctx_gl1 = {
   menu_display_gl1_draw,
   NULL,
   menu_display_gl1_viewport,
   menu_display_gl1_blend_begin,
   menu_display_gl1_blend_end,
   menu_display_gl1_restore_clear_color,
   menu_display_gl1_clear_color,
   NULL,
   menu_display_gl1_get_default_vertices,
   menu_display_gl1_get_default_tex_coords,
   menu_display_gl1_font_init_first,
   MENU_VIDEO_DRIVER_OPENGL1,
   "gl1",
   false,
   menu_display_gl1_scissor_begin,
   menu_display_gl1_scissor_end
};
