#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "sensor.h"

// 传感器结构体
typedef struct Sensor
{
    double dt;               // 传感器更新时间步长
    SensorData data;         // 传感器数据
    double last_update_time; // 上一次更新的时间
} Sensor;

// 初始化传感器
Sensor *sensor_init(double dt)
{
    Sensor *sensor = (Sensor *)malloc(sizeof(Sensor));
    if (!sensor)
        return NULL;

    sensor->dt = dt;
    sensor->last_update_time = 0.0;

    // 初始化传感器数据
    sensor->data.wheel_speed = 0.0;
    sensor->data.theta = 0.0;
    sensor->data.theta_dot = 0.0;
    sensor->data.x = 0.0;
    sensor->data.x_dot = 0.0;
    sensor->data.x_ddot = 0.0;

    return sensor;
}

// 更新传感器数据
void sensor_update(Sensor *sensor, State *state, double acceleration)
{
    if (!sensor || !state)
        return;

    // 更新传感器数据
    sensor->data.wheel_speed = state->x_dot;
    sensor->data.theta = state->theta;
    sensor->data.theta_dot = state->theta_dot;
    sensor->data.x = state->x;
    sensor->data.x_dot = state->x_dot;
    sensor->data.x_ddot = acceleration;

    // 更新最后更新时间
    sensor->last_update_time += sensor->dt;
}

// 获取传感器数据
void sensor_get_data(Sensor *sensor, SensorData *data)
{
    if (!sensor || !data)
        return;

    data->wheel_speed = sensor->data.wheel_speed;
    data->theta = sensor->data.theta;
    data->theta_dot = sensor->data.theta_dot;
    data->x = sensor->data.x;
    data->x_dot = sensor->data.x_dot;
}

// 清理传感器
void sensor_cleanup(Sensor *sensor)
{
    if (sensor)
    {
        free(sensor);
    }
}
