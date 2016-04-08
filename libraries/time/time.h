// Abstract: Time library header file.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <system/system.h>

extern unsigned int time_to_year(time_type time);
extern unsigned int time_to_month(time_type time);
extern unsigned int time_to_day(time_type time);

extern unsigned int time_to_hours(time_type time);
extern unsigned int time_to_minutes(time_type time);
extern unsigned int time_to_seconds(time_type time);

extern time_type time_get(void);
