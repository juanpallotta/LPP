"""
File: downloadSounding.py
Author: Joaquín Medina Puntero
Date of creation: 2025/04/30
Description: This script downloads reanalysis data (ERA5) for a specified [latitude, longitude], date and vertical resolution.

Before running this script, you must install the required Python packages:
    pip install numpy pandas sounderpy

--- TIP ---
To access ERA5 data you -must- set API access to the ECMWF Climate Data Store (CDS). This includes….

 ·Creating a CDS API account

 ·Setting up a CDS API personal access token

 ·Creating a $HOME/.cdsapirc file

Follow the instructions on the CDSAPI “how to” documentation 
 See: https://cds.climate.copernicus.eu/how-to-api

--- TIP ---
Is data access taking forever? 
Sometimes the NCEP (RAP-RUC, NCEP-FNL) & ECMWF CDS (ERA5) servers are down and not able to be accessed. 
Sometimes these issues are resolved within hours, other times possibly a few days.


Parameters (str):
    - Latitude (float): Latitude of the point of interest.
    - Longitud (float): Longitude of the point of interest.
    - Resolution (float): Vertical resolution in meters for the reanalysis data. If negative, no resampling is done.
    - Date (str): Date of interest in 'YYYY-MM-DD' format.
    - Hour (str): Hour of interest in 'HH' format.
    - Download_folder (str): Folder where the downloaded data will be saved.    
"""

import numpy as np
import pandas as pd
import os
import sys

import sounderpy as spy

def interpolate_segments(df, dz):
    """
    Interpolates the segments of height, temperature, and pressure data in a DataFrame.
    Performs pairwise linear interpolation as a function of height (z) in specified vertical resolution (dz).

    Parameters:
        - df (pd.DataFrame): DataFrame with columns 'z', 'T', and 'p'.
        - dz (float): Vertical resolution in meters for the interpolation.
    """
    z_new, T_new, p_new = [], [], []

    for i in range(len(df) - 1):
        z1, z2 = df['z'].iloc[i], df['z'].iloc[i+1]
        if z2 > z1:
            z_seg = np.arange(z1, z2, dz)
            if z_seg[-1] != z2:
                z_seg = np.append(z_seg, z2)
            T_seg = np.interp(z_seg, [z1, z2], [df['T'].iloc[i], df['T'].iloc[i+1]])
            p_seg = np.interp(z_seg, [z1, z2], [df['p'].iloc[i], df['p'].iloc[i+1]])
            z_new.extend(z_seg)
            T_new.extend(T_seg)
            p_new.extend(p_seg)

    return pd.DataFrame({
        'z': np.array(z_new),
        'T': np.array(T_new),
        'p': np.array(p_new)
    })


def download_era5_sounding(latitude, longitude, resolution, date, hour, download_folder):
    """
    Downloads ERA5 reanalysis data for the specified latitude and longitude, as well as the desired date and time.

    Parameters:
        - Latitude (float): Latitude of the point of interest.
        - Longitude (float): Longitude of the point of interest.
        - Resolution (float): Vertical resolution in meters for the reanalysis data. If negative, no resampling is done.
        - Date (str): Date of interest in 'YYYY-MM-DD' format.
        - Hour (str): Hour of interest in 'HH' format.
        - Download_folder (str): Folder where the downloaded data will be saved.
    Returns:
        - None: The data is saved in a CSV file in the specified folder.
    """

    # Ensure the download folder exists
    if not os.path.exists(download_folder):
        os.makedirs(download_folder)

    # Ensure the latitude and longitude are valid numbers
    if not isinstance(latitude, (int, float)) or not isinstance(longitude, (int, float)):
        raise ValueError("Latitude and longitude must be float type")
    
    if latitude < -90 or latitude > 90:
        raise ValueError("Latitude must be between -90 and 90 degrees")
    if longitude < -180 or longitude > 180:
        raise ValueError("Longitude must be between -180 and 180 degrees")
    
    # Ensure the resolution is a valid number
    if not isinstance(resolution, (int, float)):
        raise ValueError("Vetical resolution must be float type")

    # Ensure the date and hour are in the correct 
    if not isinstance(date, str) or len(date) != 10 or date[4] != '-' or date[7] != '-':
        raise ValueError("Date must be in 'YYYY-MM-DD' format")
    # if not isinstance(date, str) or not isinstance(hour, str): 
    #     raise ValueError("Fecha y hora deben ser cadenas de texto.")

    # Parse the start and end dates
    year, month, day = date.split('-')

    # Ensure the hour is in the correct format (HH)
    if len(hour) != 2 or not hour.isdigit():
        raise ValueError("Hour must be in 'HH' format")
    

    ## SounderPy downloads the data from the ERA5 reanalysis

    # Returns a python dictionary with the data
    reanalysis = spy.get_model_data('era5', [latitude, longitude], year, month, day, hour, hush=True)
     
    # Extract the height, temperature and pressure data
    df_csv = pd.DataFrame({
        'z': reanalysis['z'], 
        'T': reanalysis['T'], 
        'p': reanalysis['p']
    })

    # Reshample the data
    if resolution > 0:
        df_csv = interpolate_segments(df_csv, dz=resolution)

        # Construct the output filename
        filename = f"ERA5_{year}_{month}_{day}_{hour}_dz_{resolution}.csv"
        output_path = os.path.join(download_folder, filename)
    else:
        # Construct the output filename
        filename = f"ERA5_{year}_{month}_{day}_{hour}.csv"
        output_path = os.path.join(download_folder, filename)

    # Create the csv file
    df_csv.to_csv(output_path, index=True, header=True)
    print(f"Reanalysis data save at: {output_path}")



# MAIN FUNCTION
if __name__ == "__main__":  

    if len(sys.argv) == 7: # IF ARGUMENTS WERE PASSED AS ARGUMENTS...
        # print("Usage: python dowloadSounding.py    latitude   longitude   resolution   date     hour    download_folder")
        #                         (0)                  (1)        (2)          (3)       (4)        (5)          (6)
        # Configuration
        latitude = float(sys.argv[1]) # ie. -24.0   # Specify the latitude of the site/station
        longitude = float(sys.argv[2]) # ie. -47.0  # Specify the longitude of the site/station
        resolution = float(sys.argv[3]) # ie. 3.75  # Specify the resolution in meters (3.75m is the default LIDAR resolution)
        # Note: If negative, no resampling is done (250m resolution)
        date = sys.argv[4] # ie. "2022-09-08"  # Specify the date in 'YYYY-MM-DD' format
        hour = sys.argv[5] # ie. "12" # Specify the hour in 'HH' format
        download_folder = os.path.join(os.getcwd(), sys.argv[6] )  # ie. "SPU_soundings" # Specify the folder to save downloaded data
        # Note: Create it if it does not exist in current working directory (cwd)
    
    if len(sys.argv) == 1: # IF ITS RUN WITHOUT ARGUMENTS...
        print("Using the hardcoded values")

        # Configuration 
        latitude = -24.0     # Specify the latitude of the site/station
        longitude = -46.0    # Specify the longitude of the site/station
        resolution = 3.75    # Specify the resolution in meters (3.75m is the default LIDAR resolution)
        # Note: If negative, no resampling is done (250m resolution)
        date   = "2024-05-07"  # Specify the date in 'YYYY-MM-DD' format
        hour = "06"          # Specify the hour in 'HH' format
        download_folder = os.path.join(os.getcwd(), "sondeos")  # Specify the folder to save downloaded data. 
        # Note: Create it if it does not exist in current working directory (cwd)

    # Call the download function
    ret_down = download_era5_sounding(latitude, longitude, resolution, date, hour, download_folder)