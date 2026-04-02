#ifndef SIM_H
#define SIM_H

#include "common.h"

// 模拟器结构体（前向声明）
typedef struct Simulator Simulator;

// 初始化模拟器
Simulator* sim_init(double dt, int screen_width, int screen_height);

// 更新模拟器状态
void sim_update(Simulator *sim);

// 渲染平衡车
void sim_render(Simulator *sim);

// 设置控制输入
void sim_set_control(Simulator *sim, double left_torque, double right_torque);

// 获取状态
void sim_get_state(Simulator *sim, State *state);

// 获取加速度
void sim_get_acceleration(Simulator *sim, double *acceleration);

// 获取传感器数据
void sim_get_sensor_data(Simulator *sim, SensorData *data);

// 检查SDL是否成功初始化
int sim_is_initialized(Simulator *sim);

// 清理模拟器
void sim_cleanup(Simulator *sim);

#endif // SIM_H
