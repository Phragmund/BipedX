if (now_mode == ctrl_up) //---------------------------------------机械臂模式
{
	// 底盘停止移动
	Motor_Speed(0, 0);
	Motor_Speed(1, 0);
	Motor_Speed(2, 0);
	Motor_Speed(3, 0);

	// x轴手柄操控+参数限位
	if (PS2_LX >= 129)
		pos_x -= 2;
	if (PS2_LX <= 125)
		pos_x += 2;
	if (pos_x <= -185)
		pos_x = -185;
	if (pos_x >= 185)
		pos_x = 185;

	// z轴手柄操控+参数限位
	if (key_arr[5] == 1)
		pos_z += 2; // 控制高低
	if (key_arr[7] == 1)
		pos_z -= 2;
	if (pos_z <= 0)
		pos_z = 0;
	if (pos_z >= 180)
		pos_z = 180;

	// 末端执行器控制
	if (key_arr[14] == 1 && flag_eat == 1)
		flag_eat = 0;
	if (flag_eat == 0)
		PCA_Servo(4, 60); // 夹球
	else
		PCA_Servo(4, 170);

	// y轴手柄操控+参数限位
	if (PS2_LY >= 200)
	{
		pos_y -= 1;
		delay_ms(3);
	}
	if (PS2_LY <= 55)
	{
		pos_y += 1;
		delay_ms(3);
	}
	if (pos_y <= 115)
		pos_y = 115;
	if (pos_y >= 265)
		pos_y = 265;
	if (pos_y <= pos_x) // 由于机械结构受限，y轴需要限位
		pos_y = pos_x;
	if (pos_y <= -pos_x)
		pos_y = -pos_x;

	// 避免一些奇怪的错误
	if (sqrt(pos_x * pos_x + pos_y * pos_y) <= 265)
	{
		// 运动学逆解，计算得到舵机R1R2的角度
		Angle_R2 = 180 - asin((sqrt(pos_x * pos_x + pos_y * pos_y) - 115.0) / 150.0) * 180.0 / PI; // 解方程
		Angle_R1 = Angle_R2 - 90;
	}
	else
	{
		Angle_R2 = 90;
		Angle_R1 = 0;
	}
	Angle_R3 = 110.0 + atan((double)pos_x / (double)pos_y) * 180 / PI;

	if (Angle_R1 >= 180)
		Angle_R1 = 180;

	// 舵机控制
	PCA_Servo(10, R1_fix + Angle_R1 + 40); // 结构问题，进行一些参数补正
	PCA_Servo(6, Angle_R2);
	PCA_Servo(7, pos_z);
	PCA_Servo(8, Angle_R3);
}