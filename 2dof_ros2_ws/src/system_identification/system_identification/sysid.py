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
import scipy.signal as signal
def generate_mimo_noise(
    samples=2500,
    dt=0.035,
    Ayaw=5.0,
    Apitch=5.0,
    alpha_yaw=0.90,
    alpha_pitch=0.87,
    alpha_dc=0.995,
    clip=8.0,
    seed=None,
    showPlot=False
):
    """
    Generate smooth MIMO excitation signals for turret system identification.

    Inputs:
        yaw velocity command   [deg/s]
        pitch velocity command [deg/s]

    Designed for:
        - MIMO sysid
        - closed-loop excitation
        - velocity-input / position-output systems

    Features:
        - independent excitation channels
        - colored broadband noise
        - DC drift removal
        - smooth saturation
        - bounded position excursion
    """

    if seed is not None:
        np.random.seed(seed)

    t = np.arange(samples) * dt

    # ---------------------------------------------------------
    # 1. Independent white noise
    # ---------------------------------------------------------

    yaw_raw = np.random.randn(samples)
    pitch_raw = np.random.randn(samples)

    # ---------------------------------------------------------
    # 2. Color the noise (1st-order low-pass)
    # ---------------------------------------------------------

    yaw = signal.lfilter([1], [1, -alpha_yaw], yaw_raw)
    pitch = signal.lfilter([1], [1, -alpha_pitch], pitch_raw)

    # ---------------------------------------------------------
    # 3. Remove slow drift / DC component
    # ---------------------------------------------------------

    yaw_dc = signal.lfilter([1 - alpha_dc], [1, -alpha_dc], yaw)
    pitch_dc = signal.lfilter([1 - alpha_dc], [1, -alpha_dc], pitch)

    yaw = yaw - yaw_dc
    pitch = pitch - pitch_dc

    # ---------------------------------------------------------
    # 4. Normalize RMS
    # ---------------------------------------------------------

    yaw = yaw / np.sqrt(np.mean(yaw**2))
    pitch = pitch / np.sqrt(np.mean(pitch**2))

    # ---------------------------------------------------------
    # 5. Scale amplitudes
    # ---------------------------------------------------------

    yaw = Ayaw * yaw
    pitch = Apitch * pitch

    # ---------------------------------------------------------
    # 6. Smooth saturation (better than hard clipping)
    # ---------------------------------------------------------

    yaw = clip * np.tanh(yaw / clip)
    pitch = clip * np.tanh(pitch / clip)

    # ---------------------------------------------------------
    # 7. Optional plots
    # ---------------------------------------------------------

    if showPlot:

        plt.figure(figsize=(12, 6))

        plt.subplot(2, 1, 1)
        plt.plot(t, yaw)
        plt.title("Yaw Velocity Excitation")
        plt.ylabel("deg/s")
        plt.grid(True)

        plt.subplot(2, 1, 2)
        plt.plot(t, pitch)
        plt.title("Pitch Velocity Excitation")
        plt.ylabel("deg/s")
        plt.xlabel("Time [s]")
        plt.grid(True)

        plt.tight_layout()
        plt.show()

    return yaw, pitch, t


class SYSID(Node):

    def __init__(self):
        super().__init__("sysid")
        self.subscription = self.create_subscription(
            Float32MultiArray, "turret/state", self.listener_callback, 10
        )
        self.subscription  # prevent unused variable warning

        base_name = "PRBS_exp_yaw"

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

        #self.inputYaw, _ = generate_chirp(samples=1500, dt=0.035, f0=0.25, f1=0.9, A0=25, vary=False, phi=90, showPlot=False)
        #self.inputPitch, _= generate_chirp(samples=1500, dt=0.035, f0=1, f1=1.65, A0=25, vary=False, phi=-90, showPlot=False)
        self.inputYaw, self.inputPitch, _= generate_mimo_noise(samples=4000,dt=0.035,Ayaw=6,Apitch=5,clip=8,seed=1,showPlot=False)
        self.inputYaw=0.0
        self.publisher_ = self.create_publisher(Twist, "turret/cmd_vel", 10)
        self.log_list=[]
        timer_period = 0.035  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        msg = Twist()

        if self.i < len(self.inputPitch):
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

        timestamp = self.get_clock().now().nanoseconds * 1e-9
        if self.i < len(self.inputPitch):
            self.log_list.append([timestamp, cmd_yaw, yaw_pos, cmd_pitch, pitch_pos])
        else:
            if not self.csvfile.closed:
                self.writer.writerows(self.log_list)
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
