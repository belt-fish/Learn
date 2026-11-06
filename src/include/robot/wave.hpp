#pragma once
#include <robot/tol.hpp>
namespace robot {
class Wave : public Action {
public:
    explicit Wave(int Times);
    void times(int Times);
    int times() const { return Times_; }
    std::string name() const override;
    std::string params() const override;
    std::string execute() override;

private:
    int Times_;
};
}
