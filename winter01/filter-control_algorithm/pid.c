#include <stdio.h>
#include <stdlib.h>

// PID控制器参数结构体（封装所有参数，方便多通道PID控制）
typedef struct {
    // 1. PID核心系数
    float Kp;   // 比例系数
    float Ki;   // 积分系数
    float Kd;   // 微分系数

    // 2. 过程变量（位置式需要）
    float set_val;  // 设定值（目标值）
    float cur_val;  // 当前值（反馈值）
    float err;      // 本次误差
    float last_err; // 上一次误差
    float prev_err; // 上上次误差（增量式需要）
    float integral; // 积分累积值（位置式需要）

    // 3. 限幅保护（防止溢出/执行机构过载）
    float integral_limit; // 积分限幅（避免积分饱和）
    float output_limit;   // 输出限幅（执行机构最大/最小输出）

    // 4. 输出值
    float pid_out;    // 位置式PID输出（绝对量）
    float pid_inc_out;// 增量式PID输出（增量）
} PID_TypeDef;

/**
 * @brief  PID控制器初始化函数
 * @param  pid: PID结构体指针
 * @param  Kp: 比例系数
 * @param  Ki: 积分系数
 * @param  Kd: 微分系数
 * @param  integral_limit: 积分限幅值
 * @param  output_limit: 输出限幅值
 * @retval 无
 */
void PID_Init(PID_TypeDef *pid, float Kp, float Ki, float Kd, 
              float integral_limit, float output_limit)
{
    if (pid == NULL) return;
    // 初始化核心系数
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    // 初始化限幅
    pid->integral_limit = integral_limit;
    pid->output_limit = output_limit;
    // 初始化过程变量（清零，避免初始误差）
    pid->set_val = 0.0f;
    pid->cur_val = 0.0f;
    pid->err = 0.0f;
    pid->last_err = 0.0f;
    pid->prev_err = 0.0f;
    pid->integral = 0.0f;
    // 初始化输出
    pid->pid_out = 0.0f;
    pid->pid_inc_out = 0.0f;
}

/**
 * @brief  位置式PID计算函数
 * @param  pid: PID结构体指针
 * @param  set_val: 设定值（目标值）
 * @param  cur_val: 当前值（反馈值）
 * @retval 位置式PID输出值（绝对量）
 */
float PID_Pos_Calc(PID_TypeDef *pid, float set_val, float cur_val)
{
    if (pid == NULL) return 0.0f;
    // 更新设定值和当前值
    pid->set_val = set_val;
    pid->cur_val = cur_val;
    // 计算本次误差
    pid->err = pid->set_val - pid->cur_val;

    // 1. 积分项计算 + 积分限幅（避免积分饱和）
    pid->integral += pid->err;
    // 积分值超过上限，限制为上限；低于下限，限制为下限
    if (pid->integral > pid->integral_limit) {
        pid->integral = pid->integral_limit;
    } else if (pid->integral < -pid->integral_limit) {
        pid->integral = -pid->integral_limit;
    }

    // 2. 位置式PID核心公式：u(k) = Kp*e(k) + Ki*∑e(i) + Kd*[e(k)-e(k-1)]
    pid->pid_out = pid->Kp * pid->err + 
                   pid->Ki * pid->integral + 
                   pid->Kd * (pid->err - pid->last_err);

    // 3. 输出限幅（执行机构保护）
    if (pid->pid_out > pid->output_limit) {
        pid->pid_out = pid->output_limit;
    } else if (pid->pid_out < -pid->output_limit) {
        pid->pid_out = -pid->output_limit;
    }

    // 4. 更新上一次误差，为下一次计算做准备
    pid->last_err = pid->err;

    return pid->pid_out;
}

/**
 * @brief  增量式PID计算函数
 * @param  pid: PID结构体指针
 * @param  set_val: 设定值（目标值）
 * @param  cur_val: 当前值（反馈值）
 * @retval 增量式PID输出值（增量）
 */
