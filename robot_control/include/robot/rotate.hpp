#pragma once
#include <robot/tol.hpp>
namespace robot {
class Rotate : public Action {
public:
    explicit Rotate(int Deg);
    void Degree(int Deg);
    int degree() const { return Deg_; }
    std::string name() const override;
    std::string params() const override;
    std::string execute() override;

private:
    int Deg_;
};
}
