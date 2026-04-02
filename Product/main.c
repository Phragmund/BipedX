#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <SDL2/SDL.h>
#include "common.h"
#include "sim.h"
#include "control.h"
#include "sensor.h"

int main(int argc, char *argv[])
{
    // 初始化模拟器
    Simulator *sim = sim_init(SIM_DT, 800, 600);
    if (!sim)
    {
        fprintf(stderr, "Failed to initialize simulator\n");
        return 1;
    }

    // 输出模拟器初始化信息
    fprintf(stdout, "Simulator initialized successfully\n");

    // 初始化控制器
    Controller *controller = control_init(CONTROL_DT);
    if (!controller)
    {
        fprintf(stderr, "Failed to initialize controller\n");
        sim_cleanup(sim);
        return 1;
    }

    // 初始化传感器
    Sensor *sensor = sensor_init(SENSOR_DT);
    if (!sensor)
    {
        fprintf(stderr, "Failed to initialize sensor\n");
        control_cleanup(controller);
        sim_cleanup(sim);
        return 1;
    }

    // 主循环
    int running = 1;
    double time = 0.0;
    double sensor_time = 0.0;
    double control_time = 0.0;
    SDL_Event event;

    while (running)
    {
        // 处理SDL事件
        if (sim_is_initialized(sim))
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    running = 0;
                }
            }
        }

        // 更新模拟器状态
        sim_update(sim);

        // 更新传感器数据（每SENSOR_DT时间步）
        sensor_time += SIM_DT;
        if (sensor_time >= SENSOR_DT)
        {
            State state;
            sim_get_state(sim, &state);
            double acceleration;
            sim_get_acceleration(sim, &acceleration);
            sensor_update(sensor, &state, acceleration);
            sensor_time = 0.0;
        }

        // 更新控制器状态（每CONTROL_DT时间步）
        control_time += SIM_DT;
        if (control_time >= CONTROL_DT)
        {
            SensorData sensor_data;
            sensor_get_data(sensor, &sensor_data);
            control_update(controller, &sensor_data);

            // 应用控制输入
            Control control;
            control_get_output(controller, &control);
            sim_set_control(sim, control.left_torque, control.right_torque);

            // 输出控制信息
            fprintf(stdout, "Time: %.2f, Theta: %.4f, Torque: %.4f\n", time, sensor_data.theta, control.left_torque);

            control_time = 0.0;
        }

        // 渲染
        sim_render(sim);

        // 延迟（使用SDL_Delay，更加精确）
        SDL_Delay((Uint32)(SIM_DT * 1000));
        time += SIM_DT;

        // 运行10秒后退出
        if (time >= 10.0)
        {
            running = 0;
        }
    }

    // 清理资源
    sensor_cleanup(sensor);
    control_cleanup(controller);
    sim_cleanup(sim);

    return 0;
}
