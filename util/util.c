#include <stdio.h>
#include <stdlib.h>
#include "../include/util.h"

void DieWithError(char *errorMessage)
{
  perror(errorMessage);
  exit(1);
}
