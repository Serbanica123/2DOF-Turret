import numpy as np
from scipy.signal import chirp
import datetime
import csv
import matplotlib.pyplot as plt
import pandas as pd
from control import StateSpace, forced_response
from scipy.interpolate import interp1d
from scipy.signal import butter, filtfilt

def generate_chirp(samples=2500, dt=0.02, f0=0.6, f1=1.75, A0=25, vary=False, phi=90, showPlot=False):

    t = np.arange(samples) * dt

    x = chirp(t, f0=f0, f1=f1, t1=t[-1], method='lin', phi=phi)

    f_t = f0 + (f1 - f0) * t / t[-1]

    x_scaled = (A0 * (f_t / f0) * x) if vary else A0*x  # linear scaling

    if(showPlot):
        plt.plot(t, x_scaled)

    return x_scaled, t

def generate_prbs(
    samples=2500,
    dt=0.02,
    A0=25,
    switch_prob=0.6,
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

def compute_csv(axis, data):
    t=data["dt"].tolist()
    t0=t[0]
    t=[ti-t0 for ti in t]

    cmd_key=axis+"_cmd"
    pos_key=axis+"_pos"
    
    cmd=data[cmd_key]
    pos=data[pos_key]

    return t, cmd, pos 


def prepare_data(path):

    data= pd.read_csv(path)

    _,cmdPitch,posPitch=compute_csv("pitch", data)
    _,cmdYaw,posYaw=compute_csv("yaw", data)


    U = np.column_stack([cmdYaw, cmdPitch]).T 
    Y = np.column_stack([posYaw, posPitch]).T 
    return U, Y

generate_chirp(showPlot=True)
plt.show()
# U1, Y1= prepare_data("/home/alex/Turret/2DOF-Turret/2dof_ros2_ws/src/system_identification/logs/yaw_log_20260509_134414.csv")
# U2, Y2= prepare_data("/home/alex/Turret/2DOF-Turret/2dof_ros2_ws/src/system_identification/logs/pitch_log_20260509_140836.csv")
# U3, Y3= prepare_data("/home/alex/Turret/2DOF-Turret/2dof_ros2_ws/src/system_identification/logs/full_log_20260509_141642.csv")

# print("Y1", Y1.shape, "U1", U1.shape)
# print("Y2", Y2.shape, "U2", U2.shape)
# print("Y3", Y3.shape, "U3", U3.shape)

# N = min(
#     Y1.shape[1],
#     Y2.shape[1],
#     Y3.shape[1]
# )

# Y1 = Y1[:, :N]
# U1 = U1[:, :N]

# Y2 = Y2[:, :N]
# U2 = U2[:, :N]

# Y3 = Y3[:, :N]
# U3 = U3[:, :N]

# U = np.hstack([U1, U2, U3])
# Y = np.hstack([Y1, Y2, Y3])

# Ts = 0.02  
# from sippy_unipi import system_identification
 
# sys_id = system_identification(Y, U, "N4SID", SS_fixed_order=6, tsample=0.02)

# sys = StateSpace(sys_id.A, sys_id.B, sys_id.C, sys_id.D, Ts)
# print(sys)
# t = np.arange(U1.shape[0]) * Ts

# # simulation format
# U_sim = U.T

# _, y_sim = forced_response(sys, T=t, U=U1)
# y_sim = y_sim.T   # (N,2)

# plt.plot(t, Y[:,0], label="yaw meas")
# plt.plot(t, y_sim[:,0], '--', label="yaw sim")
# plt.show()
# plt.plot(t, Y[:,1], label="pitch meas")
# plt.plot(t, y_sim[:,1], '--', label="pitch sim")
# plt.show()
