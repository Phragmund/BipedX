#include <stdio.h>
#include <stdlib.h>

// 卡尔曼滤波参数结构体（封装所有参数，支持多通道滤波，如同时滤波温度+角速度）
typedef struct {
    float Q;  // 过程噪声协方差，需根据系统调参
    float R;  // 测量噪声协方差，需根据传感器调参
    float Xk; // 当前时刻的最优估计值（滤波后的值，输出）
    float Pk; // 当前时刻的协方差（误差方差，内部更新）
    float Xk_1; // 上一时刻的最优估计值（内部保存）
    float Pk_1; // 上一时刻的协方差（内部保存）
} KalmanFilter_TypeDef;

/**
 * @brief  一阶卡尔曼滤波初始化函数
 * @param  kf: 卡尔曼结构体指针
 * @param  Q: 过程噪声协方差（推荐初始值0.01~0.1，根据系统调整）
 * @param  R: 测量噪声协方差（推荐初始值0.1~1，根据传感器噪声调整）
 * @param  init_val: 滤波初始值（建议用传感器前几次采样的平均值）
 * @retval 无
 * @note   初始化的协方差Pk_1建议设为1.0，无需外部传入
 */
void KalmanFilter_Init(KalmanFilter_TypeDef *kf, float Q, float R, float init_val)
{
    // 合法性校验，避免空指针
    if (kf == NULL) {
        printf("Kalman Filter init error: null pointer!\n");
        return;
    }
    // 初始化核心参数
    kf->Q = Q;
    kf->R = R;
    // 初始最优估计值设为传入的初始值（如传感器初始采样值）
    kf->Xk = init_val;
    kf->Xk_1 = init_val;
    // 初始协方差设为1.0（通用初始值，后续会自动更新）
    kf->Pk = 1.0f;
    kf->Pk_1 = 1.0f;
}

/**
 * @brief  一阶卡尔曼滤波计算函数（核心）
 * @param  kf: 卡尔曼结构体指针
 * @param  measure_val: 当前时刻的测量值（传感器原始采样值，输入）
 * @retval float: 当前时刻的最优估计值（滤波后的值，输出）
 * @note   需按固定采样周期调用（如10ms采样一次，就10ms调用一次）
 */
float KalmanFilter_Calc(KalmanFilter_TypeDef *kf, float measure_val)
{
    // 合法性校验
    if (kf == NULL) {
        printf("Kalman Filter calc error: null pointer!\n");
        return 0.0f;
    }

    /************************* 第一步：预测阶段 *************************/
    // 预测当前时刻的估计值 = 上一时刻的最优估计值
    float Xk_predict = kf->Xk_1;
    // 预测当前时刻的协方差 = 上一时刻的协方差 + 过程噪声协方差Q
    float Pk_predict = kf->Pk_1 + kf->Q;

    /************************* 第二步：更新阶段 *************************/
    // 计算卡尔曼增益Kk：权衡预测值和测量值的权重
    float Kk = Pk_predict / (Pk_predict + kf->R);
    // 修正预测值，得到当前时刻最优估计值（滤波结果）
    kf->Xk = Xk_predict + Kk * (measure_val - Xk_predict);
    // 更新当前时刻的协方差，为下一时刻预测做准备
    kf->Pk = (1 - Kk) * Pk_predict;

    /************************* 保存当前值，为下一时刻做准备 *************************/
    kf->Xk_1 = kf->Xk;
    kf->Pk_1 = kf->Pk;

    // 返回滤波后的最优估计值
    return kf->Xk;
}

// 测试示例：模拟传感器采样（真实值50，叠加随机噪声），验证滤波效果
int main(void)
{
    // 1. 定义并初始化卡尔曼滤波器
    KalmanFilter_TypeDef kf;
    // 参数：Q=0.05（过程噪声），R=0.5（测量噪声），初始值50（传感器真实值）
    KalmanFilter_Init(&kf, 0.05f, 0.5f, 50.0f);

    // 模拟传感器采样数据：真实值50，叠加±2的随机噪声（模拟实际传感器的噪声）
    float measure_data[] = {51.2, 48.8, 50.9, 49.1, 51.5, 48.5, 
                            52.0, 47.8, 50.3, 49.7, 51.1, 48.9};
    // 采样数据长度
    int data_len = sizeof(measure_data) / sizeof(measure_data[0]);

    printf("===== 卡尔曼滤波测试（真实值：50）=====\n");
    printf("采样序号\t原始采样值\t滤波后值\n");
    printf("----------------------------------------\n");

    // 遍历采样数据，逐次滤波
    for (int i = 0; i < data_len; i++) {
        // 卡尔曼滤波计算
        float filter_val = KalmanFilter_Calc(&kf, measure_data[i]);
        // 打印结果（保留1位小数，直观对比）
        printf("%d\t\t%.1f\t\t%.1f\n", i+1, measure_data[i], filter_val);
    }

    return 0;
}