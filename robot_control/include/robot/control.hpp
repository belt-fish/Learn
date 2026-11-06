#pragma once
#include <memory>
#include <vector>
#include <robot/tol.hpp>

namespace robot {
enum class Type { forward, rotate, wave };

struct Params {
    int distance = 0;
    int degree = 0;
    int times = 0;
    static Params Cm(int cm) { return Params{cm, 0, 0}; }
    static Params Deg(int deg) { return Params{0, deg, 0}; }
    static Params Times(int times) { return Params{0, 0, times}; }
};

class Control {
public:
    void submit(Type type, const Params& params);
    void run();

private:
    static std::unique_ptr<Action> create(Type type, const Params& params);
    std::vector<std::unique_ptr<Action>> actions_;
};
}
