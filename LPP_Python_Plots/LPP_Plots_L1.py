
# python LPP_Plots_L1.py /mnt/Disk-1_8TB/Argentina/Cordoba/20240902/LPP_OUT/20240902_L0_L1.nc

# python LPP_Plots_L1.py /mnt/Disk-1_8TB/Brazil/Manaus/2011/8/14/LPP_OUT/14_L0_L1.nc

# python LPP_Plots_L1.py /mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/LPP_OUT/lidar_signals_L0_L1.nc
# python LPP_Plots_L1.py /mnt/Disk-1_8TB/Argentina/Cordoba/Paper_Celeste/20190927/LPP_OUT/20190927_L0_L1.nc
# python LPP_Plots_L1.py /mnt/Disk-1_8TB/Argentina/Aeroparque/2015/20150426/LPP_OUT/20150426_L0_L1.nc
# python LPP_Plots_L1.py /mnt/Disk-1_8TB/Argentina/Bariloche/20150504/LPP_OUT/20150504_L0_L1.nc

from netCDF4 import Dataset
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from scipy.interpolate import interp2d
import sys

minFactor = 0.00
maxFactor = 1.00
maxRange  = 8000

nc_file = sys.argv[1]
print("\nName of NetCDF file:", nc_file )
print("\n")

fh = Dataset( nc_file, mode='r' )

nbins      = fh.dimensions['range'].size
resolution = fh.getncattr('Range_Resolution')
indx_Ch_L1 = int(fh.groups['L1_Data'].getncattr('indxChannel_for_Cloud_Mask'))
# print("\nindx_Ch_L1: ", indx_Ch_L1)

CM         = np.array(fh["/L1_Data/Cloud_Mask"][:])
# zenith_L1  = np.array(fh["/L1_Data/Zenith_AVG_L1"][:])
pr_L1      = np.array(fh["/L1_Data/Raw_Lidar_Data_L1"][:])
time_L1    = np.array(fh["/L1_Data/Start_Time_AVG_L1"][:])
time_L1    = np.array(time_L1, dtype='datetime64[s]')

ntimes_L1 = fh['L1_Data'].dimensions['time'].size
# nZeniths_L1 = len(zenith_L1)

binMax = round(maxRange/resolution)
CM = np.delete( CM, np.s_[binMax:nbins], 1 )
nbins = binMax
r = np.arange(nbins)*resolution/1000 # [km]

RCLS_L1 = np.zeros( (ntimes_L1, nbins) ) # nbins
BG = np.zeros( ntimes_L1 )
r2 = r*r

for z in range(ntimes_L1):
  BG[z] = np.mean(pr_L1[z][indx_Ch_L1][3700:4000])
  for b in range(nbins):
    RCLS_L1[z][b] = ( pr_L1[z][indx_Ch_L1][b] - BG[z] )* r2[b]

minRCLS = min(map(min, RCLS_L1)) *minFactor
maxRCLS = max(map(max, RCLS_L1)) *maxFactor

# indxZenMin = np.argmin( zenith_L1 )
# zenith_L1[0:(indxZenMin+1)] = -1*zenith_L1[0:(indxZenMin+1)]

##### PLOT & PRINT RCLS COLORMAP #####################################################################################
# xx = np.zeros( ntimes_L1, nbins )
# yy = np.zeros( ntimes_L1, nbins )
# for t in range(ntimes_L1):
# 	for i in range(nbins): # len(r)
# 		xx[t][i] = time_L1[t]
# 		yy[t][i] = r[i]
yy, xx = np.meshgrid(r, time_L1)

# fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(24, 10))
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(24, 10))
# ax1.pcolor(xx, yy, RCLS_L1, vmin=minRCLS, vmax=maxRCLS, cmap='jet')
ax1.pcolor(xx, yy, RCLS_L1, cmap='jet')
ax2.pcolor(xx, yy, CM)

ax1.set_title('RCLS')
ax1.set(xlabel='Time [UTC]', ylabel='Range [km]')
ax2.set_title('Cloud Mask')
ax2.set(xlabel='Time [UTC]', ylabel='Range [km]')
fig.suptitle( 'Lidar Color Maps ')
# ax1.set_major_formatter(mdates.DateFormatter('%H'))
# ax2.set_major_formatter(mdates.DateFormatter('%H'))

fname_Img_RCLS_CM = nc_file + '_RCLS_CM.png'
plt.savefig(fname_Img_RCLS_CM, dpi=300, bbox_inches='tight')
# fname_Img_RCLS_CM = nc_file + '_RCLS_CM.svg'
# plt.savefig(fname_Img_RCLS_CM, dpi=300, bbox_inches='tight', format='svg')

# RCLS'S PCOLOR 

# f = interp2d(xx, yy, RCLS_L1, kind='cubic')
# xnew = np.arange(0, 30, .1)
# ynew = np.arange(0, 30, .1)
# data1 = f(xnew,ynew)
# data1 = f(xx, yy)
# Xn, Yn = np.meshgrid(xnew, ynew)

plt.figure(num=1, clear=True)
plt.pcolor(xx, yy, RCLS_L1, vmin=minRCLS, vmax=maxRCLS, cmap='jet')
# plt.pcolormesh(xx, yy, RCLS_L1, vmin=minRCLS, vmax=maxRCLS, cmap='jet', shading='gouraud')
# plt.pcolormesh(xx, yy, data1, vmin=minRCLS, vmax=maxRCLS, cmap='jet', shading='gouraud')
plt.xlabel('UTC Time')
plt.ylabel('Height [km asl]')
# plt.set_title('Range Corrected Lidar Signal')
# plt.set(xlabel='Time', ylabel='RCLS [a.u.]')

fname_Img_RCLS = nc_file + '_RCLS.png'
plt.savefig(fname_Img_RCLS, dpi=300, bbox_inches='tight')
# fname_Img_RCLS = nc_file + '_RCLS.svg'
# plt.savefig(fname_Img_RCLS, dpi=300, bbox_inches='tight', format='svg')

fh.close()
