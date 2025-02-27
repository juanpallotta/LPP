
# python Auger_ColorMap.py /mnt/Disk-1_8TB/Auger/LidarData/CO/2022/11/25/LPP_OUT/lidar-ch-20221125-012142-R125839.root_L0_L1_L2.nc

# python Auger_ColorMap.py /mnt/Disk-1_8TB/Auger/LidarData/LL/2008/02/14/LPP_OUT/lidar-ll-20080214-020105-R24645.root_L0_L1.nc

from netCDF4 import Dataset
import numpy as np
import matplotlib.pyplot as plt
import sys

minFactor = 1.00
maxFactor = 1.00

nc_file = sys.argv[1]
print("\nName of NetCDF file:", nc_file )
print("\n")

fh = Dataset( nc_file, mode='r' )

nbins      = fh.dimensions['range'].size
resolution = fh.getncattr('Range_Resolution')
indx_Ch_L1 = int(fh.groups['L1_Data'].getncattr('indxChannel_for_Cloud_Mask'))

CM         = np.array(fh["/L1_Data/Cloud_Mask"][:])
zenith_L1  = np.array(fh["/L1_Data/Zenith_AVG_L1"][:])
pr_L1      = np.array(fh["/L1_Data/Raw_Lidar_Data_L1"][:])
time_L1    = np.array(fh["/L1_Data/Start_Time_AVG_L1"][:])
time_L1    = np.array(time_L1, dtype='datetime64[s]')

ntimes_L1 = fh['L1_Data'].dimensions['time'].size
nZeniths_L1 = len(zenith_L1)

binMax = round(20000/resolution) # ~17km
CM = np.delete( CM, np.s_[binMax:nbins], 1 )
nbins = binMax
r = np.arange(nbins)*resolution/1000

RCLS_L1 = np.zeros( (nZeniths_L1, nbins) ) # nbins
BG = np.zeros( nZeniths_L1 )
r2 = r*r

for z in range(nZeniths_L1):
  BG[z] = np.mean(pr_L1[z][0][3500:4000])
  for b in range(nbins):
    RCLS_L1[z][b] = ( pr_L1[z][indx_Ch_L1][b] - BG[z] )* r2[b]

minRCLS = min(map(min, RCLS_L1)) *minFactor
maxRCLS = max(map(max, RCLS_L1)) *maxFactor

indxZenMin = np.argmin( zenith_L1 )
zenith_L1[0:(indxZenMin+1)] = -1*zenith_L1[0:(indxZenMin+1)]

xx = np.zeros( (len(zenith_L1), nbins ) )
yy = np.zeros( (len(zenith_L1), nbins ) )
for z in range(len(zenith_L1)):
	for i in range(nbins): # len(r)
		xx[z][i] = r[i] * np.sin(zenith_L1[z]*np.pi/180)
		yy[z][i] = r[i] * np.cos(zenith_L1[z]*np.pi/180)

##### PLOT & PRINT RCLS COLORMAP #####################################################################################

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(24, 10))
ax1.pcolor(xx, yy, RCLS_L1, vmin=minRCLS, vmax=maxRCLS, cmap='jet')
ax2.pcolor(xx, yy, CM)

ax1.set_title('RCLS', fontsize=20)
ax1.set(xlabel='Range [km]', ylabel='Range [km]')
ax2.set_title('Cloud Mask', fontsize=20)
ax2.set(xlabel='Range [km]', ylabel='Range [km]')

# Change the font size for x-axis and y-axis labels
ax1.xaxis.label.set_fontsize(18)
ax1.yaxis.label.set_fontsize(18)
ax2.xaxis.label.set_fontsize(18)
ax2.yaxis.label.set_fontsize(18)
# Change the font size for x-axis and y-axis tick labels
ax1.tick_params(axis='x', labelsize=16)
ax1.tick_params(axis='y', labelsize=16)
ax2.tick_params(axis='x', labelsize=16)
ax2.tick_params(axis='y', labelsize=16)

# fig.suptitle( 'Lidar Color Maps - PMT' + str(indx_Ch_L1) +  " - "  + str(time_L1[0]) )

fname_Img_RCLS_CM = nc_file + '_RCLS_CM.png'
plt.savefig(fname_Img_RCLS_CM, dpi=300, bbox_inches='tight')

fh.close()