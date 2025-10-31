import numpy as np
from scipy.signal import spectrogram
import matplotlib.pyplot as plt
import glob

fs = 100  # sampling rate (Hz)

all_data = []
for fpath in sorted(glob.glob("geophone_*.dat")):
    data = np.fromfile(fpath, dtype=np.int16)
    all_data.append(data)

data = np.concatenate(all_data)

f, t, Sxx = spectrogram(data, fs=fs, nperseg=256, noverlap=128)
plt.pcolormesh(t, f, 10*np.log10(Sxx))
plt.ylabel('Frequency [Hz]')
plt.xlabel('Time [s]')
plt.show()