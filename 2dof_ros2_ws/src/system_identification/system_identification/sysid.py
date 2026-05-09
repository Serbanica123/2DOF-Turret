import rclpy
from rclpy.node import Node
import numpy as np
import datetime
import csv
from ament_index_python.packages import get_package_share_directory
import os
import matplotlib.pyplot as plt
from scipy.signal import chirp
from geometry_msgs.msg import Twist
from std_msgs.msg import Float32MultiArray

def generate_prbs(
    samples=2500,
    dt=0.02,
    A0=25,
    switch_prob=0.35,
    levels=None,
    seed=None,
    showPlot=False
):
    """
    Generate a PRBS-like signal for system identification.

    Parameters
    ----------
    samples : int
        Number of samples.

    dt : float
        Sampling time.

    A0 : float
        Default amplitude (+/- A0).

    switch_prob : float
        Probability of switching state at each sample.
        Lower -> slower signal
        Higher -> faster excitation

    levels : list or None
        Optional discrete levels.
        Example:
            [-10, 0, 10]
        If None -> binary PRBS (+/-A0)

    seed : int or None
        Random seed.

    showPlot : bool
        Plot generated signal.

    Returns
    -------
    u : ndarray
        PRBS signal.

    t : ndarray
        Time vector.
    """

    if seed is not None:
        np.random.seed(seed)

    t = np.arange(samples) * dt

    # Binary PRBS levels
    if levels is None:
        levels = [-A0, A0]

    u = np.zeros(samples)

    # Initial state
    current = np.random.choice(levels)

    for i in range(samples):

        # Random switching
        if np.random.rand() < switch_prob:
            current = np.random.choice(levels)

        u[i] = current

    if showPlot:
        plt.figure()
        plt.plot(t, u)
        plt.title("PRBS excitation")
        plt.xlabel("Time [s]")
        plt.ylabel("Amplitude")
        plt.grid()

    return u, t


def generate_chirp(samples=2500, dt=0.02, f0=0.7, f1=1.75, A0=35, vary=False, phi=90, showPlot=False):

    t = np.arange(samples) * dt

    x = chirp(t, f0=f0, f1=f1, t1=t[-1], method='lin', phi=phi)

    f_t = f0 + (f1 - f0) * t / t[-1]

    x_scaled = (A0 * (f_t / f0) * x) if vary else A0*x  # linear scaling

    if(showPlot):
        plt.plot(t, x_scaled)

    return x_scaled, t

def generate_mimo_noise(
    samples=2500,
    dt=0.02,
    Ayaw=3.0,
    Apitch=3.0,
    alpha=0.97,
    seed=None,
    clip=5.0,
    showPlot=False
):
    import scipy.signal as signal

    if seed is not None:
        np.random.seed(seed)

    t = np.arange(samples) * dt

    # 1. Raw white noise (independent axes)
    yaw_raw = np.random.randn(samples)
    pitch_raw = np.random.randn(samples)

    # 2. Color the noise (low-pass filter)
    yaw = signal.lfilter([1], [1, -alpha], yaw_raw)
    pitch = signal.lfilter([1], [1, -alpha], pitch_raw)

    # 3. Normalize
    yaw = yaw / np.std(yaw)
    pitch = pitch / np.std(pitch)

    # 4. Scale to velocity amplitudes (deg/s)
    yaw = Ayaw * yaw
    pitch = Apitch * pitch

    # 5. Safety clipping (important for real motors)
    yaw = np.clip(yaw, -clip, clip)
    pitch = np.clip(pitch, -clip, clip)

    # 6. Plot if needed
    if showPlot:
        plt.figure()

        plt.subplot(2,1,1)
        plt.plot(t, yaw)
        plt.title("Yaw velocity noise (deg/s)")
        plt.grid()

        plt.subplot(2,1,2)
        plt.plot(t, pitch)
        plt.title("Pitch velocity noise (deg/s)")
        plt.grid()

        plt.tight_layout()

    return yaw, pitch , t


class SYSID(Node):

    def __init__(self):
        super().__init__("sysid")
        self.subscription = self.create_subscription(
            Float32MultiArray, "turret/state", self.listener_callback, 10
        )
        self.subscription  # prevent unused variable warning

        base_name = "chirp50Yaw_log"

        timestamp_str = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")

        filename = os.path.join(
            "src/system_identification/logs", f"{base_name}_{timestamp_str}.csv"
        )

        self.csvfile = open(filename, mode="w", newline="")
        self.writer = csv.writer(self.csvfile)

        self.writer.writerow(
            [
                "dt",
                "yaw_cmd",
                "yaw_pos",
                "pitch_cmd",
                "pitch_pos",
            ]
        )

        self.inputYaw, _ =generate_chirp()
        self.inputPitch, _= 0.0,0.0
        # self.inputYaw, self.inputPitch, _= generate_mimo_noise()
        self.publisher_ = self.create_publisher(Twist, "turret/cmd_vel", 10)

        timer_period = 0.02  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        msg = Twist()

        if self.i < len(self.inputYaw):
            velPitch= self.inputPitch[self.i] if isinstance(self.inputPitch, (list, np.ndarray)) else 0.0
            velYaw=self.inputYaw[self.i] if isinstance(self.inputYaw, (list, np.ndarray)) else 0.0
            
            msg.angular.y = velYaw
            msg.angular.z= velPitch
            self.i += 1
        else:
            # publish final stop command once
            msg.angular.z = 0.0
            msg.angular.y = 0.0
            self.publisher_.publish(msg)
            return

        self.publisher_.publish(msg)

    def listener_callback(self, msg):
        data = msg.data
        cmd_yaw = data[3]
        yaw_pos = data[2]
        cmd_pitch = data[0]
        pitch_pos = data[5]
        print(pitch_pos)
        
        #timestamp = self.get_clock().now().nanoseconds * 1e-9
        if self.i < len(self.inputYaw):
            self.writer.writerow(
                [self.i*0.02, cmd_yaw, yaw_pos, cmd_pitch, pitch_pos]
            )
            self.csvfile.flush()
        else:
            self.csvfile.close()
            
    def destroy_node(self):
        self.csvfile.close()
        return super().destroy_node()


def main(args=None):
    rclpy.init(args=args)

    sysid = SYSID()

    rclpy.spin(sysid)

    sysid.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
