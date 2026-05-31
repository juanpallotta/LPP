#!/usr/bin/env python3
"""
Script to read and plot data from NetCDF files.

This script reads a NetCDF file and plots:
1. Cloud_Mask and Raw_Lidar_Data_L1 variables from the L1_Data group (if they exist) as 2D images (Data level 1 plots).
2. Aerosol_Extinction_<X>nm and Aerosol_Backscatter_<X>nm variables from the L2_Data group (if they exist) as 1D and 2D (ColorMap) plots (Data level 2 plots).
3. Range_Corrected_Lidar_Signal_L2 variable from the L2_Data group (if it exists) as both 1D and 2D (ColorMap) plots.
4. AERONET and Lidar AOD comparison plots.
5. AOD_LR variable from the L2_Data group (if it exists) as a 2D image (multi-line plot).
6. AERONET Angstrom Exponent variables from the AERONET_Data group (if they exist).

Usage:
    python plot_netcdf.py <path_to_netcdf_file> <output_dir> [time_index]
    
Arguments:
    path_to_netcdf_file: Path to the NetCDF file to process
    output_dir: Directory where the PNG files must be saved
    time_index: (Optional) Time index for Aerosol_Extinction plot (default: 0)
"""

import sys
import argparse
from pathlib import Path
import netCDF4 as nc
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime, timezone


def check_group_exists(dataset, group_name):
    """
    Check if a group exists in the NetCDF dataset.
    
    Args:
        dataset: NetCDF4 Dataset object
        group_name: Name of the group to check
        
    Returns:
        bool: True if group exists, False otherwise
    """
    try:
        return group_name in dataset.groups
    except AttributeError:
        return False


def check_variable_exists(group, variable_name):
    """
    Check if a variable exists in a NetCDF group.
    
    Args:
        group: NetCDF4 Group object
        variable_name: Name of the variable to check
        
    Returns:
        bool: True if variable exists, False otherwise
    """
    try:
        return variable_name in group.variables
    except AttributeError:
        return False


def unix_to_datetime(unix_times):
    """
    Convert Unix timestamp(s) to datetime object(s).
    
    Args:
        unix_times: Single Unix timestamp or array of Unix timestamps
        
    Returns:
        datetime object or array of datetime objects
    """
    if isinstance(unix_times, (list, np.ndarray)):
        return np.array([datetime.fromtimestamp(t, tz=timezone.utc) for t in unix_times])
    else:
        return datetime.fromtimestamp(unix_times, tz=timezone.utc)


def get_l2_selected_wavelength(dataset, l2_group):
    """
    Return the selected wavelength and channel index from the L2_Data group.
    """
    channel_index = 0
    try:
        channel_index = int(getattr(l2_group, 'indxChannel_for_Fernald_inv'))
    except Exception:
        pass

    if check_variable_exists(dataset, 'Wavelengths'):
        wavelengths = np.asarray(dataset.variables['Wavelengths'][:])
        if 0 <= channel_index < len(wavelengths):
            wavelength = wavelengths[channel_index]
            if isinstance(wavelength, bytes):
                wavelength = wavelength.decode('utf-8')
            return wavelength, channel_index

    return None, channel_index


def wavelength_color(wavelength):
    """
    Return a plotting color based on the closest standard wavelength.
    """
    try:
        w = float(wavelength)
    except Exception:
        return '#000000'

    choices = {
        355.0: '#0000FF',
        532.0: '#008000',
        1064.0: '#FF0000',
    }
    closest = min(choices.keys(), key=lambda x: abs(x - w))
    return choices[closest]


def parse_wavelength_from_string(text):
    """
    Extract a wavelength value from a string containing '<X>nm'.
    """
    import re
    match = re.search(r'(\d+\.?\d*)\s*nm', str(text))
    if match:
        try:
            return float(match.group(1))
        except ValueError:
            return None
    return None


def plot_cloud_mask(dataset, output_dir=None):
    """
    Plot the Cloud_Mask variable from the L1_Data group (Data level 1 plots).
    
    Args:
        dataset: NetCDF4 Dataset object
        output_dir: Directory to save the plot (default: same as input file)
    """
    # Check if L1_Data group exists
    if not check_group_exists(dataset, 'L1_Data'):
        print("Group 'L1_Data' does not exist in the NetCDF file.")
        return
    
    l1_group = dataset.groups['L1_Data']
    
    # Check if Cloud_Mask variable exists
    if not check_variable_exists(l1_group, 'Cloud_Mask'):
        print("Warning: Variable 'Cloud_Mask' does not exist in group 'L1_Data'.")
        return
    
    # Check if Start_Time_AVG_L1 variable exists
    if not check_variable_exists(l1_group, 'Start_Time_AVG_L1'):
        print("Warning: Variable 'Start_Time_AVG_L1' does not exist in group 'L1_Data'.")
        return
    
    # Check if range variable exists
    if not check_variable_exists(l1_group, 'range'):
        print("Warning: Variable 'range' does not exist in group 'L1_Data'.")
        return
    
    # Read the data
    cloud_mask = l1_group.variables['Cloud_Mask'][:]
    unix_time = l1_group.variables['Start_Time_AVG_L1'][:]
    range_data = l1_group.variables['range'][:]
    
    # Convert Unix time to datetime
    time_datetime = unix_to_datetime(unix_time)
    
    # Create the plot
    fig, ax = plt.subplots(figsize=(12, 6))
    
    # Plot Cloud_Mask as 2D image
    # Use a colormap that highlights clouds (1) vs clear sky (0)
    # 'Blues' colormap: white for clear sky (0), blue for clouds (1)
    im = ax.pcolormesh(time_datetime, range_data, cloud_mask.T, 
                       cmap='Blues', vmin=0, vmax=1, shading='auto')
    
    # Format x-axis to show dates nicely
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d\n%H:%M:%S'))
    ax.xaxis.set_major_locator(mdates.AutoDateLocator())
    plt.setp(ax.xaxis.get_majorticklabels(), rotation=45, ha='right')
    
    # Labels and title
    ax.set_xlabel('Time (UTC)', fontsize=12, fontweight='bold')
    ax.set_ylabel('Range (m)', fontsize=12, fontweight='bold')
    ax.set_title('Layer Mask', fontsize=14, fontweight='bold')

    # Limit range to 20000 meters
    ax.set_ylim(0, 20000)
    
    # Add colorbar
    cbar = plt.colorbar(im, ax=ax, label='Layer Mask')
    cbar.set_ticks([0, 1])
    cbar.set_ticklabels(['Clear Sky', 'Cloud'])
    
    # Grid for better readability
    ax.grid(True, alpha=0.3, linestyle='--')
    
    # Tight layout to prevent label cutoff
    plt.tight_layout()
    
    # Save the plot
    if output_dir is None:
        output_path = Path('Layer_Mask.png')
    else:
        output_path = Path(output_dir) / 'Layer_Mask.png'
    
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"Plot saved successfully: {output_path.absolute()}")
    
    # Close the figure to free memory
    plt.close(fig)


