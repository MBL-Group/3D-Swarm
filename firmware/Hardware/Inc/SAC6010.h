#ifndef __SAC6010_H
#define __SAC6010_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "main.h"



uint16_t crc16(uint8_t *buffer, uint16_t buffer_length);
void sac6010_set_output(char address, float current);
void sac6010_set_current_mode(char address);
void sac6010_open_output(char address);
void sac6010_close_output(char address);

uint32_t calculate_current_to_pwm(float current, float current_scale, uint32_t pulse_scale);

void sac6010_read_from_pc(uint8_t* send_list);

extern float coils_current[8];
extern int coils_power;
	 

#ifdef __cplusplus
}
#endif

#endif /* __W25Qx_H */

