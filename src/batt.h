#ifndef _BATT_H__
#define _BATT_H__

#include <stdint.h>

void sample_batt_level();
int32_t get_avg_batt_voltage();
bool is_avg_batt_voltage_ready();
#endif /* _BATT_H__ */
