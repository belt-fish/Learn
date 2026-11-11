# ROS 2 最终任务：巡检机器人状态发布 / 订阅系统

## 实现了什么 / 用了什么写法

| 实现了什么         | 用了什么写法                                                                   |
| ------------------ | ------------------------------------------------------------------------------ |
| 消息定义           | `inspection_interfaces/msg/RobotStatus.msg`，六个字段                          |
| 消息能生成头文件   | 包里的 `CMakeLists.txt` 加 `rosidl_generate_interfaces`                        |
| 发布状态           | 节点类继承 `rclcpp::Node`，`create_publisher` + `create_wall_timer` 每秒发一次 |
| 位置按步长走       | 用 `步数 * 0.5` 算，不用浮点累加，避免误差越积越大                             |
| 电量逻辑           | `battery > 0` 才移动和耗电，耗尽后 `speed = 0`、`state = "stopped"`            |
| 参数可调           | `declare_parameter` 声明 `period_sec`、`battery_step`，launch 里可以改         |
| 订阅并显示         | `create_subscription`，回调里打印一行完整状态                                  |
| 电量告警           | 20% 和 10% 各提醒一次，用 bool 记住是否已提醒，避免每秒刷屏                    |
| 晚加入也能收到     | 发布端 QoS 用 `transient_local`                                                |
| 一条命令启动       | launch 里写两个 `Node`，用 `TimerAction` 延迟 0.5 秒起发布节点                 |
| 参数从命令行传进去 | `DeclareLaunchArgument` + `ParameterValue(..., float)`                         |
| 输出不出现浮点尾巴 | 打印前 `std::round(x * 100) / 100`                                             |

## 怎么运行

```bash
cd ros2_inspection
source /opt/ros/humble/setup.bash
colcon build
source install/setup.bash

ros2 launch inspection_bringup inspection.launch.py
# 想快点看到电量耗尽：
ros2 launch inspection_bringup inspection.launch.py battery_step:=40.0
```

## 输出

```
[robot_monitor]: 监控节点已启动，等待机器人状态...
[robot_status_publisher]: 发布: id=robot_01 pos=(0.00, 0.00) speed=0.54 battery=100% state=moving
[robot_monitor]: [监控] 编号: robot_01 | 位置: (0.00, 0.00) | 速度: 0.54 m/s | 电量: 100% | 状态: moving
[robot_status_publisher]: 发布: id=robot_01 pos=(0.50, 0.20) speed=0.54 battery=60% state=moving
[robot_monitor]: [监控] 编号: robot_01 | 位置: (0.50, 0.20) | 速度: 0.54 m/s | 电量: 60% | 状态: moving
```

`speed=0.54` 是 `sqrt(0.5² + 0.2²)` 算出来的合速度。

## 验证命令

```bash
ros2 node list
ros2 topic list
ros2 topic info /robot_status
ros2 interface show inspection_interfaces/msg/RobotStatus
ros2 topic echo /robot_status
ros2 topic hz /robot_status
```

## 踩坑

1. **不 `source install/setup.bash` 就用不了自定义消息**，报
   `The message type '...' is invalid`，看着像消息写错了，其实是环境没加载。
2. **两个节点同时起，第一条消息容易丢**，所以发布节点用 `TimerAction` 延后 0.5 秒。
