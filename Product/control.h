#ifndef CONTROL_H
#define CONTROL_H

#include "common.h"

// 控制器结构体（前向声明）
typedef struct Controller Controller;

// 初始化控制器
Controller *control_init(double dt);

// 更新控制器状态
void control_update(Controller *controller, SensorData *sensor_data);

// 获取控制输出
void control_get_output(Controller *controller, Control *control);

// 设置车轮速度设定值
void control_set_wheel_speed(Controller *controller, double speed);

// 设置电机扭矩
void control_set_torque(Controller *controller, double left_torque, double right_torque);

// 清理控制器
void control_cleanup(Controller *controller);

#endif // CONTROL_H
