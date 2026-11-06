#include <robot/forw.hpp>
#include <stdexcept>
#include <string>

namespace robot {
Forward::Forward(int Cm) : Cm_(0) {
    Distance(Cm);
}

void Forward::Distance(int Cm) {
    if (Cm <= 0) {
        throw std::invalid_argument("距离不正确");
    }
    Cm_ = Cm;
}

std::string Forward::name() const { return "前进"; }

std::string Forward::params() const {
    return "前进距离：" + std::to_string(Cm_) + " cm";
}

std::string Forward::execute() {
    return "机器人向前移动了 " + std::to_string(Cm_) + " cm";
}
}
