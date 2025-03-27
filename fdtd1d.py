import numpy as np
import scipy.io
import matplotlib.pyplot as plt

# Load the timing data from the .mat file
data = scipy.io.loadmat('timing_datanew.mat')
timing_data = data['timing_datanew'].flatten()

# Plot the timing data
plt.figure(figsize=(10, 6))
plt.plot(timing_data, label='Run Time per Timestep')
plt.xlabel('Timestep')
plt.ylabel('Run Time (seconds)')
plt.title('Run Time per Timestep for FDTD Simulation')
plt.legend()
plt.grid(True)

# Save the plot as an image file
plt.savefig('timing_data_plot.png')

print("Plot saved as timing_data_plot.png")
