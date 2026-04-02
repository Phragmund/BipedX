#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>
#include <math.h>

// 实体结构体
typedef struct
{
    float position[3];         // 位置坐标
    float size[3];             // 实体体积
    float material[3];         // 实体表面材质 (RGB)
    float static_friction;     // 静摩擦力
    float kinetic_friction;    // 滑动摩擦力
    float rolling_friction;    // 滚动摩擦力
    float density;             // 实体密度
    float center_of_mass[3];   // 质心位置
    float inertia[3];          // 转动惯量 (x, y, z轴)
    float velocity[3];         // 速度
    float angular_velocity[3]; // 角速度
} Entity;

// 平衡车结构体
typedef struct
{
    float position[3];           // 位置坐标
    float orientation;           // 朝向角度（弧度）
    float wheel_radius;          // 轮子半径
    float wheel_width;           // 轮子宽度
    float axle_length;           // 轮轴长度
    float pole_length;           // 竖直杆长度
    float pole_radius;           // 竖直杆半径
    float material[3];           // 材质颜色
    float velocity;              // 前进速度
    float angular_velocity;      // 角速度
    float pole_angle;            // 杆的倾斜角度
    float pole_angular_velocity; // 杆的角速度
} BalanceCar;

// 全局变量
Entity ground;
BalanceCar balance_car;
float camera_position[3] = {10.0f, 10.0f, 10.0f}; // 斜45度朝向地面
float camera_look_at[3] = {0.0f, 0.0f, 0.0f};
float camera_yaw = -45.0f;  // 相机偏航角
float camera_pitch = 30.0f; // 相机俯仰角

// SDL变量
SDL_Window *window = NULL;
SDL_GLContext gl_context = NULL;

// 初始化函数
void init()
{
    // 设置背景颜色
    glClearColor(0.8f, 0.8f, 0.9f, 1.0f);

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 初始化地面实体
    ground.position[0] = 0.0f;
    ground.position[1] = -5.0f;
    ground.position[2] = 0.0f;
    ground.size[0] = 1000.0f; // 增大地面尺寸
    ground.size[1] = 1.0f;
    ground.size[2] = 1000.0f;
    ground.material[0] = 0.7f;
    ground.material[1] = 0.7f;
    ground.material[2] = 0.7f;
    ground.static_friction = 0.5f;
    ground.kinetic_friction = 0.3f;
    ground.rolling_friction = 0.1f;
    ground.density = 1.0f;
    ground.center_of_mass[0] = 0.0f;
    ground.center_of_mass[1] = 0.0f;
    ground.center_of_mass[2] = 0.0f;
    ground.inertia[0] = 0.0f;
    ground.inertia[1] = 0.0f;
    ground.inertia[2] = 0.0f;
    ground.velocity[0] = 0.0f;
    ground.velocity[1] = 0.0f;
    ground.velocity[2] = 0.0f;
    ground.angular_velocity[0] = 0.0f;
    ground.angular_velocity[1] = 0.0f;
    ground.angular_velocity[2] = 0.0f;

    // 初始化平衡车
    balance_car.position[0] = 0.0f;
    balance_car.position[1] = -5.0f + 0.5f; // 位于地面上方
    balance_car.position[2] = 0.0f;
    balance_car.orientation = 0.0f;
    balance_car.wheel_radius = 0.5f;
    balance_car.wheel_width = 0.2f;
    balance_car.axle_length = 1.0f;
    balance_car.pole_length = 1.5f;
    balance_car.pole_radius = 0.05f;
    balance_car.material[0] = 1.0f;
    balance_car.material[1] = 1.0f;
    balance_car.material[2] = 1.0f;
    balance_car.velocity = 0.0f;
    balance_car.angular_velocity = 0.0f;
    balance_car.pole_angle = 0.0f;
    balance_car.pole_angular_velocity = 0.0f;
}

