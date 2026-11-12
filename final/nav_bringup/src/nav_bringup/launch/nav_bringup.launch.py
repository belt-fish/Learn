"""起 Nav2：地图 + AMCL + 规划 + 控制 + 行为树

行为树路径在启动时动态注入，所以换机器 / 换工作区都不用改参数文件。
"""
import os
import re
import tempfile

import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory
from launch.actions import OpaqueFunction, DeclareLaunchArgument, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration


def _start_nav2(context, *args, **kwargs):
    """把参数文件里的行为树路径替换成真实安装路径，再交给 nav2 官方 bringup。

    为什么要这么绕：
      bt_navigator 的 default_nav_to_pose_bt_xml 必须是绝对路径，
      而绝对路径写死在 yaml 里就换不了机器。所以启动时读一遍 yaml、
      把这一行改成当前工作区里的真实路径、写到临时文件，再传进去。
    """
    nav_bringup_dir = get_package_share_directory('nav_bringup')
    nav2_bringup_dir = get_package_share_directory('nav2_bringup')

    bt_xml = os.path.join(nav_bringup_dir, 'behavior_trees', 'pid_follow_path.xml')

    src = LaunchConfiguration('params_file').perform(context)
    with open(src, 'r', encoding='utf-8') as f:
        text = f.read()

    # 只替换这一行的值，其余（注释、顺序、缩进）原样保留
    text = re.sub(
        r'^(\s*)default_nav_to_pose_bt_xml:.*$',
        lambda m: '{}default_nav_to_pose_bt_xml: "{}"'.format(m.group(1), bt_xml),
        text, count=1, flags=re.M)

    resolved = os.path.join(tempfile.gettempdir(), 'nav2_params_resolved.yaml')
    with open(resolved, 'w', encoding='utf-8') as f:
        f.write(text)

    return [IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            [nav2_bringup_dir, '/launch', '/bringup_launch.py']),
        launch_arguments={
            'map': LaunchConfiguration('map'),
            'use_sim_time': LaunchConfiguration('use_sim_time'),
            'params_file': resolved,
            'autostart': LaunchConfiguration('autostart'),
            'use_composition': 'False',
        }.items(),
    )]


def generate_launch_description():
    nav_bringup_dir = get_package_share_directory('nav_bringup')

    rviz_config_dir = os.path.join(nav_bringup_dir, 'rviz', 'nav2.rviz')

    use_sim_time = LaunchConfiguration('use_sim_time', default='true')
    map_yaml_path = LaunchConfiguration(
        'map', default=os.path.join(nav_bringup_dir, 'maps', 'nav_start.yaml'))
    nav2_param_path = LaunchConfiguration(
        'params_file', default=os.path.join(nav_bringup_dir, 'config', 'nav2_params.yaml'))
    autostart = LaunchConfiguration('autostart', default='true')
    use_rviz = LaunchConfiguration('rviz', default='false')

    return launch.LaunchDescription([
        DeclareLaunchArgument('use_sim_time', default_value=use_sim_time,
                              description='Use simulation (Gazebo) clock if true'),
        DeclareLaunchArgument('map', default_value=map_yaml_path,
                              description='Full path to map file to load'),
        DeclareLaunchArgument('params_file', default_value=nav2_param_path,
                              description='Full path to param file to load'),
        DeclareLaunchArgument('autostart', default_value=autostart,
                              description='Automatically configure and activate all lifecycle nodes'),
        DeclareLaunchArgument('rviz', default_value=use_rviz,
                              description='Start another rviz (nav_sim.launch.py already starts one)'),

        # 行为树路径要等命令行参数解析完才知道，所以用 OpaqueFunction 延迟到运行时
        OpaqueFunction(function=_start_nav2),

        launch_ros.actions.Node(
            condition=IfCondition(use_rviz),
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', rviz_config_dir],
            parameters=[{'use_sim_time': use_sim_time}],
            output='screen'),
    ])
