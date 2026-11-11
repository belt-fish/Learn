# Python 重写：机器人动作控制系统

## 实现了什么 / 用了什么写法

| 实现了什么             | 用了什么写法                                                                 |
| ---------------------- | ---------------------------------------------------------------------------- |
| 统一三种动作的调用方式 | `abc.ABC` + `@abstractmethod`，相当于 C++ 的纯虚函数                         |
| 动作类型               | `enum.Enum` 定义 `Type.FORWARD / ROTATE / WAVE`                              |
| 参数不传错             | `@dataclass(frozen=True)` 的 `Params`，静态方法 `cm / deg / n_times` 构造    |
| 前进、旋转、挥手       | 三个子类各存一个私有参数，拼描述文字                                         |
| 参数校验               | `_CheckedAction` 里统一检查：必须是正整数，否则抛 `TypeError` / `ValueError` |
| 装不同种类的动作       | `List[Action]`，多态列表                                                     |
| 按类型造动作对象       | 类属性 `_FACTORY` 字典 + `__create` 类方法，`getattr` 按字段名取参数         |
| 控制类管理动作         | `Control` 里持有动作列表（组合），不继承任何动作                             |
| 顺序执行               | `run()` 遍历列表，只调基类方法                                               |
| 链式调用               | `submit()` 返回 `self`                                                       |
| 对外只暴露三个名字     | `robot/__init__.py` 里写 `__all__`                                           |
| 测试                   | `unittest` 写了 7 个用例，正常流程和非法输入都覆盖                           |

## 怎么运行

```bash
cd robot_control_py

python3 main.py            # 跑默认序列
python3 main.py --list     # 看支持的动作
python3 -m unittest discover -s tests -v
```

## 输出

```
机器人已启动
执行动作: 前进 前进距离: 100 cm 机器人向前移动了 100 cm
执行动作: 旋转 旋转角度: 90° 机器人向左旋转了 90°
执行动作: 挥手 挥手次数: 3 机器人完成了 3 次挥手
所有动作执行完毕
```

## 踩坑

抽象方法的函数体不能空着。把文档字符串删掉之后要补一句 `...`，
不然报 `IndentationError: expected an indented block`。

## 和 C++ 版的对应

| C++                           | Python                         |
| ----------------------------- | ------------------------------ |
| 抽象基类 `Action`             | `Action(ABC)`                  |
| `enum class Type`             | `Type(Enum)`                   |
| `struct Params` + 静态方法    | `@dataclass` + `@staticmethod` |
| `vector<unique_ptr<Action>>`  | `List[Action]`                 |
| `throw std::invalid_argument` | `raise ValueError`             |
