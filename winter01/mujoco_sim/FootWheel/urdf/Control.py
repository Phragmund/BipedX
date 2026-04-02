import mujoco_py
import numpy as np

# -------------------------- 1. 初始化仿真环境 --------------------------
# 加载XML模型
model = mujoco_py.load_model_from_path("/home/phragmund/FootWheel/urdf/FootWheel.xml")
# 创建仿真器实例（核心交互对象）
sim = mujoco_py.MjSim(model)
# 创建可视化窗口（可选，方便看效果）
viewer = mujoco_py.MjViewer(sim)

# -------------------------- 2. 获取关节ID（关键：通过关节名找索引） --------------------------
LFU_name = "LFU_J"  # 和XML中joint的name一致
LFU_id = model.actuator_name2id(LFU_name)  # 转换为数字ID
LFD_name = "LFD_J"  # 和XML中joint的name一致
LFD_id = model.actuator_name2id(LFD_name)  # 转换为数字ID
LBU_name = "LBU_J"  # 和XML中joint的name一致
LBU_id = model.actuator_name2id(LBU_name)  # 转换为数字ID
LBD_name = "LBD_J"  # 和XML中joint的name一致
LBD_id = model.actuator_name2id(LBD_name)  # 转换为数字ID
RFU_name = "RFU_J"  # 和XML中joint的name一致
RFU_id = model.actuator_name2id(RFU_name)  # 转换为数字ID
RFD_name = "RFD_J"  # 和XML中joint的name一致
RFD_id = model.actuator_name2id(RFD_name)  # 转换为数字ID
RBU_name = "RBU_J"  # 和XML中joint的name一致
RBU_id = model.actuator_name2id(RBU_name)  # 转换为数字ID
RBD_name = "RBD_J"  # 和XML中joint的name一致
RBD_id = model.actuator_name2id(RBD_name)  # 转换为数字ID
# print(f"关节 {joint_name} 的ID：{joint_id}")

# -------------------------- 3. 核心交互：控制关节 + 读取实时角度 --------------------------
# 目标角度：90度（转换为弧度）
target_angle = np.pi / 2  # 1.57弧度
# 仿真总步数
total_steps = 50000

def ctrl_leg(joint_id, target_angle):
    """恢复默认重力后，稳定不飞的PD控制函数"""
    # # 官方默认重力下的保守PD参数，绝对不爆炸
    # kp = 8.0    # 适中的比例系数，不激进
    # kd = 8.0    # 阻尼等于比例，抑制震荡
    # max_torque = 3.0  # 扭矩上限，限制最大输出
    
    # current_angle = sim.data.qpos[joint_id]
    # current_vel = sim.data.qvel[joint_id]
    # torque = kp * (target_angle - current_angle) - kd * current_vel
    # # 严格限制扭矩，杜绝数值爆炸
    # torque = max(min(torque, max_torque), -max_torque)
    # sim.data.ctrl[joint_id] = torque

for step in range(total_steps):
    # -------------- 写操作：控制关节旋转 --------------
    # 方式1：直接设置目标角度（位置控制，新手首选）
    sim.data.ctrl[LFU_id] = np.pi/6
    sim.data.ctrl[LFD_id] = np.pi/6-np.pi/4+np.pi/24
    sim.data.ctrl[LBU_id] = 0
    sim.data.ctrl[LBD_id] = 0
    sim.data.ctrl[RFU_id] = 0
    sim.data.ctrl[RFD_id] = 0-np.pi/4
    sim.data.ctrl[RBU_id] = 0
    sim.data.ctrl[RBD_id] = 0

    sim.data.ctrl[8] = -10
    sim.data.ctrl[9] = -10
    sim.data.ctrl[10] = 10
    sim.data.ctrl[11] = 10


    # ctrl_leg(0,np.pi/6)
    # ctrl_leg(1,np.pi/6)
    # ctrl_leg(2,0)
    # ctrl_leg(3,0)
    # ctrl_leg(4,0)
    # ctrl_leg(5,0)
    # ctrl_leg(6,0)
    # ctrl_leg(7,0)

    # -------------- 执行一步仿真 --------------
    sim.step()  # 每步耗时约0.002秒（默认步长）

    # -------------- 读操作：获取实时关节角度 --------------
    current_angle = sim.data.qpos[RBD_id]  # 读取当前角度（弧度）
    current_angle_deg = np.degrees(current_angle)  # 转换为角度，方便阅读
    current_vel = sim.data.qvel[RBD_id]  # 可选：读取关节角速度

    # 打印实时数据（每10步打印一次，避免刷屏）
    if step % 10 == 0:
        print(f"步骤 {step} | 关节角度：{current_angle_deg:.2f}° | 目标角度：90.00°")

    # -------------- 更新可视化窗口 --------------
    viewer.render()

# 关闭可视化窗口
viewer.close()
print("仿真结束！")