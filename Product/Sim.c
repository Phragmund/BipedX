#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "common.h"
#include "sim.h"

// 模拟器参数
typedef struct Simulator
{
    // 时间步长
    double dt;

    // 平衡车参数
    double wheel_radius;  // 轮子半径 (m)
    double wheel_mass;    // 轮子质量 (kg)
    double wheel_density; // 轮子密度 (kg/m^3)
    double body_mass;     // 车体质量 (kg)
    double body_length;   // 车体长度 (m)
    double body_density;  // 车体密度 (kg/m^3)
    double com_height;    // 质心高度 (m)

    // 材质参数
    double wheel_material;  // 车轮表面材质摩擦系数
    double ground_material; // 地面材质摩擦系数

    // 物理参数
    double wheel_inertia; // 轮子转动惯量 (kg·m^2)
    double body_inertia;  // 车体转动惯量 (kg·m^2)
    double friction_coef; // 摩擦系数

    // 状态变量
    double x;                 // 位置 (m)
    double x_dot;             // 速度 (m/s)
    double x_ddot;            // 加速度 (m/s^2)
    double theta;             // 角度 (rad)
    double theta_dot;         // 角速度 (rad/s)
    double wheel_angle;       // 轮子角度 (rad)
    double wheel_angular_vel; // 轮子角速度 (rad/s)

    // 控制输入
    double left_torque;  // 左轮扭矩 (N·m)
    double right_torque; // 右轮扭矩 (N·m)

    // 可视化参数
    int screen_width;         // 屏幕宽度
    int screen_height;        // 屏幕高度
    double scale;             // 缩放比例
    SDL_Window *window;       // SDL窗口
    SDL_Renderer *renderer;   // SDL渲染器
    SDL_GLContext gl_context; // OpenGL上下文
} Simulator;

// 初始化模拟器
Simulator *sim_init(double dt, int screen_width, int screen_height)
{
    Simulator *sim = (Simulator *)malloc(sizeof(Simulator));
    if (!sim)
        return NULL;

    // 初始化时间步长
    sim->dt = dt;

    // 初始化平衡车参数
    sim->wheel_radius = 0.05;  // 5cm
    sim->wheel_mass = 0.5;     // 0.5kg
    sim->wheel_density = 7850; // 钢铁密度
    sim->body_mass = 1.0;      // 1kg
    sim->body_length = 0.5;    // 50cm
    sim->body_density = 1000;  // 水的密度
    sim->com_height = 0.3;     // 质心高度30cm

    // 初始化材质参数
    sim->wheel_material = 0.8;  // 橡胶
    sim->ground_material = 0.6; // 混凝土

    // 计算转动惯量
    sim->wheel_inertia = 0.5 * sim->wheel_mass * sim->wheel_radius * sim->wheel_radius;
    sim->body_inertia = (1.0 / 12.0) * sim->body_mass * sim->body_length * sim->body_length;

    // 计算摩擦系数
    sim->friction_coef = sqrt(sim->wheel_material * sim->ground_material);

    // 初始化状态变量
    sim->x = 0.0;
    sim->x_dot = 0.0;
    sim->x_ddot = 0.0;
    sim->theta = 0.1; // 初始角度偏移
    sim->theta_dot = 0.0;
    sim->wheel_angle = 0.0;
    sim->wheel_angular_vel = 0.0;

    // 初始化控制输入
    sim->left_torque = 0.0;
    sim->right_torque = 0.0;

    // 初始化可视化参数
    sim->screen_width = screen_width;
    sim->screen_height = screen_height;
    sim->scale = 200.0; // 1m = 200像素

    // 尝试初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        // 继续运行，只是没有可视化界面
        sim->window = NULL;
        sim->renderer = NULL;
        sim->gl_context = NULL;
    }
    else
    {
        // 创建支持OpenGL的窗口
        sim->window = SDL_CreateWindow("平衡车仿真模拟器", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        if (!sim->window)
        {
            fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
            // 继续运行，只是没有可视化界面
            sim->renderer = NULL;
            sim->gl_context = NULL;
        }
        else
        {
            // 创建OpenGL上下文
            sim->gl_context = SDL_GL_CreateContext(sim->window);
            if (!sim->gl_context)
            {
                fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
                // 继续运行，只是没有可视化界面
                SDL_DestroyWindow(sim->window);
                sim->window = NULL;
                sim->renderer = NULL;
            }
            else
            {
                // 初始化OpenGL
                glViewport(0, 0, screen_width, screen_height);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(45.0, (double)screen_width / (double)screen_height, 0.1, 100.0);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_LIGHTING);
                glEnable(GL_LIGHT0);
            }
        }
    }

    return sim;
}

