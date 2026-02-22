import math
import matplotlib.pyplot as plt
import numpy as np

# 设置中文字体
plt.rcParams['font.sans-serif'] = ['SimHei']
plt.rcParams['axes.unicode_minus'] = False

class TwoLinkArm:
    def __init__(self, link1_length=1.0, link2_length=1.0):
        """
        初始化二连杆机械臂
        :param link1_length: 第一根连杆长度
        :param link2_length: 第二根连杆长度
        """
        self.l1 = link1_length  # 连杆1长度
        self.l2 = link2_length  # 连杆2长度
        self.theta1 = 0.0       # 关节1角度（弧度）
        self.theta2 = 0.0       # 关节2角度（弧度）
        
        # 初始化绘图
        self.fig, self.ax = plt.subplots(figsize=(8, 8))
        self.ax.set_xlim(-(self.l1+self.l2+0.2), self.l1+self.l2+0.2)
        self.ax.set_ylim(-(self.l1+self.l2+0.2), self.l1+self.l2+0.2)
        self.ax.set_aspect('equal')
        self.ax.grid(True)
        self.ax.set_xlabel('X 坐标')
        self.ax.set_ylabel('Y 坐标')
        self.ax.set_title('二连杆机械臂运动学正解可视化')
        
        # 初始化连杆和关节的绘图对象
        self.link1_line, = self.ax.plot([], [], 'b-', linewidth=5, label='连杆1')
        self.link2_line, = self.ax.plot([], [], 'r-', linewidth=5, label='连杆2')
        self.joint0_dot, = self.ax.plot([], [], 'ko', markersize=8, label='基座关节')
        self.joint1_dot, = self.ax.plot([], [], 'ko', markersize=8, label='关节1')
        self.end_effector_dot, = self.ax.plot([], [], 'ro', markersize=8, label='末端执行器')
        self.ax.legend()

    def forward_kinematics(self, theta1_deg, theta2_deg):
        """
        运动学正解计算
        :param theta1_deg: 关节1角度（度）
        :param theta2_deg: 关节2角度（度）
        :return: 关节1坐标、末端坐标、姿态向量
        """
        # 角度转换为弧度
        self.theta1 = math.radians(theta1_deg)
        self.theta2 = math.radians(theta2_deg)
        
        # 计算关节1的坐标（相对于基座）
        joint1_x = self.l1 * math.cos(self.theta1)
        joint1_y = self.l1 * math.sin(self.theta1)
        
        # 计算末端执行器的坐标（绝对坐标）
        end_x = joint1_x + self.l2 * math.cos(self.theta1 + self.theta2)
        end_y = joint1_y + self.l2 * math.sin(self.theta1 + self.theta2)
        
        # 计算姿态向量（末端的方向，单位向量）
        # 姿态角 = theta1 + theta2，姿态向量为该角度的单位向量
        phi = self.theta1 + self.theta2
        orientation_vector = [math.cos(phi), math.sin(phi)]
        
        return (joint1_x, joint1_y), (end_x, end_y), orientation_vector

    def update_plot(self, joint1_pos, end_pos):
        """
        更新可视化图像
        :param joint1_pos: 关节1坐标 (x, y)
        :param end_pos: 末端执行器坐标 (x, y)
        """
        # 更新连杆坐标
        self.link1_line.set_data([0, joint1_pos[0]], [0, joint1_pos[1]])
        self.link2_line.set_data([joint1_pos[0], end_pos[0]], [joint1_pos[1], end_pos[1]])
        
        # 更新关节坐标
        self.joint0_dot.set_data([0], [0])
        self.joint1_dot.set_data([joint1_pos[0]], [joint1_pos[1]])
        self.end_effector_dot.set_data([end_pos[0]], [end_pos[1]])
        
        # 刷新图像
        self.fig.canvas.draw()
        self.fig.canvas.flush_events()

    def run(self):
        """
        主运行函数，接收用户输入并展示结果
        """
        plt.ion()  # 开启交互模式
        try:
            # 获取用户输入的关节角度
            theta1 = float(input("请输入关节1角度（度）："))
            theta2 = float(input("请输入关节2角度（度）："))
            
            # 计算正解
            joint1_pos, end_pos, orientation = self.forward_kinematics(theta1, theta2)
            
            # 输出结果
            print("\n===== 运动学正解结果 =====")
            print(f"关节1坐标: (x={joint1_pos[0]:.4f}, y={joint1_pos[1]:.4f})")
            print(f"末端执行器绝对坐标: (x={end_pos[0]:.4f}, y={end_pos[1]:.4f})")
            print(f"末端姿态向量: [{orientation[0]:.4f}, {orientation[1]:.4f}]")
            
            # 更新可视化
            self.update_plot(joint1_pos, end_pos)
            plt.ioff()  # 关闭交互模式
            plt.show()
            
        except ValueError:
            print("输入错误！请输入有效的数字角度值。")

# 主程序入口
if __name__ == "__main__":
    # 创建二连杆机械臂实例（可自定义连杆长度，默认均为1.0）
    arm = TwoLinkArm(link1_length=1.0, link2_length=1.8)
    arm.run()