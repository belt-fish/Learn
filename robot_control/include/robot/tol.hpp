#pragma once
#include <string>
namespace robot {  // 创建空间
class Action {
public:
    Action() = default;
    virtual ~Action() = default;
    virtual std::string name() const = 0;
    virtual std::string params() const = 0;
    virtual std::string execute() = 0;
};
}
