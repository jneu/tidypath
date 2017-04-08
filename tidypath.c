#include "tidypath.h"
#include "compat.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIDYPATH_ELEMENTS_ARRAY_START_SIZE 32

typedef struct def_element
{
  const char *fragment;
  size_t length;
} element;

static bool
check_for_duplicate (const element * array, size_t num_entries, const char *fragment, size_t length)
{
  size_t i;

  for (i = 0; i < num_entries; i++)
    {
      if ((array[i].length == length) && (0 == memcmp (array[i].fragment, fragment, length)))
        {
          return true;
        }
    }

  return false;
}

char *
tidypath (const char *pathlike, const options * opts)
{
  element *element_array = NULL;
  size_t element_index = 0;
  size_t element_array_length = 0;
  const char *current_position = pathlike;

  for (;;)
    {
      const char *next_colon = strchrnul (current_position, opts->delimiter);
      size_t current_length = (size_t) (next_colon - current_position);

      if (!opts->ignore_empty || (current_length > 0))
        {
          const char *current_fragment;
          if (0 == current_length)
            {
              current_fragment = ".";
              current_length = 1;
            }
          else
            {
              current_fragment = current_position;
            }

          if (!check_for_duplicate (element_array, element_index, current_fragment, current_length))
            {
              if (element_index == element_array_length)
                {
                  size_t new_element_array_length = 2 * element_array_length;
                  if (0 == new_element_array_length)
                    {
                      new_element_array_length = TIDYPATH_ELEMENTS_ARRAY_START_SIZE;
                    }

                  element *new_element_array = realloc (element_array, new_element_array_length * sizeof (element));
                  if (NULL == new_element_array)
                    {
                      return NULL;
                    }

                  element_array = new_element_array;
                  element_array_length = new_element_array_length;
                }

              element *new_element = element_array + element_index++;
              new_element->fragment = current_fragment;
              new_element->length = current_length;
            }
        }

      if ('\0' == *next_colon)
        {
          break;
        }

      current_position = next_colon + 1;
    }

  size_t i;
  for (i = 0; i < element_index; i++)
    {
      if (0 != i)
        {
          printf ("%c", opts->delimiter);
        }

      fwrite (element_array[i].fragment, element_array[i].length, 1, stdout);
    }

  return (char *) 1;
}
