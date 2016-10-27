/* Copyright  (C) 2010-2016 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this file (string_list_map.h).
 * ---------------------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __LIBRETRO_SDK_STRING_LIST_MAP_H
#define __LIBRETRO_SDK_STRING_LIST_MAP_H

#include <retro_common_api.h>

#include <boolean.h>
#include <stdlib.h>
#include <stddef.h>
#include <lists/string_list.h>

RETRO_BEGIN_DECLS

union string_list_map_elem_attr
{
   bool  b;
   int   i;
   void *p;
};

struct string_list_map_elem
{
   char *key;
   struct string_list *data;
   union string_list_elem_attr attr;
};

struct string_list_map
{
   struct string_list_map_elem *elems;
   size_t size;
   size_t cap;
};

/**
 * string_list_map_find_elem:
 * @list             : pointer to string list map
 * @elem             : element to find inside the string list map.
 *
 * Searches for an element (@elem) inside the string list map.
 *
 * Returns: true (1) if element could be found, otherwise false (0).
 */
int string_list_map_find_elem(const struct string_list_map *list, const char *elem);

/**
 * string_list_map_new:
 *
 * Creates a new string list map. Has to be freed manually.
 *
 * Returns: new string list map if successful, otherwise NULL.
 */
struct string_list_map *string_list_map_new(void);

/**
 * string_list_map_append:
 * @list             : pointer to string list map
 * @elem             : element to add to the string list map
 * @attr             : attributes of new element.
 *
 * Appends a new element to the string list map.
 *
 * Returns: true (1) if successful, otherwise false (0).
 **/
bool string_list_map_append(struct string_list_map *list, const char *key,
      const struct string_list *elem, union string_list_elem_attr attr);

/**
 * string_list_map_free
 * @list             : pointer to string list map object
 *
 * Frees a string list map.
 */
void string_list_map_free(struct string_list_map *list);

RETRO_END_DECLS

#endif
