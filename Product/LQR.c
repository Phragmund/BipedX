#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "LQR.h"

// LQR控制器结构体
typedef struct LQRController
{
    // 状态权重矩阵 Q
    double Q[STATE_DIM][STATE_DIM];
    // 控制权重矩阵 R
    double R[CONTROL_DIM][CONTROL_DIM];
    // 反馈增益矩阵 K
    double K[CONTROL_DIM][STATE_DIM];
} LQRController;

// 初始化LQR控制器
LQRController *lqr_init(void)
{
    LQRController *lqr = (LQRController *)malloc(sizeof(LQRController));
    if (!lqr)
        return NULL;

    // 初始化状态权重矩阵 Q
    // 权重越大，对应的状态变量越重要
    lqr->Q[0][0] = 1000.0; // 位置权重（增加到100，使平衡车更倾向于保持在原地）
    lqr->Q[1][1] = 200.0;  // 速度权重
    lqr->Q[2][2] = 100.0; // 角度权重（最重要）
    lqr->Q[3][3] = 10.0;  // 角速度权重

    // 初始化控制权重矩阵 R
    // 权重越大，控制输入越受限制
    lqr->R[0][0] = 0.1; // 左轮扭矩权重
    lqr->R[1][1] = 0.1; // 右轮扭矩权重

    // 初始化反馈增益矩阵 K
    // 这里使用预计算的增益值，实际应用中应该通过求解Riccati方程得到
    // 这些值是针对特定平衡车模型的示例值
    lqr->K[0][0] = 20.0;  // 位置反馈增益（增加到20，增强位置控制）
    lqr->K[0][1] = 10.0;  // 速度反馈增益
    lqr->K[0][2] = 200.0; // 角度反馈增益（增加到200，提高角度响应速度）
    lqr->K[0][3] = 50.0;  // 角速度反馈增益（增加到50，提高角速度响应速度）

    lqr->K[1][0] = 20.0;  // 位置反馈增益（增加到20，增强位置控制）
    lqr->K[1][1] = 10.0;  // 速度反馈增益
    lqr->K[1][2] = 200.0; // 角度反馈增益（增加到200，提高角度响应速度）
    lqr->K[1][3] = 50.0;  // 角速度反馈增益（增加到50，提高角速度响应速度）

    return lqr;
}

// 计算LQR控制量
void lqr_compute_control(LQRController *lqr, State *state, Control *control)
{
    if (!lqr || !state || !control)
        return;

    // 计算控制量：u = K * x（对于倒立摆系统）
    double left_torque = 0.0;
    double right_torque = 0.0;

    left_torque += lqr->K[0][0] * state->x;
    left_torque += lqr->K[0][1] * state->x_dot;
    left_torque += lqr->K[0][2] * state->theta;
    left_torque += lqr->K[0][3] * state->theta_dot;

    right_torque += lqr->K[1][0] * state->x;
    right_torque += lqr->K[1][1] * state->x_dot;
    right_torque += lqr->K[1][2] * state->theta;
    right_torque += lqr->K[1][3] * state->theta_dot;

    // 限制控制量的范围
    double max_torque = 10.0; // 最大扭矩（增加到10 N·m）
    if (left_torque > max_torque)
        left_torque = max_torque;
    if (left_torque < -max_torque)
        left_torque = -max_torque;
    if (right_torque > max_torque)
        right_torque = max_torque;
    if (right_torque < -max_torque)
        right_torque = -max_torque;

    control->left_torque = left_torque;
    control->right_torque = right_torque;
}

// 清理LQR控制器
void lqr_cleanup(LQRController *lqr)
{
    if (lqr)
    {
        free(lqr);
    }
}
