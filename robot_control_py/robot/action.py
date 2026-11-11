from __future__ import annotations

from abc import ABC, abstractmethod
from dataclasses import dataclass
from enum import Enum

class Type(Enum):

    FORWARD = "forward"
    ROTATE = "rotate"
    WAVE = "wave"

@dataclass(frozen=True)
class Params:

    distance_cm: int = 0
    degree: int = 0
    times: int = 0

    @staticmethod
    def cm(distance: int) -> "Params":
        return Params(distance_cm=distance)

    @staticmethod
    def deg(degree: int) -> "Params":
        return Params(degree=degree)

    @staticmethod
    def n_times(times: int) -> "Params":
        return Params(times=times)

class Action(ABC):

    @abstractmethod
    def name(self) -> str:
        ...

    @abstractmethod
    def params(self) -> str:
        ...

    @abstractmethod
    def execute(self) -> str:
        ...

    def __str__(self) -> str:
        return f"{self.name()} {self.params()}"
