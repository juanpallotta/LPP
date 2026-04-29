
# General considerations for the plotting variables

- Make a Python script that can read a NetCDF file and plot the data from it. Name it Auger_ColorMap.py.
- The plots must be saved as PNG files.
- The Python script must receive the path to the NetCDF file as the first argument to the Python script.
- The folder where the PNG files must be saved must be passed as the second argument to the Python script. Create this folder if it does not exist, and the name should be "LPP_OUT".
- A netcdf file example is located in the same folder as this script (@file:20251118_L0_L1_L2.nc).
- Some variables are located in groups, which must be checked if they exist. Check first if the group exists, and if it does, then check if the variable exists.
- If the variable "range" must be used in an axis, make it as maximun range as 20000 meters.
- The time variable (it could be "Start_Time_AVG_L1" or "Start_Time_AVG_L2") must be converted to human-readable time. Also, left it in UTC time, as it is the time of the data read from the netcdf file. The time variable is a vector and contain the time in Unix-time. In the plots of the time variable is used in an axis, clarify the time is in UTC time.
- If some variable in a 2D plot have values that are too large, try to adjust the scale of the plot to show the data better.
- The NetCDF file received as argument has this structure: lidar-<site>-<YYYYMMDD>-<HHMMSS>-<RX>.root
where:
<site>: could be "ch", "la", "ll" or "lm".
<YYYYMMDD>-<HHMMSS>: "YYYY" a number representing the year with four digits. "MM" are the two digits for the months and "DD" are the two digits for the day. The <RX> is the run number. 
After ".root" is not relevant.
All the images saved must contain the site, the date and time, and the run number in the name.

For the code, use the @file:Auger_ColorMap.py as an example of how the code should look like, specially related to the colormaps plots.


# PLOTS
- The variables to plot are:
## Data level 1 plots
    -   Inside group "L1_Data" (check if this group exists):
        -   Variable: "Cloud_Mask". It is a Matrix, and should be plotted as a 2D image as a colormap plot. Use the image lidar-ch-20240318-064831-R133397.root_L0_L1.nc_RCLS_CM.png as an example of how the plot should look like.
        Since this is a scan-plot, each profile is a column of the matrix "Cloud_Mask" and the values must be plotted based on the zenith angle of the scan, as a fan-shaped domain. The zenith angle is given by the variable "Zenith_AVG_L1" in the group "L1_Data". This mean that the 
        The values of the matrix "/L1_Data/Cloud_Mask" are binary (0 and 1), where 1 is cloud and 0 is clear sky. Use a colormap that highlights the clouds.
        Save the plot as a PNG file and name it as "Cloud_Mask.png".

## Data level 2 plots
    -   Inside group "L2_Data" (check if this group exists):

        -   Variable: "Range_Corrected_Lidar_Signal_L2". It is a 3D Matrix (time, channels, range), and should be plotted as a colormap. For the plotting, follows the instruction of the variable "Cloud_Mask" in the group "L1_Data". In the title, put the time in human-readable format and the channels selected. 
        Save the plot as a PNG file and name it as "Range_Corrected_Lidar_Signal_L2_ColorMap.png".

        -   Variable: "Range_Corrected_Lidar_Signal_L2". It is a 3D Matrix (time, channels, range), and should be plotted as a 2D plot. Use the time slice corresponding to the second argument of the script. The channel index must be taken from the global attributes from the group "L2_Data", variable name "indxChannel_for_Fernald_inv". The y-axis must be taken from the variable "/L2_Data/range", which is a vector and contain the range in meters. Draw the grid lines of the plot. Save the plot as a PNG file and name it as "Range_Corrected_Lidar_Signal_L2_Time_Slice.png".

        - Variables "L2_Data/Aerosol_Backscattering_<X>nm" and "L2_Data/Aerosol_Extinction_<X>nm" (time, LRs, range). Use the time slice corresponding to the second argument of the script. The LRs slice must be taken as the middle index of the dimension LRs of the matrix. The y-axis must be taken from the variable "/L2_Data/range", which is a vector and contain the range in meters. Write the units of each variable. Draw the grid lines of the plot. In the title, put the time in human-readable format and the LRs selected. Save the plot as a PNG file and name it as "Aerosol_Backscattering_532nm.png" and "Aerosol_Extinction_532nm.png". 

        -   Variable: "L2_Data/Aerosol_Backscattering_<X>nm". It is a 3D Matrix (time, channels, range), and should be plotted as a colormap. For the plotting, follows the instruction of the variable "Cloud_Mask" in the group "L1_Data". In the title, put the time in human-readable format and the channels selected. 
        Save the plot as a PNG file and name it as "Aerosol_Backscattering_<X>nm_ColorMap.png".