def plot_range_corrected_lidar_signal_l1_colormap(dataset, output_dir=None):
    """
    Plot range-corrected Raw_Lidar_Data_L1 from the L1_Data group as a 2D ColorMap.

    A channel slice is selected via the indxChannel_for_Cloud_Mask group attribute.
    Each (time, range) value is multiplied by range squared before plotting.
    """
    if not check_group_exists(dataset, 'L1_Data'):
        print("Group 'L1_Data' does not exist in the NetCDF file.")
        return

    l1_group = dataset.groups['L1_Data']

    if not check_variable_exists(l1_group, 'Raw_Lidar_Data_L1'):
        print("Warning: Variable 'Raw_Lidar_Data_L1' does not exist in group 'L1_Data'.")
        return

    if not check_variable_exists(l1_group, 'Start_Time_AVG_L1'):
        print("Warning: Variable 'Start_Time_AVG_L1' does not exist in group 'L1_Data'.")
        return

    if not check_variable_exists(l1_group, 'range'):
        print("Warning: Variable 'range' does not exist in group 'L1_Data'.")
        return

    try:
        channel_index = int(getattr(l1_group, 'indxChannel_for_Cloud_Mask'))
    except AttributeError:
        print("Warning: Attribute 'indxChannel_for_Cloud_Mask' not found in L1_Data. Using 0.")
        channel_index = 0

    data = l1_group.variables['Raw_Lidar_Data_L1'][:]
    unix_time = l1_group.variables['Start_Time_AVG_L1'][:]
    range_data = l1_group.variables['range'][:]

    time_dim, chan_dim, range_dim = data.shape

    if channel_index < 0 or channel_index >= chan_dim:
        print(f"Warning: Channel index {channel_index} is out of bounds [0, {chan_dim - 1}]. Using 0.")
        channel_index = 0

    signal_map = np.asarray(data[:, channel_index, :], dtype=float)
    range_sq = np.square(np.asarray(range_data, dtype=float))
    signal_map = signal_map * range_sq[np.newaxis, :]

    time_datetime = unix_to_datetime(unix_time)

    fig, ax = plt.subplots(figsize=(12, 6))

    mask_range = (range_data >= 0) & (range_data <= 20000)
    data_subset = signal_map[:, mask_range]

    if data_subset.size > 0:
        data_to_scale = np.asanyarray(data_subset)
        if hasattr(data_to_scale, 'filled'):
            data_to_scale = data_to_scale.filled(np.nan)

        v_min = np.nanpercentile(data_to_scale, 5)
        v_max = np.nanpercentile(data_to_scale, 95)
    else:
        v_min, v_max = None, None

    im = ax.pcolormesh(
        time_datetime, range_data, signal_map.T,
        cmap='jet', shading='auto', vmin=v_min, vmax=v_max,
    )

    ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d\n%H:%M:%S'))
    ax.xaxis.set_major_locator(mdates.AutoDateLocator())
    plt.setp(ax.xaxis.get_majorticklabels(), rotation=45, ha='right')

    ax.set_xlabel('Time (UTC)', fontsize=12, fontweight='bold')
    ax.set_ylabel('Range (m)', fontsize=12, fontweight='bold')

    start_time_str = time_datetime[0].strftime('%Y-%m-%d %H:%M:%S UTC')
    ax.set_title(
        f'Range Corrected Lidar Signal (L1) - Start Time: {start_time_str}, Channel: {channel_index}',
        fontsize=14, fontweight='bold',
    )

    ax.set_ylim(0, 20000)
    plt.colorbar(im, ax=ax, label='Range Corrected Signal')
    ax.grid(True, alpha=0.3, linestyle='--')

    plt.tight_layout()

    if output_dir is None:
        output_path = Path('Range_Corrected_Lidar_Signal_L1_ColorMap.png')
    else:
        output_path = Path(output_dir) / 'Range_Corrected_Lidar_Signal_L1_ColorMap.png'

    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"Plot saved successfully: {output_path.absolute()}")
    plt.close(fig)


def find_aerosol_extinction_variables(group):
    """
    Find all Aerosol_Extinction_<X>nm variables in a group.
    
    Args:
        group: NetCDF4 Group object
        
    Returns:
        list: List of variable names matching the pattern
    """
    aerosol_vars = []
    for var_name in group.variables.keys():
        if var_name.startswith('Aerosol_Extinction_') and var_name.endswith('nm'):
            aerosol_vars.append(var_name)
    return aerosol_vars


