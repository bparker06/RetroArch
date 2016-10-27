/* vim: set tabstop=3 shiftwidth=3 expandtab: */

/*  RetroArch - A frontend for libretro.
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

/* FIXME: C89 (non-GNU) will not build because of an apparent issue in
 * libretro-common? (using undefined things like struct timespec) */

/* gcc -Wall -Werror -O2 -std=gnu89 -I../../libretro-common/include -o poconv poconv.c ../../libretro-common/lists/string_list.c ../../libretro-common/compat/compat_strl.c ../../libretro-common/lists/string_list_map.c */

/* FIXME: Some assumptions are made about the input file(s):
 * - no single line in the input file nor any string literal (after concatenation) are larger than 4096 bytes
 * - no literal can contain any double quotes within (including translated .po files)
 * - no _tr() macro can appear more than once on a line
 * - all contiguous multi-line literals that will be translated MUST be entirely encapsulated in a _tr() macro, even if there are ifdefs around some of the literals
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <compat/strl.h>
#include <lists/string_list_map.h>
#include <lists/string_list.h>

#define BUFSIZE 4096

#define STR(s) #s

enum state_t
{
   STATE_CASE,
   STATE_NEW_STR,
   STATE_CONTINUE_STR
};

int main(int argc, char *argv[])
{
   FILE *f;
   size_t bufsize = BUFSIZE;
   char *buf = (char*)malloc(bufsize);
   enum state_t state = STATE_CASE;
   unsigned case_count = 0;
   struct string_list *list = NULL;
   struct string_list_map *map = string_list_map_new();
   char cur_case[BUFSIZE] = {0};
   unsigned map_i;

   if (argc < 2)
   {
      fprintf(stderr, "Usage: %s msg_hash_us.c\n\n"
              "Generates a new translation file based off of the english strings in msg_hash_us.c.\n"
              "The argument to this program is the existing english translation file, and the\n"
              "output will be sent to stdout.\n",
              argv[0]);
      exit(1);
   }

   f = fopen(argv[1], "rb");

   if (!f)
   {
      fprintf(stderr, "Could not open file %s for reading.\n", argv[1]);
      exit(1);
   }

   while (!feof(f))
   {
      char *line = fgets(buf, bufsize, f);

      if (!line)
         break;

      if (state == STATE_CASE)
      {
         char *case_stmt = strstr(buf, "case ");

         if (case_stmt)
         {
            char *colon = strrchr(buf, ':');

            if (colon)
               case_stmt[colon-case_stmt] = '\0';

            case_stmt += 5;

            strlcpy(cur_case, case_stmt, sizeof(cur_case));

            case_count++;
            state = STATE_NEW_STR;
         }
      }

      if (state != STATE_CONTINUE_STR)
      {
         char *literal = strstr(buf, "_tr(\"");
         char *end;

         if (literal)
            literal += 4;
         else
           continue;

         end = strstr(literal, "\")");

         if (end)
         {
            struct string_list *l = string_list_new();
            union string_list_elem_attr attr = {0};

            literal[end-literal+1] = '\0';
            state = STATE_CASE;

            string_list_append(l, literal, attr);
            string_list_map_append(map, cur_case, l, attr);
            string_list_free(l);
         }
         else
         {
            union string_list_elem_attr attr = {0};

            state = STATE_CONTINUE_STR;

            if (list)
               string_list_free(list);

            list = string_list_new();

            if (!list)
            {
               fprintf(stderr, "Could not create string list.\n");
               exit(1);
            }

            string_list_append(list, literal, attr);
         }
      }
      else if (state == STATE_CONTINUE_STR)
      {
         char *start = strstr(buf, "\"");
         char *end = strstr(buf, "\")");
         char *literal;

         if (start)
         {
            if (end)
            {
               union string_list_elem_attr attr = {0};
               unsigned i;

               state = STATE_CASE;
               literal = start;
               literal[end-start+1] = '\0';

               string_list_append(list, literal, attr);
               string_list_map_append(map, cur_case, list, attr);
            }
            else
            {
               literal = start;

               if (list)
               {
                  union string_list_elem_attr attr = {0};
                  string_list_append(list, literal, attr);
               }
            }
         }
      }
   }

   fclose(f);

   free(buf);

   printf(""
STR(msgid ""\n
msgstr ""\n
"Project-Id-Version: \n"\n
"POT-Creation-Date: \n"\n
"PO-Revision-Date: \n"\n
"Last-Translator: \n"\n
"Language-Team: \n"\n
"MIME-Version: 1.0\n"\n
"Content-Type: text/plain; charset=utf-8\n"\n
"Content-Transfer-Encoding: 8bit\n"\n
"X-Generator: RetroArch\n"\n));

   for (map_i = 0; map_i < map->size; map_i++)
   {
      char buf[BUFSIZE] = {0};
      const struct string_list *l = map->elems[map_i].data;
      const char *lead = l->size > 1 ? "\"\"\n" : "";

      string_list_join_concat(buf, sizeof(buf), l, "");

      printf("\nmsgctxt: \"%s\"\n", map->elems[map_i].key);
      printf("msgid: %s%s\n", lead, buf);
      printf("msgstr: \"\"\n");
   }

   return 0;
}
