#include <Eigen/Dense>

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {

constexpr double kPi = 3.14159265358979323846;

Eigen::Matrix3d rotY(double deg) {
    const double r = deg * kPi / 180.0;
    Eigen::Matrix3d m;
    m << std::cos(r), 0, std::sin(r),
         0, 1, 0,
         -std::sin(r), 0, std::cos(r);
    return m;
}

Eigen::Matrix3d rotZ(double deg) {
    const double r = deg * kPi / 180.0;
    Eigen::Matrix3d m;
    m << std::cos(r), -std::sin(r), 0,
         std::sin(r), std::cos(r), 0,
         0, 0, 1;
    return m;
}

}

int main() {

    const Eigen::Vector3d p_co(0.4, 1.1, 3.2);
    const Eigen::Matrix3d R_co = rotZ(60.0);
    const Eigen::Vector3d p_wc(2.3, -1.7, 1.2);
    const Eigen::Matrix3d R_wc = rotY(90.0);

    Eigen::Matrix3d R_wo = R_wc * R_co;
    const Eigen::Vector3d p_wo = R_wc * p_co + p_wc;

    R_wo = R_wo.unaryExpr([](double v) { return std::abs(v) < 1e-9 ? 0.0 : v; });

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "p_wo = " << p_wo.transpose() << " m\n";
    std::cout << "R_wo =\n" << R_wo << "\n";

    const Eigen::Vector3d expect(5.5, -0.6, 0.8);
    std::cout << ((p_wo - expect).norm() < 1e-9 ? "结果正确\n" : "结果不对\n");
    return 0;
}
