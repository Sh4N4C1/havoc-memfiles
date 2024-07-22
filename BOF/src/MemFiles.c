#include "bofdefs.h"
#include "beacon.h"
#include "Base.c"

void go(char* argv[], int argc)
{
    if(!bofstart())
        return;


    printoutput(TRUE);
    return;
}
