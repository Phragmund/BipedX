#ifndef SENSOR_H
#define SENSOR_H

#include "common.h"

// 传感器结构体（前向声明）
typedef struct Sensor Sensor;

// 初始化传感器
Sensor* sensor_init(double dt);

// 更新传感器数据
void sensor_update(Sensor *sensor, State *state, double acceleration);

// 获取传感器数据
void sensor_get_data(Sensor *sensor, SensorData *data);

// 清理传感器
void sensor_cleanup(Sensor *sensor);

#endif // SENSOR_H
