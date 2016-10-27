/* Copyright  (C) 2010-2016 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this file (string_list_map.c).
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <lists/string_list_map.h>
#include <retro_assert.h>
#include <compat/strl.h>
#include <compat/posix_string.h>

/**
 * string_list_map_free
 * @list             : pointer to string list map object
 *
 * Frees a string list map.
 */
void string_list_map_free(struct string_list_map *list)
{
   size_t i;
   if (!list)
      return;

   for (i = 0; i < list->size; i++)
   {
      string_list_free(list->elems[i].data);
      free(list->elems[i].key);
   }
   free(list->elems);
   free(list);
}

/**
 * string_list_map_capacity:
 * @list             : pointer to string list map
 * @cap              : new capacity for string list map.
 *
 * Change maximum capacity of string list map's size.
 *
 * Returns: true (1) if successful, otherwise false (0).
 **/
static bool string_list_map_capacity(struct string_list_map *list, size_t cap)
{
   struct string_list_map_elem *new_data = NULL;
   retro_assert(cap > list->size);

   new_data = (struct string_list_map_elem*)
      realloc(list->elems, cap * sizeof(*new_data));

   if (!new_data)
      return false;

   if (cap > list->cap)
      memset(&new_data[list->cap], 0, sizeof(*new_data) * (cap - list->cap));

   list->elems = new_data;
   list->cap   = cap;
   return true;
}

/**
 * string_list_map_new:
 *
 * Creates a new string list map. Has to be freed manually.
 *
 * Returns: new string list map if successful, otherwise NULL.
 */
struct string_list_map *string_list_map_new(void)
{
   struct string_list_map *list = (struct string_list_map*)
      calloc(1, sizeof(*list));

   if (!list)
      return NULL;

   if (!string_list_map_capacity(list, 32))
   {
      string_list_map_free(list);
      return NULL;
   }

   return list;
}

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
      const struct string_list *elem, union string_list_elem_attr attr)
{
   struct string_list *data_dup = NULL;
   unsigned i;

   if (list->size >= list->cap &&
         !string_list_map_capacity(list, list->cap * 2))
      return false;

   if (elem->size == 0)
      return false;

   data_dup = string_list_new();

   for (i = 0; i < elem->size; i++)
   {
      string_list_append(data_dup, elem->elems[i].data, attr);
   }

   list->elems[list->size].key = strdup(key);
   list->elems[list->size].data = data_dup;
   list->elems[list->size].attr = attr;

   list->size++;
   return true;
}

/**
 * string_list_map_find_elem:
 * @list             : pointer to string list map
 * @elem             : element to find inside the string list map.
 *
 * Searches for an element (@elem) inside the string list map.
 *
 * Returns: true (1) if element could be found, otherwise false (0).
 */
int string_list_map_find_elem(const struct string_list_map *list, const char *elem)
{
   size_t i;

   if (!list)
      return false;

   for (i = 0; i < list->size; i++)
   {
      if (strcasecmp(list->elems[i].key, elem) == 0)
         return i+1;
   }

   return false;
}

