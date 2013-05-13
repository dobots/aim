#include "ov7670.h"

#define PORT_EXP_ADDR 0x27

int cam_init(void);
int cam_select(int cam);
int cam_powerdown_all(void);
int cam_brightness( signed int val);
int cam_frequency( signed int val);
