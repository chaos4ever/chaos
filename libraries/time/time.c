// Abstract: Time/date library.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2015-2016 chaos development

#include "time.h"

// Get the year number for the given chaos time.
unsigned int time_to_year(time_type time)
{
    return time;
}

// Get the month number for the given chaos time.
unsigned int time_to_month(time_type time)
{
    return time;
}

// Get the day of month number for the given chaos time.
unsigned int time_to_day(time_type time)
{
    return time;
}

// Get the hours count for the given chaos time.
unsigned int time_to_hours(time_type time)
{
    return ((time / 3600) % 24);
}

// Get the minutes count for the given chaos time.
unsigned int time_to_minutes(time_type time)
{
    return ((time / 60) % 60);
}

// Get the seconds count for the given chaos time.
unsigned int time_to_seconds(time_type time)
{
    return (time % 60);
}

// Get the time.
time_type time_get(void)
{
    kernelfs_time_type kernelfs_time;

    kernelfs_time.kernelfs_class = KERNELFS_CLASS_TIME_READ;
    system_call_kernelfs_entry_read(&kernelfs_time);

    return kernelfs_time.time;
}
