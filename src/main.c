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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
check_for_null_alloc (void *p)
{
  if (NULL == p)
    {
      fprintf (stderr, "memory allocation failed\n");
      exit (EXIT_FAILURE);
    }
}

static void
show_version (void)
{
  printf ("%s\n", PACKAGE_VERSION);
}

static void
show_usage (void)
{
  printf (PACKAGE_STRING "\n\n");

  printf ("tidypath [options] [pathlike_string (defaults to $PATH)]\n");
  printf ("    -a, --aggressive          also prune absolute fragments that match previous\n");
  printf ("                              inodes, do not exist, or are not directories\n");
  printf ("    -d, --delimiter <char>    specify the delimiter to use; defaults to :\n");
  printf ("    -e, --env                 the first argument will specify the name of an\n");
  printf ("                              environment variable to use\n");
  printf ("    -h, --help                show this help message and exit\n");
  printf ("    -i, --ignore-empty        prune empty fragments\n");
  printf ("    -n, --nop                 do not prune any fragments for any reason\n");
  printf ("    -p, --pretty-print        print each fragment separated by a newline\n");
  printf ("    -V, --version             show the version of tidypath and exit\n");
}

typedef struct def_prog_options
{
  bool env;                     // argv[1] represents the name of an environment var to use
  bool nop;                     // do no pruning
  bool pretty_print;            // print each fragment separated by a new-line
  bool version;                 // show the version and exit
} prog_options;

static void
parse_args (int argc, char ***p_argv, prog_options * prog_opts, options * opts)
{
  const struct option longopts[10] = {
    {"aggressive", no_argument, NULL, 'a'},
    {"delimiter", required_argument, NULL, 'd'},
    {"env", no_argument, NULL, 'e'},
    {"help", no_argument, NULL, 'h'},
    {"ignore-empty", no_argument, NULL, 'i'},
    {"ignore-eol", no_argument, NULL, 'l'},
    {"nop", no_argument, NULL, 'n'},
    {"pretty-print", no_argument, NULL, 'p'},
    {"version", no_argument, NULL, 'V'},
    {NULL, 0, NULL, 0}
  };

  memset (prog_opts, 0, sizeof (prog_options));

  memset (opts, 0, sizeof (options));
#if !FREE_MEMORY
  opts->allow_leaks = true;
#endif
  opts->delimiter = ':';

  int opts_rv;
  while (-1 != (opts_rv = getopt_long (argc, *p_argv, "ad:ehinpV", longopts, NULL)))
    {
      switch (opts_rv)
        {
        case 'a':
          opts->aggressive = true;
          break;
        case 'd':
          opts->delimiter = *optarg;
          break;
        case 'e':
          prog_opts->env = true;
          break;
        case 'h':
          show_usage ();
          exit (EXIT_SUCCESS);
          break;
        case 'i':
          opts->ignore_empty = true;
          break;
        case 'n':
          prog_opts->nop = true;
          break;
        case 'p':
          prog_opts->pretty_print = true;
          break;
        case 'V':
          prog_opts->version = true;
          break;
        default:
          show_usage ();
          exit (EXIT_FAILURE);
        }
    }
  *p_argv += optind;

  if (prog_opts->version)
    {
      show_version ();
      exit (EXIT_SUCCESS);
    }
}

static const char *
get_pathlike_from_env (const char *env_var_name)
{
  const char *env_value = getenv (env_var_name);
  if (NULL == env_value)
    {
      fprintf (stderr, "%s environment variable not found\n", env_var_name);
      exit (EXIT_FAILURE);
    }

  return env_value;
}

/*
 * Find the string to operate on:
 * 1. If no arg is given, use $PATH
 * 2. If --env was specified, the first arg is the name of the env var to use
 * 3. Otherwise, operate on the first arg
 */

static const char *
get_pathlike (const char *argv0, bool use_env)
{
  const char *pathlike;

  if (NULL == argv0)
    {
      pathlike = get_pathlike_from_env ("PATH");
    }
  else if (use_env)
    {
      pathlike = get_pathlike_from_env (argv0);
    }
  else
    {
      pathlike = argv0;
    }

  return pathlike;
}

static void
pretty_print (const char *pathlike, char delimiter)
{
  const char *begin;
  const char *end;

  for (begin = pathlike, end = pathlike; '\0' != *end; begin = end + 1)
    {
      end = strchrnul (begin, delimiter);

      size_t length = (size_t) (end - begin);
      if (length > 0)
        {
          fwrite (begin, length, 1, stdout);
        }

      putchar ('\n');
    }
}

int
main (int argc, char *argv[])
{
  /*
   * Parse command line options
   */
  prog_options prog_opts;
  options opts;

  parse_args (argc, &argv, &prog_opts, &opts);

  /*
   * Get our pathlike string
   */
  const char *pathlike = get_pathlike (*argv, prog_opts.env);

  /*
   * Run tidypath... or not
   */
  char *tidy;
  if (prog_opts.nop)
    {
      tidy = strdup (pathlike);
    }
  else
    {
      tidy = tidypath (pathlike, &opts);
    }
  check_for_null_alloc (tidy);

  /*
   * Output the result
   */
  if (prog_opts.pretty_print)
    {
      pretty_print (tidy, opts.delimiter);
    }
  else
    {
      fputs (tidy, stdout);
    }

#if FREE_MEMORY
  free (tidy);
#endif

  return EXIT_SUCCESS;
}