// 更新模拟器状态
void sim_update(Simulator *sim)
{
    // 计算合力和合力矩
    double total_mass = sim->body_mass + 2 * sim->wheel_mass;
    double com_x = sim->x;
    double com_y = sim->com_height;

    // 重力
    double gravity_force = total_mass * GRAVITY;

    // 摩擦力
    double friction_force = sim->friction_coef * gravity_force;

    // 计算加速度
    double control_torque = (sim->left_torque + sim->right_torque) / 2.0;
    double linear_acceleration = control_torque / (sim->wheel_radius * total_mass);
    if (sim->x_dot != 0)
    {
        double friction_acceleration = friction_force / total_mass * (-sim->x_dot / fabs(sim->x_dot));
        linear_acceleration += friction_acceleration;
    }
    sim->x_ddot = linear_acceleration;

    // 计算角加速度
    double gravity_torque = sim->body_mass * GRAVITY * sim->com_height * sin(sim->theta);
    double inertial_torque = sim->body_mass * sim->com_height * linear_acceleration * cos(sim->theta);
    double total_torque = gravity_torque - inertial_torque;
    double angular_acceleration = total_torque / (sim->body_inertia + sim->body_mass * sim->com_height * sim->com_height);

    // 更新状态
    sim->x_dot += linear_acceleration * sim->dt;
    sim->x += sim->x_dot * sim->dt;

    sim->theta_dot += angular_acceleration * sim->dt;
    sim->theta += sim->theta_dot * sim->dt;

    sim->wheel_angular_vel = sim->x_dot / sim->wheel_radius;
    sim->wheel_angle += sim->wheel_angular_vel * sim->dt;
}

