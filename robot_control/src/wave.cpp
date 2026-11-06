#include <robot/wave.hpp>
#include <stdexcept>
#include <string>

namespace robot {
Wave::Wave(int Times) : Times_(0) {
    times(Times);
}

void Wave::times(int Times) {
    if (Times <= 0) {
        throw std::invalid_argument("次数不正确");
    }
    Times_ = Times;
}

std::string Wave::name() const { return "挥手"; }

std::string Wave::params() const {
    return "挥手次数：" + std::to_string(Times_);
}

std::string Wave::execute() {
    return "机器人完成了 " + std::to_string(Times_) + " 次挥手";
}
}
