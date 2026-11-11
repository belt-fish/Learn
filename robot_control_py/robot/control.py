from __future__ import annotations

from typing import Dict, List, Tuple, Type as TypingType

from .action import Action, Params, Type
from .actions import Forward, Rotate, Wave

class Control:

    _FACTORY: Dict[Type, Tuple[TypingType[Action], str]] = {
        Type.FORWARD: (Forward, "distance_cm"),
        Type.ROTATE: (Rotate, "degree"),
        Type.WAVE: (Wave, "times"),
    }

    def __init__(self) -> None:
        self.__actions: List[Action] = []

    def submit(self, action_type: Type, params: Params) -> "Control":
        self.__actions.append(self.__create(action_type, params))
        return self

    def run(self) -> None:
        print("机器人已启动")
        for action in self.__actions:
            print(f"执行动作: {action.name()} {action.params()} {action.execute()}")
        print("所有动作执行完毕")

    def clear(self) -> None:
        self.__actions.clear()

    @property
    def count(self) -> int:
        return len(self.__actions)

    @classmethod
    def __create(cls, action_type: Type, params: Params) -> Action:
        if not isinstance(action_type, Type):
            raise TypeError("action_type 必须是 Type 枚举")
        if not isinstance(params, Params):
            raise TypeError("params 必须是 Params")

        factory = cls._FACTORY.get(action_type)
        if factory is None:
            raise ValueError(f"未知的动作类型：{action_type}")

        action_cls, field = factory
        value = getattr(params, field)
        if value == 0:
            raise ValueError(f"动作 {action_type.value} 缺少参数 {field}，无法执行")

        return action_cls(value)
