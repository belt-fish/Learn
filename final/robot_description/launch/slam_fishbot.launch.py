"""fishbot + Gazebo Classic + SLAM 建图"""
import os

import launch
import launch_ros
from launch.actions import IncludeLaunchDescription, TimerAction, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import Command, LaunchConfiguration
from launch_ros.parameter_descriptions import ParameterValue
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg = get_package_share_directory('robot_description')
    gazebo_ros = get_package_share_directory('gazebo_ros')

    world = os.path.join(pkg, 'worlds', 'wall_demo', 'warehouse_classic.world')
    model = os.path.join(pkg, 'urdf', 'sim_robot', 'fishbot_classic.urdf.xacro')
    rviz_cfg = os.path.join(pkg, 'config', 'rviz', 'slam.rviz')

    gzserver = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(gazebo_ros, 'launch', 'gzserver.launch.py')),
        launch_arguments={'world': world}.items())
    gzclient = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(gazebo_ros, 'launch', 'gzclient.launch.py')))

    rsp = launch_ros.actions.Node(
        package='robot_state_publisher', executable='robot_state_publisher',
        parameters=[{'robot_description': ParameterValue(
            Command(['xacro ', model]), value_type=str), 'use_sim_time': True}],
        output='screen')

    spawn = TimerAction(period=6.0, actions=[
        launch_ros.actions.Node(
            package='gazebo_ros', executable='spawn_entity.py',
            arguments=['-topic', 'robot_description', '-entity', 'fishbot',
                       '-x', '0', '-y', '0', '-z', '0.05'],
            output='screen')])

    odom_tf = launch_ros.actions.Node(
        package='robot_description', executable='odom_tf_broadcaster.py',
        name='odom_tf_broadcaster',
        parameters=[{'use_sim_time': True}], output='screen')

    joint_state_publisher_node = launch_ros.actions.Node(
        package='joint_state_publisher', executable='joint_state_publisher',
        parameters=[{'use_sim_time': True}], output='screen')

    watchdog = launch_ros.actions.Node(
        package='robot_description', executable='cmd_vel_watchdog.py',
        name='cmd_vel_watchdog', parameters=[{'use_sim_time': True}],
        output='screen')

    slam = TimerAction(period=10.0, actions=[
        launch_ros.actions.Node(
            package='slam_toolbox', executable='async_slam_toolbox_node',
            name='slam_toolbox', output='screen',
            parameters=[{
                'use_sim_time': True,
                'odom_frame': 'odom',
                'base_frame': 'base_footprint',
                'scan_topic': '/scan',
                'use_scan_matching': True,
                'minimum_travel_distance': 0.15,
                'minimum_travel_heading': 0.15,
                'mode': 'mapping',
                'resolution': 0.04,
                'max_laser_range': 11.0,
                'map_update_interval': 1.0,
            }])])

    rviz = TimerAction(period=12.0, actions=[
        launch_ros.actions.Node(
            package='rviz2', executable='rviz2',
            arguments=['-d', rviz_cfg],
            parameters=[{'use_sim_time': True}], output='screen')])

    return launch.LaunchDescription([
        DeclareLaunchArgument('world', default_value=world),
        gzserver, gzclient, rsp, joint_state_publisher_node, odom_tf, watchdog, spawn, slam, rviz,
    ])
