# 最终验收：导航（仿真 + Nav2 + RViz）

小车在 Gazebo 仿真里跑 Nav2：激光建好的地图 → AMCL 定位 → 全局/局部规划 → 自定义 PID 控制器出速度 → 车走到目标点。

## 目录

```
final/
├── README.md
├── 常见问题.md            踩过的坑和排查命令
├── robot_description/     机器人和仿真：URDF、world、两个辅助节点
├── nav_bringup/           Nav2 参数、行为树、地图、RViz 配置、launch
└── nav_plugins/           自定义 PID 控制器（pluginlib 插件）
```

| 目录                                                             | 里面是什么                                                             |
| ---------------------------------------------------------------- | ---------------------------------------------------------------------- |
| `robot_description/urdf/`                                        | fishbot 的 xacro：底盘、轮子、万向轮、激光、相机、IMU                  |
| `robot_description/worlds/`                                      | `warehouse_classic.world`，36 个箱子当障碍                             |
| `robot_description/launch/nav_sim.launch.py`                     | 起 Gazebo + 机器人 + TF + 速度看门狗                                   |
| `robot_description/scripts/`                                     | `odom_tf_broadcaster.py` 发 TF，`cmd_vel_watchdog.py` 3 秒没指令就刹车 |
| `nav_bringup/src/nav_bringup/config/nav2_params.yaml`            | 全部 Nav2 参数                                                         |
| `nav_bringup/src/nav_bringup/maps/nav_start.yaml`                | 用的地图，0.03 m 一格，997×798                                         |
| `nav_bringup/src/nav_bringup/behavior_trees/pid_follow_path.xml` | 用自定义控制器替换默认 FollowPath                                      |
| `nav_bringup/src/nav_bringup/rviz/nav2.rviz`                     | RViz 布局                                                              |
| `nav_plugins/controller/`                                        | PID 控制器插件，`controller/PidController`                             |

## RViz 配置说明

配置在 `nav_bringup/src/nav_bringup/rviz/nav2.rviz`，是从 nav2_bringup 的
`nav2_default_view.rviz` 复制过来改的。

和默认的区别只有一个：**RobotModel 打开**（默认是关的，不开看不到车在哪）。
视角、其他显示项都保持默认（正交俯视、TF、粒子云、标记这些默认都开着）。

launch 里用 `-d` 指定这份配置：

```python
rviz_config_dir = os.path.join(nav_bringup_dir, 'rviz', 'nav2.rviz')
arguments=['-d', rviz_config_dir]
```

## 为什么不用点 2D Pose Estimate

`nav2_params.yaml` 里 amcl 配了自带的初始位姿：

```yaml
amcl:
  ros__parameters:
    set_initial_pose: true
    initial_pose:
      x: 0.0
      y: 0.0
      z: 0.0
      yaw: 0.0
```

而机器人的出生点也是 `(0, 0, 0.05)`（`nav_sim.launch.py` 里的 `spawn_entity`），
两个对得上，所以 AMCL 启动就知道自己在哪，不用手点。

换了出生点或者换一张地图时，这个初值就不对了，要么改 `initial_pose`，
要么在 RViz 里点一次 **2D Pose Estimate**。

## 编译

```bash
cd final
source /opt/ros/humble/setup.bash
colcon build
source install/setup.bash
```

三个包一起编，编完 `final/install/` 里都齐了。

## 启动

开两个终端：一个起仿真，一个起 Nav2。具体命令见下面「古法启动」。

## 古法启动

**每个终端都要先 source**，少一步就报 `Package not found`。

### 终端 1：仿真

```bash
cd ~/robo/final
source /opt/ros/humble/setup.bash
source install/setup.bash
export ROS_LOCALHOST_ONLY=1

ros2 launch robot_description nav_sim.launch.py
```

**等 20 秒左右**，看到 Gazebo 里机器人落到地上，终端不再刷 `spawn` 相关日志，再往下走。
这个 launch 起的是：gzserver + gzclient + robot_state_publisher + joint_state_publisher +
`odom_tf_broadcaster` + `cmd_vel_watchdog` + 生成机器人。

### 终端 2：Nav2 + RViz

```bash
cd ~/robo/final
source /opt/ros/humble/setup.bash
source install/setup.bash
export ROS_LOCALHOST_ONLY=1

ros2 launch nav_bringup nav_bringup.launch.py rviz:=true
```

