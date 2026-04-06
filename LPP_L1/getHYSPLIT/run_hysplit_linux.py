import os
import subprocess
import datetime
import sys
import argparse
import urllib.request
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import cartopy.crs as ccrs
import cartopy.feature as cfeature
import numpy as np
from scipy.interpolate import interp1d

# python3 run_hysplit_linux.py --date 2025-11-17-12 --lat -34.56 --lon -58.41 --alt 5000 --hours -24 --res 7.5 --hysplit_exec ~/hysplit/exec/hyts_std --profile_exec ~/hysplit/exec/profile --output_path ./figures

# --- CONFIGURATION ---
# THESE VARIABLES CAN BE PASSED AS ARGUMENTS - START
PATH_HYSPLIT_EXEC = "~/hysplit/exec/hyts_std"
PATH_PROFILE_EXEC = "~/hysplit/exec/profile"
PATH_MET_DIR = "~/hysplit/metdata"
PATH_OUTPUT = "./figures"
# Simulation Parameters -- THESE VARIABLES CAN BE PASSED AS ARGUMENTS
SIMULATION_DATE = datetime.datetime(2025, 11, 17, 12) 
START_LAT = -34.56
START_LON = -58.41
ALTITUDE = 5000.0    # Altitude (meters AGL)
PROFILE_RES = 7.5    # Resolution for interpolated profile (meters)
RUN_HOURS = "-24"    # Duration. Negative for backward run
# THESE VARIABLES CAN BE PASSED AS ARGUMENTS - END

HYSPLIT_EXEC = os.path.expanduser(PATH_HYSPLIT_EXEC) 
PROFILE_EXEC = os.path.expanduser(PATH_PROFILE_EXEC)
MET_DIR = os.path.expanduser(PATH_MET_DIR)
WORKING_DIR = os.getcwd()

# Output configuration
OUTPUT_PATH = os.path.join(WORKING_DIR, PATH_OUTPUT)
OUTPUT_FILENAME = "tdump_output"
PLOT_FILENAME = "trajectory_plot.png"
MET_PLOT_FILENAME = "met_profile_plot.png"
PROFILE_HIGH_RES_FILENAME = "Temp_Press_profile_high_res.csv"

# Ensure output directory exists
os.makedirs(OUTPUT_PATH, exist_ok=True)

# ---------------------

