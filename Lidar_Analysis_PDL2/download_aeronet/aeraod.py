"""
File: aeraod.py
Author: Nicolas N. de Oliveira
Date of creation: 2024/07/17
Description: This script downloads AERONET AOD data (version 3) for a specified site, date range, data type, data format, and AOD level.

In case you prefer to download the AERONET data manually, please, use the download tool of the "AEROSOL OPTICAL DEPTH (V3)-SOLAR" version (direct sun algorithm).
The direct link is: https://aeronet.gsfc.nasa.gov/new_web/webtool_aod_v3.html
This will download the rith file format capable to be used in LPP (Lidar Processing Pipeline)

Parameters (str):
    - site: The name of the AERONET site/station (e.g., 'CUIABA-MIRANDA');
    - start_date: The start date for the data in 'YYYY-MM-DD' format;
    - end_date: The end date for the data in 'YYYY-MM-DD' format;
    - data_format: The format of the data ('All points', 'daily averages', 'monthly averages');
    - aod_level: The level of AOD data to download ('1.0', '1.5', '2.0');
    - download_folder: The folder where the downloaded data will be saved.
"""

import os
import wget
# import pandas as pd
import sys

def download_aeronet_aod(site, start_date, end_date, data_type, data_format, aod_level, download_folder):
   
    # Ensure the download folder exists
    if not os.path.exists(download_folder):
        os.makedirs(download_folder)

    # Prepare the URL for the download
    base_url = "https://aeronet.gsfc.nasa.gov/cgi-bin/print_web_data_v3?site={}&year={}&month={}&day={}&year2={}&month2={}&day2={}&AOD{}=1&AVG={}&if_no_html=1"

    # Parse the start and end dates
    start_year, start_month, start_day = start_date.split('-')
    end_year, end_month, end_day = end_date.split('-')

    # Select the data format
    if data_format.lower() == "all points":
        avg = 10
    elif data_format.lower() == "daily averages":
        avg = 20
    elif data_format.lower() == "monthly averages":
        avg = 30
    else:
        raise ValueError("Invalid data format specified. Choose 'All points', 'daily averages' or 'monthly averages'.")

    # Construct the full URL
    download_url = base_url.format(site, start_year, start_month, start_day, end_year, end_month, end_day, aod_level, avg)
    
    # Construct the output filename
    filename = f"{site}_{start_date}_to_{end_date}_AOD{aod_level}_{data_format.replace(' ', '_')}.txt"
    output_path = os.path.join(download_folder, filename)

    if os.path.isfile(output_path) ==False:
        try: # THE FILE DOES NOT EXIST --> DOWNLOAD IT!
            print("\nDownloading data...")
            wget.download(download_url, out=output_path)
            print(f"\nDownloaded data to: {output_path}")
        except Exception as e:
            print(f"Error downloading data: {e}")
    else:
        print(f"AERONET file already exist: {output_path}")

# MAIN FUNCTION
if __name__ == "__main__":

    if len(sys.argv) == 8: # IF ARGUMENTS WERE PASSED AS ARGUMENTS...
        # print("Usage: python aeraod.py site_name date_initial date_final data_type data_format aod_level download_folder")
        #                         (0)        (1)       (2)           (3)       (4)       (5)        (6)         (7)
        # Configuration
        site_name    = sys.argv[1] # ie. "CEILAP-BA"   # Specify the site/station name
        date_initial = sys.argv[2] # ie. "2022-09-08"  # Specify the start date in 'YYYY-MM-DD' format
        date_final   = sys.argv[3] # ie. "2022-09-08"  # Specify the end date in 'YYYY-MM-DD' format
        data_type    = sys.argv[4] # ie. "Aerosol Optical Depth (AOD) with Precipitable Water and Angstrom Parameter" or "Total Optical Depth based on AOD Level*"
        data_format  = sys.argv[5] # ie. "All points"  # Choose the data format: 'All points', 'daily averages', or 'monthly averages'
        aod_level    = sys.argv[6] # ie. "10"          #  Choose the AOD level: '10' for 1.0, '15' for 1.5, or '20' for 2.0
        download_folder = os.path.join(os.getcwd(), sys.argv[7] )  # Specify the folder to save downloaded data
    
    if len(sys.argv) == 1: # IF ITS RUN WITHOUT ARGUMENTS...
        print("Using the hardcoded values")

        # Configuration
        site_name    = "Granada"   # Specify the site/station name
        date_initial = "2024-09-17"  # Specify the start date in 'YYYY-MM-DD' format
        date_final   = "2024-09-17"  # Specify the end date in 'YYYY-MM-DD' format
        data_type    = "Aerosol Optical Depth (AOD) with Precipitable Water and Angstrom Parameter" # or "Total Optical Depth based on AOD Level*"
        data_format  = "all points"  # Choose the data format: 'all points', 'daily averages', or 'monthly averages'
        aod_level    = "10"          #  Choose the AOD level: '10' for 1.0, '15' for 1.5, or '20' for 2.0
        # download_folder = os.path.join(os.getcwd(), "/mnt/Disk-1_8TB/Colombia/AERONET_LPP/" )  # Specify the folder to save downloaded data
        download_folder = os.path.join(os.getcwd(), "/mnt/c/Users/jpall/Desktop/Granada_Alhambra_Lidar/AERONET_LPP/" )  # Specify the folder to save downloaded data

    # Call the download function
    ret_down = download_aeronet_aod(site_name, date_initial, date_final, data_type, data_format, aod_level, download_folder)
