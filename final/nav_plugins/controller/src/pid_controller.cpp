#include "controller/pid_controller.hpp"

#include <nav2_util/node_utils.hpp>
#include <nav2_util/geometry_utils.hpp>
#include <nav2_core/exceptions.hpp>
#include <pluginlib/class_list_macros.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2/exceptions.h>
#include <tf2/time.h>
#include <cmath>
#include <limits>
#include <algorithm>
#include <stdexcept>

void PidController::configure(const rclcpp_lifecycle::LifecycleNode::WeakPtr & parent,
    std::string name, std::shared_ptr<tf2_ros::Buffer> tf,
    std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap)
{
    node_ = parent;
    tf_ = tf;
    costmap_ = costmap;
    plugin_name_ = name;

    auto node = node_.lock();
    if (!node) {
        throw std::runtime_error("PidController: 父节点失效");
    }

    logger_ = node->get_logger();
    clock_ = node->get_clock();

    nav2_util::declare_parameter_if_not_declared(
        node, plugin_name_ + ".max_linear_speed", rclcpp::ParameterValue(1.0));
    nav2_util::declare_parameter_if_not_declared(
        node, plugin_name_ + ".max_angular_speed", rclcpp::ParameterValue(0.5));
    nav2_util::declare_parameter_if_not_declared(
        node, plugin_name_ + ".kp_ang", rclcpp::ParameterValue(1.0));
    nav2_util::declare_parameter_if_not_declared(
        node, plugin_name_ + ".ki_ang", rclcpp::ParameterValue(0.0));
    nav2_util::declare_parameter_if_not_declared(
        node, plugin_name_ + ".kd_ang", rclcpp::ParameterValue(1.0));
    nav2_util::declare_parameter_if_not_declared(
        node, plugin_name_ + ".ki_ang_max", rclcpp::ParameterValue(1.0));

    max_linear_speed_ = node->get_parameter(
        plugin_name_ + ".max_linear_speed").as_double();
    max_angular_speed_ = node->get_parameter(
        plugin_name_ + ".max_angular_speed").as_double();
    kp_ang_ = node->get_parameter(plugin_name_ + ".kp_ang").as_double();
    ki_ang_ = node->get_parameter(plugin_name_ + ".ki_ang").as_double();
    kd_ang_ = node->get_parameter(plugin_name_ + ".kd_ang").as_double();
    ki_ang_max_ = node->get_parameter(plugin_name_ + ".ki_ang_max").as_double();

    base_linear_speed_ = max_linear_speed_;

    d_err_ = 0.0;
    i_err_ = 0.0;
    has_last_time_ = false;
    last_nearest_idx_ = 0;

    RCLCPP_INFO(logger_,
        "PID 控制器配置完成: kp=%.2f ki=%.2f kd=%.2f, v_max=%.2f, w_max=%.2f",
        kp_ang_, ki_ang_, kd_ang_, max_linear_speed_, max_angular_speed_);
}

void PidController::cleanup()
{
    node_.reset();
    clock_.reset();
    tf_.reset();
    costmap_.reset();
    RCLCPP_INFO(logger_, "pid插件已清理");
}

void PidController::activate()
{
    RCLCPP_INFO(logger_, "pid插件已激活");
}

void PidController::deactivate()
{

}

void PidController::setPlan(const nav_msgs::msg::Path & path)
{

    bool goal_changed = global_path_.poses.empty() || path.poses.empty();
    if (!goal_changed) {
        const auto & old_goal = global_path_.poses.back().pose.position;
        const auto & new_goal = path.poses.back().pose.position;
        goal_changed = std::hypot(new_goal.x - old_goal.x, new_goal.y - old_goal.y) > 0.1;
    }

    global_path_ = path;

    last_nearest_idx_ = 0;

    if (goal_changed) {
        has_last_time_ = false;
        last_err_ = 0.0;
        i_err_ = 0.0;
        d_err_ = 0.0;
    }
}

void PidController::setSpeedLimit(const double & speed_limit, const bool & percentage)
{

    if (speed_limit <= 0.0) {
        max_linear_speed_ = base_linear_speed_;
        return;
    }

    if (percentage) {

        const double ratio = (speed_limit > 1.0) ? speed_limit / 100.0 : speed_limit;
        max_linear_speed_ = base_linear_speed_ * ratio;
    } else {

        max_linear_speed_ = std::min(speed_limit, base_linear_speed_);
    }
}