def get_current_met_info():
    """Calculates the correct GDAS filename for the simulation date."""
    day = SIMULATION_DATE.day
    week = min((day - 1) // 7 + 1, 5)
    mon_str = SIMULATION_DATE.strftime("%b").lower()
    year_str = SIMULATION_DATE.strftime("%y")
    return f"gdas1.{mon_str}{year_str}.w{week}"

def download_weather_data(filename):
    """Downloads weather file from NOAA if missing."""
    os.makedirs(MET_DIR, exist_ok=True)
    file_path = os.path.join(MET_DIR, filename)
    
    if os.path.exists(file_path):
        print(f"Weather data found: {filename}")
        return file_path
    
    url = f"https://www.ready.noaa.gov/data/archives/gdas1/{filename}"
    print(f"Downloading {filename} from NOAA... (approx 500MB)")
    
    def download_progress_hook(count, block_size, total_size):
        percent = int(count * block_size * 100 / total_size)
        sys.stdout.write(f"\rDownloading: {percent}%")
        sys.stdout.flush()

    try:
        urllib.request.urlretrieve(url, file_path, reporthook=download_progress_hook)
        print() 
        
        # Validation
        if os.path.getsize(file_path) < 10000:
            with open(file_path, 'r', errors='ignore') as f:
                head = f.read(100)
                if "<html" in head.lower() or "<!doctype" in head.lower():
                    os.remove(file_path)
                    raise ValueError(f"Downloaded file is HTML (404 Not Found). URL: {url}")

        print("Download complete.")
        return file_path
    except Exception as e:
        print(f"\nError downloading data: {e}")
        sys.exit(1)

def setup_hysplit_config():
    """Ensures ASCDATA.CFG is present in the working directory via symlink."""
    asc_cfg = os.path.join(WORKING_DIR, "ASCDATA.CFG")
    if os.path.exists(asc_cfg):
        return

    exec_dir = os.path.dirname(HYSPLIT_EXEC)
    hysplit_root = os.path.dirname(exec_dir)
    asc_source = os.path.join(hysplit_root, "bdyfiles", "ASCDATA.CFG")
    
    if os.path.exists(asc_source):
        try:
            os.symlink(asc_source, asc_cfg)
            print(f"Created symlink to ASCDATA.CFG")
        except Exception as e:
            print(f"Warning: Could not create symlink for ASCDATA.CFG: {e}")
    else:
        print(f"Warning: ASCDATA.CFG not found at {asc_source}.")

def create_control_file(met_file_path):
    """Creates the CONTROL file for HYSPLIT."""
    start_time = SIMULATION_DATE.strftime("%y %m %d %H")
    met_dir = os.path.dirname(met_file_path) + "/"
    met_file = os.path.basename(met_file_path)

    control = f"""{start_time}
1
{START_LAT} {START_LON} {ALTITUDE}
{RUN_HOURS}
0
10000.0
1
{met_dir}
{met_file}
{WORKING_DIR}/
{OUTPUT_FILENAME}
"""
    with open("CONTROL", "w") as f:
        f.write(control)
    print("CONTROL file created.")

def run_model():
    """Executes the HYSPLIT model."""
    if not os.path.exists(HYSPLIT_EXEC):
        print(f"Error: Executable not found at {HYSPLIT_EXEC}")
        return

    subprocess.run(["chmod", "+x", HYSPLIT_EXEC])
    print(f"Running HYSPLIT...")
    
    try:
        result = subprocess.run([HYSPLIT_EXEC], cwd=WORKING_DIR, capture_output=True, text=True)
        if result.returncode == 0:
            print(f"Success! Model finished.")
        else:
            print("Error running model:", result.stderr)
    except Exception as e:
        print(f"Execution failed: {e}")

def extract_met_profile():
    """Extracts meteorological profile using the HYSPLIT profile utility."""
    if not os.path.exists(PROFILE_EXEC):
        print(f"Warning: Profile executable not found at {PROFILE_EXEC}. Skipping profile extraction.")
        return

    # GDAS files start at day 1, 8, 15, or 22 at 00:00
    day = SIMULATION_DATE.day
    start_day = 22 if day >= 22 else 15 if day >= 15 else 8 if day >= 8 else 1
    start_of_file = SIMULATION_DATE.replace(day=start_day, hour=0, minute=0, second=0, microsecond=0)
    offset_hrs = int((SIMULATION_DATE - start_of_file).total_seconds() / 3600)

    met_file = get_current_met_info()
    print(f"Extracting profile for {SIMULATION_DATE} (Offset: {offset_hrs}h) at {START_LAT}, {START_LON}...")
    
    subprocess.run(["chmod", "+x", PROFILE_EXEC])
    cmd = [PROFILE_EXEC, f"-d{MET_DIR}/", f"-f{met_file}", f"-y{START_LAT}", f"-x{START_LON}", f"-o{offset_hrs}"]
    
    try:
        result = subprocess.run(cmd, cwd=WORKING_DIR, capture_output=True, text=True)
        if result.returncode == 0:
            if os.path.exists("profile.txt"):
                print("Profile extraction successful.")
            else:
                print("Warning: profile.txt not generated.")
        else:
            print(f"Error running profile utility: {result.stderr}")
    except Exception as e:
        print(f"Profile extraction failed: {e}")

def plot_met_profile():
    """Parses, interpolates, saves, and plots the meteorological profile."""
    if not os.path.exists("profile.txt"):
        print("No profile.txt found to plot.")
        return

    pressures, heights, temps = [], [], []
    try:
        with open("profile.txt", 'r') as f:
            start_3d = False
            for line in f:
                if "3D Fields" in line:
                    start_3d = True
                    continue
                if start_3d:
                    parts = line.split()
                    if not parts or parts[0] in ["HGTS", "m"]: continue
                    try:
                        pressures.append(float(parts[0]))
                        heights.append(float(parts[1]))
                        temps.append(float(parts[2]))
                    except (ValueError, IndexError): continue
    except Exception as e:
        print(f"Error parsing profile: {e}"); return

    if not temps: return

    # Interpolation to high resolution
    h_min, h_max = min(heights), max(heights)
    h_grid = np.arange(h_min, h_max, PROFILE_RES)
    
    f_temp = interp1d(heights, [t + 273.15 for t in temps], kind='linear', fill_value="extrapolate")
    f_log_p = interp1d(heights, np.log([p * 100 for p in pressures]), kind='linear', fill_value="extrapolate")
    
    t_high = f_temp(h_grid)
    p_high = np.exp(f_log_p(h_grid))

    # Save CSV
    csv_path = os.path.join(OUTPUT_PATH, PROFILE_HIGH_RES_FILENAME)
    np.savetxt(csv_path, np.column_stack((h_grid, t_high, p_high)), fmt='%.2f', delimiter=',')
    print(f"High-resolution profile saved to: {csv_path}")

    # Plotting
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 6))
    ax1.plot(t_high, h_grid, 'b-', label=f'High-Res ({PROFILE_RES}m)')
    ax1.set(title="Temperature vs Height", xlabel="Temperature (K)", ylabel="Height (m)")
    ax1.grid(True, linestyle='--', alpha=0.6); ax1.legend()

    ax2.plot(p_high, h_grid, 'r-', label=f'High-Res ({PROFILE_RES}m)')
    ax2.set(title="Pressure vs Height", xlabel="Pressure (Pa)", ylabel="Height (m)", xscale='log')
    ax2.grid(True, linestyle='--', alpha=0.6); ax2.legend()
    
    from matplotlib.ticker import ScalarFormatter
    ax2.xaxis.set_major_formatter(ScalarFormatter())

    plt.tight_layout()
    plt.savefig(os.path.join(OUTPUT_PATH, MET_PLOT_FILENAME))
    print(f"Profile plot saved to: {OUTPUT_PATH}/{MET_PLOT_FILENAME}")

    if os.path.exists("profile.txt"): os.remove("profile.txt")

