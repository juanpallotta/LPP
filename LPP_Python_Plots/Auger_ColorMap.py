#!/usr/bin/env python3
"""
Auger_ColorMap.py
Updated script according to prompt_Auger.md instructions.
Processes L1 and L2 lidar data from NetCDF files.
"""

import sys
import os
import argparse
import numpy as np
import netCDF4 as nc
import matplotlib.pyplot as plt
import re
from datetime import datetime, timezone
from matplotlib.colors import ListedColormap, LogNorm

def unix_to_datetime(t_unix):
    """Convert Unix timestamp to UTC datetime string."""
    try:
        # Handle potential masked arrays or scalars
        t = float(t_unix)
        dt = datetime.fromtimestamp(t, tz=timezone.utc)
        return dt.strftime('%Y-%m-%d %H:%M:%S UTC')
    except Exception:
        return "Unknown Time"

def ensure_dir(path):
    """Ensure directory exists."""
    if not os.path.exists(path):
        os.makedirs(path)

def extract_metadata(filename):
    """Extract site, date, time, and run number from filename."""
    basename = os.path.basename(filename)
    parts = basename.split('-')
    
    # Defaults
    site = "unknown_site"
    date = "unknown_date"
    time = "unknown_time"
    run = "unknown_run"
    
    # Following pattern: lidar-<site>-<YYYYMMDD>-<HHMMSS>-<RX>.root
    if len(parts) >= 5 and parts[0] == "lidar":
        site = parts[1]
        date = parts[2]
        time = parts[3] # HHMMSS
        # parts[4] contains <RX>.root
        run_part = parts[4]
        if ".root" in run_part:
            run = run_part.split(".root")[0]
        else:
            run = run_part
    else:
        # Fallback if pattern doesn't match exactly
        date_match = re.search(r'(\d{8})', basename)
        if date_match:
            date = date_match.group(1)
        
        # Look for a 6-digit time (HHMMSS) that isn't part of the 8-digit date
        all_6_digits = re.findall(r'(\d{6})', basename)
        for d in all_6_digits:
            if d not in date:
                time = d
                break
        
        for s in ["ch", "la", "ll", "lm"]:
            if f"-{s}-" in basename.lower() or basename.lower().startswith(f"{s}_"):
                site = s
                break
        
        run_match = re.search(r'(R\d+)', basename)
        if run_match:
            run = run_match.group(1)
            
    return site, date, time, run

def get_edges(centers):
    """Calculate edges for pcolormesh from centers."""
    if len(centers) < 2:
        val = centers[0]
        return np.array([val - 0.5, val + 0.5])
    edges = np.zeros(len(centers) + 1)
    edges[1:-1] = (centers[:-1] + centers[1:]) / 2
    edges[0] = centers[0] - (edges[1] - centers[0])
    edges[-1] = centers[-1] + (centers[-1] - edges[-2])
    return edges

def plot_fan_scan(ax, r, angles, data, title, ylabel, xlabel='Horizontal Distance [km]', cmap='jet', vmin=None, vmax=None, is_binary=False):
    """
    Creates a fan-shaped plot in the provided axis.
    """
    # Mirroring logic from v0 example
    angles_mod = angles.copy()
    idx_min = np.argmin(angles_mod)
    if idx_min > 0:
        angles_mod[0:(idx_min+1)] = -1.0 * angles_mod[0:(idx_min+1)]
    
    zen_rad = np.radians(angles_mod)
    z_edges = get_edges(zen_rad)
    r_edges = get_edges(r)
    r_edges[0] = max(0, r_edges[0])
    
    ZE, RE = np.meshgrid(z_edges, r_edges, indexing='ij')
    XX = RE * np.sin(ZE)
    YY = RE * np.cos(ZE)
    
    if is_binary:
        # Custom binary colormap for Cloud Mask
        my_cmap = ListedColormap(['#1B263B', '#FFD700']) # Navy, Gold
        pc = ax.pcolormesh(XX, YY, data, cmap=my_cmap, vmin=0, vmax=1, shading='flat')
        cbar = plt.colorbar(pc, ax=ax, ticks=[0.25, 0.75])
        cbar.ax.set_yticklabels(['Clear Sky', 'Cloud'])
    else:
        # Standard signal colormap
        pc = ax.pcolormesh(XX, YY, data, cmap=cmap, vmin=vmin, vmax=vmax, shading='flat')
        plt.colorbar(pc, ax=ax, label='Signal Intensity')

    ax.set_title(title, fontsize=16, fontweight='bold')
    ax.set_xlabel(xlabel, fontsize=12)
    ax.set_ylabel(ylabel, fontsize=12)
    ax.set_aspect('equal')
    ax.set_ylim(0, 20) # Max 20km
    ax.grid(True, linestyle='--', alpha=0.3)

