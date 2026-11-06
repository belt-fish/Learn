#include <robot/rotate.hpp>
#include <stdexcept>
#include <string>

namespace robot {
Rotate::Rotate(int Deg) : Deg_(0) {
    Degree(Deg);
}

void Rotate::Degree(int Deg) {
    if (Deg < -180 || Deg > 180) {
        throw std::invalid_argument("角度不正确");
    }
    Deg_ = Deg;
}

std::string Rotate::name() const { return "旋转"; }

std::string Rotate::params() const {
    return "旋转角度：" + std::to_string(Deg_) + "°";
}

std::string Rotate::execute() {
    if (Deg_ == 0) {
        return "机器人原地旋转了 0°";
    }
    const std::string direction = Deg_ > 0 ? "向左" : "向右";
    const int value = Deg_ > 0 ? Deg_ : -Deg_;
    return "机器人" + direction + "旋转了 " + std::to_string(value) + "°";
}
}
