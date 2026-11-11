from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, TimerAction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    period_sec = LaunchConfiguration('period_sec')
    battery_step = LaunchConfiguration('battery_step')

    monitor = Node(
        package='inspection_robot',
        executable='status_monitor',
        name='robot_monitor',
        output='screen',
    )

    publisher = Node(
        package='inspection_robot',
        executable='status_publisher',
        name='robot_status_publisher',
        output='screen',
        parameters=[{
            'period_sec': ParameterValue(period_sec, value_type=float),
            'battery_step': ParameterValue(battery_step, value_type=float),
        }],
    )

    return LaunchDescription([
        DeclareLaunchArgument(
            'period_sec',
            default_value='1.0',
            description='状态发布周期（秒），题目要求 1 秒',
        ),
        DeclareLaunchArgument(
            'battery_step',
            default_value='1.0',
            description='每次更新消耗的电量（%），题目要求 1%',
        ),
        # 先起监控节点，再延迟 0.5 秒起发布节点，
        # 避免“首次状态”在订阅者完成发现前就发出去而丢失。
        monitor,
        TimerAction(period=0.5, actions=[publisher]),
    ])
