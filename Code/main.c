#include "stdio.h"

#include "Inc/hal.h"

typedef struct
{
    // ---- 由 EE1 负责采集并更新 (硬件输入层) ----
    float imu_pitch;      // 滤波后的俯仰角
    float imu_gyro_y;     // Y轴角速度
    int motor_data;       // 电机数据回传
    float motor_speed_L;  // 左轮实时转速
    float motor_speed_R;  // 右轮实时转速
    float bus_voltage;    // 实时母线电压 (防烧板与能量回收判断依据)
    uint16_t remote_ch_0; // 遥控器数据包,0x00表示没收到
    int remote_delay;     // 遥控器延迟
    int remote_ch_A;      // 遥控器模式切换通道值
    int remote_ch_B;      // 遥控器速度通道值
    int hw_switch_state;  // 手动安全开关状态 (1:ON, 0:OFF)
    int servo_action;     // 舵机or丝杆控制

    // ---- 由 AE1 负责计算并写入 (算法输出层) ----
    float target_torque_L; // 计算出的左轮目标扭矩
    float target_torque_R; // 计算出的右轮目标扭矩
    int energy_regen_en;   // 能量回收使能标志 (0:禁用, 1:启动)
    int fsm_state;         // 系统状态机状态 (用于控制RGB灯色)
    int buzzer_cmd;        // 蜂鸣器控制指令 (0:关, 1:短鸣, 2:急促, 3:长鸣)
} Robot_State_t;

#define _STATE_SELFCHECK 1
#define _STATE_PREPARE 2
#define _STATE_FOOT_RUNNING 3
#define _STATE_WHEEL_RUNNING 4
#define _STATE_FOOT_TO_WHEEL 5
#define _STATE_WHEEL_TO_FOOT 6
#define _STATE_STOP 7
#define _FOOT 0
#define _WHEEL 1

int set_state = 0; // 状态变量
int imu_pitch_fix = 0;
int foot_change_right; // 判断脚部机构位置是否正确
int user_prepared;     // 判断用户是否穿戴完成
Robot_State_t robot_ctrl;

void base_init()
{
    printf("MCU_init\n");
    HAL_Delay(100);
    imu_pitch_fix = -robot_ctrl.imu_pitch; // 传感器零漂补正值

    set_state = _STATE_PREPARE;
    return;
}

void err_warning()
{
    robot_ctrl.fsm_state = 0xFF << 16 | 0x00 << 8 | 0x00; // 设置RGB灯光数据包为(255,0,0)红色
    robot_ctrl.buzzer_cmd = 2;
    HAL_Delay(500);
    robot_ctrl.fsm_state = 0x00 << 16 | 0x00 << 8 | 0x00; // 设置RGB灯光数据包为(0,0,0)黑色
    robot_ctrl.buzzer_cmd = 0;
    HAL_Delay(500);
    robot_ctrl.fsm_state = 0xFF << 16 | 0x00 << 8 | 0x00; // 设置RGB灯光数据包为(255,0,0)红色
    robot_ctrl.buzzer_cmd = 2;
    HAL_Delay(500);

    return;
}

void normal_rect()
{
    robot_ctrl.fsm_state = 0x00 << 16 | 0x00 << 8 | 0xFF; // 设置RGB灯光数据包为(0,0,255)绿色
    HAL_Delay(500);
    robot_ctrl.fsm_state = 0x00 << 16 | 0x00 << 8 | 0x00; // 设置RGB灯光数据包为(0,0,0)黑色
    HAL_Delay(500);
    robot_ctrl.fsm_state = 0x00 << 16 | 0x00 << 8 | 0xFF; // 设置RGB灯光数据包为(0,0,255)绿色
    HAL_Delay(500);
}

void balance_on()
{
    // 开启位置环PID控制平衡
    // 可能使用FreeRTOS的Task实现状态机进程与平衡解算进程并行
    return;
}

void balance_off()
{
    return;
}

void state_selfcheck()
{
    if (robot_ctrl.remote_ch_0 == 0x00 || robot_ctrl.remote_delay > 30 || robot_ctrl.motor_data == 0x00)
    {
        err_warning();
        return;
    }

    set_state = _STATE_PREPARE;
    return;
}

void state_prepare()
{
    if (robot_ctrl.imu_pitch > 30 || robot_ctrl.imu_pitch < 30 || !foot_change_right)
    {
        err_warning();
        return;
    }
    if (user_prepared == 1)
    {
        normal_rect();
        robot_ctrl.servo_action = _FOOT;
        set_state = _STATE_FOOT_RUNNING;
    }
    return;
}

void state_foot_running()
{
    if (robot_ctrl.imu_pitch > 30 || robot_ctrl.imu_pitch < 30)
    {
        err_warning();
        set_state = _STATE_STOP;
        return;
    }
    if (robot_ctrl.remote_ch_A == 1)
    {
        normal_rect();
        robot_ctrl.servo_action = _FOOT;
        set_state = _STATE_FOOT_TO_WHEEL;
    }
    return;
}

void state_foot_to_wheel()
{
    balance_on();
    robot_ctrl.servo_action = _WHEEL;
    normal_rect();
    set_state = _STATE_WHEEL_RUNNING;
    return;
}

void state_wheel_running()
{
    if (robot_ctrl.imu_pitch > 30 || robot_ctrl.imu_pitch < 30)
    {
        err_warning();
        set_state = _STATE_STOP;
        return;
    }
    if (robot_ctrl.remote_ch_A == 1)
    {
        normal_rect();
        robot_ctrl.servo_action = _WHEEL;
        set_state = _STATE_WHEEL_TO_FOOT;
    }
    return;
}

void state_wheel_to_foot()
{
    robot_ctrl.servo_action = _FOOT;
    normal_rect();
    balance_off();
    set_state = _STATE_FOOT_RUNNING;
    return;
}

void state_stop()
{
    robot_ctrl.target_torque_L = 0;
    robot_ctrl.target_torque_R = 0;
    robot_ctrl.energy_regen_en = 1;
    return;
}

int main()
{
    base_init();

    while (1)
    {
        switch (set_state)
        {
        case 0:
        {
            break;
        }
        case _STATE_SELFCHECK:
        {
            state_selfcheck();
            break;
        }
        case _STATE_PREPARE:
        {
            state_prepare();
            break;
        }
        case _STATE_FOOT_RUNNING:
        {
            state_foot_running();
            break;
        }
        case _STATE_WHEEL_RUNNING:
        {
            state_wheel_running();
            break;
        }
        case _STATE_FOOT_TO_WHEEL:
        {
            state_foot_to_wheel();
            break;
        }
        case _STATE_WHEEL_TO_FOOT:
        {
            state_wheel_to_foot();
            break;
        }
        case _STATE_STOP:
        {
            state_stop();
            break;
        }
        default:
            printf("err state\n");
            set_state = _STATE_STOP;
            break;
        }
    }
    return 0;
}