// 渲染平衡车
void sim_render(Simulator *sim)
{
    // 如果没有OpenGL上下文，直接返回
    if (!sim || !sim->window || !sim->gl_context)
    {
        return;
    }

    // 清除颜色和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 设置相机位置
    gluLookAt(1.0, 0.5, 2.0,  // 相机位置
              0.0, 0.0, 0.0,  // 观察点
              0.0, 1.0, 0.0); // 向上向量

    // 绘制地面
    glBegin(GL_QUADS);
    glColor3f(0.6, 0.6, 0.6);
    glVertex3f(-5.0, -0.05, -5.0);
    glVertex3f(5.0, -0.05, -5.0);
    glVertex3f(5.0, -0.05, 5.0);
    glVertex3f(-5.0, -0.05, 5.0);
    glEnd();

    // 绘制网格线
    glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_LINES);
    for (float i = -5.0; i <= 5.0; i += 1.0)
    {
        glVertex3f(i, -0.05, -5.0);
        glVertex3f(i, -0.05, 5.0);
        glVertex3f(-5.0, -0.05, i);
        glVertex3f(5.0, -0.05, i);
    }
    glEnd();

    // 移动到平衡车位置
    glTranslatef(sim->x, 0.0, 0.0);

    // 绘制左轮
    glPushMatrix();
    glTranslatef(-0.1, 0.0, 0.0);
    glColor3f(0.3, 0.3, 0.3);
    GLUquadricObj *wheel1 = gluNewQuadric();
    gluCylinder(wheel1, sim->wheel_radius, sim->wheel_radius, 0.05, 32, 32);
    gluDeleteQuadric(wheel1);
    glPopMatrix();

    // 绘制右轮
    glPushMatrix();
    glTranslatef(0.1, 0.0, 0.0);
    glColor3f(0.3, 0.3, 0.3);
    GLUquadricObj *wheel2 = gluNewQuadric();
    gluCylinder(wheel2, sim->wheel_radius, sim->wheel_radius, 0.05, 32, 32);
    gluDeleteQuadric(wheel2);
    glPopMatrix();

    // 绘制车身底座
    glPushMatrix();
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex3f(-0.15, sim->wheel_radius, -0.1);
    glVertex3f(0.15, sim->wheel_radius, -0.1);
    glVertex3f(0.15, sim->wheel_radius, 0.1);
    glVertex3f(-0.15, sim->wheel_radius, 0.1);
    glEnd();
    glPopMatrix();

    // 绘制车体（考虑倾斜角度）
    glPushMatrix();
    glTranslatef(0.0, sim->wheel_radius, 0.0);
    glRotatef(sim->theta * 180.0 / PI, 0.0, 0.0, 1.0);
    glColor3f(0.0, 0.0, 0.8);
    glBegin(GL_QUADS);
    // 前面
    glVertex3f(-0.05, 0.0, 0.05);
    glVertex3f(0.05, 0.0, 0.05);
    glVertex3f(0.05, sim->body_length, 0.05);
    glVertex3f(-0.05, sim->body_length, 0.05);
    // 后面
    glVertex3f(-0.05, 0.0, -0.05);
    glVertex3f(0.05, 0.0, -0.05);
    glVertex3f(0.05, sim->body_length, -0.05);
    glVertex3f(-0.05, sim->body_length, -0.05);
    // 左面
    glVertex3f(-0.05, 0.0, -0.05);
    glVertex3f(-0.05, 0.0, 0.05);
    glVertex3f(-0.05, sim->body_length, 0.05);
    glVertex3f(-0.05, sim->body_length, -0.05);
    // 右面
    glVertex3f(0.05, 0.0, -0.05);
    glVertex3f(0.05, 0.0, 0.05);
    glVertex3f(0.05, sim->body_length, 0.05);
    glVertex3f(0.05, sim->body_length, -0.05);
    glEnd();
    glPopMatrix();

    // 绘制质心
    glPushMatrix();
    glTranslatef(0.0, sim->wheel_radius + sim->com_height * cos(sim->theta), sim->com_height * sin(sim->theta));
    glColor3f(1.0, 0.0, 0.0);
    GLUquadricObj *com = gluNewQuadric();
    gluSphere(com, 0.03, 16, 16);
    gluDeleteQuadric(com);
    glPopMatrix();

    // 刷新屏幕
    SDL_GL_SwapWindow(sim->window);
}

// 设置控制输入
void sim_set_control(Simulator *sim, double left_torque, double right_torque)
{
    sim->left_torque = left_torque;
    sim->right_torque = right_torque;
}

// 获取状态
void sim_get_state(Simulator *sim, State *state)
{
    if (sim && state)
    {
        state->x = sim->x;
        state->x_dot = sim->x_dot;
        state->theta = sim->theta;
        state->theta_dot = sim->theta_dot;
    }
}

// 获取加速度
void sim_get_acceleration(Simulator *sim, double *acceleration)
{
    if (sim && acceleration)
    {
        *acceleration = sim->x_ddot;
    }
}

// 获取传感器数据
void sim_get_sensor_data(Simulator *sim, SensorData *data)
{
    if (sim && data)
    {
        data->wheel_speed = sim->x_dot;
        data->theta = sim->theta;
        data->theta_dot = sim->theta_dot;
        data->x = sim->x;
        data->x_dot = sim->x_dot;
        data->x_ddot = sim->x_ddot;
    }
}

// 检查SDL是否成功初始化
int sim_is_initialized(Simulator *sim)
{
    return (sim && sim->window && sim->gl_context);
}

// 清理模拟器
void sim_cleanup(Simulator *sim)
{
    if (sim)
    {
        if (sim->gl_context)
        {
            SDL_GL_DeleteContext(sim->gl_context);
        }
        if (sim->renderer)
        {
            SDL_DestroyRenderer(sim->renderer);
        }
        if (sim->window)
        {
            SDL_DestroyWindow(sim->window);
        }
        SDL_Quit();
        free(sim);
    }
}

// 主函数（用于测试）已移至main.c
