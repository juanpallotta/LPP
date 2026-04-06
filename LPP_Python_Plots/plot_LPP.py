#!/usr/bin/env python3
"""
Script to read and plot data from NetCDF files.

This script reads a NetCDF file and plots:
1. Cloud_Mask variable from the L1_Data group (if it exists) as a 2D image (Data level 1 plots).
2. Aerosol_Extinction_<X>nm and Aerosol_Backscattering_<X>nm variables from the L2_Data group (if they exist) as 1D and 2D (ColorMap) plots (Data level 2 plots).
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
        output_path = Path('Cloud_Mask.png')
    else:
        output_path = Path(output_dir) / 'Cloud_Mask.png'
    
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"Plot saved successfully: {output_path.absolute()}")
    
    # Close the figure to free memory
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
    
    Args:
        dataset: NetCDF4 Dataset object
        time_index: Time index to plot (default: 0)
        output_dir: Directory to save the plot (default: same as input file)
    """
    # Check if L2_Data group exists
    if not check_group_exists(dataset, 'L2_Data'):
        print("Group 'L2_Data' does not exist in the NetCDF file.")
        return
    
    l2_group = dataset.groups['L2_Data']
    
    # Find all Aerosol_Extinction variables
    aerosol_vars = find_aerosol_extinction_variables(l2_group)
    
    if not aerosol_vars:
        print("Warning: No Aerosol_Extinction_<X>nm variables found in group 'L2_Data'.")
        return
    
    print(f"Found Aerosol_Extinction variables: {aerosol_vars}")
    
    # Check if range variable exists
    if not check_variable_exists(l2_group, 'range'):
        print("Warning: Variable 'range' does not exist in group 'L2_Data'.")
        return
    
    # Read the range data
    range_data = l2_group.variables['range'][:]
    
    # Plot each Aerosol_Extinction variable
    for var_name in aerosol_vars:
        var = l2_group.variables[var_name]
        
        # Check dimensions
        if len(var.shape) != 3:
            print(f"Warning: Variable '{var_name}' does not have 3 dimensions. Skipping.")
            continue
        
        # Get the data
        data = var[:]
        time_dim, lrs_dim, range_dim = data.shape
        
        # Validate time index
        if time_index < 0 or time_index >= time_dim:
            print(f"Warning: Time index {time_index} is out of bounds [0, {time_dim-1}]. Using index 0.")
            time_index = 0
        
        # Get the middle LRs index
        middle_lrs_index = lrs_dim // 2
        
        # Extract the slice: [time_index, middle_lrs_index, :]
        extinction_profile = data[time_index, middle_lrs_index, :]
        
        # Get LRs value if available
        lrs_value = None
        if check_variable_exists(l2_group, 'LRs'):
            lrs_values = l2_group.variables['LRs'][:]
            if middle_lrs_index < len(lrs_values):
                lrs_value = lrs_values[middle_lrs_index]
        
        # Create the plot
        fig, ax = plt.subplots(figsize=(10, 8))
        
        # Plot as a line plot (extinction on x-axis, range on y-axis as requested)
        ax.plot(extinction_profile, range_data, linewidth=1.5, color='#2E86AB', alpha=0.8)
        
        # Get human-readable time if variable exists
        human_time = f"Index {time_index}"
        if check_variable_exists(l2_group, 'Start_Time_AVG_L2'):
            unix_times = l2_group.variables['Start_Time_AVG_L2'][:]
            if time_index < len(unix_times):
                dt = unix_to_datetime(unix_times[time_index])
                human_time = dt.strftime('%Y-%m-%d %H:%M:%S UTC')
        
        # Labels and title
        wavelength = var_name.replace('Aerosol_Extinction_', '').replace('nm', '')
        ax.set_ylabel('Range (m)', fontsize=12, fontweight='bold')
        ax.set_xlabel(f'Aerosol Extinction Coefficient (m⁻¹) @ {wavelength}nm', fontsize=12, fontweight='bold')
        
        title = f'Aerosol Extinction Profile'
        if lrs_value is not None:
            title += f'\nTime: {human_time}, LR: {lrs_value:.1f} sr'
        else:
            title += f'\nTime: {human_time}, LRs Index: {middle_lrs_index}'
        ax.set_title(title, fontsize=14, fontweight='bold')
        
        # Adjust scale to show data better if values are too large or have spikes
        # Filter data within the range of interest (up to 20000m)
        mask = (range_data >= 0) & (range_data <= 20000)
        profile_subset = extinction_profile[mask]
        
        if len(profile_subset) > 0:
            # Use percentiles for robust scaling to ignore extreme spikes/noise
            # This helps "show the data better" as requested
            # Convert MaskedArray to filled array with NaNs to avoid UserWarning
            data_to_scale = np.asanyarray(profile_subset)
            if hasattr(data_to_scale, 'filled'):
                data_to_scale = data_to_scale.filled(np.nan)
                
            v_min = np.nanpercentile(data_to_scale, 1)
            v_max = np.nanpercentile(data_to_scale, 99)
            
            # Add some padding
            padding = (v_max - v_min) * 0.1 if v_max > v_min else 0.1
            ax.set_xlim(v_min - padding, v_max + padding)
            
            # If the data spans several orders of magnitude, consider log scale?
            # But extinction often has negative noise values, so symlog is better
            # However, for now, robust linear scaling is usually preferred for profiles.
        
        # Limit range to 20000 meters on Y-axis now
        ax.set_ylim(0, 20000)
        
        # Draw the grid lines of the plot (as requested in prompt.md)
        ax.grid(True, which='major', linestyle='-', linewidth='0.5', color='gray', alpha=0.5)
        ax.grid(True, which='minor', linestyle=':', linewidth='0.25', color='gray', alpha=0.3)
        ax.minorticks_on()
        
        # Tight layout to prevent label cutoff
        plt.tight_layout()
        
        # Save the plot
        if output_dir is None:
            output_path = Path(f'{var_name}.png')
        else:
            output_path = Path(output_dir) / f'{var_name}.png'
        
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        print(f"Plot saved successfully: {output_path.absolute()}")
        
        # Close the figure to free memory
        plt.close(fig)