def process_file(nc_file, out_dir, time_idx):
    ensure_dir(out_dir)
    site, date, time, run = extract_metadata(nc_file)
    file_prefix = f"{site}_{date}_{time}_{run}_"
    
    with nc.Dataset(nc_file, 'r') as ds:
        # --- Level 1 Data ---
        if 'L1_Data' in ds.groups:
            l1 = ds.groups['L1_Data']
            res = ds.getncattr('Range_Resolution')
            
            if 'Cloud_Mask' in l1.variables:
                cm = l1.variables['Cloud_Mask'][:] # (time, range)
                zenith_l1 = l1.variables['Zenith_AVG_L1'][:] # (time)
                
                # Limit range to 20000m
                n_bins = cm.shape[1]
                bin_limit = min(n_bins, int(round(20000 / res)))
                cm_plot = cm[:, :bin_limit]
                r_km = np.arange(bin_limit) * res / 1000.0
                
                fig, ax = plt.subplots(figsize=(10, 8))
                plot_fan_scan(ax, r_km, zenith_l1, cm_plot, 'Cloud Mask', 'Altitude [km]', is_binary=True)
                
                out_path = os.path.join(out_dir, f'{file_prefix}Cloud_Mask.png')
                plt.savefig(out_path, dpi=300, bbox_inches='tight')
                plt.close()
                print(f"Saved: {out_path}")

        # --- Level 2 Data ---
        if 'L2_Data' in ds.groups:
            l2 = ds.groups['L2_Data']
            r_l2 = l2.variables['range'][:] / 1000.0 # Range in km
            bin_limit_l2 = np.searchsorted(r_l2, 20.0, side='right')
            r_l2_plot = r_l2[:bin_limit_l2]
            
            # 1. RCLS L2
            if 'Range_Corrected_Lidar_Signal_L2' in l2.variables:
                rcls_l2 = l2.variables['Range_Corrected_Lidar_Signal_L2'][:] # (time, channels, range)
                try:
                    chan_idx = int(l2.getncattr('indxChannel_for_Fernald_inv'))
                except:
                    chan_idx = 0
                
                data_rcls = rcls_l2[:, chan_idx, :bin_limit_l2]
                times_l2 = l2.variables['Start_Time_AVG_L2'][:]
                dt_list = [unix_to_datetime(t) for t in times_l2]
                
                v_min = np.nanpercentile(data_rcls, 5)
                v_max = np.nanpercentile(data_rcls, 95)

                # B. Range_Corrected_Lidar_Signal_L2_ColorMap.png (Fan Scan)
                if 'Zenith_AVG_L2' in l2.variables:
                    zenith_l2 = l2.variables['Zenith_AVG_L2'][:]
                    fig, ax = plt.subplots(figsize=(10, 8))
                    t_str = dt_list[0] if len(dt_list)>0 else ""
                    plot_fan_scan(ax, r_l2_plot, zenith_l2, data_rcls, 
                                  f'RCLS L2 ColorMap - {t_str}\nChannel {chan_idx}', 'Range [km]', 
                                  vmin=v_min, vmax=v_max)
                    out_path = os.path.join(out_dir, f'{file_prefix}Range_Corrected_Lidar_Signal_L2_ColorMap.png')
                    plt.savefig(out_path, dpi=300, bbox_inches='tight')
                    plt.close()
                    print(f"Saved: {out_path}")

                # C. Range_Corrected_Lidar_Signal_L2_Time_Slice.png (1D Profile)
                idx = min(time_idx, len(times_l2)-1)
                fig, ax = plt.subplots(figsize=(6, 8))
                ax.plot(data_rcls[idx, :], r_l2_plot)
                ax.set_title(f'RCLS Profile - {dt_list[idx]}\nChannel {chan_idx}', fontsize=12)
                ax.set_xlabel('Signal')
                ax.set_ylabel('Range [km]')
                ax.grid(True, linestyle='--', alpha=0.3)
                out_path = os.path.join(out_dir, f'{file_prefix}Range_Corrected_Lidar_Signal_L2_Time_Slice.png')
                plt.savefig(out_path, dpi=300, bbox_inches='tight')
                plt.close()
                print(f"Saved: {out_path}")

            # 2. Aerosol Backscattering and Extinction
            # Iterate through available wavelengths
            for var_name in l2.variables:
                if 'Aerosol_Backscatter' in var_name or 'Aerosol_Extinction' in var_name:
                    data = l2.variables[var_name][:] # (time, LRs, range)
                    unit = l2.variables[var_name].units if hasattr(l2.variables[var_name], 'units') else ""
                    wl = var_name.split('_')[-1] # e.g. 532nm
                    
                    time_dim, lrs_dim, range_dim = data.shape
                    lr_middle_idx = lrs_dim // 2
                    
                    idx = min(time_idx, time_dim - 1)
                    profile = data[idx, lr_middle_idx, :bin_limit_l2]
                    
                    # 1D Profile plot
                    fig, ax = plt.subplots(figsize=(6, 8))
                    ax.plot(profile, r_l2_plot)
                    ax.set_title(f'{var_name}\n{dt_list[idx]} (LR index {lr_middle_idx})', fontsize=12)
                    ax.set_xlabel(f'Value [{unit}]')
                    ax.set_ylabel('Range [km]')
                    ax.grid(True, linestyle='--', alpha=0.3)
                    
                    # Construct fixed filenames based on prompt if possible, or generic
                    save_name = f"{var_name}.png"
                    if wl == "532nm":
                        save_name = f"{var_name.split('_')[1]}_532nm.png"
                    
                    out_path = os.path.join(out_dir, f"{file_prefix}{save_name}")
                    plt.savefig(out_path, dpi=300, bbox_inches='tight')
                    plt.close()
                    print(f"Saved: {out_path}")
                    
                    # Colormap Fan Scan for Backscattering
                    if 'Aerosol_Backscatter' in var_name and 'Zenith_AVG_L2' in l2.variables:
                        zenith_l2 = l2.variables['Zenith_AVG_L2'][:]
                        data_slice = data[:, lr_middle_idx, :bin_limit_l2]
                        
                        fig, ax = plt.subplots(figsize=(10, 8))
                        t_str = dt_list[0] if len(dt_list)>0 else ""
                        v_min = np.nanpercentile(data_slice, 5)
                        v_max = np.nanpercentile(data_slice, 95)
                        
                        plot_fan_scan(ax, r_l2_plot, zenith_l2, data_slice, 
                                      f'{var_name} ColorMap - {t_str}\nLR index {lr_middle_idx}', 
                                      'Range [km]', vmin=v_min, vmax=v_max)
                        
                        out_path = os.path.join(out_dir, f"{file_prefix}{var_name}_ColorMap.png")
                        plt.savefig(out_path, dpi=300, bbox_inches='tight')
                        plt.close()
                        print(f"Saved: {out_path}")

def main():
    parser = argparse.ArgumentParser(description='Auger NetCDF Plotter')
    parser.add_argument('nc_file', help='Path to NetCDF file')
    parser.add_argument('out_dir', nargs='?', default='LPP_OUT', help='Output directory')
    parser.add_argument('time_idx', type=int, nargs='?', default=0, help='Time slice index for 1D plots')
    
    args = parser.parse_args()
    
    if not os.path.exists(args.nc_file):
        print(f"Error: File {args.nc_file} not found.")
        sys.exit(1)
        
    process_file(args.nc_file, args.out_dir, args.time_idx)

if __name__ == '__main__':
    main()
