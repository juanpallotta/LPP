# GetRadiosounding

This tool allows you to retrieve atmospheric sounding data directly from the University of Wyoming database. It has been enhanced to support retrieving data for **multiple stations** and **multiple date ranges** simultaneously.

## Prerequisites

- Python 3
- Virtual Environment (`venv` module)

## Installation

A bash script `install.sh` is provided to set up a Python virtual environment and install the required dependencies automatically.

```bash
chmod +x install.sh
./install.sh
```

## Usage

After installation, the script uses the created virtual environment to run. You can run `get_UWyoming_snd.py` via the provided `execute.sh` wrapper, which automatically handles activating the virtual environment.

```bash
./execute.sh [options]
```

Alternatively, you can activate the environment and run python manually:

```bash
source venv/bin/activate
python get_UWyoming_snd.py [options]
```

### Options
- `-y`, `--year`: Year(s) to retrieve data from (comma-separated).
- `-m`, `--month`: Month(s) to retrieve data from (comma-separated).
- `-i`, `--InitialDate`: Initial date(s) of the period to retrieve from [DD][HH] (comma-separated).
- `-e`, `--EndDate`: End date(s) of the period to retrieve from [DD][HH] (comma-separated).
- `-r`, `--Region`: Region code (e.g., `samer`, `europe`, `mideast`).
- `-s`, `--station`: Station number(s) (comma-separated).
- `-f`, `--station-file`: Text file containing stations to retrieve (e.g., `Airports_with_ACARS_Soundings.dat`).
- `-o`, `--output`: Output folder for CSV files (default: `./radiosondes`).

### Examples

**Example 1: Single station and single date range**

Using the script directly:

```bash
python get_UWyoming_snd.py -y 2021 -m 9 -i 0100 -e 0200 -r samer -s 87576
```

or using the wrapper (recommended):

```bash
./execute.sh -y 2021 -m 9 -i 0100 -e 0200 -r samer -s 87576
```

**Example 2: Multiple stations**

Using the script directly:

```bash
python get_UWyoming_snd.py -y 2021 -m 9 -i 0100 -e 0200 -r samer -s 87576,87155
```

or using the wrapper (recommended):

```bash
./execute.sh -y 2021 -m 9 -i 0100 -e 0200 -r samer -s 87576,87155
```

**Example 3: Multiple date ranges**
Provide the same number of comma-separated items for the dates:

Using the script directly:

```bash
python get_UWyoming_snd.py -y 2021,2022 -m 9,10 -i 0100,0500 -e 0200,0600 -r samer -s 87576
```

or using the wrapper (recommended):

```bash
./execute.sh -y 2021,2022 -m 9,10 -i 0100,0500 -e 0200,0600 -r samer -s 87576
```

**Example 4: Using a station file**
Read WMO IDs from a TSV/CSV format file such as the updated `Airports_with_ACARS_Soundings.dat`:

Using the script directly:

```bash
python get_UWyoming_snd.py -y 2021 -m 9 -i 0100 -e 0200 -r samer -f Airports_with_ACARS_Soundings.dat
```

or using the wrapper (recommended):

```bash
./execute.sh -y 2021 -m 9 -i 0100 -e 0200 -r samer -f Airports_with_ACARS_Soundings.dat
```
<!-- pilar cordoba -->
<!-- clear ; ./execute.sh -y 2026 -m 1 -i 1400 -e 1500 -r samer -s 87344 -o "/mnt/c/lidarData/Argentina/Pilar/Radiosonde/" -->

<!-- SP -->
<!-- clear ; ./execute.sh -y 2019 -m 10 -i 1000 -e 1100 -r samer -s 83779 -o "/mnt/c/lidarData/SPU/Radiosonde/" -->

<!-- PILAR, CORDOBA -->
<!-- clear ; ./execute.sh -y 2026 -m 1 -i 1000 -e 1100 -r samer -s 87344 -o "/mnt/c/lidarData/Argentina/Pilar/Radiosonde/" -->

<!-- MURCIA, ESPANA -->
<!-- clear ; ./execute.sh -y 2023 -m 2 -i 1400 -e 1500 -r samer -s 87344 -o "/mnt/c/lidarData/Granada/Radiosondeo/" -->