def plot_trajectory():
    """Reads tdump and creates trajectory plots."""
    tdump = os.path.join(WORKING_DIR, OUTPUT_FILENAME)
    if not os.path.exists(tdump): return

    lats, lons, heights, times = [], [], [], []
    try:
        with open(tdump, 'r') as f:
            for line in f:
                p = line.split()
                if len(p) > 11 and p[0] == '1':
                    try:
                        y = int(p[2]) + 2000 if int(p[2]) < 100 else int(p[2])
                        times.append(datetime.datetime(y, int(p[3]), int(p[4]), int(p[5])))
                        lats.append(float(p[9])); lons.append(float(p[10])); heights.append(float(p[11]))
                    except ValueError: continue
    except Exception as e: print(f"Error reading tdump: {e}"); return

    if not lats: return
    data = sorted(zip(times, lats, lons, heights), key=lambda x: x[0])
    times, lats, lons, heights = zip(*data)

    fig = plt.figure(figsize=(10, 10))
    # Map Plot
    ax1 = fig.add_subplot(2, 1, 1, projection=ccrs.PlateCarree())
    ax1.add_feature(cfeature.LAND, facecolor='lightgray')
    ax1.add_feature(cfeature.OCEAN, facecolor='lightblue')
    ax1.add_feature(cfeature.COASTLINE); ax1.add_feature(cfeature.BORDERS, linestyle=':')
    ax1.plot(lons, lats, '-o', markersize=3, color='blue', transform=ccrs.PlateCarree())
    ax1.plot(lons[0], lats[0], 'g^', markersize=10, label='Start', transform=ccrs.PlateCarree())
    ax1.plot(lons[-1], lats[-1], 'rs', markersize=10, label='End', transform=ccrs.PlateCarree())
    ax1.set_title(f"HYSPLIT Trajectory ({RUN_HOURS} hours)")
    gl = ax1.gridlines(draw_labels=True, linestyle='--', alpha=0.6)
    gl.top_labels = gl.right_labels = False
    m = 5.0
    ax1.set_extent([min(lons)-m, max(lons)+m, min(lats)-m, max(lats)+m], crs=ccrs.PlateCarree())
    ax1.legend()

    # Altitude Plot
    ax2 = fig.add_subplot(2, 1, 2)
    ax2.plot(times, heights, '-o', markersize=3, color='purple')
    ax2.set(title="Vertical Motion", xlabel="Time", ylabel="Height AGL (m)")
    ax2.grid(True, linestyle='--', alpha=0.6)
    ax2.xaxis.set_major_formatter(mdates.DateFormatter('%m/%d - %H'))
    plt.setp(ax2.get_xticklabels(), rotation=45, ha="right")
    ax2.invert_xaxis()
    
    plt.tight_layout()
    plt.savefig(os.path.join(OUTPUT_PATH, PLOT_FILENAME))
    print(f"Trajectory plot saved to: {OUTPUT_PATH}/{PLOT_FILENAME}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Run HYSPLIT trajectory and profile.')
    parser.add_argument('--date', type=str, default=SIMULATION_DATE.strftime("%Y-%m-%d-%H"),
                        help=f'Simulation date (YYYY-MM-DD-HH). Default: {SIMULATION_DATE.strftime("%Y-%m-%d-%H")}')
    parser.add_argument('--lat', type=float, default=START_LAT, help=f'Starting latitude. Default: {START_LAT}')
    parser.add_argument('--lon', type=float, default=START_LON, help=f'Starting longitude. Default: {START_LON}')
    parser.add_argument('--alt', type=float, default=ALTITUDE, help=f'Starting altitude (m AGL). Default: {ALTITUDE}')
    parser.add_argument('--hours', type=str, default=RUN_HOURS, help=f'Run duration (hours). Default: {RUN_HOURS}')
    parser.add_argument('--res', type=float, default=PROFILE_RES, help=f'Profile resolution (m). Default: {PROFILE_RES}')
    parser.add_argument('--hysplit_exec', type=str, default=PATH_HYSPLIT_EXEC, help=f'HYSPLIT executable path. Default: {PATH_HYSPLIT_EXEC}')
    parser.add_argument('--profile_exec', type=str, default=PATH_PROFILE_EXEC, help=f'Profile utility path. Default: {PATH_PROFILE_EXEC}')
    parser.add_argument('--met_dir', type=str, default=PATH_MET_DIR, help=f'Meteorological data directory. Default: {PATH_MET_DIR}')
    parser.add_argument('--output_path', type=str, default=PATH_OUTPUT, help=f'Output directory for figures and data. Default: {PATH_OUTPUT}')
    
    args = parser.parse_args()

    # Update global variables with parsed arguments
    SIMULATION_DATE = datetime.datetime.strptime(args.date, "%Y-%m-%d-%H")
    START_LAT = args.lat
    START_LON = args.lon
    ALTITUDE = args.alt
    RUN_HOURS = args.hours
    PROFILE_RES = args.res
    HYSPLIT_EXEC = os.path.expanduser(args.hysplit_exec)
    PROFILE_EXEC = os.path.expanduser(args.profile_exec)
    MET_DIR = os.path.expanduser(args.met_dir)
    # If output_path is relative, join it with WORKING_DIR; otherwise use as is. Expand ~ if present.
    cli_output_path = os.path.expanduser(args.output_path)
    if not os.path.isabs(cli_output_path):
        OUTPUT_PATH = os.path.join(WORKING_DIR, cli_output_path)
    else:
        OUTPUT_PATH = cli_output_path
    
    # Re-ensure output directory exists in case it was changed via CLI
    os.makedirs(OUTPUT_PATH, exist_ok=True)

    setup_hysplit_config()
    met_file = get_current_met_info()
    full_met_path = download_weather_data(met_file)
    create_control_file(full_met_path)
    run_model()
    plot_trajectory()
    extract_met_profile()
    plot_met_profile()