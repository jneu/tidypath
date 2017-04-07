#include "tidypath.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIDYPATH_VERSION "0.0.2"

#define CHECK_FOR_NULL_ALLOC(p) \
  do \
    { \
      if (NULL == (p)) \
        { \
          fprintf (stderr, "memory allocation failed\n"); \
          exit (EXIT_FAILURE); \
        } \
    } \
  while (0)

static void
show_version (void)
{
  printf ("%s\n", TIDYPATH_VERSION);
}

static void
show_usage (void)
{
  printf ("tidypath ");
  show_version ();

  printf ("tidypath [options] [pathlike_string]\n");
  printf ("    -a, --aggressive\n");
  printf ("    -d, --delimiter <char>\n");
  printf ("    -e, --env\n");
  printf ("    -h, --help             show this help message and exit\n");
  printf ("    -i, --ignore-empty\n");
  printf ("    -l, --ignore-eol\n");
  printf ("    -n, --nop\n");
  printf ("    -p, --pretty-print\n");
  printf ("    -V, --version          show the version of tidypath and exit\n");
}

typedef struct def_prog_options
{
  bool env;
  bool ignore_eol;
  bool nop;
  bool pretty_print;
  bool version;
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
  opts->delimiter = ':';

  int opts_rv;
  while (-1 != (opts_rv = getopt_long (argc, *p_argv, "ad:ehilnpV", longopts, NULL)))
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
        case 'l':
          prog_opts->ignore_eol = true;
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

static char *
get_pathlike_from_env (const char *env_var_name)
{
  const char *env_value = getenv (env_var_name);
  if (NULL == env_value)
    {
      fprintf (stderr, "%s environment variable not found\n", env_var_name);
      exit (EXIT_FAILURE);
    }

  char *pathlike = strdup (env_value);
  CHECK_FOR_NULL_ALLOC (pathlike);

  return pathlike;
}

static char *
get_pathlike (const char *argv0, bool use_env)
{
  char *pathlike;

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
      pathlike = strdup (argv0);
      CHECK_FOR_NULL_ALLOC (pathlike);
    }

  return pathlike;
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
  char *pathlike = get_pathlike (*argv, prog_opts.env);

  /*
   * Run tidypath
   */
  char *tidy = tidypath (pathlike, &opts);
  CHECK_FOR_NULL_ALLOC (tidy);

  return EXIT_SUCCESS;
}