def find_aerosol_backscattering_variables(group):
    """
    Find all Aerosol_Backscattering_<X>nm variables in a group.
    
    Args:
        group: NetCDF4 Group object
        
    Returns:
        list: List of variable names matching the pattern
    """
    aerosol_vars = []
    for var_name in group.variables.keys():
        if var_name.startswith('Aerosol_Backscattering_') and var_name.endswith('nm'):
            aerosol_vars.append(var_name)
    return aerosol_vars


def plot_aerosol_backscattering(dataset, time_index=0, output_dir=None):
    """
    Plot the Aerosol_Backscattering_<X>nm variables from the L2_Data group.
    
    Args:
        dataset: NetCDF4 Dataset object
        time_index: Time index to plot (default: 0)
        output_dir: Directory to save the plot (default: same as input file)
    """
    # Check if L2_Data group exists
    if not check_group_exists(dataset, 'L2_Data'):
        return
    
    l2_group = dataset.groups['L2_Data']
    
    # Find all Aerosol_Backscattering variables
    aerosol_vars = find_aerosol_backscattering_variables(l2_group)
    
    if not aerosol_vars:
        print("Note: No Aerosol_Backscattering_<X>nm variables found in group 'L2_Data'.")
        return
    
    print(f"Found Aerosol_Backscattering variables: {aerosol_vars}")
    
    # Check if range variable exists
    if not check_variable_exists(l2_group, 'range'):
        return
    
    # Read the range data
    range_data = l2_group.variables['range'][:]
    
    # Plot each Aerosol_Backscattering variable
    for var_name in aerosol_vars:
        var = l2_group.variables[var_name]
        
        # Check dimensions
        if len(var.shape) != 3:
            continue
        
        # Get the data
        data = var[:]
        time_dim, lrs_dim, range_dim = data.shape
        
        # Validate time index
        if time_index < 0 or time_index >= time_dim:
            time_index = 0
        
        # Get the middle LRs index
        middle_lrs_index = lrs_dim // 2
        
        # Extract the slice: [time_index, middle_lrs_index, :]
        backscattering_profile = data[time_index, middle_lrs_index, :]
        
        # Get LRs value if available
        lrs_value = None
        if check_variable_exists(l2_group, 'LRs'):
            lrs_values = l2_group.variables['LRs'][:]
            if middle_lrs_index < len(lrs_values):
                lrs_value = lrs_values[middle_lrs_index]
        
        # Create the plot
        fig, ax = plt.subplots(figsize=(10, 8))
        
        # Plot
        ax.plot(backscattering_profile, range_data, linewidth=1.5, color='#D81159', alpha=0.8)
        
        # Get human-readable time
        human_time = f"Index {time_index}"
        if check_variable_exists(l2_group, 'Start_Time_AVG_L2'):
            unix_times = l2_group.variables['Start_Time_AVG_L2'][:]
            if time_index < len(unix_times):
                dt = unix_to_datetime(unix_times[time_index])
                human_time = dt.strftime('%Y-%m-%d %H:%M:%S UTC')
        
        # Title and Labels
        title = f'Aerosol Backscattering Profile'
        if lrs_value is not None:
            title += f'\nTime: {human_time}, LR: {lrs_value:.1f} sr'
        else:
            title += f'\nTime: {human_time}, LRs Index: {middle_lrs_index}'
        ax.set_title(title, fontsize=14, fontweight='bold')
        
        ax.set_xlabel(f"{var_name.replace('_', ' ')}", fontsize=12, fontweight='bold')
        ax.set_ylabel('Range (m)', fontsize=12, fontweight='bold')
        
        # Set range limit to 20000m
        ax.set_ylim(0, 20000)
        
        # Robust scaling
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
        
        # Grid lines
        ax.grid(True, which='both', linestyle='--', linewidth=0.5, alpha=0.7)
        ax.minorticks_on()
        
        plt.tight_layout()
        
        # Save the plot
        if output_dir is None:
            output_path = Path(f'{var_name}.png')
        else:
            output_path = Path(output_dir) / f'{var_name}.png'
        
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        print(f"Plot saved successfully: {output_path.absolute()}")
        
        # Close the figure
        plt.close(fig)


