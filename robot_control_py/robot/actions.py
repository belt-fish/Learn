from __future__ import annotations

from .action import Action

class _CheckedAction(Action):

    def _require_positive(self, value: int, what: str) -> int:
        if isinstance(value, bool) or not isinstance(value, int):
            raise TypeError(f"{what}必须是整数，收到的是 {type(value).__name__}")
        if value <= 0:
            raise ValueError(f"{what}必须大于 0，收到的是 {value}")
        return value

class Forward(_CheckedAction):

    def __init__(self, distance_cm: int):
        self.__distance = self._require_positive(distance_cm, "前进距离")

    def name(self) -> str:
        return "前进"

    def params(self) -> str:
        return f"前进距离: {self.__distance} cm"

    def execute(self) -> str:
        return f"机器人向前移动了 {self.__distance} cm"

class Rotate(_CheckedAction):

    def __init__(self, degree: int):
        self.__degree = self._require_positive(degree, "旋转角度")

    def name(self) -> str:
        return "旋转"

    def params(self) -> str:
        return f"旋转角度: {self.__degree}°"

    def execute(self) -> str:
        return f"机器人向左旋转了 {self.__degree}°"

class Wave(_CheckedAction):

    def __init__(self, times: int):
        self.__times = self._require_positive(times, "挥手次数")

    def name(self) -> str:
        return "挥手"

    def params(self) -> str:
        return f"挥手次数: {self.__times}"

    def execute(self) -> str:
        return f"机器人完成了 {self.__times} 次挥手"
