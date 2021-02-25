

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>

extern volatile uint16_t system_flags;
extern volatile uint8_t system_operation_mode;
extern volatile uint32_t timeout_limit_ms;
extern volatile uint32_t timeout_counter_ms;
extern volatile uint16_t this_node_id;

#endif