"""由 ROS 节点发布 odom -> base_footprint

原来由 Gazebo 的差速驱动插件发，它盖的时间戳会倒退，
RViz/slam_toolbox 收到后会报 TF_OLD_DATA 并丢弃，导致 TF 链断裂、车"僵住"。
改用普通节点后时间戳永远单调递增。
"""
import rclpy
from geometry_msgs.msg import TransformStamped
from nav_msgs.msg import Odometry
from rclpy.node import Node
from tf2_ros import TransformBroadcaster

class OdomTF(Node):
    def __init__(self):
        super().__init__('odom_tf_broadcaster')
        self.br = TransformBroadcaster(self)
        self.create_subscription(Odometry, '/odom', self.cb, 50)
        self.count = 0
        self.get_logger().info('odom->base_footprint 由本节点发布')

    def cb(self, msg):
        t = TransformStamped()
        t.header.stamp = msg.header.stamp
        t.header.frame_id = 'odom'
        t.child_frame_id = 'base_footprint'
        p = msg.pose.pose.position
        q = msg.pose.pose.orientation
        t.transform.translation.x = p.x
        t.transform.translation.y = p.y
        t.transform.translation.z = p.z
        t.transform.rotation = q
        self.br.sendTransform(t)

def main():
    rclpy.init()
    n = OdomTF()
    try:
        rclpy.spin(n)
    except KeyboardInterrupt:
        pass
    finally:
        n.destroy_node()
        try:
            rclpy.shutdown()
        except Exception:
            pass

if __name__ == '__main__':
    main()
