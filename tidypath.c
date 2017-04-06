/* @@@ how to run tidypath? */
/* @@@ don't forget to strip out the trailing newline */
/* @@@ validate the size of PATH at the very beginning */
/* @@@ finish basic functionality */
/* @@@ just clean it all up to look pretty */
/* @@@ autoconf (strchrnul) */
/* @@@ add tests */
/* @@@ free memory? */
/* @@@ all these type are all messed up everywhere... size_t, int, int32_t???!??! */
/* @@@ separate out the core so we others can link in the functionality */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEMENTS_ARRAY_DEFAULT_SIZE 32

char *
strchrnul (const char *s, int c)
{
  const char *found = strchr (s, c);

  if (NULL == found)
    {
      found = s + strlen (s);
    }

  return (char *) found;
}

enum
{
  TIDYPATH_EXIT_SUCCESS = 0,
  TIDYPATH_EXIT_NO_MEM = 1,
  TIDYPATH_EXIT_NO_ENV_VAR = 2
};

typedef struct def_element
{
  const char *fragment;
  uint32_t length;
} element;

static bool
check_for_duplicate (const element * array, size_t num_entries, const char *fragment, uint32_t length)
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

static void
tidypath (const char *pathlike_string)
{
  element *element_array;
  size_t element_array_length = ELEMENTS_ARRAY_DEFAULT_SIZE;
  size_t element_index = 0;
  const char *current_position = pathlike_string;
  const char *next_colon;
  size_t i;

  element_array = malloc (ELEMENTS_ARRAY_DEFAULT_SIZE * sizeof (element));
  if (NULL == element_array)
    {
      fprintf (stderr, "memory allocation failed\n");
      exit (TIDYPATH_EXIT_NO_MEM);
    }

  do
    {
      const char *current_fragment;
      int32_t current_length;

      next_colon = strchrnul (current_position, ':');
      current_length = (int32_t) (next_colon - current_position);

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
          element *new_element;

          if (element_index == element_array_length)
            {
              element *new_element_array = realloc (element_array, 2 * element_array_length * sizeof (element));
              if (NULL == new_element_array)
                {
                  fprintf (stderr, "memory allocation failed\n");
                  exit (TIDYPATH_EXIT_NO_MEM);
                }

              element_array = new_element_array;
              element_array_length *= 2;
            }

          new_element = element_array + element_index++;
          new_element->fragment = current_fragment;
          new_element->length = current_length;
        }

      current_position = next_colon + 1;
    }
  while ('\0' != *next_colon);

  for (i = 0; i < element_index; i++)
    {
      if (0 != i)
        {
          printf (":");
        }

      fwrite (element_array[i].fragment, element_array[i].length, 1, stdout);
    }
}

int
main (void)
{
  const char *path;

  path = getenv ("PATH");
  if (NULL == path)
    {
      fprintf (stderr, "PATH environment variable not found\n");
      exit (TIDYPATH_EXIT_NO_ENV_VAR);
    }

  tidypath (path);

  return TIDYPATH_EXIT_SUCCESS;
}
