# HYSPLIT Installation Instructions

These instructions guide you through installing HYSPLIT and the necessary Python dependencies to run the `run_hysplit_linux.py` script.

## 1. Register and Download HYSPLIT

The HYSPLIT model requires registration with NOAA Air Resources Laboratory (ARL) for downloading the software, even for trajectory modeling.

1. Go to the [HYSPLIT Registration Page](https://www.ready.noaa.gov/HYSPLIT_register.php).
2. Complete the registration form. You must have a formal affiliation with an institution engaged in atmospheric sciences (government, commercial, educational, or non-profit), or be sponsored by an existing registered user.
3. Once approved (this may take a few days), you will receive an email with download instructions and credentials.
4. Download the **Linux** binary distribution (e.g., `hysplit.v5.3.0_Linux_x86_64.tar.gz`) from the link provided in the email.

## 2. Install HYSPLIT

The Python script assumes HYSPLIT is installed in your home directory under `~/hysplit`.

1.  Open a terminal.
2.  Move the downloaded file to your home directory (or where you want it).
3.  Extract the archive:
    ```bash
    cd ~
    tar -xvzf hysplit.v*.tar.gz
    ```
4.  **Important:** Rename the extracted folder to `hysplit` if it isn't already directly named that (often it extracts to something like `hysplit.v5.3.0`).
    ```bash
    mv hysplit.v* hysplit
    ```
    *Note: The script looks for the executable at `~/hysplit/exec/hyts_std`.*

## 3. Install Python Dependencies

The `run_hysplit_linux.py` script requires Python 3 and several libraries for plotting and scientific computing (matplotlib, cartopy, numpy, and scipy).

**Option A: Using requirements.txt (Recommended)**

1.  Navigate to the `getHYSPLIT` directory:
    ```bash
    cd .../getHYSPLIT/
    ```
2.  Install all dependencies from `requirements.txt`:
    ```bash
    pip install -r requirements.txt
    ```
    *Or if you are using a virtual environment or conda, install it there.*

## 4. Run the Script

1.  Navigate to the directory containing the script `run_hysplit_linux.py`:
    ```bash
    cd ..../getHYSPLIT/
    ```
2.  Run the script:
    ```bash
    python3 run_hysplit_linux.py
    ```

### Troubleshooting
- **Executable not found**: Check that `~/hysplit/exec/hyts_std` exists. If your HYSPLIT version has a different executable name, edit the `HYSPLIT_EXEC` variable in `run_hysplit_linux.py`.
- **Permission denied**: Ensure the executable has run permissions:
    ```bash
    chmod +x ~/hysplit/exec/hyts_std
    ```
