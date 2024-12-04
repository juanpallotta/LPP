
# python LPP_Plots_L2.py /mnt/Disk-1_8TB/Argentina/Cordoba/20220908/LPP_OUT  

# python LPP_Plots_L2.py /mnt/Disk-1_8TB/Brazil/Manaus/2011/8/14/LPP_OUT/14_L0_L1_L2.nc 54
# python LPP_Plots_L2.py /mnt/Disk-1_8TB/Brazil/SPU/20200914/lidar_signals/LPP_OUT/lidar_signals_L0_L1_L2.nc 5
# python LPP_Plots_L2.py /mnt/Disk-1_8TB/Brazil/SPU/20200914/test/LPP_OUT/test_L0_L1_L2.nc 5
# python LPP_Plots_L2.py /home/juan/LidarAnalysisCode/LPP/signalsTest/Pappalardo_2004/LPP_OUT/Pappalardo_2004_L0_L1_L2.nc 0

# python LPP_Plots_L2.py /mnt/Disk-1_8TB/Granada/20230214/RS_20230214_0925/test/LPP_OUT/test_L0_L1_L2.nc 0

# python LPP_Plots_L2.py /mnt/Disk-1_8TB/Auger/LidarData/CO/2023/05/18/LPP_OUT/lidar-ch-20230518-014231-R128282.root_L0_L1_L2.nc 20

from netCDF4 import Dataset
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import sys

indx_Max = 1500
minFactor = 0.00
maxFactor = 1.00

nc_file = sys.argv[1]
print("\nName of NetCDF file:", nc_file )
print("\n")

indx_Time_to_Plot = int(1)
if len(sys.argv) ==3:
  indx_Time_to_Plot = int(sys.argv[2])
# print("\nindx_Time_to_Plot: ", indx_Time_to_Plot)

fh = Dataset( nc_file, mode='r' )

nbins      = fh.dimensions['range'].size
resolution = fh.getncattr('Range_Resolution')
indx_Ch_L2 = int(fh.groups['L2_Data'].getncattr('indxChannel_for_Fernald_inv'))
r          = (np.arange(nbins)+1) *resolution*1e-3 - (resolution*1e-3)/2        # r = (np.arange(nbins)+1) *resolution*1e-3 

# alpha_Aer = fh["/L2_Data/Aerosol_Extinction_355nm"][:]
# beta_Aer  = fh["/L2_Data/Aerosol_Backscattering_355nm"][:]
alpha_Aer = fh["/L2_Data/Aerosol_Extinction_532nm"][:]
beta_Aer  = fh["/L2_Data/Aerosol_Backscattering_532nm"][:]
RCLS_L2   = fh["/L2_Data/Range_Corrected_Lidar_Signal_L2"][:]

LRs       = fh["/L2_Data/LRs"]
AOD_LR    = np.array(fh["/L2_Data/AOD_LR"]).transpose()
time_L2   = np.array(fh["/L2_Data/Start_Time_AVG_L2"][:])
time_L2   = np.array(time_L2, dtype='datetime64[s]')

# AOD_AERO    = np.array(fh["/L2_Data/AERONET_AOD"]).transpose()
# AERONET_TIME= np.array(fh["/L2_Data/AERONET_time"][:])
# AERONET_TIME= np.array(AERONET_TIME, dtype='datetime64[s]')

# time_L1    = np.array(fh["/L1_Data/Start_Time_AVG_L1"][:])
# AOD_Aeronet = [0.40482,0.40504, 0.40922, 0.40922, 0.41612, 0.40242, 0.40242, 0.40454, 0.46024, 0.46024, 0.51445, 0.54434, 0.54434, 0.56323, 0.54509, 0.54509, 0.54375, 0.56074, 0.56074, 0.58241, 0.60262, 0.60262, 0.60262]

plt.figure(num=4,figsize=(12,3),clear=True)
for l in np.arange(len(LRs), step=2):
  plt.scatter( time_L2, AOD_LR[l,:], linestyle=':', label=str(LRs[l]) + ' sr')
  # plt.scatter( AERONET_TIME, AOD_AERO[:], linestyle=':', label='AOD AERONET' )
# plt.scatter( time_L2[0:23], AOD_Aeronet, color='black', marker="o", label=' Aeronet') # , linestyle='dotted'

# PLOT AOD vs TIME vs LR
plt.xlabel('UTC Time [h]')
# plt.ylabel('AOD @ 355 nm')
plt.ylabel('AOD @ 532 nm')
# plt.ylim(-0.01, 2)
plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
plt.legend( loc='upper right', bbox_to_anchor=(1.12, 1.0) )
plt.grid()