def plot_aerosol_extinction(dataset, time_index=0, output_dir=None):
    """
    Plot the Aerosol_Extinction_<X>nm variables from the L2_Data group (Data level 2 plots).
    """
    if not check_group_exists(dataset, 'L2_Data'):
        print("Group 'L2_Data' does not exist in the NetCDF file.")
        return

    l2_group = dataset.groups['L2_Data']
    aerosol_vars = find_aerosol_extinction_variables(l2_group)

    if not aerosol_vars:
        print("Warning: No Aerosol_Extinction_<X>nm variables found in group 'L2_Data'.")
        return

    print(f"Found Aerosol_Extinction variables: {aerosol_vars}")

    if not check_variable_exists(l2_group, 'range'):
        print("Warning: Variable 'range' does not exist in group 'L2_Data'.")
        return

    range_data = l2_group.variables['range'][:]
    selected_wavelength, selected_channel = get_l2_selected_wavelength(dataset, l2_group)
    wavelength_str = f"Channel {selected_channel}"
    if selected_wavelength is not None:
        wavelength_str = f"{selected_wavelength} nm"

    for var_name in aerosol_vars:
        var = l2_group.variables[var_name]
        if len(var.shape) != 3:
            print(f"Warning: Variable '{var_name}' does not have 3 dimensions. Skipping.")
            continue

        data = var[:]
        time_dim, lrs_dim, range_dim = data.shape

        if time_index < 0 or time_index >= time_dim:
            print(f"Warning: Time index {time_index} is out of bounds [0, {time_dim-1}]. Using index 0.")
            time_index = 0

        middle_lrs_index = lrs_dim // 2
        extinction_profile = data[time_index, middle_lrs_index, :]

        lrs_value = None
        if check_variable_exists(l2_group, 'LRs'):
            lrs_values = l2_group.variables['LRs'][:]
            if middle_lrs_index < len(lrs_values):
                lrs_value = lrs_values[middle_lrs_index]

        fig, ax = plt.subplots(figsize=(10, 8))
        plot_color = wavelength_color(selected_wavelength)
        ax.plot(extinction_profile, range_data, linewidth=1.5, color=plot_color, alpha=0.8)

        human_time = f"Index {time_index}"
        if check_variable_exists(l2_group, 'Start_Time_AVG_L2'):
            unix_times = l2_group.variables['Start_Time_AVG_L2'][:]
            if time_index < len(unix_times):
                dt = unix_to_datetime(unix_times[time_index])
                human_time = dt.strftime('%Y-%m-%d %H:%M:%S UTC')

        units = getattr(var, 'units', '')
        x_label = f'Aerosol Extinction {wavelength_str}'
        if units:
            x_label += f' ({units})'

        ax.set_xlabel(x_label, fontsize=12, fontweight='bold')
        ax.set_ylabel('Range (m)', fontsize=12, fontweight='bold')

        title = f'Aerosol Extinction Profile - {wavelength_str}'
        if lrs_value is not None:
            title += f'\nTime: {human_time}, LR: {lrs_value:.1f} sr'
        else:
            title += f'\nTime: {human_time}, LRs Index: {middle_lrs_index}'
        ax.set_title(title, fontsize=14, fontweight='bold')

        mask = (range_data >= 0) & (range_data <= 20000)
        profile_subset = extinction_profile[mask]
        if len(profile_subset) > 0:
            data_to_scale = np.asanyarray(profile_subset)
            if hasattr(data_to_scale, 'filled'):
                data_to_scale = data_to_scale.filled(np.nan)

            v_min = np.nanpercentile(data_to_scale, 1)
            v_max = np.nanpercentile(data_to_scale, 99)
            padding = (v_max - v_min) * 0.1 if v_max > v_min else 0.1
            ax.set_xlim(v_min - padding, v_max + padding)

        ax.set_ylim(0, 20000)
        ax.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)
        ax.minorticks_on()

        plt.tight_layout()
        if output_dir is None:
            output_path = Path(f'{var_name}.png')
        else:
            output_path = Path(output_dir) / f'{var_name}.png'

        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        print(f"Plot saved successfully: {output_path.absolute()}")
        plt.close(fig)


def find_Aerosol_Backscatter_variables(group):
    """
    Find all Aerosol_Backscatter_<X>nm variables in a group.
    
    Args:
        group: NetCDF4 Group object
        
    Returns:
        list: List of variable names matching the pattern
    """
    aerosol_vars = []
    for var_name in group.variables.keys():
        if var_name.startswith('Aerosol_Backscatter_') and var_name.endswith('nm'):
            aerosol_vars.append(var_name)
    return aerosol_vars


def plot_Aerosol_Backscatter(dataset, time_index=0, output_dir=None):
    """
    Plot the Aerosol_Backscatter_<X>nm variables from the L2_Data group.
    """
    if not check_group_exists(dataset, 'L2_Data'):
        return

    l2_group = dataset.groups['L2_Data']
    aerosol_vars = find_Aerosol_Backscatter_variables(l2_group)

    if not aerosol_vars:
        print("Note: No Aerosol_Backscatter_<X>nm variables found in group 'L2_Data'.")
        return

    print(f"Found Aerosol_Backscatter variables: {aerosol_vars}")

    if not check_variable_exists(l2_group, 'range'):
        return

    range_data = l2_group.variables['range'][:]
    selected_wavelength, selected_channel = get_l2_selected_wavelength(dataset, l2_group)
    wavelength_str = f"Channel {selected_channel}"
    if selected_wavelength is not None:
        wavelength_str = f"{selected_wavelength} nm"

    for var_name in aerosol_vars:
        var = l2_group.variables[var_name]
        if len(var.shape) != 3:
            continue

        data = var[:]
        time_dim, lrs_dim, range_dim = data.shape

        if time_index < 0 or time_index >= time_dim:
            time_index = 0

        middle_lrs_index = lrs_dim // 2
        backscattering_profile = data[time_index, middle_lrs_index, :]

        lrs_value = None
        if check_variable_exists(l2_group, 'LRs'):
            lrs_values = l2_group.variables['LRs'][:]
            if middle_lrs_index < len(lrs_values):
                lrs_value = lrs_values[middle_lrs_index]

        fig, ax = plt.subplots(figsize=(10, 8))
        plot_color = wavelength_color(selected_wavelength)
        ax.plot(backscattering_profile, range_data, linewidth=1.5, color=plot_color, alpha=0.8)

        human_time = f"Index {time_index}"
        if check_variable_exists(l2_group, 'Start_Time_AVG_L2'):
            unix_times = l2_group.variables['Start_Time_AVG_L2'][:]
            if time_index < len(unix_times):
                dt = unix_to_datetime(unix_times[time_index])
                human_time = dt.strftime('%Y-%m-%d %H:%M:%S UTC')

        units = getattr(var, 'units', '')
        x_label = f'Aerosol Backscatter {wavelength_str}'
        if units:
            x_label += f' ({units})'
        ax.set_xlabel(x_label, fontsize=12, fontweight='bold')
        ax.set_ylabel('Range (m)', fontsize=12, fontweight='bold')

        title = f'Aerosol Backscatter Profile - {wavelength_str}'
        if lrs_value is not None:
            title += f'\nTime: {human_time}, LR: {lrs_value:.1f} sr'
        else:
            title += f'\nTime: {human_time}, LRs Index: {middle_lrs_index}'
        ax.set_title(title, fontsize=14, fontweight='bold')

        ax.set_ylim(0, 20000)
        mask = (range_data >= 0) & (range_data <= 20000)
        profile_subset = backscattering_profile[mask]
        if len(profile_subset) > 0:
            data_to_scale = np.asanyarray(profile_subset)
            if hasattr(data_to_scale, 'filled'):
                data_to_scale = data_to_scale.filled(np.nan)

            v_min = np.nanpercentile(data_to_scale, 1)
            v_max = np.nanpercentile(data_to_scale, 99)
            padding = (v_max - v_min) * 0.1 if v_max > v_min else 0.1
            ax.set_xlim(v_min - padding, v_max + padding)

        ax.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)
        ax.minorticks_on()

        plt.tight_layout()

        if output_dir is None:
            output_path = Path(f'{var_name}.png')
        else:
            output_path = Path(output_dir) / f'{var_name}.png'

        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        print(f"Plot saved successfully: {output_path.absolute()}")
        plt.close(fig)


