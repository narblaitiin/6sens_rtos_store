import numpy as np
from scipy.signal import spectrogram
import matplotlib.pyplot as plt
import glob
import argparse

file_folder = "lfs"
fs = 100  # sampling rate (Hz)

def show_spectrogram(data_folder: str, show_raw_signal: bool):
    # Concatenate data from all file in the data folder
    all_data = []
    for fpath in sorted(glob.glob(data_folder + "/geophone_*.dat")):
        data = np.fromfile(fpath, dtype=np.int16)
        all_data.append(data)
    data = np.concatenate(all_data)

    # Separate data into chunks of 128 samples 
    batch_size = 128
    lta_factor = 10
    n_batches = data.size // batch_size
    trimmed = data[: n_batches * batch_size]
    batches = trimmed.reshape(n_batches, batch_size)

    # Compute, for each chunk, its min, max, and average value.
    mins = batches.min(axis=1)
    maxs = batches.max(axis=1)
    means = batches.mean(axis=1)
    # Compute lta/sta and er ratios
    lta_sta = [means[i]/means[min(0,i-lta_factor):i].sum()*max(i,10) for i in range(len(means))]
    er = [pow(means[i+1],2)/pow(means[i],2) for i in range(len(means)-1)]
    er = [1] + er
    mer = np.pow(er*np.square(means),3)

    # Compute spectrogram
    f, t, Sxx = spectrogram(data, fs=fs, nperseg=256, noverlap=128)

    # Display raw signal
    if show_raw_signal:
        plt.plot(data)
        plt.ylabel('Amplitude')
        plt.xlabel('Time [s]')
        plt.show()


    fig, (ax1, ax2, ax3, ax4) = plt.subplots(4, 1, sharex='all', figsize=(10, 8))

    # Spectrogram on the first subplot
    im = ax1.pcolormesh(t, f, 10 * np.log10(Sxx), shading='gouraud')
    ax1.set_ylabel('Frequency [Hz]')
    ax1.set_title('Spectrogram')

    # Truncate series to common length to avoid shape mismatches
    n = min(len(t), len(maxs), len(mins), len(means))
    print("Size is ", n)
    t_trunc = t[:n]

    # Max, Min, Mean on the second subplot
    ax2.plot(t_trunc, maxs[:n], 'r-', linewidth=1.5, label='Max')
    ax2.plot(t_trunc, mins[:n], 'b-', linewidth=1.5, label='Min')
    ax2.plot(t_trunc, means[:n], 'g-', linewidth=1.5, label='Mean')
    # fig.colorbar(im, ax=ax1, label='Power (dB)')
    ax2.set_ylabel('Amplitude')
    ax2.set_xlabel('Time [s]')
    ax2.legend()

    # STA/LTA ratio on the third subplot
    ax3.plot(t_trunc, lta_sta[:n],  linewidth=1.5, label='STA/LTA')
    ax3.set_ylabel('STA/LTA ratio')
    ax3.set_xlabel('Time [s]')
    ax3.legend()

    # MER on the fourth subplot
    ax4.plot(t_trunc, mer[:n],  linewidth=1.5)
    ax4.set_ylabel('MER ratio')
    ax4.set_xlabel('Time [s]')
    ax4.legend()
    plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Script to analyze vibration data extracted from SASTRESS sensor. "
                    "Display spectrogram, metrics (min, max, mean), STA/LTA, and MER (modified energy ratios)."
    )
    parser.add_argument("-d", "--dir", default="lfs", help="Data folder path (default: lfs)")
    parser.add_argument("--raw", help="Show raw signal (default: false)", action='store_true')
    args = parser.parse_args()

    show_spectrogram(args.dir, args.raw)