#ifndef _ME_STATE_CONTROL_H
#define _ME_STATE_CONTROL_H


#include "LTE_Include.h"
#include "bsp.h"
#include "ME3630_Uart.h"
#include "app_cfg.h"
#include "stm32f10x_type.h"

void MEIint();
void MESetAtState(eAT_State state, bool retflag, uint32_t option);
void METimeTask();
void MERunTask();
#endif // !_ME_STATE_CONTROL_H

