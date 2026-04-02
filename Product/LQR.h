#ifndef LQR_H
#define LQR_H

#include "common.h"

// LQR控制器结构体（前向声明）
typedef struct LQRController LQRController;

// 初始化LQR控制器
LQRController* lqr_init(void);

// 计算LQR控制量
void lqr_compute_control(LQRController *lqr, State *state, Control *control);

// 清理LQR控制器
void lqr_cleanup(LQRController *lqr);

#endif // LQR_H