def plot_Aerosol_Backscatter_colormap(dataset, output_dir=None):
    """
    Plot the Aerosol_Backscatter_<X>nm variables from the L2_Data group as a ColorMap (Data level 2 plots).
    Follows the instruction of the variable "Cloud_Mask" in the group "L1_Data".
    
    Args:
        dataset: NetCDF4 Dataset object
        output_dir: Directory to save the plot
    """
    # Check if L2_Data group exists
    if not check_group_exists(dataset, 'L2_Data'):
        return
    
    l2_group = dataset.groups['L2_Data']
    
    # Find all Aerosol_Backscatter variables
    aerosol_vars = find_Aerosol_Backscatter_variables(l2_group)
    
    if not aerosol_vars:
        return
    
    # Check if necessary variables exist
    if not check_variable_exists(l2_group, 'Start_Time_AVG_L2') or \
       not check_variable_exists(l2_group, 'range'):
        return
    
    # Read common data
    unix_time = l2_group.variables['Start_Time_AVG_L2'][:]
    range_data = l2_group.variables['range'][:]
    time_datetime = unix_to_datetime(unix_time)
    
    # Get the selected wavelength
    selected_wavelength, selected_channel = get_l2_selected_wavelength(dataset, l2_group)
    wavelength_str = f"Channel {selected_channel}"
    if selected_wavelength is not None:
        wavelength_str = f"{selected_wavelength} nm"
    
    # Get LRs if available
    lrs_values = None
    if check_variable_exists(l2_group, 'LRs'):
        lrs_values = l2_group.variables['LRs'][:]
        
    for var_name in aerosol_vars:
        var = l2_group.variables[var_name]
        if len(var.shape) != 3:
            continue
            
        data = var[:]
        time_dim, lrs_dim, range_dim = data.shape
        
        # Take the middle LRs index as requested (consistent with 1D plot)
        middle_lrs_index = lrs_dim // 2
        lrs_val = "Unknown"
        if lrs_values is not None and middle_lrs_index < len(lrs_values):
            lrs_val = f"{lrs_values[middle_lrs_index]:.1f} sr"
        
        # Extract the slice (time, range)
        signal_map = data[:, middle_lrs_index, :]
        
        # Create the plot
        fig, ax = plt.subplots(figsize=(12, 6))
        
        # Robust scaling
        mask_range = (range_data >= 0) & (range_data <= 20000)
        data_subset = signal_map[:, mask_range]
        if data_subset.size > 0:
            data_to_scale = np.asanyarray(data_subset)
            if hasattr(data_to_scale, 'filled'):
                data_to_scale = data_to_scale.filled(np.nan)
            v_min = np.nanpercentile(data_to_scale, 5)
            v_max = np.nanpercentile(data_to_scale, 95)
        else:
            v_min, v_max = None, None

        im = ax.pcolormesh(time_datetime, range_data, signal_map.T, 
                           cmap='jet', shading='auto', vmin=v_min, vmax=v_max)
        
        # Format axes
        ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d\n%H:%M:%S'))
        ax.xaxis.set_major_locator(mdates.AutoDateLocator())
        plt.setp(ax.xaxis.get_majorticklabels(), rotation=45, ha='right')
        
        ax.set_xlabel('Time (UTC)', fontsize=12, fontweight='bold')
        ax.set_ylabel('Range (m)', fontsize=12, fontweight='bold')
        
        # Set range limit
        ax.set_ylim(0, 20000)
        
        start_time_str = time_datetime[0].strftime('%Y-%m-%d %H:%M:%S UTC')
        ax.set_title(f'{var_name.replace("_", " ")} (L2)\nStart Time: {start_time_str}, Wavelength: {wavelength_str}, LR: {lrs_val}', 
                     fontsize=14, fontweight='bold')
        
        # Colorbar
        plt.colorbar(im, ax=ax, label='Backscattering Coefficient')
        
        ax.grid(True, alpha=0.3, linestyle='--')
        plt.tight_layout()
        
        # Save plot: Aerosol_Backscatter_<X>nm_ColorMap.png
        output_filename = f"{var_name}_ColorMap.png"
        if output_dir is None:
            output_path = Path(output_filename)
        else:
            output_path = Path(output_dir) / output_filename
            
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        print(f"Plot saved successfully: {output_path.absolute()}")
        plt.close(fig)


def plot_aerosol_extinction_colormap(dataset, output_dir=None):
    """
    Plot the Aerosol_Extinction_<X>nm variables from the L2_Data group as a ColorMap.
    """
    if not check_group_exists(dataset, 'L2_Data'):
        print("Warning: Group 'L2_Data' does not exist in the NetCDF file for Extinction ColorMap.")
        return

    l2_group = dataset.groups['L2_Data']
    aerosol_vars = find_aerosol_extinction_variables(l2_group)
    if not aerosol_vars:
        print("Note: No Aerosol_Extinction_<X>nm variables found in group 'L2_Data'.")
        return

    if not check_variable_exists(l2_group, 'Start_Time_AVG_L2') or not check_variable_exists(l2_group, 'range'):
        print("Warning: Missing required variables for Extinction ColorMap.")
        return

    unix_time = l2_group.variables['Start_Time_AVG_L2'][:]
    range_data = l2_group.variables['range'][:]
    time_datetime = unix_to_datetime(unix_time)
    selected_wavelength, selected_channel = get_l2_selected_wavelength(dataset, l2_group)
    wavelength_str = f"Channel {selected_channel}"
    if selected_wavelength is not None:
        wavelength_str = f"{selected_wavelength} nm"

    lrs_values = None
    if check_variable_exists(l2_group, 'LRs'):
        lrs_values = l2_group.variables['LRs'][:]

    for var_name in aerosol_vars:
        var = l2_group.variables[var_name]
        if len(var.shape) != 3:
            continue

        data = var[:]
        time_dim, lrs_dim, range_dim = data.shape
        middle_lrs_index = lrs_dim // 2
        lrs_val = 'Unknown'
        if lrs_values is not None and middle_lrs_index < len(lrs_values):
            lrs_val = f'{lrs_values[middle_lrs_index]:.1f} sr'

        signal_map = data[:, middle_lrs_index, :]

        fig, ax = plt.subplots(figsize=(12, 6))
        mask_range = (range_data >= 0) & (range_data <= 20000)
        data_subset = signal_map[:, mask_range]
        if data_subset.size > 0:
            data_to_scale = np.asanyarray(data_subset)
            if hasattr(data_to_scale, 'filled'):
                data_to_scale = data_to_scale.filled(np.nan)
            v_min = np.nanpercentile(data_to_scale, 5)
            v_max = np.nanpercentile(data_to_scale, 95)
        else:
            v_min, v_max = None, None

        im = ax.pcolormesh(time_datetime, range_data, signal_map.T,
                           cmap='jet', shading='auto', vmin=v_min, vmax=v_max)

        ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d\n%H:%M:%S'))
        ax.xaxis.set_major_locator(mdates.AutoDateLocator())
        plt.setp(ax.xaxis.get_majorticklabels(), rotation=45, ha='right')

        ax.set_xlabel('Time (UTC)', fontsize=12, fontweight='bold')
        ax.set_ylabel('Range (m)', fontsize=12, fontweight='bold')

        start_time_str = time_datetime[0].strftime('%Y-%m-%d %H:%M:%S UTC')
        ax.set_title(
            f'{var_name.replace("_", " ")} (L2)\nStart Time: {start_time_str}, Wavelength: {wavelength_str}, LR: {lrs_val}',
            fontsize=14, fontweight='bold'
        )

        ax.set_ylim(0, 20000)
        plt.colorbar(im, ax=ax, label='Extinction Coefficient')
        ax.grid(True, alpha=0.3, linestyle='--')

        plt.tight_layout()
        output_filename = f"{var_name}_ColorMap.png"
        if output_dir is None:
            output_path = Path(output_filename)
        else:
            output_path = Path(output_dir) / output_filename

        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        print(f"Plot saved successfully: {output_path.absolute()}")
        plt.close(fig)


