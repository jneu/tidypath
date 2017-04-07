#ifndef TIDYPATH_H
#define TIDYPATH_H

#include <stdbool.h>

typedef struct def_options
{
  bool aggressive;
  bool ignore_empty;
  char delimiter;
} options;

char *tidypath (const char *pathlike, const options * opts);

#endif
