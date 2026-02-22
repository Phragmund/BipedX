# -*- coding: utf-8 -*-

import warnings
warnings.filterwarnings('ignore')
from ultralytics import YOLO

if __name__ == '__main__':
    model = YOLO(model=r'ultralytics/cfg/models/11/yolo11.yaml')
    model.load('yolo11n.pt') # 加载预训练权重,改进或者做对比实验时候不建议打开，因为用预训练模型整体精度没有很明显的提升
    model.train(data=r'custom.yaml',#配置文件，指定数据集路径
                imgsz=(480,640),#图片size，640*640
                epochs=250,#训练轮次
                batch=80,#批次大小，即每次迭代输入的图片数量，GPU好可以调大一点，加快训练速度
                workers=0,#0表示单线程
                device='',#指定设备，留空自动选择，优先GPU
                optimizer='SGD',#优化器选择，此处是随机梯度下降
                close_mosaic=3,#数据增强的关闭轮次
                resume=False,#是否从上次中断的结果继续训练
                project='runs/train',#结果保存路径
                name='exp',#文件夹名称
                single_cls=False,#是否将所有类别视为单一类别训练。False 表示按数据集原类别训练（多类训练）。
                cache=False,#是否缓存数据集到内存或磁盘，以加速数据加载。
                )
