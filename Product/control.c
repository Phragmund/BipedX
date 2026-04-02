#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "control.h"
#include "LQR.h"

// 控制器结构体
typedef struct Controller {
    double dt;              // 控制器时间步长
    Control control;        // 控制输出
    LQRController *lqr;     // LQR控制器
    double wheel_speed_setpoint;  // 车轮速度设定值
} Controller;

// 初始化控制器
Controller* control_init(double dt) {
    Controller *controller = (Controller*)malloc(sizeof(Controller));
    if (!controller) return NULL;
    
    controller->dt = dt;
    controller->control.left_torque = 0.0;
    controller->control.right_torque = 0.0;
    controller->wheel_speed_setpoint = 0.0;
    
    // 初始化LQR控制器
    controller->lqr = lqr_init();
    if (!controller->lqr) {
        free(controller);
        return NULL;
    }
    
    return controller;
}

// 更新控制器状态
void control_update(Controller *controller, SensorData *sensor_data) {
    if (!controller || !sensor_data) return;
    
    // 构建状态向量
    State state;
    state.x = sensor_data->x;
    state.x_dot = sensor_data->x_dot;
    state.theta = sensor_data->theta;
    state.theta_dot = sensor_data->theta_dot;
    
    // 使用LQR算法计算控制量
    lqr_compute_control(controller->lqr, &state, &controller->control);
    
    // 可以在这里添加额外的控制逻辑，例如速度控制
    // 例如：根据车轮速度设定值和实际速度的差异调整控制量
}

// 获取控制输出
void control_get_output(Controller *controller, Control *control) {
    if (!controller || !control) return;
    
    control->left_torque = controller->control.left_torque;
    control->right_torque = controller->control.right_torque;
}

// 设置车轮速度设定值
void control_set_wheel_speed(Controller *controller, double speed) {
    if (controller) {
        controller->wheel_speed_setpoint = speed;
    }
}

// 设置电机扭矩
void control_set_torque(Controller *controller, double left_torque, double right_torque) {
    if (controller) {
        controller->control.left_torque = left_torque;
        controller->control.right_torque = right_torque;
    }
}

// 清理控制器
void control_cleanup(Controller *controller) {
    if (controller) {
        if (controller->lqr) {
            lqr_cleanup(controller->lqr);
        }
        free(controller);
    }
}
