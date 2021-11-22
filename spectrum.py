#!/usr/bin/env python3
import matplotlib.pyplot as plt 
import matplotlib.animation as animation 
import numpy as np 
import sys
import scipy.constants
import matplotlib.cm as colormap
# plt.style.use('dark_background')

if len(sys.argv) != 2:
	print("Invocation: %s <CSV Data File>\n\n"%(sys.argv[0]))
	sys.exit(0)

argv = sys.argv

ifile = open(argv[1])
fmin = 0
fmax = 0
spectrum = []
counter = 0
for line in ifile:
	words = line.rstrip().split(',')
	if counter == 0:
		fmin = float(words[2])
		fmax = float(words[3])
	
	spectrum.append(words[4:])
	counter += 1
ifile.close()

spectrum = np.array(spectrum, dtype = float)

fig = plt.figure(figsize = (20, 20)) 
ax = plt.axes(xlim=(fmin, fmax), ylim=(-50, 50)) 
line, = ax.plot([], [], lw=0.5, color = 'k')
ax.axvline((fmin+fmax)*0.5, ls = '--', color = 'b')

vel = np.arange(-10, 11, 2, dtype = float)*1e3
c = scipy.constants.c

cfreq = (fmin + fmax)*0.5

doppler_freqs = cfreq*np.sqrt((1-vel/c)/(1+vel/c))

cmap = colormap.get_cmap('Spectral')
for d in doppler_freqs:
	if d == cfreq:
		continue
	color_idx = (d - doppler_freqs.min()) / (doppler_freqs.max() - doppler_freqs.min())
	ax.axvline(d, ls = '--', color = cmap(color_idx)) #c = mcolors.to_rgb())

# initialization function 
def init(): 
	# creating an empty plot/frame 
	line.set_data([], []) 
	return line, 

# lists to store x and y axis points 
xdata = np.linspace(fmin, fmax, spectrum.shape[1], endpoint = True)

# animation function 
def animate(i):
	fig.suptitle('Time: %d s'%(i + 1))
	ydata = spectrum[i]
	line.set_data(xdata, ydata) 
	return line, 
	
# setting a title for the plot 
plt.title('Spectrum analysis') 
# hiding the axis details 
# plt.axis('off') 

# call the animator	 
anim = animation.FuncAnimation(fig, animate, init_func=init, interval=1000, blit=False, frames = spectrum.shape[0])
# plt.show()

# save the animation as mp4 video file 
anim.save('%s.mp4'%(argv[1].rstrip('.csv'))) 
