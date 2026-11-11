# 学习笔记（robo）

重庆大学 27 赛季视觉算法入门培训的学习笔记，方向：**导航**。
机器是 Windows + Ubuntu 双系统，笔记里的命令都在 Ubuntu 22.04 下跑通。

- **笔记**：按培训文档的部分拆开，一个部分一个 `.md`
- **任务**：每个实践任务一个目录，目录里都有 `README.md`（题目要求、实现了什么、怎么跑、效果图、踩坑）

## 目录结构

```text
robo/
├── 1.1_part.md ~ 5.2_part.md     13 份笔记
├── eigen_task/                   Eigen 坐标变换
├── opencv_image_tool/            OpenCV 基础：图片处理小工具
├── opencv_armor/                 OpenCV 进阶：装甲板灯条识别
├── robot_control/                动作控制系统（C++）
├── robot_control_py/             动作控制系统（Python）
├── ros2_inspection/              巡检机器人（ROS 2）
└── final/                        最终验收：导航（仿真 + Nav2 + RViz）
```

## 笔记目录

| 部分 | 文件 | 主要内容 |
|---|---|---|
| 一、开发环境与工程规范 | [1.1 Linux 基础](1.1_part.md) | 文件、权限、进程、apt、三种编译方式 |
| 一、开发环境与工程规范 | [1.2 Git 基础](1.2_part.md) | 三个区、分支、撤销、冲突、远程仓库 |
| 二、编程基础 | [2.1 C++ 基础](2.1_part.md) | 语法、类、指针引用、STL、常见报错 |
| 二、编程基础 | [2.2 CMake 基础](2.2_part.md) | 最小模板、库、ROS 2 包模板 |
| 二、编程基础 | [2.3 Python 基础](2.3_part.md) | 容器、类、文件异常、标准库 |
| 二、编程基础 | [2.4 ROS 2 基础](2.4_part.md) | 五概念、工作空间、命令表、QoS |
| 三、视觉数学与图像 | [3.1 OpenCV 基础](3.1_part.md) | Mat、颜色、ROI、算子、cv_bridge |
| 三、视觉数学与图像 | [3.2 Eigen 基础](3.2_part.md) | 矩阵、解方程、旋转、齐次变换 |
| 四、神经网络基础 | [4.1 相关理论基础](4.1_part.md) | 卷积尺寸、损失、YOLO、mAP |
| 四、神经网络基础 | [4.2 环境部署](4.2_part.md) | 版本清单、安装、验证 |
| 四、神经网络基础 | [4.3 数据标注规范](4.3_part.md) | YOLO txt、换算公式、data.yaml |
| 四、神经网络基础 | [4.4 训练与推理](4.4_part.md) | train/val/predict/export |
| 最终验收 | [5.2 导航方向](5.2_part.md) | 建图→保存→定位→规划→控制 + 调试实录 |

> 自瞄方向和机械臂方向不是本方向的要求，文档在桌面「选看_」开头。

## 任务导航

点任务名进目录，目录里的 `README.md` 写了题目要求、实现了什么、怎么跑、效果图和踩坑。

| 任务 | 对应笔记 | 环境 | 运行 | 状态 |
|---|---|---|---|---|
| [Eigen 坐标变换](eigen_task) | [3.2](3.2_part.md) | C++ / Eigen | `./build/eigen_task` | 完成 |
| [图片处理小工具](opencv_image_tool) | [3.1](3.1_part.md) | C++ / OpenCV | `./build/image_tool` | 完成 |
| [装甲板灯条识别](opencv_armor) | [3.1](3.1_part.md) | C++ / OpenCV | `./build/armor` | 完成 |
| [动作控制系统](robot_control) | [2.2](2.2_part.md) | C++ / CMake | `./build/a_exe` | 完成 |
| [动作控制系统（Python）](robot_control_py) | [2.3](2.3_part.md) | Python | `python3 main.py` | 完成 |
| [巡检机器人](ros2_inspection) | [2.4](2.4_part.md) | ROS 2 / C++ | `ros2 launch inspection_bringup inspection.launch.py` | 完成 |
| [**最终验收：导航**](final) | [5.2](5.2_part.md) | ROS 2 / Gazebo | 见 `final/README.md` 的古法启动 | 完成 |

> 最终验收的演示视频在 [`final/演示视频.mp4`](final/演示视频.mp4)（4.6 MB），录的是启动到导航到点的全过程。
|  |  |  |  |  |
|  |  |  |  |  |

## 编译

三个 OpenCV / Eigen 任务是普通 CMake 工程：

```bash
for d in eigen_task opencv_image_tool opencv_armor; do
  (cd $d && cmake -B build -S . && cmake --build build -j)
done
```

`robot_control` 同理（`cd robot_control && cmake -B build -S . && cmake --build build -j`）。

ROS 2 那个：

```bash
cd ros2_inspection
source /opt/ros/humble/setup.bash
colcon build && source install/setup.bash
```

Python 那个不用编译。

## 效果图

两个 OpenCV 任务的 `demo/` 里放了运行结果，README 里直接能看到。

| 任务 | 效果 |
|---|---|
| 图片处理小工具 | 原图标注 ROI、灰度图、裁出来的 ROI |
| 装甲板灯条识别 | 蓝色掩膜、灯条 + 装甲板识别结果 |

## 环境版本

| 项目 | 版本 |
|---|---|
| 系统 | Ubuntu 22.04.5 LTS（Windows 双系统） |
| ROS | Humble |
| g++ | 11.4（C++17） |
| CMake | 3.22.1 |
| Python | 3.10.12 |
| OpenCV | 4.5.4（C++）/ cv2 5.0.0（Python） |
| Eigen | 3.4.0 |
| PyTorch | 2.11.0+cu128 |
| ultralytics | 8.4.191 |
| git | 2.34.1 |

## 踩坑

- **不 source 就找不到包**：每个新终端都要 `source install/setup.bash`，报错不会提示你没 source
- **改完 CMakeLists 没重编**：`install/` 里是副本，改了等于没改
- **OpenCV 是 BGR 不是 RGB**：和 RGB 库对接时红蓝互换
- **cv::Mat 赋值是浅拷贝**：改副本会改到原图，要 `clone()`
- **Eigen 四元数顺序 `(w,x,y,z)`**：ROS 消息是 `(x,y,z,w)`，搞反不报错但旋转全错
- **YOLO 类别 id 从 0 开始**：写成 1 会永远少一类
- **训练和推理的 imgsz 要一致**：不一致精度掉，且不报错
- **建图和导航不能同时开**：两边都发 `map→odom`，TF 会跳
- **调参一次只改一个**：抖动、漂移、牵引力互相牵连，一起改就分不清是谁的锅
- **双系统时间差 8 小时**：Windows 和 Ubuntu 对硬件时钟的时区解释不一样，不是系统坏了
