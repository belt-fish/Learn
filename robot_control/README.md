# CMake 综合实践：机器人动作控制系统（C++）

## 实现了什么 / 用了什么写法

| 实现了什么             | 用了什么写法                                                                 |
| ---------------------- | ---------------------------------------------------------------------------- |
| 统一三种动作的调用方式 | 抽象基类 `Action`，三个纯虚函数 `name / params / execute`                    |
| 让基类指针能正确析构   | `virtual ~Action() = default`                                                |
| 前进、旋转、挥手       | 三个子类各存一个参数，各自拼接描述文字                                       |
| 装不同种类的动作       | `std::vector<std::unique_ptr<Action>>`，多态容器                             |
| 按类型造动作对象       | `Control::create()` 工厂函数，`switch` 枚举造对应子类                        |
| 外部传参统一           | `struct Params` + 静态工厂 `Cm / Deg / Times`                                |
| 控制类管理动作         | `Control` 组合（has-a）一个动作列表，不继承任何动作                          |
| 顺序执行               | `Control::run()` 遍历列表，只用基类接口调用                                  |
| 参数非法时报错         | `throw std::invalid_argument`，`main` 里统一 catch                           |
| 编译成库再链接         | `add_library(a_lib STATIC ...)` + `add_executable` + `target_link_libraries` |
| 让使用者找到头文件     | `target_include_directories(a_lib PUBLIC include)`                           |

## 怎么运行

```bash
cd robot_control
cmake -B build -S .
cmake --build build -j
./build/a_exe
```

## 输出

```
机器人已启动
执行动作：前进 前进距离：100 cm 机器人向前移动了 100 cm
执行动作：旋转 旋转角度：90° 机器人向左旋转了 90°
执行动作：挥手 挥手次数：3 机器人完成了 3 次挥手
所有动作执行完毕
```

## 踩坑

1. `std::unique_ptr` 不能拷贝，`vector<unique_ptr<Action>>` 只能用
   `push_back(std::move(...))` 或 `emplace_back`，直接 push 编译不过。
2. CMake 里 `target_include_directories` 忘了写 `PUBLIC`，
   库自己能编，可执行文件却报找不到 `robot/control.hpp`。
