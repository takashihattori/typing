/* Time.c
*/

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#ifdef __GNUG__
#include <std.h>
#endif

#include "Time.h"

static long start_time;

void time_start()
{
    time(&start_time);
}

int time_now()
{
    long now_time;
    time(&now_time);

    return now_time - start_time;
}
