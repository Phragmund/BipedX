# BipedX
## winter01
### About
寒假第1-4周学习内容总结
### Dependencies
库或包：Numpy，Matplotlib，yolov11，ROS

软件：Matlab，Simulink

### Project Structure

#### Week 1-2

- Google编程风格
  - 目录：./code_style_guides
  - 说明：
    - 内容较多，涉及了一些之前不常用的语法知识点，还没完全学透
    - 后续会结合项目实际内容总结一份注意事项文档
- 滤波算法与控制算法
  - 目录：./filter-control_algorithm
  - 说明：
    - 简单总结了一下当前掌握的各种滤波算法与控制算法
    - 上学期完成的学习文档见../doc
    - 本周更新了具体的实现代码
- 串口数据简单处理
  - 目录：./serial_data_process
  - 说明：
    - Python脚本实现对串口数据文件（.csv）的处理与呈现
    - 使用Matplotlib库进行数据图像绘制
    - 运行serial_data_process.py，即可生成simulated_serial_data.csv文件对应的图像
- 仿真环境搭建
  - 目录：./simulation_env
  - 说明：
    - FOCUS冬令营那一周有其他事，所以后面这周补充完成了一部分任务
    - 已完成ROS安装，能够正常运行
- Git与SSH的使用
  - 目录：./git_guides
  - 说明：
    - 花了较多时间熟悉这个系统，目前能够比较熟练地完成各种操作
    - 整理了一份相关指令的使用指南
    - 后来发现Sourcetree挺好用的，不用背指令

#### Week3-4

- 仿真环境探究
  - 目录：./simulation_env
  - 说明：
    - 本周完成了导入GO1、实现键盘操作等任务点，同时安装了Mujoco仿真环境
    - 正在补充ROS相关知识

- Matplotlib进阶指南
  - 目录：./matplotlib_guide
  - 说明：
    - 进一步学习了该库的其他函数方法，能够熟练运用其进行数据绘制操作
    - 补充了一份自用的函数速查文档
- 机器人学导论
  - 目录：../doc/book_materials
  - 说明：
    - 阅读了前几章，掌握了机器人学的部分基本概念，比如自由度，奇异点，正运动学，逆运动学解算，雅可比矩阵，运动学等等
    - 重点理解掌握了正运动学和逆运动学部分的知识
    - 网上找到的电子版，供大家参考
- 运动学实践
  - 目录：./robot_kinematics
  - 说明：
    - forward_kinematics.py是一个二连杆正运动学解算程序，对于给定长度的二连杆，输入两个关节的角度，输出末端坐标
    - inverse_kinematics.c是我们校内赛小车的机械臂的运动学逆解部分，输入末端的xy平面坐标，舵机控制机械臂末段夹爪以垂直姿态运动至指定坐标位置，涉及3个自由度的计算。由于实物的局限，并没有考虑多解、多姿态等问题，算是对运动学逆解的一个简单的尝试）
- Simulink仿真
  - 目录：./simulink_guide
  - 说明：
    - 课题项目，对3508电机进行物理建模仿真，并进行PID控制
    - 使用matlab软件，simulink工具完成了仿真实验
    - M3508_sim.slx为仿真模型文件，附学习文档Markdown文件
- yolo强化学习
  - 目录：./yolo_project
  - 说明：
    - 之前做过有关的内容，复习了一下训练的整个流程，总结了一份文档
    - 模型没炼太久，效果凑合着看还行）
