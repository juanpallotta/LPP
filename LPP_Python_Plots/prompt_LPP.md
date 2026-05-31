
# General considerations for the plotting variables

- Make a Python script that can read a NetCDF file and plot the data from it. Name it plot_LPP.py.
- The plots must be saved as PNG files.
- The Python script must receive the path to the NetCDF file as the first argument to the Python script.
- The folder where the PNG files must be saved must be passed as the second argument to the Python script. Create this folder if it does not exist, and the name should be "LPP_OUT".
- A netcdf file example is located in the same folder as this script (@file:20251118_L0_L1_L2.nc).
- Some variables are located in groups, which must be checked if they exist. Check first if the group exists, and if it does, then check if the variable exists.
- If the variable "range" must be used in an axis, make it as maximun range as 20000 meters.
- The time variable (it could be "Start_Time_AVG_L1" or "Start_Time_AVG_L2") must be converted to human-readable time. Also, left it in UTC time, as it is the time of the data read from the netcdf file. The time variable is a vector and contain the time in Unix-time. In the plots of the time variable is used in an axis, clarify the time is in UTC time.
- If some variable in a 2D plot have values that are too large, try to adjust the scale of the plot to show the data better.

# PLOTS
- For all the variables:
    - If the wavelength to plot is 355 nm (or closer number), plot it in blue.
    - If the wavelength to plot is 532 nm (or closer number), plot it in green.
    - If the wavelength to plot is 1064 nm (or closer number), plot it in red.
- The variables to plot are:
## Data level 1 plots
    -   Inside group "L1_Data" (check if this group exists):
        -   Variable: "Cloud_Mask". It is a Matrix, and should be plotted as a 2D image. The x-axis must be taken from the variable "Start_Time_AVG_L1", which is a vector and contain the time in Unix-time. Convert it to human-readable time. The y-axis must be taken from the variable "/L1_Data/range", which is a vector and contain the range in meters. The values of the matrix "/L1_Data/Cloud_Mask" are binary (0 and 1), where 1 is cloud and 0 is clear sky. Use a colormap that highlights the clouds. Save the plot as a PNG file and name it as "Layer_Mask.png".
        - Variable "Raw_Lidar_Data_L1": It is a 3D Matrix (time, channels, range), and should be plotted as a 2D plot for a specific "channels" slice of the matrix. The channels index must be taken from the global attributes from the group "L1_Data", variable name "indxChannel_for_Cloud_Mask". Before to plot, the slice selected of the matrix "Raw_Lidar_Data_L1" must be multiplied point by point by the array "/L1_Data/range" squared.
        The y-axis must be taken from the variable "/L1_Data/range", which is a vector and contain the range in meters. Draw the grid lines of the plot. In the title, put the time in human-readable format and the channels selected. Save the plot as a PNG file and name it as "Range_Corrected_Lidar_Signal_L1_ColorMap.png". 

        It is a Matrix, and should be plotted as a 2D image. The x-axis must be taken from the variable "Start_Time_AVG_L1", which is a vector and contain the time in Unix-time. Convert it to human-readable time. The y-axis must be taken from the variable "/L1_Data/range", which is a vector and contain the range in meters. Before plotting, each column of this matrix must be multiplied by the variable "/L1_Data/range" squared. The title of this plot must the time in human-readable format and the channels selected. Save the plot as a PNG file and name it as "Range_Corrected_Lidar_Signal_L2_ColorMap.png".
 
