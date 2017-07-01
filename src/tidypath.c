/*
 * MIT License
 *
 * Copyright (c) 2017 Joshua Neuheisel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "tidypath.h"
#if NEED_STRCHRNUL
#include "compat/strchrnul.h"
#endif

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Start by allocating this many elements in our working array
#define TIDYPATH_ELEMENTS_ARRAY_START_SIZE 32

// Create default options that are used if tidypath is called with NULL opts
static options s_default_options = { false, false, false, ':' };

// Struct to hold metadata about each path fragment
typedef struct def_element
{
  char *fragment;
  size_t length;
  dev_t st_dev;
  ino_t st_ino;
} element;

/*
 * Determine if the fragment should be kept or pruned. Also, fill in the st_dev and st_ino fields
 * if needed.
 */

static bool
should_keep (const element * array, size_t num_entries, const char *fragment, size_t length, bool aggressive,
             dev_t * st_dev, ino_t * st_ino)
{
  size_t i;

  /*
   * Special rules for absolute paths when performing aggressive pruning
   */
  if (aggressive && ('/' == *fragment))
    {
      struct stat buf;

      int rv = stat (fragment, &buf);
      if ((0 != rv) || (!S_ISDIR (buf.st_mode)))
        {
          return false;
        }

      for (i = 0; i < num_entries; i++)
        {
          if (('/' == *array[i].fragment) && (array[i].st_dev == buf.st_dev) && (array[i].st_ino == buf.st_ino))
            {
              return false;
            }
        }

      *st_dev = buf.st_dev;
      *st_ino = buf.st_ino;
    }
  else
    {
      for (i = 0; i < num_entries; i++)
        {
          if ((array[i].length == length) && (0 == memcmp (array[i].fragment, fragment, length)))
            {
              return false;
            }
        }
    }

  return true;
}

char *
tidypath (const char *pathlike, const options * opts)
{
  /*
   * If we are passed a NULL string, return an empty string.
   * This allows us to chain: `tidypath (getenv ("PATH"), NULL);`.
   */
  if (NULL == pathlike)
    {
      return strdup ("");
    }

  /*
   * If no options are given, choose the defaults
   */
  if (NULL == opts)
    {
      opts = &s_default_options;
    }

  // Set the output string to NULL in case we need to bail on an error
  char *output = NULL;

  /*
   * Loop over all the fragments, ignoring those we don't want to keep
   */

  element *element_array = NULL;
  size_t element_index = 0;
  size_t element_array_length = 0;

  const char *current_position;
  const char *next_colon;

  for (current_position = pathlike, next_colon = pathlike; '\0' != *next_colon; current_position = next_colon + 1)
    {
      next_colon = strchrnul (current_position, opts->delimiter);
      size_t current_length = (size_t) (next_colon - current_position);

      if (!opts->ignore_empty || (current_length > 0))
        {
          char *current_fragment;

          if (0 == current_length)
            {
              current_length = 1;
              current_fragment = strdup (".");
            }
          else
            {
              current_fragment = strndup (current_position, current_length);
            }

          if (NULL == current_fragment)
            {
              goto DONE;
            }

          dev_t st_dev;
          ino_t st_ino;
          if (should_keep (element_array, element_index, current_fragment, current_length, opts->aggressive,
                           &st_dev, &st_ino))
            {
              /*
               * If we don't have enough room in our working array, double it...
               */
              if (element_index == element_array_length)
                {
                  // ... but check for overflow!
                  if (element_array_length > (SIZE_MAX / 2 / sizeof (element)))
                    {
                      if (!opts->allow_leaks)
                        {
                          free (current_fragment);
                        }

                      errno = ENOMEM;
                      goto DONE;
                    }

                  size_t new_element_array_length;
                  if (0 == element_array_length)
                    {
                      new_element_array_length = TIDYPATH_ELEMENTS_ARRAY_START_SIZE;
                    }
                  else
                    {
                      new_element_array_length = 2 * element_array_length;
                    }

                  element *new_element_array = realloc (element_array, new_element_array_length * sizeof (element));
                  if (NULL == new_element_array)
                    {
                      if (!opts->allow_leaks)
                        {
                          free (current_fragment);
                        }

                      goto DONE;
                    }

                  element_array = new_element_array;
                  element_array_length = new_element_array_length;
                }

              element *new_element = element_array + element_index++;
              new_element->fragment = current_fragment;
              new_element->length = current_length;
              if (opts->aggressive && ('/' == *current_fragment))
                {
                  new_element->st_dev = st_dev;
                  new_element->st_ino = st_ino;
                }

            }
          else
            {
              if (!opts->allow_leaks)
                {
                  free (current_fragment);
                }
            }
        }
    }

  /*
   * If we have no fragments, return an empty string
   */
  if (0 == element_index)
    {
      output = strdup ("");

      goto DONE;
    }

  /*
   * Compute the size of the output string...
   */
  size_t new_length = 0;

  size_t i;
  for (i = 0; i < element_index; i++)
    {
      new_length += element_array[i].length;
    }

  // ... but watch for overflow!
  if (new_length > (SIZE_MAX - element_index))
    {
      errno = ENOMEM;
      goto DONE;
    }

  new_length += element_index;  // Room for (element_index - 1) delimiters and the trailing '\0'

  /*
   * Create the output string and fill it in
   */

  output = malloc (new_length);
  if (NULL == output)
    {
      goto DONE;
    }

  char *p_output = output;
  for (i = 0; i < element_index; i++)
    {
      if (0 != i)
        {
          *p_output++ = opts->delimiter;
        }

      memcpy (p_output, element_array[i].fragment, element_array[i].length);
      p_output += element_array[i].length;
    }
  *p_output = '\0';

DONE:
  /*
   * If desired, free our interal allocations
   */
  if (!opts->allow_leaks)
    {
      if (element_index > 0)
        {
          for (i = 0; i < element_index; i++)
            {
              free (element_array[i].fragment);
            }

          free (element_array);
        }
    }

  return output;
}
