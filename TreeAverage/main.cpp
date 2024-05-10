#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include "llist.h"

#ifndef NULL
#define NULL (0)
#endif

#define SUPPRESS_WARNING(_x) ((void) _x )

int main(int argc, char** argv)
{
    SUPPRESS_WARNING(argc);
    SUPPRESS_WARNING(argv);

    cout << "hello! test started" << endl;
    return 0;
}
