#include <robot/control.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

#include <robot/forw.hpp>
#include <robot/rotate.hpp>
#include <robot/wave.hpp>

namespace robot {

std::unique_ptr<Action> Control::create(Type type, const Params& params) {
    switch (type) {
        case Type::forward:
            return std::make_unique<Forward>(params.distance);
        case Type::rotate:
            return std::make_unique<Rotate>(params.degree);
        case Type::wave:
            return std::make_unique<Wave>(params.times);
    }
    throw std::invalid_argument("未知的动作类型");
}

void Control::submit(Type type, const Params& params) {
    actions_.push_back(create(type, params));
}

void Control::run() {
    std::cout << "机器人已启动\n";

    for (auto& action : actions_) {
        std::cout << "执行动作：" << action->name() << ' ' << action->params() << ' '
                  << action->execute() << '\n';
    }

    std::cout << "所有动作执行完毕\n";
}

}
