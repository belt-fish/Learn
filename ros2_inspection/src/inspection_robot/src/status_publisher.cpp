#include <chrono>
#include <cmath>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "inspection_interfaces/msg/robot_status.hpp"

using RobotStatus = inspection_interfaces::msg::RobotStatus;

/// 机器人状态发布节点：模拟巡检机器人沿固定路线移动并上报状态。
class StatusPublisher : public rclcpp::Node {
public:
    StatusPublisher() : Node("robot_status_publisher") {
        // 参数：默认值与题目要求一致（1 秒一次、每次耗电 1%）
        robot_id_ = declare_parameter<std::string>("robot_id", "robot_01");
        period_sec_ = declare_parameter<double>("period_sec", 1.0);
        battery_step_ = declare_parameter<double>("battery_step", 1.0);

        if (period_sec_ < 0.01) {
            period_sec_ = 0.01;
        }
        if (battery_step_ <= 0.0) {
            battery_step_ = 1.0;
        }

        // transient_local：晚加入的订阅者也能收到最近一次状态
        const auto qos = rclcpp::QoS(rclcpp::KeepLast(10)).transient_local();
        publisher_ = create_publisher<RobotStatus>("robot_status", qos);

        // 先发布一次初始状态：位置 (0,0)、电量 100%、状态 moving
        update_motion_state();
        publish_status();

        const auto period = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::duration<double>(period_sec_));
        timer_ = create_wall_timer(period, [this]() { tick(); });

        RCLCPP_INFO(get_logger(),
                    "机器人 %s 已启动：周期 %.2fs，每周期耗电 %.2f%%",
                    robot_id_.c_str(), period_sec_, battery_step_);
    }

private:
    /// 每个周期：电量未耗尽就移动并耗电，然后更新运动状态并发布。
    void tick() {
        if (battery_ > 0.0) {
            ++steps_;
            x_ = steps_ * kStepX;   // 用步数乘步长，避免浮点累加误差
            y_ = steps_ * kStepY;
            battery_ -= battery_step_;
            if (battery_ < 0.0) {
                battery_ = 0.0;
            }
        }
        update_motion_state();
        publish_status();
    }

    /// 电量耗尽 -> 速度 0、状态 stopped；否则按固定步长计算合速度。
    void update_motion_state() {
        if (battery_ <= 0.0) {
            speed_ = 0.0;
            state_ = "stopped";
        } else {
            speed_ = std::sqrt(kStepX * kStepX + kStepY * kStepY);
            state_ = "moving";
        }
    }

    void publish_status() {
        RobotStatus msg;
        msg.robot_id = robot_id_;
        // 统一保留小数位，避免 0.6000000000000001 这种浮点尾巴
        msg.x = std::round(x_ * 100.0) / 100.0;
        msg.y = std::round(y_ * 100.0) / 100.0;
        msg.speed = std::round(speed_ * 1000.0) / 1000.0;
        msg.battery = battery_;
        msg.state = state_;

        publisher_->publish(msg);
        RCLCPP_INFO(get_logger(),
                    "发布: id=%s pos=(%.2f, %.2f) speed=%.2f battery=%.0f%% state=%s",
                    msg.robot_id.c_str(), msg.x, msg.y, msg.speed, msg.battery,
                    msg.state.c_str());
    }

    static constexpr double kStepX = 0.5;  // 每周期 X 增量 (m)
    static constexpr double kStepY = 0.2;  // 每周期 Y 增量 (m)

    std::string robot_id_;
    double period_sec_;
    double battery_step_;

    int steps_ = 0;
    double x_ = 0.0;
    double y_ = 0.0;
    double speed_ = 0.0;
    double battery_ = 100.0;
    std::string state_ = "moving";

    rclcpp::Publisher<RobotStatus>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<StatusPublisher>());
    rclcpp::shutdown();
    return 0;
}
