#ifndef COMMON_H
#define COMMON_H

// 物理常量
#define GRAVITY 9.81  // 重力加速度 (m/s^2)
#define PI 3.14159265359

// 状态向量维度
#define STATE_DIM 4
// 控制向量维度
#define CONTROL_DIM 2

// 状态向量结构
typedef struct {
    double x;          // 位置 (m)
    double x_dot;      // 速度 (m/s)
    double theta;      // 角度 (rad)
    double theta_dot;  // 角速度 (rad/s)
} State;

// 控制向量结构
typedef struct {
    double left_torque;   // 左轮扭矩 (N·m)
    double right_torque;  // 右轮扭矩 (N·m)
} Control;

// 传感器数据结构
typedef struct {
    double wheel_speed;  // 车轮速度 (m/s)
    double theta;        // 车体角度 (rad)
    double theta_dot;    // 车体角速度 (rad/s)
    double x;            // 位置 (m)
    double x_dot;        // 速度 (m/s)
    double x_ddot;       // 加速度 (m/s^2)
} SensorData;

// 时间参数
#define SIM_DT 0.01      // 模拟器时间步长 (s)
#define SENSOR_DT 0.005  // 传感器更新时间步长 (s)
#define CONTROL_DT 0.01  // 控制器更新时间步长 (s)

#endif // COMMON_H