fname_Img = nc_file + '_AODvsLRvsTime.png'
plt.savefig(fname_Img, dpi=300, bbox_inches='tight')

# nPoint_avg = fh["/L2_Data/Fernald_smooth_bins"][:]
# print("\nFernald_smooth_bins: ", nPoint_avg[indx_Ch_L2])

##### PLOT AEROSOL ALPHA AND BETA FOR A INDEX TIME PASSED AS ARGUMENT #####################################################################################

plt.figure(num=1,figsize=(8,6),clear=True)
for l in np.arange(len(LRs), step=1):
    plt.plot( alpha_Aer[indx_Time_to_Plot][l][0:indx_Max]*1e6, r[0:indx_Max], linestyle='-', label=str(LRs[l]) + ' sr')

# COD = 7.5*np.sum( alpha_Aer[indx_Time_to_Plot][0][1000:2000] )
# print("\nCOD:", COD )
# plt.xlim(0, 10) ;
plt.xlabel('Aerosols Extinction [Mm$^{-1}$]')
plt.ylabel('Range [km]')
plt.legend(loc='upper right')
plt.title(time_L2[indx_Time_to_Plot])
plt.grid()

fname_Img = nc_file + '_alphaAer_LR_indxT' + str(indx_Time_to_Plot) + '.png'
plt.savefig(fname_Img, dpi=300, bbox_inches='tight')

# ___ BETA _________

plt.figure(num=1,figsize=(8,6),clear=True)
for l in np.arange(len(LRs), step=1):
    plt.plot( beta_Aer[indx_Time_to_Plot][l][0:indx_Max]*1e6, r[0:indx_Max], linestyle='-', label=str(LRs[l]) + ' sr')

plt.xlabel('Aerosols Backscattering [Mm$^{-1}$ sr$^{-1}$]')
plt.ylabel('Range [km]')
plt.legend(loc='upper right')
plt.title(time_L2[indx_Time_to_Plot])
plt.grid()

fname_Img = nc_file + '_betaaAer_LR_indxT' + str(indx_Time_to_Plot) + '.png'
plt.savefig(fname_Img, dpi=300, bbox_inches='tight')

##### PLOT RCLS FOR THE SAME TIME OF ALPHA #####################################################################################

plt.figure(num=1,figsize=(8,6),clear=True)
# plt.plot( r[0:indx_Max], RCLS_L2[indx_Time_to_Plot][indx_Ch_L2][0:indx_Max]*1e6, linestyle='-')
plt.plot( RCLS_L2[indx_Time_to_Plot][indx_Ch_L2][0:indx_Max]*1e6, r[0:indx_Max], linestyle='-')
# plt.xlabel('Range [km]')
plt.xlabel('RCLS L2 [au]')
# plt.ylabel('RCLS L2 [au]')
plt.ylabel('Range [km]')
# plt.ylim(-1, 5)
plt.title(time_L2[indx_Time_to_Plot])
# plt.legend(loc='upper right')
plt.grid()

fname_Img = nc_file + '_RCLS_L2_indxT' + str(indx_Time_to_Plot) + '.png'
plt.savefig(fname_Img, dpi=300, bbox_inches='tight')

##### PLOT ALPHA COLORMAP #####################################################################################

ntimes_L2 = fh['L2_Data'].dimensions['time'].size
alpha_a = np.zeros( (ntimes_L2, indx_Max) )
for t in range(ntimes_L2):
  for b in range(indx_Max):
    alpha_a[t][b] = alpha_Aer[t][0][b]

minAlpha = min(map(min, alpha_a)) *minFactor
maxAlpha = max(map(max, alpha_a)) *maxFactor

yy, xx = np.meshgrid(r[0:indx_Max], time_L2[0:indx_Max])
plt.figure(num=1,figsize=(8,6),clear=True)
plt.pcolor(xx, yy, alpha_a, cmap='jet')
# plt.pcolor(xx, yy, alpha_a, vmin=minAlpha, vmax=maxAlpha, cmap='jet')

# plt.set_title('Alpha Aer')
# plt.set(xlabel='Range [m]', ylabel='Alpha Aer [1/m]')

fname_Img_Alpha_ColorMaps = nc_file + '_Alpha_CM.png'
plt.savefig(fname_Img_Alpha_ColorMaps, dpi=300, bbox_inches='tight')

##########################################################################################

fh.close()