def plot_range_corrected_lidar_signal(dataset, time_index=0, output_dir=None):
    """
    Plot the Range_Corrected_Lidar_Signal_L2 variable from the L2_Data group.
    """
    # Check if L2_Data group exists
    if not check_group_exists(dataset, 'L2_Data'):
        print("Group 'L2_Data' does not exist in the NetCDF file.")
        return
    
    l2_group = dataset.groups['L2_Data']
    
    # Check if variable exists
    if not check_variable_exists(l2_group, 'Range_Corrected_Lidar_Signal_L2'):
        print("Variable 'Range_Corrected_Lidar_Signal_L2' does not exist.")
        return
    
    # Check if range variable exists
    if not check_variable_exists(l2_group, 'range'):
        print("Variable 'range' does not exist.")
        return
    
    # Get channel index from group attribute
    try:
        channel_index = getattr(l2_group, 'indxChannel_for_Fernald_inv')
    except AttributeError:
        print("Warning: Attribute 'indxChannel_for_Fernald_inv' not found in L2_Data. Using 0.")
        channel_index = 0
    
    # Read the data
    var = l2_group.variables['Range_Corrected_Lidar_Signal_L2']
    data = var[:]
    range_data = l2_group.variables['range'][:]
    
    # Check dimensions (time, channels, range)
    time_dim, chan_dim, range_dim = data.shape
    
    # Validate indices
    if time_index < 0 or time_index >= time_dim:
        print(f"Warning: Time index {time_index} is out of bounds [0, {time_dim-1}]. Using 0.")
        time_index = 0
        
    if channel_index < 0 or channel_index >= chan_dim:
        print(f"Warning: Channel index {channel_index} is out of bounds [0, {chan_dim-1}]. Using 0.")
        channel_index = 0
        
    # Extract the slice: [time_index, channel_index, :]
    signal_profile = data[time_index, channel_index, :]
    
    # Get human-readable time
    human_time = f"Index {time_index}"
    if check_variable_exists(l2_group, 'Start_Time_AVG_L2'):
        unix_times = l2_group.variables['Start_Time_AVG_L2'][:]
        if time_index < len(unix_times):
            dt = unix_to_datetime(unix_times[time_index])
            human_time = dt.strftime('%Y-%m-%d %H:%M:%S UTC')

    # Try to get wavelength for the selected channel from root-level Wavelengths
    selected_wavelength = None
    wavelength_str = f"Channel {channel_index}"
    if check_variable_exists(dataset, 'Wavelengths'):
        wavelengths = dataset.variables['Wavelengths'][:]
        if channel_index < len(wavelengths):
            selected_wavelength = wavelengths[channel_index]
            wavelength_str = f"{selected_wavelength} nm"
        else:
            wavelength_str = f"Channel {channel_index}"
    
    # Create the plot
    fig, ax = plt.subplots(figsize=(10, 8))
    
    plot_color = wavelength_color(selected_wavelength)
    ax.plot(signal_profile, range_data, linewidth=1.5, color=plot_color, alpha=0.8)
    
    # Labels and title
    ax.set_ylabel('Range (m)', fontsize=12, fontweight='bold')
    ax.set_xlabel('Range Corrected Lidar Signal', fontsize=12, fontweight='bold')
    
    title = f'Range Corrected Lidar Signal (L2)'
    title += f'\nTime: {human_time}, Wavelength: {wavelength_str}'
    ax.set_title(title, fontsize=14, fontweight='bold')
    
    # Adjust scale to show data better
    mask = (range_data >= 0) & (range_data <= 20000)
    profile_subset = signal_profile[mask]
    
    if len(profile_subset) > 0:
        # Convert MaskedArray to filled array with NaNs to avoid UserWarning
        data_to_scale = np.asanyarray(profile_subset)
        if hasattr(data_to_scale, 'filled'):
            data_to_scale = data_to_scale.filled(np.nan)
            
        v_min = np.nanpercentile(data_to_scale, 1)
        v_max = np.nanpercentile(data_to_scale, 99)
        padding = (v_max - v_min) * 0.1 if v_max > v_min else 0.1
        ax.set_xlim(v_min - padding, v_max + padding)
    
    # Limit range to 20000 meters on Y-axis
    ax.set_ylim(0, 20000)
    
    # Draw the grid lines of the plot (as requested in prompt.md)
    ax.grid(True, which='major', linestyle='-', linewidth='0.5', color='gray', alpha=0.5)
    ax.grid(True, which='minor', linestyle=':', linewidth='0.25', color='gray', alpha=0.3)
    ax.minorticks_on()
    
    # Tight layout to prevent label cutoff
    plt.tight_layout()
    
    # Save the plot
    if output_dir is None:
        output_path = Path('Range_Corrected_Lidar_Signal_L2.png')
    else:
        output_path = Path(output_dir) / 'Range_Corrected_Lidar_Signal_L2.png'
    
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"Plot saved successfully: {output_path.absolute()}")
    
    # Close the figure to free memory
    plt.close(fig)


