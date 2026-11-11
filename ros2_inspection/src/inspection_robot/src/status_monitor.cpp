#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "inspection_interfaces/msg/robot_status.hpp"

using RobotStatus = inspection_interfaces::msg::RobotStatus;

/// 监控节点：订阅机器人状态，实时打印并做电量/状态判断。
class StatusMonitor : public rclcpp::Node {
public:
    StatusMonitor() : Node("robot_monitor") {
        // 与发布端保持一致的 QoS，才能收到“最近一次”状态
        const auto qos = rclcpp::QoS(rclcpp::KeepLast(10)).transient_local();
        subscription_ = create_subscription<RobotStatus>(
            "robot_status", qos,
            [this](const RobotStatus::SharedPtr msg) { on_status(msg); });

        RCLCPP_INFO(get_logger(), "监控节点已启动，等待机器人状态...");
    }

private:
    void on_status(const RobotStatus::SharedPtr msg) {
        // 1) 数据解析与显示
        RCLCPP_INFO(get_logger(),
                    "[监控] 编号: %s | 位置: (%.2f, %.2f) | 速度: %.2f m/s | 电量: %.0f%% | 状态: %s",
                    msg->robot_id.c_str(), msg->x, msg->y, msg->speed,
                    msg->battery, msg->state.c_str());

        // 2) 状态判断：只在跨过阈值时提醒一次，避免每秒刷屏
        check_battery(msg->battery);
        check_state(msg->state);
    }

    void check_battery(double battery) {
        if (battery <= 0.0) {
            if (!dead_) {
                dead_ = true;
                RCLCPP_ERROR(get_logger(), "电量耗尽：机器人已停止移动，请安排充电");
            }
        } else if (battery <= 10.0) {
            if (!critical_) {
                critical_ = true;
                RCLCPP_WARN(get_logger(), "严重低电量：剩余 %.0f%%，请立即返航", battery);
            }
        } else if (battery <= 20.0) {
            if (!low_) {
                low_ = true;
                RCLCPP_WARN(get_logger(), "低电量提醒：剩余 %.0f%%", battery);
            }
        }
    }

    void check_state(const std::string& state) {
        if (state == "stopped" && !reported_stop_) {
            reported_stop_ = true;
            RCLCPP_WARN(get_logger(), "状态判断：机器人已进入停止状态");
        }
    }

    rclcpp::Subscription<RobotStatus>::SharedPtr subscription_;

    bool low_ = false;           // 20% 提醒是否已发
    bool critical_ = false;      // 10% 提醒是否已发
    bool dead_ = false;          // 0% 提醒是否已发
    bool reported_stop_ = false; // 停止状态是否已提醒
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<StatusMonitor>());
    rclcpp::shutdown();
    return 0;
}