double PidController::calculateAngDiff(const geometry_msgs::msg::PoseStamped &current_pose, const geometry_msgs::msg::PoseStamped &next_pose)
{
    const double dx = next_pose.pose.position.x - current_pose.pose.position.x;
    const double dy = next_pose.pose.position.y - current_pose.pose.position.y;
    const double target_yaw = atan2(dy, dx);

    const auto q = current_pose.pose.orientation;
    const double current_yaw = atan2(2.0 * (q.w * q.z + q.x * q.y),
        1.0 - 2.0 * (q.y * q.y + q.z * q.z)
    );

    const double diff = target_yaw - current_yaw;
    return atan2(std::sin(diff), std::cos(diff));
}

geometry_msgs::msg::PoseStamped PidController::getNextPose (const geometry_msgs::msg::PoseStamped &current_pose)
{
    constexpr double lookahead_s = 0.5;
    double min_dist = std::numeric_limits<double>::max();

    for(size_t i = last_nearest_idx_; i < global_path_.poses.size(); ++i)
    {
        double dist_x = global_path_.poses[i].pose.position.x - current_pose.pose.position.x;
        double dist_y = global_path_.poses[i].pose.position.y - current_pose.pose.position.y;
        double dist = std::hypot(dist_x, dist_y);

        if(dist < min_dist)
        {
            min_dist = dist;
            last_nearest_idx_ = i;
        }
    }

    for(size_t i = last_nearest_idx_; i < global_path_.poses.size(); i++)
    {
        double dist_x = global_path_.poses[i].pose.position.x - current_pose.pose.position.x;
        double dist_y = global_path_.poses[i].pose.position.y - current_pose.pose.position.y;
        double dist = std::hypot(dist_x, dist_y);

        if(dist >= lookahead_s)
        {
            return global_path_.poses[i];
        }
    }

    return global_path_.poses.back();
}

geometry_msgs::msg::TwistStamped PidController::computeVelocityCommands(
    const geometry_msgs::msg::PoseStamped & pose,
    const geometry_msgs::msg::Twist & velocity,
    nav2_core::GoalChecker * goal_checker)
{
    (void) velocity;
    (void) goal_checker;

    geometry_msgs::msg::TwistStamped cmd;
    cmd.header = pose.header;

    if(global_path_.poses.empty())
    {
        return cmd;
    }

    geometry_msgs::msg::PoseStamped plan_pose = pose;
    if (!global_path_.header.frame_id.empty() &&
        pose.header.frame_id != global_path_.header.frame_id)
    {
        try
        {
            const auto tf_pose = tf_->lookupTransform(
                global_path_.header.frame_id, pose.header.frame_id, tf2::TimePointZero);
            tf2::doTransform(pose, plan_pose, tf_pose);
        }
        catch (const tf2::TransformException & ex)
        {
            RCLCPP_WARN_THROTTLE(logger_, *clock_, 2000, "TF 变换失败: %s", ex.what());
            return cmd;
        }
    }

    const auto target = getNextPose(plan_pose);
    const auto err    = calculateAngDiff(plan_pose, target);
    const rclcpp::Time now = clock_ -> now();
    double ang_z = kp_ang_ * err;

    if(has_last_time_)
    {
        const double dt = (now - last_time_).seconds();
        if(dt > 0)
        {
            i_err_ += err * dt;
            i_err_ = std::clamp(i_err_, -ki_ang_max_, ki_ang_max_);

            d_err_ = (err - last_err_) / dt;

            ang_z = kp_ang_*err + ki_ang_*i_err_ + kd_ang_*d_err_;
        }
    }

    cmd.twist.angular.z = std::clamp(ang_z, -max_angular_speed_, max_angular_speed_);

    if(std::fabs(err) > 0.8)
    {
        cmd.twist.linear.x = 0.0;
    }
    else
    {
        cmd.twist.linear.x = max_linear_speed_ * (1.0 - std::fabs(err) / 0.8);
    }

    has_last_time_ = true;
    last_err_ = err;
    last_time_ = now;

    RCLCPP_INFO_THROTTLE(logger_, *clock_, 2000,
        "PID: err=%.2f rad, vx=%.2f m/s, wz=%.2f rad/s",
        err, cmd.twist.linear.x, cmd.twist.angular.z);

    return cmd;
}

PLUGINLIB_EXPORT_CLASS(PidController, nav2_core::Controller)
