#include "compat.h"

#include <string.h>

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