## Data level 2 plots
    -   Inside group "L2_Data" (check if this group exists):

        -   Variable: "Range_Corrected_Lidar_Signal_L2". It is a 3D Matrix (time, channels, range), and should be plotted as a 2D plot for a specific "time" and "channels" slice of the matrix. The time index must be passed as the second argument of the script. The channels index must be taken from the global attributes from the group "L2_Data", variable name "indxChannel_for_Fernald_inv". The y-axis must be taken from the variable "/L2_Data/range", which is a vector and contain the range in meters. Draw the grid lines of the plot. In the title, put the time in human-readable format and the wavelenth selected, obtained from the variable "/Wavelengths" (located in the root of the NetCDF file), index "/L2_Data/indxChannel_for_Fernald_inv". Save the plot as a PNG file and name it as "Range_Corrected_Lidar_Signal_L2.png".

        -   Variable: "Range_Corrected_Lidar_Signal_L2". It is a 3D Matrix (time, channels, range), and should be plotted as a pcolor. The x-axis must be taken from the variable "Start_Time_AVG_L2", which is a vector and contain the time in Unix-time. Convert it to human-readable time. The y-axis must be taken from the variable "/L2_Data/range", which is a vector and contain the range in meters. The channels index must be taken from the global attributes from the group "L2_Data", variable name "indxChannel_for_Fernald_inv". In the title, put the time in human-readable format and the wavelenth selected, obtained from the variable "/Wavelengths" (located in the root of the NetCDF file), index "/L2_Data/indxChannel_for_Fernald_inv". Save the plot as a PNG file and name it as "Range_Corrected_Lidar_Signal_L2_ColorMap.png".

        -   Variable: "/L2_Data/AERONET_Data/AERONET_AOD_<X>nm_at_LidarTime", where <X> is a number. Check if the sub-group "AERONET_Data" exists. If it does not exist, skip this variable. If it exists, the variable "AERONET_AOD_<X>nm_at_LidarTime", which is a vector, must be plotted as a 2D. If the value of the variable is negative, do not plot the point. In the x-axis must be taken as the mean value of the variables "L2_Data/Start_Time_AVG_L2" and "L2_Data/Stop_Time_AVG_L2", which both are a vector and contain the time in Unix-time and in UTC time (do not convert it to local time). 
        This variable must be plotted togther with the variable "L2_Data/LIDAR_AERONET_synergy/AOD_Lidar_<X>nm_vs_AERONET" (if it exists). It has to have the same dimension as the variable "L2_Data/AERONET_Data/AERONET_AOD_<X>nm_at_LidarTime". In the title, put the time in human-readable format. Make a tick mark different than the previous figure, to distinguis the data from AERONET and the data from LIDAR. Make it with different types of points and different colors. 
        Make a sub-plot, with the variable "L2_Data/LIDAR_AERONET_synergy/LR_inv_<X>nm" (if it exists). Use as x-axis the same time variable than the previous figure, and considere the same conditions as the previous figure. 
        Make a another sub-plot with the variable "L2_Data/AERONET_Data/AERONET_Angs_<X>nm_at_LidarTime". This is a vector and should be plotted as a 2D image. 
        In the x-axis must be taken as the mean value of the variables "L2_Data/Start_Time_AVG_L2" and "L2_Data/Stop_Time_AVG_L2", which both are a vector and contain the time in Unix-time and in UTC time (do not convert it to local time).
        Save the plot as a PNG file and name it as "AERONET_vs_LIDAR_AOD_<X>nm_LRs_Sinergy_Angstrom_Exponent_<X>-<Y>_at_LidarTime.png". 

        -   Variable: "L2_Data/AOD_LR". This is a matrix (time, LRs), and should be plotted as a 2D image. The x-axis must be taken from the variable "Start_Time_AVG_L2", which is a vector and contain the time in Unix-time. Convert it to human-readable time. The y-axis must be taken from each of the slices corresponding to LRs dimensions. Take the legend for each of the slices corresponding to LRs dimensions from the variable "L2_Data/LRs". Write the wavelenth used based on how is used in others variables. Save the plot as a PNG file and name it as "AOD_LR.png".

        - Variables "L2_Data/Aerosol_Backscatter_<X>nm" and "L2_Data/Aerosol_Extinction_<X>nm" (time, LRs, range). Use the time slice corresponding to the second argument of the script. The LRs slice must be taken as the middle index of the dimension LRs of the matrix. The y-axis must be taken from the variable "/L2_Data/range", which is a vector and contain the range in meters. Write the units of each variable. In the x-axis, put the name of the variable with the wavelenght used, taken from the wavelenth selected, obtained from the variable "/Wavelengths" (located in the root of the NetCDF file), index "/L2_Data/indxChannel_for_Fernald_inv". Draw the grid lines of the plot. In the title, put the time in human-readable format and the LRs selected. Save the plot as a PNG file and name it as "Aerosol_Backscatter_532nm.png" and "Aerosol_Extinction_532nm.png". 

        -   Variable: "L2_Data/Aerosol_Extinction_<X>nm". It is a 3D Matrix (time, LRs, range), and should be plotted as a colormap. For the plotting, follows the instruction of the variable "Cloud_Mask" in the group "L1_Data". Plot also its wavelenght, as it used in the instruction from the previous variables "L2_Data/Aerosol_Backscatter_<X>nm" and "L2_Data/Aerosol_Extinction_<X>nm". In the title, put the time in human-readable format and the LRs selected. 
        Save the plot as a PNG file and name it as "Aerosol_Backscatter_<X>nm_ColorMap.png".


        
        
