import matplotlib.pyplot as plt
import numpy as np

plt.figure(figsize=(10, 10),dpi=80) # 设置图表大小(10in*6in)和分辨率(每英寸像素数)

plt.subplot(2, 2, 1) # 创建 2 行 2 列的子图，选中第 1 个

fig, axes=plt.subplots(3, 3, figsize=(10, 8))
# -fig: 画布对象
# -axes: 子图数组（可通过axes[0,0]访问第一个子图）

plt.tight_layout() # 自动调整子图间距，避免重叠

plt.figure(figsize=(15, 10),dpi=80)

#-----------------折线图
x = np.linspace(0, 10, 100)  # 生成0到10的100个均匀点
y = np.sin(x)
plt.plot(x, y, label='sin(x)', color='red', linestyle='-', linewidth=2, marker='o', markersize=4)

# 参数说明：
# - x/y: 横/纵坐标数据
# - label: 图例标签
# - color: 线条颜色（支持'red'/'blue'或十六进制'#FF0000'）
# - linestyle: 线条样式（'-'实线/'--'虚线/':'点线/'-.'点划线）
# - linewidth: 线条宽度
# - marker: 数据点标记（'o'圆点/'s'正方形/'^'三角形等）
# - markersize: 标记大小

#-----------------散点图
x = np.random.rand(500) #散布在0-1之间的500个随机值
y = np.random.rand(500)
colors = np.random.rand(500)  # 点的颜色
sizes = 100 * np.random.rand(500)  # 点的大小

# 核心函数：plt.scatter()
plt.scatter(x, y, c=colors, s=sizes, alpha=0.7, cmap='coolwarm')
# 参数说明：
# - c: 颜色（可传数组或固定值）
# - alpha: 透明度（0-1）
# - cmap: 颜色映射表（内置如'viridis'/'coolwarm'等）

#-----------------柱状图
x = ['A', 'B', 'C', 'D']
y = [0.2, 0.35, 0.30, 0.25]

plt.bar(x, y, width=0.6, color='skyblue', edgecolor='black')
# 条形图（横向）：plt.barh()
# 参数说明：
# - color: 填充颜色
# - edgecolor: 边框颜色 
# plt.barh(x, y, height=0.6, color='lightgreen')

#-----------------直方图
data = np.random.randn(100)  # 生成1000个正态分布数据
#直接对数组内的元素进行统计

plt.hist(data,  width=0.6 , bins=30, color='orange', alpha=0.7, edgecolor='black')
# 参数说明：
# - bins: 直方图的区间数（越多越细）

#-----------------饼图
labels = ['苹果', '香蕉', '橙子', '葡萄']
sizes = [30, 25, 20, 25]
explode = (0.1, 0, 0, 0)  # 突出第一个扇区

plt.pie(sizes, explode=explode, labels=labels, autopct='%1.1f%%',
        shadow=True, startangle=90, colors=['red', 'yellow', 'orange', 'purple'])
# 参数说明：
# - explode: 突出显示某个扇区（如(0.1, 0, 0, 0)突出第一个）
# - labels: 扇区标签
# - autopct: 显示百分比格式
# - shadow: 是否显示阴影
# - startangle: 起始角度
plt.axis('equal')  # 保证饼图为正圆形

#-----------------其他
plt.title('正弦函数曲线', fontsize=16, fontfamily='SimHei')# 设置标题，指定字体为SimHei（中文字体）

plt.xlabel('X轴', fontsize=12)# 设置坐标轴标签
plt.ylabel('Y轴', fontsize=12)

plt.xlim(0, 10)# 设置X轴范围
plt.ylim(-1.5, 1.5)

plt.grid(True, linestyle=':', alpha=0.5)#显示网格
# 参数说明：
# - linestyle: 网格线样式（'--'虚线/':'点线/'-.'点划线）

plt.text(5, 0, '中心点', fontsize=10, color='blue')# 在图表上添加文本注释

plt.annotate('峰值', xy=(np.pi/2, 1), xytext=(2, 1.2), arrowprops=dict(arrowstyle='->'))# 添加注释（箭头）
# 参数说明：
# - xy: 注释点坐标
# - xytext: 注释文本位置
# - arrowprops: 箭头属性（如arrowstyle）

plt.legend() # 显示图例
plt.show() # 显示图表
plt.ion() # 开启交互模式，支持动态更新

x.extend(new_x) # 更新数据列表
y.extend(new_y) #添加至原列表末尾

fig.canvas.draw()# 刷新图表显示
fig.canvas.flush_events()# 处理事件队列，确保图表更新生效