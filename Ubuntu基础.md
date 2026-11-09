# Ununtu基础

## 1. Ubuntu 22.04 常用指令
- `pwd`：显示绝对路径
- `ls`：查看目录文件
- `ls -la`：查看详细列表
- `cd ~/xxx`：切换目录
- `cd ..`：返回上一级
- `mkdir xxx`：创建文件夹
- `touch xxx`：创建空白文件
- `rm xxx`：删除文件
- `rm -r xxx`：删除文件夹
- `cat xxx`：查看文件内容
- 一键安装工具：`wget http://fishros.com/install -O fishros && . fishros`

## 2. 从编译到运行

### (1) g++ 直接编译
```bash
g++ xxx.cpp -o xx    # 源文件编译为可执行文件
./xx                 # 运行可执行文件
# 注：如果不写 -o，默认会生成 a.out
```

### (2) CMake 编译
注意：前提是项目目录下必须写好了 CMakeLists.txt
```bash
mkdir build
cd build
cmake ..
make                
./xx                 # 运行程序
```

### (3) ROS 2 colcon 编译
```bash
cd ~/xx              # 进入工作空间根目录
rosdep install --from-paths src --ignore-src -r -y   # 安装依赖（通常只有第一次需要）
colcon build         # 编译
source install/setup.bash                            # 刷新环境
ros2 run xx包名 xxx可执行文件名                       # 运行
```
