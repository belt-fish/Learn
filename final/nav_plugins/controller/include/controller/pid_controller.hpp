#pragma once

#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <nav2_core/controller.hpp>
#include <nav2_util/robot_utils.hpp>

class PidController : public nav2_core::Controller
{
public:
    PidController() = default;
    ~PidController() = default;

    void configure(
    const rclcpp_lifecycle::LifecycleNode::WeakPtr & parent,
    std::string name, std::shared_ptr<tf2_ros::Buffer> tf,
    std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap) override;

    void cleanup() override;

    void activate() override;

    void deactivate() override;

    void setPlan(const nav_msgs::msg::Path & path) override;

    geometry_msgs::msg::TwistStamped computeVelocityCommands(
    const geometry_msgs::msg::PoseStamped & pose,
    const geometry_msgs::msg::Twist & velocity,
    nav2_core::GoalChecker * goal_checker) override;

    void setSpeedLimit(const double & speed_limit, const bool & percentage) override;

private:
    rclcpp_lifecycle::LifecycleNode::WeakPtr node_;
    rclcpp::Logger logger_{rclcpp::get_logger("PidController")};
    rclcpp::Clock::SharedPtr clock_;
    std::string plugin_name_;
    std::shared_ptr<tf2_ros::Buffer> tf_;
    std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_;
    nav_msgs::msg::Path global_path_;

    double max_angular_speed_;
    double max_linear_speed_;
    double base_linear_speed_ = 1.0;

    double kp_ang_ = 1.0;
    double ki_ang_ = 0.0;
    double kd_ang_ = 1.0;
    double ki_ang_max_ = 1.0;

    double d_err_ = 0;
    double last_err_ = 0;
    double i_err_ = 0;
    rclcpp::Time last_time_;
    bool has_last_time_ = false;

    size_t last_nearest_idx_ = 0;

    geometry_msgs::msg::PoseStamped getNextPose(const geometry_msgs::msg::PoseStamped &current_pose);
    double calculateAngDiff(const geometry_msgs::msg::PoseStamped &current_pose, const geometry_msgs::msg::PoseStamped &next_pose);
};
