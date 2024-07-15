import os
import numpy as np
import matplotlib.pyplot as plt
from naoth.log import Reader as LogReader

def analyze_and_plot_frame_times(log_file, outlier_threshhold):
    frame_numbers = []
    time_differences = []
    time_differences_outliers = []
    outliers = []
    
    with LogReader(log_file) as reader:
        for frame_number, frame in enumerate(reader.read(), 1):
            if 'FrameInfo' in frame:
                frame_time = frame['FrameInfo'].time
                
                if frame_number > 1:
                    time_diff_ms = frame_time - prev_frame_time
                    print(frame_time, prev_frame_time, time_diff_ms)

                    if time_diff_ms < outlier_threshhold:
                        frame_numbers.append(frame_number)
                        time_differences.append(time_diff_ms)
                    else:
                        outliers.append(frame_number)
                        time_differences_outliers.append(time_diff_ms)
                    
                prev_frame_time = frame_time

     # Plotting all in one figure
    plt.figure(figsize=(12, 15))

    # Plot 1: All points
    plt.subplot(3, 1, 1)
    plt.scatter(frame_numbers, time_differences, alpha=0.5, s=2)
    #jitter = np.random.normal(0, 0.1, len(time_differences))
    #plt.scatter(frame_numbers, time_differences + jitter, alpha=0.5, s=2)
    plt.title('Time Differences Between Frames')
    #plt.xlabel('Frame Number')
    plt.ylabel('Time Difference (ms)')
    #plt.legend()

    # Plot 2: All extreme outliers
    plt.subplot(3, 1, 2)
    plt.scatter(outliers, time_differences_outliers, alpha=0.5, s=2)
    #jitter = np.random.normal(0, 0.1, len(time_differences_outliers))
    #plt.scatter(outliers, time_differences_outliers + jitter, alpha=0.5, s=2)
    plt.title('Time Differences Between Frames')
    #plt.xlabel('Frame Number')
    plt.ylabel('Time Difference (ms)')
    #plt.legend()

    # Plot 3: Histogram
    plt.subplot(3, 1, 3)
    plt.hist(time_differences, bins=30, edgecolor='black')
    plt.title('Histogram of Time Differences')
    plt.xlabel('Time Difference (ms)')
    plt.ylabel('Frequency')
    #plt.legend()

    plt.tight_layout()
    plt.show()

    # Print statistics
    avg_diff = sum(time_differences) / len(time_differences)
    print(f"\nAverage time difference: {avg_diff:.2f} ms")
    print(f"Minimum time difference: {min(time_differences):.2f} ms")
    print(f"Maximum time difference: {max(time_differences):.2f} ms")
    print(f"Outliers time difference: {time_differences_outliers} ms")

if __name__ == "__main__":
    analyze_and_plot_frame_times('game.log', 40)
    #analyze_and_plot_frame_times('sensor.log', 20)