`rviz:=true` 是开 RViz；不带这个参数就不开 RViz，只起 Nav2。

### 终端 3：查状态

```bash
source /opt/ros/humble/setup.bash
source ~/robo/final/install/setup.bash
export ROS_LOCALHOST_ONLY=1

ros2 node list            # 应该有 9 个 Nav2 节点 + 仿真那边的节点
ros2 topic hz /scan       # 激光频率
ros2 topic echo /map --once --field info.resolution
```

> **三个终端的 `ROS_LOCALHOST_ONLY` 必须一致**，否则话题互相看不见。

## 发目标点

1. 等所有 Nav2 节点变成 `active`（终端里会刷 `[map_server-1] ... active`）
2. RViz 工具栏点 **「2D Goal Pose」** 或 **「Nav2 Goal」**
3. 在地图空地上点一下 → 拖一下定车头方向 → 松手
4. 车会规划路径并开过去

两个按钮都能用，都往 `/goal_pose` 发：

| 按钮 | 谁提供的 | 发到 |
|---|---|---|
| 2D Goal Pose | RViz 自带的 `SetGoal` | `/goal_pose` |
| Nav2 Goal | `nav2_rviz_plugins` 的 `GoalTool` | `/goal_pose` |

`bt_navigator` 订阅了 `/goal_pose`，所以点哪个都能触发导航。
区别只是顺手程度：2D Goal Pose 是 RViz 原生的，点完就走；
Nav2 Goal 和右侧 Navigation2 面板联动，适合连续发多个点、中途暂停取消。

## 看到什么算成功

```
✓ 终端里 9 个 Nav2 节点全部 active
✓ RViz 里有地图（灰白）、激光点、绿色的全局路径
✓ 点目标后车会动，遇到障碍会绕
✓ 到点后终端打印 reached the goal / SUCCEEDED
```

导航节点清单：`map_server`、`amcl`、`controller_server`、`smoother_server`、
`planner_server`、`behavior_server`、`bt_navigator`、`waypoint_follower`、`velocity_smoother`。

## 关键参数在哪

| 想改什么     | 改哪里                                                      | 默认值                          |
| ------------ | ----------------------------------------------------------- | ------------------------------- |
| 速度上限     | `nav2_params.yaml` 里 `velocity_smoother` 的 `max_velocity` | `[0.26, 0.0, 1.0]`              |
| 障碍膨胀     | 两个 costmap 的 `inflation_radius`                          | `0.30`                          |
| 车半径       | `robot_radius`                                              | `0.15`                          |
| 地图         | `nav_start.yaml` / 换图改 `map`                             | 0.03 m/格，997×798              |
| 用哪个控制器 | `controller_plugins`                                        | `FollowPath` 和 `PidController` |
| 用哪棵行为树 | `default_nav_to_pose_bt_xml`                                | `pid_follow_path.xml`           |

## 踩坑

1. **不 source 就找不到包**：每个新终端都要 `source install/setup.bash`。
2. **建图和导航不能同时开**：两边都发 `map→odom`，TF 会来回跳，车在地图上乱飘。
3. **点了目标没反应**：先看 Nav2 节点是不是都 `active`，
   再确认 `ros2 topic info /goal_pose` 的订阅者里有 `bt_navigator`。
4. **仿真起太快 Nav2 会等不到 TF**：起完仿真等 20 秒再起 Nav2。
5. **改了代码不重新 `colcon build`**，跑的还是在 `install/` 里的旧版本。

## 演示视频

`演示视频.mp4`：1280×800，H.264，2.2 MB，1 分 05 秒。

> 选这个尺寸是为了能在 GitHub 网页上直接点开播放（文件超过 4 MB 页面就不给预览了）。
> 本地还留了一版 1600×1000 的，画面更清楚，需要的话再传。

录的时候仿真和 Nav2 已经起来了，画面是 Gazebo 和 RViz 并排：

- RViz 左边 `Displays` 能看到各层的开关
- 中间是地图、激光点和规划出来的路径
- 左下 `Navigation 2` 面板显示 Navigation / Localization 状态、ETA、剩余距离
- Gazebo 那边能看到小车在仓库里走

**不是从零启动的录像**，启动命令看上面「古法启动」。

系统打不开 mp4 的话，装一下 H.264 解码器：

```bash
sudo apt install gstreamer1.0-libav
```
