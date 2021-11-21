import matplotlib.pyplot as plt 
import matplotlib.animation as animation 
import numpy as np 
import sys
from PIL import Image
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
vmin = spectrum.min()
if vmin == -np.inf:
    spectrum[np.where(spectrum==spectrum.min())] = -200
vmax = spectrum.max()
vmin = spectrum.min()
print(vmin, vmax)

# spectrum = (spectrum - vmin) / (vmax - vmin)
# spectrum = np.exp(spectrum)

# vmax = spectrum.max()
# vmin = spectrum.min()
# print(vmin, vmax)
spectrum = 255 * (spectrum - vmin) / (vmax - vmin)

vmax = spectrum.max()
vmin = spectrum.min()
print(vmin, vmax)

maxsize = (1080, 1080)

image = Image.fromarray(spectrum)

tn_image = image.resize(maxsize)

plt.figure()
extent = [fmin, fmax, 0, fmax - fmin]
plt.imshow(np.array(tn_image), extent = extent, origin='lower')
plt.colorbar()
plt.show()