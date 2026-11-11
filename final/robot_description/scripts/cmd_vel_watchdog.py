"""速度看门狗：0.5 秒收不到运动指令就自动刹车

问题：Gazebo 的差速驱动会永远保持最后一条 /cmd_vel 指令。
      发指令的进程退出、或按键窗口失焦之后，车会一直开（"不受控制"）。
做法：本节点也挂在 /cmd_vel 上，检测到指令停更就补发零速度。
      自己发的零速度不算"新指令"，所以不会自激循环。
"""
import time

import rclpy
from geometry_msgs.msg import Twist
from rclpy.node import Node

EPS = 1e-3
TIMEOUT = 3.0

class Watchdog(Node):
    def __init__(self):
        super().__init__('cmd_vel_watchdog')
        self.pub = self.create_publisher(Twist, '/cmd_vel', 10)
        self.create_subscription(Twist, '/cmd_vel', self.on_cmd, 50)
        self.armed = False
        self.last_time = 0.0
        self.create_timer(0.05, self.tick)
        self.get_logger().info(
            f'看门狗启动：超过 {TIMEOUT} 秒没有运动指令就自动刹车')

    def on_cmd(self, msg):
        moving = (abs(msg.linear.x) > EPS or abs(msg.linear.y) > EPS
                  or abs(msg.angular.z) > EPS)
        if moving:
            self.armed = True
            self.last_time = time.monotonic()

    def tick(self):
        if not self.armed:
            return
        if time.monotonic() - self.last_time > TIMEOUT:
            self.pub.publish(Twist())
            self.armed = False
            self.get_logger().info('指令超时，已自动刹车',
                                   throttle_duration_sec=2.0)

def main():
    rclpy.init()
    n = Watchdog()
    try:
        rclpy.spin(n)
    except KeyboardInterrupt:
        pass
    finally:
        n.pub.publish(Twist())
        n.destroy_node()
        try:
            rclpy.shutdown()
        except Exception:
            pass

if __name__ == '__main__':
    main()
