from ultralytics import YOLO
import cv2

# 加载训练好的最佳模型
model = YOLO('runs/train/exp17/weights/best.pt')
#model = YOLO('best_football.pt')

# 测试单张图片
# test_image_path = 'datasets/data/images/train/00127.jpg'  # 从验证集选一张图
test_image_path = 'image.png'  # 从验证集选一张图

results = model(
    test_image_path,
    conf=0.5,  # 置信度阈值（只保留≥0.5的检测结果）
    iou=0.4    # NMS IoU阈值（过滤重叠框）
)

# 处理结果：绘制边界框并保存
for result in results:
    annotated_img = result.plot()  # 生成带标注的图像
    output_path = 'test_result.jpg'
    cv2.imwrite(output_path, annotated_img)
    print(f"测试结果已保存至：{output_path}")

    # 打印检测到的车牌信息
    for box in result.boxes:
        x1, y1, x2, y2 = map(int, box.xyxy[0].tolist())
        conf = box.conf[0].item()
        print(f"球位置：({x1},{y1})-({x2},{y2})，置信度：{conf:.2f}")