float PID_Inc_Calc(PID_TypeDef *pid, float set_val, float cur_val)
{
    if (pid == NULL) return 0.0f;
    // 更新设定值和当前值
    pid->set_val = set_val;
    pid->cur_val = cur_val;
    // 计算本次误差
    pid->err = pid->set_val - pid->cur_val;

    // 1. 增量式PID核心公式：Δu(k) = Kp*[e(k)-2e(k-1)+e(k-2)] + Ki*e(k) + Kd*[e(k)-2e(k-1)+e(k-2)]
    // 简化：Δu(k) = Kp*Δe + Ki*e(k) + Kd*Δ²e
    pid->pid_inc_out = pid->Kp * (pid->err - pid->last_err) + 
                       pid->Ki * pid->err + 
                       pid->Kd * (pid->err - 2*pid->last_err + pid->prev_err);

    // 2. 输出限幅（增量限幅，避免单次调节幅度过大）
    if (pid->pid_inc_out > pid->output_limit) {
        pid->pid_inc_out = pid->output_limit;
    } else if (pid->pid_inc_out < -pid->output_limit) {
        pid->pid_inc_out = -pid->output_limit;
    }

    // 3. 更新误差序列：上上次→上一次，上一次→本次
    pid->prev_err = pid->last_err;
    pid->last_err = pid->err;

    return pid->pid_inc_out;
}

// 测试示例：模拟电机转速控制（目标转速1000rpm，反馈值逐步逼近）
int main(void)
{
    // 1. 定义并初始化PID控制器（位置式）
    PID_TypeDef pid_pos;
    // 参数：Kp=2.0, Ki=0.5, Kd=0.1，积分限幅50，输出限幅200（模拟PWM占空比0-200）
    PID_Init(&pid_pos, 2.0f, 0.5f, 0.1f, 50.0f, 200.0f);

    // 2. 定义并初始化PID控制器（增量式）
    PID_TypeDef pid_inc;
    // 增量式参数可适当调整，输出限幅50（单次PWM增量不超过50）
    PID_Init(&pid_inc, 2.0f, 0.5f, 0.1f, 50.0f, 50.0f);

    // 模拟反馈值（电机转速），初始0rpm，逐步逼近目标1000rpm
    float cur_speed = 0.0f;
    // 目标转速
    float target_speed = 1000.0f;
    // 模拟迭代次数（控制周期）
    int i;

    printf("===== 位置式PID测试（目标转速1000rpm）=====\n");
    printf("迭代次数\t当前转速\tPID输出\n");
    for (i = 0; i < 50; i++) {
        // 位置式PID计算，输出控制量（如PWM占空比）
        float pos_out = PID_Pos_Calc(&pid_pos, target_speed, cur_speed);
        // 模拟电机响应：控制量驱动转速上升（简单比例模拟，实际需根据硬件调整）
        cur_speed += pos_out * 0.8f;
        // 打印结果
        printf("%d\t\t%.1f\t\t%.1f\n", i+1, cur_speed, pos_out);
    }

    // 重置当前转速，测试增量式
    cur_speed = 0.0f;
    printf("\n===== 增量式PID测试（目标转速1000rpm）=====\n");
    printf("迭代次数\t当前转速\tPID增量输出\n");
    // 增量式需要累积输出（因为输出是增量）
    float inc_total_out = 0.0f;
    for (i = 0; i < 50; i++) {
        // 增量式PID计算，输出增量
        float inc_out = PID_Inc_Calc(&pid_inc, target_speed, cur_speed);
        // 累积增量得到总控制量
        inc_total_out += inc_out;
        // 模拟电机响应
        cur_speed += inc_total_out * 0.08f;
        // 打印结果
        printf("%d\t\t%.1f\t\t%.1f\n", i+1, cur_speed, inc_out);
    }

    return 0;
}