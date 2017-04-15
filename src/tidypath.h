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

#ifndef TIDYPATH_H
#define TIDYPATH_H

#include <stdbool.h>

/*
 * Options that control the behavior of tidypath.
 *
 * Aggressive pruning means remove any absolute paths that do not exist,
 * are not directories, or match inodes with previous fragments.
 */

typedef struct def_options
{
  bool aggressive;              // aggressive pruning
  bool ignore_empty;            // prune empty paths
  bool allow_leaks;             // do not free internal memory allocations
  char delimiter;               // char that delimits fragments
} options;

/*
 * tidypath takes a string and an optional options struct. If opts is NULL,
 * default options are used: { false, false, false, ':' }
 *
 * tidypath returns a newly allocated string, which should be free()-ed when no
 * longer needed. It returns NULL on any memory allocation errors.
 *
 * tidypath copies pathlike to the output string applying the following
 * algorithm:
 * 1. pathlike is logically separated into fragments separated by the delimiter
 * 2. each empty fragment is logically converted to '.'
 * 3. scanning left to right, each fragment which matches a previous fragment
 *    is removed
 */

char *tidypath (const char *pathlike, const options * opts);

#endif