def plot_range_corrected_lidar_signal_colormap(dataset, output_dir=None):
    """
    Plot the Range_Corrected_Lidar_Signal_L2 variable from the L2_Data group as a ColorMap.
    """
    # Check if L2_Data group exists
    if not check_group_exists(dataset, 'L2_Data'):
        print("Warning: Group 'L2_Data' does not exist in the NetCDF file for ColorMap.")
        return
    
    l2_group = dataset.groups['L2_Data']
    
    # Check if variable exists
    if not check_variable_exists(l2_group, 'Range_Corrected_Lidar_Signal_L2'):
        print("Warning: Variable 'Range_Corrected_Lidar_Signal_L2' does not exist in group 'L2_Data' for ColorMap.")
        return
    
    # Check if Start_Time_AVG_L2 exists
    if not check_variable_exists(l2_group, 'Start_Time_AVG_L2'):
        print("Warning: Variable 'Start_Time_AVG_L2' does not exist in group 'L2_Data' for ColorMap.")
        return
        
    # Check if range variable exists
    if not check_variable_exists(l2_group, 'range'):
        print("Warning: Variable 'range' does not exist in group 'L2_Data' for ColorMap.")
        return
    
    # Get channel index from group attribute
    try:
        channel_index = getattr(l2_group, 'indxChannel_for_Fernald_inv')
    except AttributeError:
        print("Warning: Attribute 'indxChannel_for_Fernald_inv' not found in L2_Data for ColorMap. Using 0.")
        channel_index = 0
    
    # Read the data
    var = l2_group.variables['Range_Corrected_Lidar_Signal_L2']
    data = var[:]
    unix_time = l2_group.variables['Start_Time_AVG_L2'][:]
    range_data = l2_group.variables['range'][:]
    
    # Check dimensions (time, channels, range)
    time_dim, chan_dim, range_dim = data.shape
    
    if channel_index < 0 or channel_index >= chan_dim:
        print(f"Warning: Channel index {channel_index} is out of bounds for ColorMap. Using 0.")
        channel_index = 0
        
    # Extract the slice for the selected channel: (time, range)
    signal_map = data[:, channel_index, :]
    
    # Get wavelength string from root-level Wavelengths variable
    wavelength_str = f"Channel {channel_index}"
    if check_variable_exists(dataset, 'Wavelengths'):
        wavelengths = dataset.variables['Wavelengths'][:]
        if channel_index < len(wavelengths):
            wavelength = wavelengths[channel_index]
            wavelength_str = f"{wavelength} nm"

    # Convert Unix time to datetime
    time_datetime = unix_to_datetime(unix_time)
    
    # Create the plot
    fig, ax = plt.subplots(figsize=(12, 6))
    
    # Find scaling values (robust scaling to show data better)
    # Mask range up to 20000m for scaling calculation
    mask_range = (range_data >= 0) & (range_data <= 20000)
    data_subset = signal_map[:, mask_range]
    
    if data_subset.size > 0:
        # Convert MaskedArray to filled array with NaNs to avoid UserWarning
        data_to_scale = np.asanyarray(data_subset)
        if hasattr(data_to_scale, 'filled'):
            data_to_scale = data_to_scale.filled(np.nan)
            
        v_min = np.nanpercentile(data_to_scale, 5)
        v_max = np.nanpercentile(data_to_scale, 95)
    else:
        v_min, v_max = None, None

    # Plot using pcolormesh
    im = ax.pcolormesh(time_datetime, range_data, signal_map.T, 
                       cmap='jet', shading='auto', vmin=v_min, vmax=v_max)
    
    # Format x-axis to show dates nicely
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d\n%H:%M:%S'))
    ax.xaxis.set_major_locator(mdates.AutoDateLocator())
    plt.setp(ax.xaxis.get_majorticklabels(), rotation=45, ha='right')
    
    # Labels and title
    ax.set_xlabel('Time (UTC)', fontsize=12, fontweight='bold')
    ax.set_ylabel('Range (m)', fontsize=12, fontweight='bold')
    
    # Start time for title
    start_time_str = time_datetime[0].strftime('%Y-%m-%d %H:%M:%S UTC')
    ax.set_title(
        f'Range Corrected Lidar Signal (L2) - Start Time: {start_time_str}, {wavelength_str}',
        fontsize=14, fontweight='bold'
    )
    
    # Limit range to 20000 meters
    ax.set_ylim(0, 20000)
    
    # Add colorbar
    plt.colorbar(im, ax=ax, label='Range Corrected Lidar Signal')
    
    # Grid for better readability
    ax.grid(True, alpha=0.3, linestyle='--')
    
    # Tight layout
    plt.tight_layout()
    
    # Save the plot
    if output_dir is None:
        output_path = Path('Range_Corrected_Lidar_Signal_L2_ColorMap.png')
    else:
        output_path = Path(output_dir) / 'Range_Corrected_Lidar_Signal_L2_ColorMap.png'
    
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"Plot saved successfully: {output_path.absolute()}")
    
    # Close the figure
    plt.close(fig)


