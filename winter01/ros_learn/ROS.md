# ROS2

## 节点常用CLI

- 运行节点

	```bash
	ros2 run <package_name> <executable_name>
	```

- 查看节点列表

  ```bash
  ros2 node list
  ```

- 查看节点信息√
  ```bash
  ros2 node info <node_name>
  ```

- 重映射节点名称

  ```bash
  ros2 run turtlesim turtlesim_node --ros-args --remap __node:=my_turtle
  ```

- 运行节点时设置参数

  ```bash
  ros2 run example_parameters_rclcpp parameters_basic --ros-args -p rcl_log_level:=10
  ```

  

## 工作空间

一个工作空间下可以有多个功能包，一个功能包可以包含若干个节点

新建工作空间

```bash
cd d2lros2/chapt2/ #进入指定文件夹
mkdir -p chapt2_ws/src #创建工作空间，-p递归创建，工作空间命名规范：_ws，同时创建src文件夹
```

## 功能包

存放节点的地方，根据编译方式不同分为三种类型，py，cpp，cpp增强版

- 安装功能包
  ```bash
  sudo apt install ros-<version>-package_name
  ```
- 创建功能包
	```bash
	ros2 pkg create <package-name>  --build-type  {cmake,ament_cmake,ament_python}  --dependencies <依赖名字>

- 列出可执行文件

  ```bash
  ros2 pkg executables <file-name>
  ```

- 列出所有的包

  ```bash
  ros2 pkg list
  ```

- 输出某个包所在路径的前缀

  ```bash
  ros2 pkg prefix  <package-name>
  ```

- 列出包的清单描述文件

  ```bash
  ros2 pkg xml <package-name>
  ```

## Colcon编译

- 编译工程（cd进入对应的工作空间）

  ```bash
  colcon build
  ```

- 编译后生成如下目录结构

  ```bash
  .
  ├── build：储存中间文件，每个包有一个子文件夹
  ├── install：软件包安装在这里，同样每个包一个子文件夹
  ├── log：目录包含每个colcon调用的各种日志信息
  └── src：工作空间的主要文件目录
  ```

## 运行编写的节点

cd进入工作空间

- 先source一下

  ```bash
  source install/setup.bash
  ```

- 运行发布者节点

  ```bash
  ros2 run examples_rclcpp_minimal_publisher publisher_member_function
  ```

- 运行订阅者节点

  ```bash
  ros2 run examples_rclcpp_minimal_subscriber subscriber_member_function
  ```

## 关于CMake

CMake是用于自动查找依赖的编译工具

编写一份txt，然后使用CMake编译，系统会依据txt的内容自动查找编译所依赖的文件，无需每次编译都手动输入一大堆指令

## 功能包

- 在工作空间下创建功能包

  ```bash
  cd chapt2_ws/src
  ros2 pkg create example_cpp --build-type ament_cmake --dependencies rclcpp
  ```
  - pkg create 是创建包的意思
  - build-type 用来指定该包的编译类型，一共有三个可选项ament_python、ament_cmake、cmake
  - dependencies 指的是这个功能包的依赖，这里填了一个ros2的C++客户端接口rclcpp

- 创建完成后的工作目录如下

  ```bash
  .
  └── src
      └── example_cpp
          ├── CMakeLists.txt
          ├── include
          │   └── example_cpp
          ├── package.xml
          └── src
  ```

## 节点

### RCLCPP编写节点

- 创建节点

  ```bash
  .
  └── src
    └── example_cpp
        ├── CMakeLists.txt
        ├── include
        │   └── example_cpp
        ├── package.xml
        └── src
        	└── node_01.cpp--->创建节点文件如下

- 编写节点代码

  ```cpp
  #include "rclcpp/rclcpp.hpp"
  
  int main(int argc, char **argv)
  {
      /* 初始化rclcpp  */
      rclcpp::init(argc, argv);
      /*产生一个node_01的节点*/
      auto node = std::make_shared<rclcpp::Node>("node_01");
      // 打印一句自我介绍
      RCLCPP_INFO(node->get_logger(), "node_01节点已经启动.");
      /* 运行节点，并检测退出信号 Ctrl+C*/
      rclcpp::spin(node);
      /* 停止运行 */
      rclcpp::shutdown();
      return 0;
  }
  ```

- 在CmakeLists.txt中添加以下内容，让编译器编译node_01,并安装到install目录

  ```bash
  add_executable(node_01 src/node_01.cpp)
  ament_target_dependencies(node_01 rclcpp)
  install(TARGETS
    node_01
    DESTINATION lib/${PROJECT_NAME}
  )
  ```

- 编译节点（工作空间内）

  ```bash
  colcon build
  ```

- source环境

  ```bash
  source install/setup.bash
  ```

- 运行节点

  ```bash
  ros2 run example_cpp node_01
  ```

  看见1 package finished就说明成功了，可以再使用ros2 node list查看现有节点

### RCLPY编写节点

- 创建Python功能包

  ```bash
  cd chapt2/chapt2_ws/src/
  ros2 pkg create example_py  --build-type ament_python --dependencies rclpy
  ```

- 创建完成后目录结构如下

  ```bash
  .
  ├── example_py
  │   └── __init__.py
  ├── package.xml
  ├── resource
  │   └── example_py
  ├── setup.cfg
  ├── setup.py
  └── test
      ├── test_copyright.py
      ├── test_flake8.py
      └── test_pep257.py
  ```

- 编写程序

  ```python
  import rclpy
  from rclpy.node import Node
  
  def main(args=None):
      """
      ros2运行该节点的入口函数
      编写ROS2节点的一般步骤
      1. 导入库文件
      2. 初始化客户端库
      3. 新建节点对象
      4. spin循环节点
      5. 关闭客户端库
      """
      rclpy.init(args=args) # 初始化rclpy
      node = Node("node_02")  # 新建一个节点
      node.get_logger().info("大家好，我是node_02.")
      rclpy.spin(node) # 保持节点运行，检测是否收到退出指令（Ctrl+C）
      rclpy.shutdown() # 关闭rclpy
  ```

- 修改setup.py，声明一个ROS2节点，声明后colcon build才能检测到

  ```python
     entry_points={
          'console_scripts': [
              "node_02 = example_py.node_02:main"
          ],
      },
  )
  ```

- 在vscode终端中编译节点并source环境

  ```bash
  cd chapt2/chapt2_ws
  colcon build
  source install/setup.bash
  ```

- 运行节点

  ```bash
  ros2 run example_py node_02
  ```

- 