// 实现简单的相机函数
void setCamera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
{
    // 计算视线方向
    float f[3];
    f[0] = centerX - eyeX;
    f[1] = centerY - eyeY;
    f[2] = centerZ - eyeZ;

    // 归一化
    float len = sqrt(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
    f[0] /= len;
    f[1] /= len;
    f[2] /= len;

    // 计算右向量
    float s[3];
    s[0] = f[1] * upZ - f[2] * upY;
    s[1] = f[2] * upX - f[0] * upZ;
    s[2] = f[0] * upY - f[1] * upX;

    // 归一化
    len = sqrt(s[0] * s[0] + s[1] * s[1] + s[2] * s[2]);
    s[0] /= len;
    s[1] /= len;
    s[2] /= len;

    // 计算上向量
    float u[3];
    u[0] = s[1] * f[2] - s[2] * f[1];
    u[1] = s[2] * f[0] - s[0] * f[2];
    u[2] = s[0] * f[1] - s[1] * f[0];

    // 构建视图矩阵
    float viewMatrix[16] = {
        s[0], u[0], -f[0], 0.0f,
        s[1], u[1], -f[1], 0.0f,
        s[2], u[2], -f[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};

    // 应用平移
    float translateMatrix[16] = {
        1.0f, 0.0f, 0.0f, -eyeX,
        0.0f, 1.0f, 0.0f, -eyeY,
        0.0f, 0.0f, 1.0f, -eyeZ,
        0.0f, 0.0f, 0.0f, 1.0f};

    // 应用矩阵
    glMultMatrixf(translateMatrix);
    glMultMatrixf(viewMatrix);
}

// 绘制地面网格纹理
void drawGround()
{
    glPushMatrix();
    glTranslatef(ground.position[0], ground.position[1], ground.position[2]);

    // 设置地面材质
    glColor3fv(ground.material);

    // 绘制地面
    glBegin(GL_QUADS);
    glVertex3f(-ground.size[0] / 2, 0.0f, -ground.size[2] / 2);
    glVertex3f(ground.size[0] / 2, 0.0f, -ground.size[2] / 2);
    glVertex3f(ground.size[0] / 2, 0.0f, ground.size[2] / 2);
    glVertex3f(-ground.size[0] / 2, 0.0f, ground.size[2] / 2);
    glEnd();

    // 绘制网格纹理
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(1.0f);

    // 绘制水平线
    for (float z = -ground.size[2] / 2; z <= ground.size[2] / 2; z += 1.0f)
    {
        glBegin(GL_LINES);
        glVertex3f(-ground.size[0] / 2, 0.01f, z);
        glVertex3f(ground.size[0] / 2, 0.01f, z);
        glEnd();
    }

    // 绘制垂直线
    for (float x = -ground.size[0] / 2; x <= ground.size[0] / 2; x += 1.0f)
    {
        glBegin(GL_LINES);
        glVertex3f(x, 0.01f, -ground.size[2] / 2);
        glVertex3f(x, 0.01f, ground.size[2] / 2);
        glEnd();
    }

    glPopMatrix();
}

// 绘制平衡车
void drawBalanceCar()
{
    glPushMatrix();

    // 移动到平衡车位置
    glTranslatef(balance_car.position[0], balance_car.position[1], balance_car.position[2]);

    // 旋转平衡车朝向
    glRotatef(balance_car.orientation * 180.0f / M_PI, 0.0f, 1.0f, 0.0f);

    // 绘制轮轴
    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glScalef(balance_car.axle_length, balance_car.wheel_width * 0.8f, balance_car.wheel_width * 0.8f);
    glBegin(GL_QUADS);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();
    glPopMatrix();

    // 绘制左轮子
    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(-balance_car.axle_length / 2, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // 绘制轮胎
    glColor3f(0.1f, 0.1f, 0.1f);
    float wheel_steps = 32.0f;
    for (float i = 0; i < wheel_steps; i++)
    {
        float angle1 = (i / wheel_steps) * 2.0f * M_PI;
        float angle2 = ((i + 1) / wheel_steps) * 2.0f * M_PI;

        glBegin(GL_QUADS);
        glVertex3f(balance_car.wheel_radius * cos(angle1), -balance_car.wheel_width / 2, balance_car.wheel_radius * sin(angle1));
        glVertex3f(balance_car.wheel_radius * cos(angle2), -balance_car.wheel_width / 2, balance_car.wheel_radius * sin(angle2));
        glVertex3f(balance_car.wheel_radius * cos(angle2), balance_car.wheel_width / 2, balance_car.wheel_radius * sin(angle2));
        glVertex3f(balance_car.wheel_radius * cos(angle1), balance_car.wheel_width / 2, balance_car.wheel_radius * sin(angle1));
        glEnd();
    }

    // 绘制轮毂
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    for (float i = 0; i < wheel_steps; i++)
    {
        float angle1 = (i / wheel_steps) * 2.0f * M_PI;
        float angle2 = ((i + 1) / wheel_steps) * 2.0f * M_PI;

        glVertex3f(balance_car.wheel_radius * 0.8f * cos(angle1), 0.0f, balance_car.wheel_radius * 0.8f * sin(angle1));
        glVertex3f(balance_car.wheel_radius * 0.8f * cos(angle2), 0.0f, balance_car.wheel_radius * 0.8f * sin(angle2));
        glVertex3f(balance_car.wheel_radius * cos(angle2), 0.0f, balance_car.wheel_radius * sin(angle2));
        glVertex3f(balance_car.wheel_radius * cos(angle1), 0.0f, balance_car.wheel_radius * sin(angle1));
    }
    glEnd();

    glPopMatrix();

    // 绘制右轮子
    glPushMatrix();
    glTranslatef(balance_car.axle_length / 2, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    // 绘制轮胎
    glColor3f(0.1f, 0.1f, 0.1f);
    for (float i = 0; i < wheel_steps; i++)
    {
        float angle1 = (i / wheel_steps) * 2.0f * M_PI;
        float angle2 = ((i + 1) / wheel_steps) * 2.0f * M_PI;

        glBegin(GL_QUADS);
        glVertex3f(balance_car.wheel_radius * cos(angle1), -balance_car.wheel_width / 2, balance_car.wheel_radius * sin(angle1));
        glVertex3f(balance_car.wheel_radius * cos(angle2), -balance_car.wheel_width / 2, balance_car.wheel_radius * sin(angle2));
        glVertex3f(balance_car.wheel_radius * cos(angle2), balance_car.wheel_width / 2, balance_car.wheel_radius * sin(angle2));
        glVertex3f(balance_car.wheel_radius * cos(angle1), balance_car.wheel_width / 2, balance_car.wheel_radius * sin(angle1));
        glEnd();
    }

    // 绘制轮毂
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    for (float i = 0; i < wheel_steps; i++)
    {
        float angle1 = (i / wheel_steps) * 2.0f * M_PI;
        float angle2 = ((i + 1) / wheel_steps) * 2.0f * M_PI;

        glVertex3f(balance_car.wheel_radius * 0.8f * cos(angle1), 0.0f, balance_car.wheel_radius * 0.8f * sin(angle1));
        glVertex3f(balance_car.wheel_radius * 0.8f * cos(angle2), 0.0f, balance_car.wheel_radius * 0.8f * sin(angle2));
        glVertex3f(balance_car.wheel_radius * cos(angle2), 0.0f, balance_car.wheel_radius * sin(angle2));
        glVertex3f(balance_car.wheel_radius * cos(angle1), 0.0f, balance_car.wheel_radius * sin(angle1));
    }
    glEnd();

    glPopMatrix();

    // 绘制竖直杆
    glColor3f(balance_car.material[0], balance_car.material[1], balance_car.material[2]);
    glPushMatrix();
    glRotatef(balance_car.pole_angle * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, balance_car.wheel_radius, 0.0f);

    // 绘制杆
    float pole_steps = 16.0f;
    for (float i = 0; i < pole_steps; i++)
    {
        float angle1 = (i / pole_steps) * 2.0f * M_PI;
        float angle2 = ((i + 1) / pole_steps) * 2.0f * M_PI;

        glBegin(GL_QUADS);
        glVertex3f(balance_car.pole_radius * cos(angle1), 0.0f, balance_car.pole_radius * sin(angle1));
        glVertex3f(balance_car.pole_radius * cos(angle2), 0.0f, balance_car.pole_radius * sin(angle2));
        glVertex3f(balance_car.pole_radius * cos(angle2), balance_car.pole_length, balance_car.pole_radius * sin(angle2));
        glVertex3f(balance_car.pole_radius * cos(angle1), balance_car.pole_length, balance_car.pole_radius * sin(angle1));
        glEnd();
    }

    glPopMatrix();

    glPopMatrix();
}

// 显示函数
void display()
{
    // 清空颜色缓冲区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 设置相机
    glLoadIdentity();
    setCamera(
        camera_position[0], camera_position[1], camera_position[2],
        camera_look_at[0], camera_look_at[1], camera_look_at[2],
        0.0f, 1.0f, 0.0f);

    // 绘制地面
    drawGround();

    // 绘制平衡车
    drawBalanceCar();

    // 交换缓冲区
    SDL_GL_SwapWindow(window);
}

// 主函数
int main(int argc, char **argv)
{
    // 初始化SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL 初始化失败: %s\n", SDL_GetError());
        return -1;
    }

    // 设置SDL窗口属性
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // 创建窗口
    window = SDL_CreateWindow(
        "机器人仿真环境",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        printf("窗口创建失败: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // 创建OpenGL上下文
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL)
    {
        printf("OpenGL上下文创建失败: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 初始化
    init();

    // 设置视口
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // 使用glFrustum替代gluPerspective
    float aspect = (float)width / (float)height;
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 5000.0f; // 增大远裁剪面
    float top = nearPlane * tan(fov * M_PI / 360.0f);
    float bottom = -top;
    float right = top * aspect;
    float left = -right;
    glFrustum(left, right, bottom, top, nearPlane, farPlane);

    glMatrixMode(GL_MODELVIEW);

    // 主循环
    int running = 1;
    SDL_Event event;

    while (running)
    {
        // 处理事件
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                // 处理键盘事件
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    // 向上箭头键，增加俯仰角
                    camera_pitch += 5.0f;
                    if (camera_pitch > 89.0f)
                        camera_pitch = 89.0f;
                    break;
                case SDLK_DOWN:
                    // 向下箭头键，减少俯仰角
                    camera_pitch -= 5.0f;
                    if (camera_pitch < -89.0f)
                        camera_pitch = -89.0f;
                    break;
                case SDLK_LEFT:
                    // 向左箭头键，减少偏航角
                    camera_yaw -= 5.0f;
                    break;
                case SDLK_RIGHT:
                    // 向右箭头键，增加偏航角
                    camera_yaw += 5.0f;
                    break;
                }

                // 根据偏航角和俯仰角更新相机位置
                float radius = 15.0f;
                camera_position[0] = radius * cos(camera_pitch * M_PI / 180.0f) * cos(camera_yaw * M_PI / 180.0f);
                camera_position[1] = radius * sin(camera_pitch * M_PI / 180.0f);
                camera_position[2] = radius * cos(camera_pitch * M_PI / 180.0f) * sin(camera_yaw * M_PI / 180.0f);
            }
        }

        // 显示
        display();

        // 延迟
        SDL_Delay(16); // 约60 FPS
    }

    // 清理
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}