def plot_aeronet_data(dataset, output_dir=None):
    """
    Plot the AERONET_AOD_<X>nm_at_LidarTime variables from the AERONET_Data sub-group in L2_Data.
    """
    if not check_group_exists(dataset, 'L2_Data'):
        return

    l2_group = dataset.groups['L2_Data']

    if 'AERONET_Data' not in l2_group.groups:
        print("Note: Sub-group 'AERONET_Data' not found in L2_Data. Skipping.")
        return

    aeronet_group = l2_group.groups['AERONET_Data']

    # Get the time for x-axis as the mean of Start_Time_AVG_L2 and Stop_Time_AVG_L2
    if not check_variable_exists(l2_group, 'Start_Time_AVG_L2') or not check_variable_exists(l2_group, 'Stop_Time_AVG_L2'):
        print("Warning: Variables 'Start_Time_AVG_L2' or 'Stop_Time_AVG_L2' not found in L2_Data. Cannot compute x-axis.")
        return

    start_time = l2_group.variables['Start_Time_AVG_L2'][:].astype(np.float64)
    stop_time = l2_group.variables['Stop_Time_AVG_L2'][:].astype(np.float64)
    mean_time = (start_time + stop_time) / 2.0
    time_datetime = unix_to_datetime(mean_time)

    # Find matching variables: AERONET_AOD_<X>nm_at_LidarTime
    aeronet_vars = []
    for var_name in aeronet_group.variables.keys():
        if var_name.startswith('AERONET_AOD_') and var_name.endswith('nm_at_LidarTime'):
            aeronet_vars.append(var_name)

    if not aeronet_vars:
        print("Note: No AERONET_AOD_<X>nm_at_LidarTime variables found.")
        return

    for var_name in aeronet_vars:
        data = aeronet_group.variables[var_name][:]
        
        # Check dimensions
        if len(data) != len(mean_time):
            print(f"Warning: Dimension mismatch for {var_name}. Expected {len(mean_time)}, got {len(data)}. Skipping.")
            continue

        wavelength = var_name.replace('AERONET_AOD_', '').replace('nm_at_LidarTime', '')
        
        # Check for synergy variables
        synergy_aod_data = None
        synergy_lr_data = None
        synergy_aod_var_name = f'AOD_Lidar_{wavelength}nm_vs_AERONET'
        synergy_lr_var_name = f'LR_inv_{wavelength}nm'
        
        if 'LIDAR_AERONET_synergy' in l2_group.groups:
            synergy_group = l2_group.groups['LIDAR_AERONET_synergy']
            if synergy_aod_var_name in synergy_group.variables:
                synergy_aod_data = synergy_group.variables[synergy_aod_var_name][:]
                if len(synergy_aod_data) != len(mean_time):
                    synergy_aod_data = None # Dimension mismatch
            if synergy_lr_var_name in synergy_group.variables:
                synergy_lr_data = synergy_group.variables[synergy_lr_var_name][:]
                if len(synergy_lr_data) != len(mean_time):
                    synergy_lr_data = None
        
        # Filter negative values: "If the value of the variable is negative, do not plot the point."
        aeronet_mask = (data >= 0)
        filtered_aeronet_time = time_datetime[aeronet_mask]
        filtered_aeronet_aod = data[aeronet_mask]

        filtered_lidar_time = None
        filtered_lidar_aod = None
        if synergy_aod_data is not None:
            lidar_mask = (synergy_aod_data >= 0)
            filtered_lidar_time = time_datetime[lidar_mask]
            filtered_lidar_aod = synergy_aod_data[lidar_mask]

        filtered_lr_time = None
        filtered_lidar_lr = None
        if synergy_lr_data is not None:
            lr_mask = (synergy_lr_data >= 0)
            filtered_lr_time = time_datetime[lr_mask]
            filtered_lidar_lr = synergy_lr_data[lr_mask]

        if len(filtered_aeronet_time) == 0 and (filtered_lidar_time is None or len(filtered_lidar_time) == 0):
            print(f"Note: No valid (positive) points found for {var_name}. Skipping plot.")
            continue

        # Find Angstrom Exponent variables
        angstrom_vars = [v for v in aeronet_group.variables.keys() if v.startswith('AERONET_Angs_') and v.endswith('_at_LidarTime')]
        
        # fallback: if not found, try AERONET_Angstrom_Exponent_
        if not angstrom_vars:
            angstrom_vars = [v for v in aeronet_group.variables.keys() if v.startswith('AERONET_Angstrom_Exponent_')]

        angstrom_data = None
        angstrom_suffix = "X-Y"
        filtered_angs_time = None
        filtered_angs_data = None

        if angstrom_vars:
            angstrom_var_name = angstrom_vars[0]
            angstrom_data = aeronet_group.variables[angstrom_var_name][:]
            if angstrom_var_name.startswith('AERONET_Angs_'):
                angstrom_suffix = angstrom_var_name.replace('AERONET_Angs_', '').replace('_at_LidarTime', '')
            else:
                angstrom_suffix = angstrom_var_name.replace('AERONET_Angstrom_Exponent_', '')
            
            if len(angstrom_data) == len(mean_time):
                angs_mask = (angstrom_data >= 0)
                filtered_angs_time = time_datetime[angs_mask]
                filtered_angs_data = angstrom_data[angs_mask]
            else:
                angstrom_data = None # Dimension mismatch

        # Create the plot
        num_subplots = 1
        if filtered_lidar_lr is not None: num_subplots += 1
        if filtered_angs_data is not None: num_subplots += 1

        if num_subplots > 1:
            fig, axes = plt.subplots(num_subplots, 1, figsize=(12, 4 * num_subplots), sharex=False)
            if num_subplots == 2:
                ax1, ax2 = axes
                ax3 = None
            else:
                ax1, ax2, ax3 = axes
            plt.subplots_adjust(hspace=0.3)
        else:
            fig, ax1 = plt.subplots(figsize=(12, 6))
            ax2 = ax3 = None
        
        # Plot AOD comparison on ax1
        aeronet_color = '#1f77b4'  # blue
        lidar_color = '#ff7f0e'    # orange

        if len(filtered_aeronet_time) > 0:
            ax1.scatter(filtered_aeronet_time, filtered_aeronet_aod, marker='o', color=aeronet_color,
                        edgecolor='black', linewidth=0.5, s=40,
                        label=f'AERONET AOD {wavelength}nm')

        if filtered_lidar_aod is not None and len(filtered_lidar_time) > 0:
            ax1.scatter(filtered_lidar_time, filtered_lidar_aod, marker='x', color=lidar_color,
                        linewidth=1.5, s=60,
                        label=f'Lidar AOD {wavelength}nm')

        ax1.legend(loc='best')
        ax1.set_ylabel(f'AOD at {wavelength}nm', fontsize=12, fontweight='bold')
        
        # Title with time in human-readable format
        human_time_start = time_datetime[0].strftime('%Y-%m-%d %H:%M:%S')
        human_time_end = time_datetime[-1].strftime('%H:%M:%S')
        ax1.set_title(f'Synergy Plot at {wavelength}nm\nTime: {human_time_start} to {human_time_end} (UTC)', fontsize=14, fontweight='bold')
        
        # Plot LR on ax2 if available
        if ax2 is not None and filtered_lidar_lr is not None and len(filtered_lr_time) > 0:
            ax2.scatter(filtered_lr_time, filtered_lidar_lr, marker='D', color='#2ca02c',
                        edgecolor='black', linewidth=0.5, s=45,
                        label=f'Lidar Ratio {wavelength}nm')
            ax2.set_ylabel(f'LR (sr) at {wavelength}nm', fontsize=12, fontweight='bold')
            ax2.legend(loc='best')
            
        # Plot Angstrom Exponent on ax3 if available
        if ax3 is not None and filtered_angs_data is not None and len(filtered_angs_time) > 0:
            ax3.scatter(filtered_angs_time, filtered_angs_data, marker='s', color='#8E44AD', 
                        edgecolor='black', linewidth=0.5, s=40, label=f'Angstrom Exp {angstrom_suffix}')
            ax3.set_ylabel('Angstrom Exp', fontsize=12, fontweight='bold')
            ax3.legend(loc='best')

        # Draw grid lines for all active subplots
        active_axes = [ax for ax in [ax1, ax2, ax3] if ax is not None]
        for ax in active_axes:
            ax.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)
            ax.minorticks_on()
            ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M:%S'))
            ax.xaxis.set_major_locator(mdates.AutoDateLocator())
            plt.setp(ax.xaxis.get_majorticklabels(), rotation=45, ha='right')
            ax.set_xlabel('Time (UTC)', fontsize=12, fontweight='bold')

        # Save the plot
        filename = f'AERONET_vs_LIDAR_AOD_{wavelength}nm_LRs_Sinergy_Angstrom_Exponent_{angstrom_suffix}_at_LidarTime.png'
        if output_dir is None:
            output_path = Path(filename)
        else:
            output_path = Path(output_dir) / filename
        
        plt.tight_layout()
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        print(f"Plot saved successfully: {output_path.absolute()}")
        plt.close(fig)


