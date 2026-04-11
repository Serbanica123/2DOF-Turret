from launch import LaunchDescription
from launch.actions import ExecuteProcess
from launch_ros.actions import Node


def generate_launch_description():
    agent = ExecuteProcess(
        cmd=[
            'MicroXRCEAgent',
            'serial',
            '--dev', '/dev/ttyUSB0',
            '-b', '115200'
        ],
        output='screen'
    )

    controller = Node(
        package='turret_control',
        executable='position_controller',
        name='position_controller',
        output='screen',
        emulate_tty=True
    )

    return LaunchDescription([
        agent,
        controller
    ])
