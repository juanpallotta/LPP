# NetCDF Cloud Mask Plotter

This Python script reads NetCDF files and plots the Cloud_Mask data from the L1_Data group.

## Features

- ✅ Reads NetCDF files with group structure support
- ✅ Checks for group and variable existence before processing
- ✅ Converts Unix timestamps to human-readable datetime format
- ✅ Plots Cloud_Mask as a 2D image with proper axes
- ✅ Uses appropriate colormap to highlight clouds vs clear sky
- ✅ Saves high-quality PNG output (300 DPI)

## Installation

Install the required dependencies:

```bash
pip install -r requirements.txt
```

## Usage

### Basic Usage

```bash
python plot_netcdf.py <path_to_netcdf_file>
```

### Example

```bash
python plot_netcdf.py A_L0_L1_L2.nc
```

### Advanced Usage

Specify an output directory for the plot:

```bash
python plot_netcdf.py A_L0_L1_L2.nc -o output_directory
```

### Help

View all available options:

```bash
python plot_netcdf.py --help
```

## Output

The script generates a file named `Cloud_Mask.png` containing:

- **X-axis**: Time (converted from Unix timestamp to human-readable format)
- **Y-axis**: Range in meters
- **Color**: Binary cloud mask (white = clear sky, blue = cloud)
- **Resolution**: 300 DPI for publication-quality output

## Data Requirements

The NetCDF file must contain:

- A group named `L1_Data`
- Within `L1_Data`:
  - `Cloud_Mask`: 2D matrix with binary values (0 = clear sky, 1 = cloud)
  - `Start_Time_AVG_L1`: 1D vector with Unix timestamps
  - `range`: 1D vector with range values in meters

## Error Handling

The script includes robust error handling:

- Checks if the input file exists
- Verifies group existence before accessing
- Validates variable presence before reading
- Provides informative error messages

## Example Output

The generated plot shows cloud detection over time and altitude, with:
- Clear sky areas in white
- Cloud areas in blue
- Time axis with formatted dates
- Range axis in meters
- Color legend for interpretation
