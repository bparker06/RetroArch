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

/* gcc -Wall -Werror -O2 -std=gnu89 -I../../libretro-common/include -o poconv poconv.c ../../libretro-common/lists/string_list.c ../../libretro-common/compat/compat_strl.c */

/* FIXME: Some assumptions are made about the input file:
 * - no literal can contain any double quotes within
 * - no _tr() macro can appear more than once on a line
 * - all contiguous multi-line literals that will be translated MUST be entirely encapsulated in a _tr() macro, even if there are ifdefs around some of the literals
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <compat/strl.h>
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
   char cur_case[BUFSIZE] = {0};

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

   printf(""
STR(msgid ""\n
msgstr ""\n
"Project-Id-Version: \n"\n
"POT-Creation-Date: \n"\n
"PO-Revision-Date: \n"\n
"Last-Translator: \n"\n
"Language: en_US\n"\n
"Language-Team: \n"\n
"MIME-Version: 1.0\n"\n
"Content-Type: text/plain; charset=utf-8\n"\n
"Content-Transfer-Encoding: 8bit\n"\n
"X-Generator: RetroArch\n"\n\n));

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

            if (case_count > 0)
               printf("\n");

            printf("msgctxt \"%s\"\n", case_stmt);

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
            literal[end-literal+1] = '\0';
            state = STATE_CASE;

            printf("msgid %s\n", literal);
            printf("msgstr %s\n", literal);
         }
         else
         {
            union string_list_elem_attr attr = {0};

            if (cur_case[0] && state == STATE_CASE)
               printf("\nmsgctxt \"%s\"\n", cur_case);

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

               printf("msgid \"\"\n");

               for (i = 0; i < list->size; i++)
               {
                  const char *str = list->elems[i].data;
                  printf("%s", str);
               }

               printf("\nmsgstr \"\"\n");

               for (i = 0; i < list->size; i++)
               {
                  const char *str = list->elems[i].data;
                  printf("%s", str);
               }

               printf("\n");
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

   return 0;
}
