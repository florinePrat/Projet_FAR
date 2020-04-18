#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

int isValid(char *pseudo)
{
    if (strlen(pseudo)>0 && strlen(pseudo)<10)
    {
        if (int regexec("[:alnum:]", pseudo,0, NULL, 0) == 0)
        {
            return 1;
        }
        else
        {
          return 0;
        }
    }
    else
    {
      return 0;
    }
}
