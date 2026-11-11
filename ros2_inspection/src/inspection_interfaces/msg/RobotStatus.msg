# 巡检机器人状态
string robot_id    # 机器人编号，例如 robot_01
float64 x          # 当前 X 坐标，单位 m
float64 y          # 当前 Y 坐标，单位 m
float64 speed      # 当前速度，单位 m/s
float64 battery    # 剩余电量，单位 %
string state       # 运行状态：moving / stopped
