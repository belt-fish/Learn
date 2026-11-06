#pragma once
#include <robot/tol.hpp>
namespace robot {
class Forward : public Action {
public:
    explicit Forward(int Cm);
    void Distance(int Cm);
    int distance() const { return Cm_; }
    std::string name() const override;
    std::string params() const override;
    std::string execute() override;

private:
    int Cm_;
};
}