def plot_aod_lr(dataset, output_dir=None):
    """
    Plot the AOD_LR variable from the L2_Data group.
    
    AOD_LR is a matrix (time, LRs). It should be plotted as a multi-line plot:
    x-axis: Start_Time_AVG_L2 (converted to UTC human-readable)
    y-axis: AOD_LR values for each LR slice
    legend: L2_Data/LRs values
    """
    # Check if L2_Data group exists
    if not check_group_exists(dataset, 'L2_Data'):
        return

    l2_group = dataset.groups['L2_Data']

    # Check if AOD_LR variable exists
    if not check_variable_exists(l2_group, 'AOD_LR'):
        print("Note: Variable 'AOD_LR' not found in L2_Data. Skipping.")
        return

    # Check if Start_Time_AVG_L2 exists
    if not check_variable_exists(l2_group, 'Start_Time_AVG_L2'):
        print("Warning: Variable 'Start_Time_AVG_L2' not found for AOD_LR plot.")
        return

    # Check if LRs exists
    if not check_variable_exists(l2_group, 'LRs'):
        print("Warning: Variable 'LRs' not found for AOD_LR plot.")
        return

    # Read data
    aod_lr = l2_group.variables['AOD_LR'][:]
    unix_time = l2_group.variables['Start_Time_AVG_L2'][:]
    lrs = l2_group.variables['LRs'][:]

    # Convert Unix time to datetime
    time_datetime = unix_to_datetime(unix_time)
    selected_wavelength, selected_channel = get_l2_selected_wavelength(dataset, l2_group)
    wavelength_str = f'Channel {selected_channel}'
    if selected_wavelength is not None:
        wavelength_str = f'{selected_wavelength} nm'

    # Create the plot
    fig, ax = plt.subplots(figsize=(12, 8))

    # Plot each LR slice
    # aod_lr shape is (time, LRs)
    num_lrs = len(lrs)
    for i in range(num_lrs):
        ax.plot(time_datetime, aod_lr[:, i], label=f'LR = {lrs[i]} sr')

    # Formatting
    ax.set_xlabel('Time (UTC)', fontsize=12, fontweight='bold')
    ax.set_ylabel('AOD', fontsize=12, fontweight='bold')
    
    date_str = time_datetime[0].strftime('%Y-%m-%d')
    ax.set_title(
        f'AOD for different Lidar Ratios (LRs) - {wavelength_str} - {date_str} (UTC)',
        fontsize=14, fontweight='bold'
    )
    
    # Format x-axis to show times nicely
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M:%S'))
    ax.xaxis.set_major_locator(mdates.AutoDateLocator())
    plt.setp(ax.xaxis.get_majorticklabels(), rotation=45, ha='right')

    # Grid
    ax.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)
    
    # Legend - might be large, so put it outside or in a scrollable way if many
    if num_lrs > 10:
        ax.legend(loc='center left', bbox_to_anchor=(1, 0.5), fontsize='small', ncol=2 if num_lrs > 20 else 1)
    else:
        ax.legend(loc='best')

    plt.tight_layout()

    # Save the plot
    if output_dir is None:
        output_path = Path('AOD_LR.png')
    else:
        output_path = Path(output_dir) / 'AOD_LR.png'

    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"Plot saved successfully: {output_path.absolute()}")

    # Close the figure
    plt.close(fig)


def main():
    """Main function to parse arguments and process NetCDF file."""
    parser = argparse.ArgumentParser(
        description='Read and plot data from NetCDF files.',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    python plot_netcdf.py data.nc
    python plot_netcdf.py /path/to/data.nc
        """
    )
    parser.add_argument('netcdf_file', type=str, 
                       help='Path to the NetCDF file to process')
    parser.add_argument('output_dir', type=str,
                       help='Directory where the PNG files must be saved')
    parser.add_argument('time_index', type=int, nargs='?', default=0,
                       help='Time index for Aerosol_Extinction plot (default: 0)')
    
    args = parser.parse_args()
    
    # Check if file exists
    netcdf_path = Path(args.netcdf_file)
    if not netcdf_path.exists():
        print(f"Error: File '{netcdf_path}' does not exist.")
        sys.exit(1)
    
    if not netcdf_path.is_file():
        print(f"Error: '{netcdf_path}' is not a file.")
        sys.exit(1)
    
    # Create output directory if it doesn't exist
    output_path = Path(args.output_dir)
    output_path.mkdir(parents=True, exist_ok=True)
    
    print(f"Reading NetCDF file: {netcdf_path.absolute()}")
    
    try:
        # Open the NetCDF file
        with nc.Dataset(netcdf_path, 'r') as dataset:
            print(f"Successfully opened NetCDF file.")
            print(f"Available groups: {list(dataset.groups.keys())}")
            
            # Plot Cloud_Mask
            plot_cloud_mask(dataset, args.output_dir)

            # Plot range-corrected Raw_Lidar_Data_L1 ColorMap
            plot_range_corrected_lidar_signal_l1_colormap(dataset, args.output_dir)
            
            # Plot Aerosol_Extinction
            plot_aerosol_extinction(dataset, args.time_index, args.output_dir)
            
            # Plot Aerosol_Backscatter
            plot_Aerosol_Backscatter(dataset, args.time_index, args.output_dir)
            
            # Plot Aerosol_Backscatter ColorMap
            plot_Aerosol_Backscatter_colormap(dataset, args.output_dir)

            # Plot Aerosol_Extinction ColorMap
            plot_aerosol_extinction_colormap(dataset, args.output_dir)
            
            # Plot Range Corrected Lidar Signal
            plot_range_corrected_lidar_signal(dataset, args.time_index, args.output_dir)
            
            # Plot Range Corrected Lidar Signal ColorMap
            plot_range_corrected_lidar_signal_colormap(dataset, args.output_dir)
            
            # Plot AERONET Data
            plot_aeronet_data(dataset, args.output_dir)
            
            # Plot AOD_LR
            plot_aod_lr(dataset, args.output_dir)
            
    except Exception as e:
        print(f"Error processing NetCDF file: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
    
    print("Processing complete!")


if __name__ == '__main__':
    main()
