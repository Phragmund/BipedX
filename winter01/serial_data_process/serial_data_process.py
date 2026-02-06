import pandas as pd
import matplotlib.pyplot as plt
import time
import os

# 配置项（可根据你的CSV文件修改）
CSV_FILE_PATH = "simulated_serial_data.csv"  # 模拟串口数据的CSV文件路径
X_COLUMN = "timestamp"  # CSV中X轴数据列名（时间戳/序号）
Y_COLUMN = "value"      # CSV中Y轴数据列名（串口采集的数值）
UPDATE_INTERVAL = 1.0   # 动态图更新间隔（秒）
MAX_DATA_POINTS = 100   # 图表最多显示的数据点数量（防止数据过多卡顿）

def init_csv_file():
    """
    初始化模拟CSV文件（如果文件不存在），生成一些测试数据
    模拟串口数据的格式：timestamp（时间戳/序号）, value（采集值）
    """
    if not os.path.exists(CSV_FILE_PATH):
        print(f"未找到CSV文件，正在生成模拟数据文件：{CSV_FILE_PATH}")
        # 生成初始测试数据
        init_data = {
            X_COLUMN: list(range(20)),
            Y_COLUMN: [i + (i % 7) * 0.5 for i in range(20)]  # 生成带轻微波动的模拟数据
        }
        pd.DataFrame(init_data).to_csv(CSV_FILE_PATH, index=False)
        print("模拟CSV文件生成完成")

def dynamic_plot_serial_data():
    """
    读取CSV文件，绘制动态折线图
    支持增量读取，模拟串口数据的实时更新
    """
    # 初始化matplotlib，开启交互模式（支持动态更新）
    plt.ion()
    fig, ax = plt.subplots(figsize=(12, 6))
    x_data = []
    y_data = []
    line, = ax.plot(x_data, y_data, color='royalblue', linewidth=2, label=Y_COLUMN)
    
    # 设置图表样式
    ax.set_title("模拟串口数据动态折线图", fontsize=14)
    ax.set_xlabel(X_COLUMN, fontsize=12)
    ax.set_ylabel(Y_COLUMN, fontsize=12)
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.legend(loc='upper right')
    
    # 记录上一次读取的行数（实现增量读取，避免重复绘制）
    last_read_rows = 0
    
    try:
        while True:
            # 步骤1：读取CSV文件最新数据
            if not os.path.exists(CSV_FILE_PATH):
                time.sleep(UPDATE_INTERVAL)
                continue
            
            # 读取整个CSV文件（实际场景可优化为只读取新增行）
            df = pd.read_csv(CSV_FILE_PATH)
            
            # 校验CSV文件列是否完整
            if X_COLUMN not in df.columns or Y_COLUMN not in df.columns:
                print(f"错误：CSV文件缺少{X_COLUMN}或{Y_COLUMN}列")
                time.sleep(UPDATE_INTERVAL)
                continue
            
            # 步骤2：提取新增数据（从上一次读取的行数开始）
            current_total_rows = len(df)
            if current_total_rows > last_read_rows:
                new_df = df.iloc[last_read_rows:]  # 提取新增行数据
                new_x = new_df[X_COLUMN].tolist()
                new_y = new_df[Y_COLUMN].tolist()
                
                # 更新数据列表
                x_data.extend(new_x)
                y_data.extend(new_y)
                
                # 限制数据点数量（防止图表卡顿）
                if len(x_data) > MAX_DATA_POINTS:
                    x_data = x_data[-MAX_DATA_POINTS:]
                    y_data = y_data[-MAX_DATA_POINTS:]
                
                # 更新上一次读取的行数
                last_read_rows = current_total_rows
            
            # 步骤3：更新折线图
            line.set_data(x_data, y_data)
            
            # 自动调整坐标轴范围（适配最新数据）
            if x_data and y_data:
                ax.set_xlim(min(x_data), max(x_data))
                ax.set_ylim(min(y_data) - 1, max(y_data) + 1)
            
            # 刷新图表
            fig.canvas.draw()
            fig.canvas.flush_events()
            
            # 模拟串口数据更新间隔
            time.sleep(UPDATE_INTERVAL)
    
    except KeyboardInterrupt:
        print("\n程序已停止运行")
    finally:
        # 关闭交互模式，显示最终图表
        plt.ioff()
        plt.show()

if __name__ == "__main__":
    # 第一步：初始化模拟CSV文件
    init_csv_file()
    
    # 第二步：启动动态绘图
    dynamic_plot_serial_data()