def plot_aerosol_backscattering_colormap(dataset, output_dir=None):
    """
    Plot the Aerosol_Backscattering_<X>nm variables from the L2_Data group as a ColorMap (Data level 2 plots).
    Follows the instruction of the variable "Cloud_Mask" in the group "L1_Data".
    
    Args:
        dataset: NetCDF4 Dataset object
        output_dir: Directory to save the plot
    """
    # Check if L2_Data group exists
    if not check_group_exists(dataset, 'L2_Data'):
        return
    
    l2_group = dataset.groups['L2_Data']
    
    # Find all Aerosol_Backscattering variables
    aerosol_vars = find_aerosol_backscattering_variables(l2_group)
    
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
        
        # Title: "In the title, put the time in human-readable format and the LRs selected"
        start_time_str = time_datetime[0].strftime('%Y-%m-%d %H:%M:%S UTC')
        ax.set_title(f'{var_name.replace("_", " ")} ColorMap\nStart Time: {start_time_str}, LR: {lrs_val}', 
                     fontsize=14, fontweight='bold')
        
        # Colorbar
        plt.colorbar(im, ax=ax, label='Backscattering Coefficient')
        
        ax.grid(True, alpha=0.3, linestyle='--')
        plt.tight_layout()
        
        # Save plot: Aerosol_Backscattering_<X>nm_ColorMap.png
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
        print("Warning: Group 'L2_Data' does not exist in the NetCDF file.")
        return
    
    l2_group = dataset.groups['L2_Data']
    
    # Check if variable exists
    if not check_variable_exists(l2_group, 'Range_Corrected_Lidar_Signal_L2'):
        print("Warning: Variable 'Range_Corrected_Lidar_Signal_L2' does not exist in group 'L2_Data'.")
        return
    
    # Check if range variable exists
    if not check_variable_exists(l2_group, 'range'):
        print("Warning: Variable 'range' does not exist in group 'L2_Data'.")
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

    # Create the plot
    fig, ax = plt.subplots(figsize=(10, 8))
    
    # Plot as a line plot (signal on x-axis, range on y-axis)
    ax.plot(signal_profile, range_data, linewidth=1.5, color='#D33F49', alpha=0.8)
    
    # Labels and title
    ax.set_ylabel('Range (m)', fontsize=12, fontweight='bold')
    ax.set_xlabel('Range Corrected Lidar Signal', fontsize=12, fontweight='bold')
    
    title = f'Range Corrected Lidar Signal (L2)'
    title += f'\nTime: {human_time}, Channel: {channel_index}'
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
    # ax.set_title(f'Range Corrected Lidar Signal ColorMap', fontsize=14, fontweight='bold')
    ax.set_title(f'Range Corrected Lidar Signal ColorMap - Start Time: {start_time_str}, Channel: {channel_index}', fontsize=14, fontweight='bold')
    # ax.set_title(f'Range Corrected Lidar Signal ColorMap\nStart Time: {start_time_str}, Channel: {channel_index}', fontsize=14, fontweight='bold')
    
    # Limit range to 20000 meters
    ax.set_ylim(0, 20000)
    
    # Add colorbar
    plt.colorbar(im, ax=ax, label='Signal Intensity')
    
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
    # Check if L2_Data group exists
    if not check_group_exists(dataset, 'L2_Data'):
        return

    l2_group = dataset.groups['L2_Data']

    # Check if AERONET_Data sub-group exists
    if 'AERONET_Data' not in l2_group.groups:
        print("Note: Sub-group 'AERONET_Data' not found in L2_Data. Skipping.")
        return

    aeronet_group = l2_group.groups['AERONET_Data']

    # Check if AERONET_Time exists
    if not check_variable_exists(aeronet_group, 'AERONET_Time'):
        print("Warning: Variable 'AERONET_Time' not found in AERONET_Data.")
        return

    # Find matching variables: AERONET_AOD_<X>nm_at_LidarTime
    aeronet_vars = []
    for var_name in aeronet_group.variables.keys():
        if var_name.startswith('AERONET_AOD_') and var_name.endswith('nm_at_LidarTime'):
            aeronet_vars.append(var_name)

    if not aeronet_vars:
        print("Note: No AERONET_AOD_<X>nm_at_LidarTime variables found.")
        return

    # Read time data
    # The variables end with '_at_LidarTime', which indicates they match the Lidar time dimension
    # (time_LIDAR), while 'AERONET_Time' matches the AERONET time dimension (time_AERONET).
    # We'll try to get the Lidar time for these specific variables.
    lidar_time = None
    if check_variable_exists(l2_group, 'Start_Time_AVG_L2'):
        lidar_time = l2_group.variables['Start_Time_AVG_L2'][:]
    
    # We also read AERONET_Time in case it's needed for other variables
    aeronet_time = aeronet_group.variables['AERONET_Time'][:]

    for var_name in aeronet_vars:
        data = aeronet_group.variables[var_name][:]
        
        # Determine the correct time vector based on dimensions
        var_obj = aeronet_group.variables[var_name]
        # Try to get AERONET_Time first for the x-axis variable
        # But the prompt says: "In the x-axis must be taken from the variable L2_Data/AERONET_Data/AERONET_Time"
        # However, it also says the variable "AERONET_AOD_..._at_LidarTime" must be plotted together with 
        # "L2_Data/LIDAR_AERONET_synergy/AOD_Lidar_532nm_vs_AERONET" and they have the same dimension.
        # Based on inspection, these variables match 'time_LIDAR' (Start_Time_AVG_L2).
        
        target_time = None
        if 'time_LIDAR' in var_obj.dimensions and lidar_time is not None:
            target_time = lidar_time
        elif 'time_AERONET' in var_obj.dimensions:
            target_time = aeronet_time
        else:
            # Match by shape
            if len(data) == len(lidar_time) if lidar_time is not None else -1:
                target_time = lidar_time
            elif len(data) == len(aeronet_time):
                target_time = aeronet_time
        
        if target_time is None:
            print(f"Warning: Could not find matching time dimension for {var_name}. Skipping.")
            continue

        # Convert target time to UTC datetime
        time_datetime = unix_to_datetime(target_time)
        
        # Check for synergy variables: 
        # 1. AOD Lidar: L2_Data/LIDAR_AERONET_synergy/AOD_Lidar_532nm_vs_AERONET
        # 2. LR Lidar: L2_Data/LIDAR_AERONET_synergy/LR_inv_532nm
        synergy_aod_data = None
        synergy_lr_data = None
        wavelength = var_name.replace('AERONET_AOD_', '').replace('nm_at_LidarTime', '')
        synergy_aod_var_name = f'AOD_Lidar_{wavelength}nm_vs_AERONET'
        synergy_lr_var_name = f'LR_inv_{wavelength}nm'
        
        if 'LIDAR_AERONET_synergy' in l2_group.groups:
            synergy_group = l2_group.groups['LIDAR_AERONET_synergy']
            if synergy_aod_var_name in synergy_group.variables:
                synergy_aod_data = synergy_group.variables[synergy_aod_var_name][:]
            if synergy_lr_var_name in synergy_group.variables:
                synergy_lr_data = synergy_group.variables[synergy_lr_var_name][:]
        
        # Filter negative values: "If the value of the variable is negative, do not plot the point."
        # Filter main data, synergy AOD, and synergy LR.
        
        valid_mask = (data >= 0)
        if synergy_aod_data is not None:
             valid_mask &= (synergy_aod_data >= 0)
        if synergy_lr_data is not None:
             valid_mask &= (synergy_lr_data >= 0)
        
        filtered_time = time_datetime[valid_mask]
        filtered_aeronet_aod = data[valid_mask]
        filtered_lidar_aod = synergy_aod_data[valid_mask] if synergy_aod_data is not None else None
        filtered_lidar_lr = synergy_lr_data[valid_mask] if synergy_lr_data is not None else None

        if len(filtered_time) == 0:
            print(f"Note: No valid (positive) points found for {var_name}. Skipping plot.")
            continue

        # Find Angstrom Exponent variables
        angstrom_vars = [v for v in aeronet_group.variables.keys() if v.startswith('AERONET_Angstrom_Exponent_')]
        angstrom_data = None
        angstrom_var_name = ""
        if angstrom_vars:
            angstrom_var_name = angstrom_vars[0]
            angstrom_data = aeronet_group.variables[angstrom_var_name][:]
            angstrom_suffix = angstrom_var_name.replace('AERONET_Angstrom_Exponent_', '')
        else:
            angstrom_suffix = "X-Y"

        # Create the plot
        # We want 3 subplots if both LR and Angstrom are available
        num_subplots = 1
        if filtered_lidar_lr is not None: num_subplots += 1
        if angstrom_data is not None: num_subplots += 1

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
        ax1.plot(filtered_time, filtered_aeronet_aod, 'o', linewidth=1.5, color='#E67E22', label=f'AERONET AOD {wavelength}nm', markersize=6)
        if filtered_lidar_aod is not None:
            ax1.plot(filtered_time, filtered_lidar_aod, 'x', linewidth=1.5, color='#2980B9', label=f'Lidar AOD {wavelength}nm', markersize=6)
        
        ax1.legend(loc='best')
        ax1.set_ylabel(f'AOD at {wavelength}nm', fontsize=12, fontweight='bold')
        date_str = filtered_time[0].strftime('%Y-%m-%d')
        ax1.set_title(f'Synergy Plot at {wavelength}nm - {date_str} (UTC)', fontsize=14, fontweight='bold')
        
        # Plot LR on ax2 if available
        if ax2 is not None:
            ax2.plot(filtered_time, filtered_lidar_lr, 'd', linewidth=1.5, color='#27AE60', label=f'Lidar Ratio {wavelength}nm', markersize=6)
            ax2.set_ylabel(f'LR (sr) at {wavelength}nm', fontsize=12, fontweight='bold')
            ax2.legend(loc='best')
            
        # Plot Angstrom Exponent on ax3 if available
        if ax3 is not None:
            # Use aeronet_time and filtered data if necessary, but prompt says:
            # "x-axis must be taken from the variable L2_Data/AERONET_Data/AERONET_Time"
            aeronet_time_dt = unix_to_datetime(aeronet_time)
            # Filter negative values for Angstrom as well? Prompt doesn't say, but usually good.
            # Let's keep it simple as per prompt.
            ax3.plot(aeronet_time_dt, angstrom_data, 's', linewidth=1.5, color='#8E44AD', label=f'Angstrom Exp {angstrom_suffix}', markersize=6)
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
        filename = f'AERONET_vs_LIDAR_AOD_{wavelength}nm_LRsSinergy_Angstrom_Exponent_{angstrom_suffix}_at_LidarTime.png'
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
    ax.set_title(f'AOD for different Lidar Ratios (LRs) - {date_str} (UTC)', fontsize=14, fontweight='bold')
    
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
            
            # Plot Aerosol_Extinction
            plot_aerosol_extinction(dataset, args.time_index, args.output_dir)
            
            # Plot Aerosol_Backscattering
            plot_aerosol_backscattering(dataset, args.time_index, args.output_dir)
            
            # Plot Aerosol_Backscattering ColorMap
            plot_aerosol_backscattering_colormap(dataset, args.output_dir)
            
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
