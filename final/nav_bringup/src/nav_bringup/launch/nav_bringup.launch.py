import os
import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.conditions import IfCondition

def generate_launch_description():
    nav_bringup_dir = get_package_share_directory('nav_bringup')
    nav2_bringup_dir = get_package_share_directory('nav2_bringup')

    rviz_config_dir = os.path.join(nav_bringup_dir, 'rviz', 'nav2.rviz')

    use_sim_time = launch.substitutions.LaunchConfiguration(
        'use_sim_time', default='true')
    map_yaml_path = launch.substitutions.LaunchConfiguration(
        'map', default=os.path.join(nav_bringup_dir, 'maps', 'nav_start.yaml'))
    nav2_param_path = launch.substitutions.LaunchConfiguration(
        'params_file', default=os.path.join(nav_bringup_dir, 'config', 'nav2_params.yaml'))
    autostart = launch.substitutions.LaunchConfiguration(
        'autostart', default='true')
    use_rviz = launch.substitutions.LaunchConfiguration(
        'rviz', default='false')

    return launch.LaunchDescription([
        launch.actions.DeclareLaunchArgument('use_sim_time', default_value=use_sim_time,
                                             description='Use simulation (Gazebo) clock if true'),
        launch.actions.DeclareLaunchArgument('map', default_value=map_yaml_path,
                                             description='Full path to map file to load'),
        launch.actions.DeclareLaunchArgument('params_file', default_value=nav2_param_path,
                                             description='Full path to param file to load'),
        launch.actions.DeclareLaunchArgument('autostart', default_value=autostart,
                                             description='Automatically configure and activate all lifecycle nodes'),
        launch.actions.DeclareLaunchArgument('rviz', default_value=use_rviz,
                                             description='Start another rviz (display_robot.launch.py already starts one)'),

        launch.actions.IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                [nav2_bringup_dir, '/launch', '/bringup_launch.py']),
            launch_arguments={
                'map': map_yaml_path,
                'use_sim_time': use_sim_time,
                'params_file': nav2_param_path,
                'autostart': autostart,
                'use_composition': 'False',
            }.items(),
        ),

        launch_ros.actions.Node(
            condition=IfCondition(use_rviz),
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', rviz_config_dir],
            parameters=[{'use_sim_time': use_sim_time}],
            output='screen'),
    ])
