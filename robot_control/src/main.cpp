#include <exception>
#include <iostream>

#include <robot/control.hpp>

int main() {
    robot::Control controller;

    try {
        controller.submit(robot::Type::forward, robot::Params::Cm(100));
        controller.submit(robot::Type::rotate, robot::Params::Deg(90));
        controller.submit(robot::Type::wave, robot::Params::Times(3));

        controller.run();
    } catch (const std::exception& e) {
        std::cerr << "错误：" << e.what() << '\n';
        return 1;
    }

    return 0;
}
