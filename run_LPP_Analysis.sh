#!/bin/bash
# ==============================================================================
# Lidar Processing Pipeline (LPP) Analysis Runner v1.0
# ==============================================================================
# Refactored script to execute every module of the LPP software.
# Outputs are formatted with beautiful, clear terminal styled messages.
# ==============================================================================

# ANSI Color Codes
CLR_RESET="\033[0m"
CLR_BOLD="\033[1m"
CLR_DIM="\033[2m"
CLR_HEADER="\033[1;36m"   # Bold Cyan
CLR_SUCCESS="\033[1;32m"  # Bold Green
CLR_INFO="\033[1;34m"     # Bold Blue
CLR_WARN="\033[1;33m"     # Bold Yellow
CLR_ERROR="\033[1;31m"    # Bold Red
CLR_SUB="\033[1;35m"      # Bold Magenta

# Helper Functions for Pretty Printing
print_header() {
    echo -e "\n${CLR_HEADER}====================================================================${CLR_RESET}"
    echo -e "${CLR_HEADER}  $1 ${CLR_RESET}"
    echo -e "${CLR_HEADER}====================================================================${CLR_RESET}"
}

print_section() {
    echo -e "\n${CLR_SUB}---> $1${CLR_RESET}"
}

print_info() {
    echo -e "  ${CLR_INFO}[INFO]${CLR_RESET} $1"
}

print_success() {
    echo -e "  ${CLR_SUCCESS}[OK]${CLR_RESET} $1"
}

print_warn() {
    echo -e "  ${CLR_WARN}[WARN]${CLR_RESET} $1"
}

print_error() {
    echo -e "  ${CLR_ERROR}[ERROR]${CLR_RESET} $1"
}

lower() {
  echo "$1" | tr '[:upper:]' '[:lower:]'
}

# --- 1. Sourcing Settings ---
print_header "LPP RUN SETTINGS INITIALIZATION"

SETTINGS_FILE="./Lidar_Configuration_Files/LPP_Run_Settings.sh"
if [ $# -gt 0 ]; then
    SETTINGS_FILE="$1"
fi

if [ -f "$SETTINGS_FILE" ]; then
    print_info "Sourcing settings from: ${CLR_BOLD}${SETTINGS_FILE}${CLR_RESET}"
    source "$SETTINGS_FILE"
else
    print_error "Settings file not found: $SETTINGS_FILE"
    exit 1
fi

# --- 2. Getting Absolute Paths to Modules ---
print_section "Locating LPP Executable Modules..."

PATH_TO_LPP="$(pwd)/"
print_info "LPP Root: $PATH_TO_LPP"

# Find paths to modules with fallbacks/checks
L0_SRC=$(find . -name lpp0.cpp -print -quit)
L1_SRC=$(find . -name lpp1.cpp -print -quit)
L2_SRC=$(find . -name lpp2.cpp -print -quit)
PLOT_SRC=$(find .. -name plot_LPP.py -print -quit)

if [ -n "$L0_SRC" ]; then
    PATH_TO_L0="$(dirname "$(realpath "$L0_SRC")")/"
    print_success "Found L0 module path: $PATH_TO_L0"
else
    print_warn "Could not find lpp0.cpp. Defaulting L0 path to current dir."
    PATH_TO_L0="./LPP_L0/"
fi

if [ -n "$L1_SRC" ]; then
    PATH_TO_L1="$(dirname "$(realpath "$L1_SRC")")/"
    print_success "Found L1 module path: $PATH_TO_L1"
else
    print_warn "Could not find lpp1.cpp. Defaulting L1 path to current dir."
    PATH_TO_L1="./LPP_L1/"
fi

if [ -n "$L2_SRC" ]; then
    PATH_TO_L2="$(dirname "$(realpath "$L2_SRC")")/"
    print_success "Found L2 module path: $PATH_TO_L2"
else
    print_warn "Could not find lpp2.cpp. Defaulting L2 path to current dir."
    PATH_TO_L2="./LPP_L2/"
fi

if [ -n "$PLOT_SRC" ]; then
    PATH_TO_LPP_PLOT="$(dirname "$(realpath "$PLOT_SRC")")/"
    print_success "Found Python Plot module path: $PATH_TO_LPP_PLOT"
else
    print_warn "Could not find plot_LPP.py. Defaulting Plot path to LPP_Python_Plots."
    PATH_TO_LPP_PLOT="./LPP_Python_Plots/"
fi

# --- 3. Python Virtualenv Detection ---
print_section "Detecting Python environment..."
PYTHON_CMD="python3"
if [ -x "${PATH_TO_LPP}lidar_env/bin/python3" ]; then
    PYTHON_CMD="${PATH_TO_LPP}lidar_env/bin/python3"
    print_success "Using LPP Virtualenv: $PYTHON_CMD"
elif [ -x "./lidar_env/bin/python3" ]; then
    PYTHON_CMD="./lidar_env/bin/python3"
    print_success "Using Local Virtualenv: $PYTHON_CMD"
else
    print_info "No local virtual environment found. Using system $PYTHON_CMD"
fi

# --- 4. Input Configuration and Validation ---
print_header "VALIDATING CONFIGURATION & INPUT PATHS"

if [ -z "$PATH_IN" ]; then
    print_error "PATH_IN variable is not defined in settings."
    exit 1
fi

print_info "Configured Input Path (PATH_IN): $PATH_IN"
print_info "Active Levels: L0=$(lower "$L0") | L1=$(lower "$L1") | L2=$(lower "$L2")"

if [ -d "$PATH_IN" ]; then
    print_info "Input path is a DIRECTORY."
    if [[ "$(lower "$L0")" == "no" ]]; then
        print_error "PATH_IN is a directory, so L0 must be set as 'yes' in run settings."
        exit 1
    else
        # Find subdirectories containing raw data (WSL / Linux)
        PATH_IN_LIST=$(find "$PATH_IN" -type d)
        if [[ "$(lower "$L2")" == "yes" ]] && [[ "$(lower "$L1")" == "no" ]]; then
            print_warn "L2 requires L1. Enabling L1 auto-processing."
            L1="yes"
        fi
    fi
else
    print_info "Input path is a FILE."
    if [[ "$(lower "$L0")" == "yes" ]]; then
        print_error "PATH_IN is a file. L0 must be set to 'no' in run settings."
        exit 1
    fi
    
    if [[ "$(lower "$L1")" == "yes" ]] || [[ "$(lower "$L2")" == "yes" ]]; then
        PATH_IN_LIST="$PATH_IN"
    else
        print_error "L1 or L2 must be set to 'yes' to process a file input."
        exit 1
    fi
fi

# --- 5. Main Processing Loop ---
print_header "STARTING PIPELINE ANALYSIS"

for paths_In in $PATH_IN_LIST; do
    PROCES_SENTRY=1
    
    # Check if subdirectory should be skipped
    if [[ "$(lower "$L0")" == "yes" ]]; then
        if [[ "$paths_In" == *"/LPP_OUT"* ]]; then
            print_info "Skipping folder containing 'LPP_OUT': $paths_In"
            PROCES_SENTRY=0
        fi
    fi
    
    if [ $PROCES_SENTRY -eq 1 ]; then
        echo -e "\n${CLR_BOLD}────────────────────────────────────────────────────────────────────${CLR_RESET}"
        echo -e "${CLR_INFO}Processing:${CLR_RESET} ${CLR_BOLD}$paths_In${CLR_RESET}"
        echo -e "${CLR_BOLD}────────────────────────────────────────────────────────────────────${CLR_RESET}"
        
        # Setup specific file output names depending on inputs
        if [[ "$(lower "$L0")" == "yes" ]]; then
            PATH_IN_L0="${paths_In}/"
            PATH_LPP_OUT="${PATH_IN_L0}LPP_OUT/"
            
            print_info "Creating output directory: $PATH_LPP_OUT"
            mkdir -p "$PATH_LPP_OUT"
            
            PATH_FILE_OUT_L0="${PATH_LPP_OUT}$(basename "$paths_In")_L0.nc"
            PATH_FILE_IN_L1="$PATH_FILE_OUT_L0"
            PATH_FILE_OUT_L1="${PATH_FILE_IN_L1%.*}_L1.nc"
            PATH_FILE_OUT_L1_CLOUD_DATA="${PATH_FILE_OUT_L1%.*}_CLOUD_DATA.dat"
            PATH_FILE_IN_L2="$PATH_FILE_OUT_L1"
            PATH_FILE_OUT_L2="${PATH_FILE_IN_L2%.*}_L2.nc"
        else
            if [[ "$(lower "$L1")" == "yes" ]]; then
                PATH_FILE_IN_L1="$paths_In"
                PATH_FILE_OUT_L1="${paths_In%.*}_L1.nc"
                PATH_FILE_OUT_L1_CLOUD_DATA="${PATH_FILE_OUT_L1%.*}_CLOUD_DATA.dat"
                
                if [[ "$(lower "$L2")" == "yes" ]]; then
                    PATH_FILE_IN_L2="$PATH_FILE_OUT_L1"
                    PATH_FILE_OUT_L2="${PATH_FILE_IN_L2%.*}_L2.nc"
                fi
            else
                if [[ "$(lower "$L2")" == "yes" ]]; then
                    PATH_FILE_IN_L2="$paths_In"
                    PATH_FILE_OUT_L2="${PATH_FILE_IN_L2%.*}_L2.nc"
                fi
            fi
        fi

        PATH_FILE_OUT=""

        # --- RUN LPP0 MODULE ---
        if [[ "$(lower "$L0")" == "yes" ]]; then
            print_section "Running Module LPP0 (Licel -> NetCDF)..."
            print_info "Command: ./lpp0 \"$PATH_IN_L0\" \"$PATH_FILE_OUT_L0\" \"$FILE_CONF\""
            
            (cd "$PATH_TO_L0" && ./lpp0 "$PATH_IN_L0" "$PATH_FILE_OUT_L0" "$FILE_CONF")
            
            if [ $? -eq 0 ]; then
                print_success "Module LPP0 completed successfully."
                PATH_FILE_OUT="$PATH_FILE_OUT_L0"
            else
                print_error "Module LPP0 failed!"
                exit 1
            fi
        fi

        # --- RUN LPP1 MODULE ---
        if [[ "$(lower "$L1")" == "yes" ]]; then
            print_section "Running Module LPP1 (Corrections & Layer Mask)..."
            
            # Clean up old level 1 files
            if [ -f "$PATH_FILE_OUT_L1" ]; then
                print_info "Removing old Level 1 file: $PATH_FILE_OUT_L1"
                rm "$PATH_FILE_OUT_L1"
                sleep 0.5
            fi
            
            if [ -f "$PATH_FILE_IN_L1" ]; then
                print_info "Command: ./lpp1 \"$PATH_FILE_IN_L1\" \"$PATH_FILE_OUT_L1\" \"$FILE_CONF\""
                (cd "$PATH_TO_L1" && ./lpp1 "$PATH_FILE_IN_L1" "$PATH_FILE_OUT_L1" "$FILE_CONF")
                
                if [ $? -eq 0 ]; then
                    print_success "Module LPP1 completed successfully."
                    # Remove L0 intermediate file if generated in this flow
                    if [[ "$(lower "$L0")" == "yes" ]] && [ -f "$PATH_FILE_IN_L1" ]; then
                        rm "$PATH_FILE_IN_L1"
                    fi
                    PATH_FILE_OUT="$PATH_FILE_OUT_L1"
                else
                    print_error "Module LPP1 failed!"
                    exit 1
                fi
            else
                print_error "L1 input file does not exist: $PATH_FILE_IN_L1"
                exit 1
            fi
        fi

        # --- RUN LPP2 MODULE ---
        if [[ "$(lower "$L2")" == "yes" ]]; then
            print_section "Running Module LPP2 (Aerosol Inversion)..."
            
            # Clean up old level 2 files
            if [ -f "$PATH_FILE_OUT_L2" ]; then
                print_info "Removing old Level 2 file: $PATH_FILE_OUT_L2"
                rm "$PATH_FILE_OUT_L2"
                sleep 0.5
            fi
            
            if [ -f "$PATH_FILE_IN_L2" ]; then
                print_info "Command: ./lpp2 \"$PATH_FILE_IN_L2\" \"$PATH_FILE_OUT_L2\" \"$FILE_CONF\""
                (cd "$PATH_TO_L2" && ./lpp2 "$PATH_FILE_IN_L2" "$PATH_FILE_OUT_L2" "$FILE_CONF")
                
                if [ $? -eq 0 ]; then
                    print_success "Module LPP2 completed successfully."
                    # Remove L1 intermediate file if L0-L1-L2 pipeline was run fully
                    if [[ "$(lower "$L1")" == "yes" ]] && [ -f "$PATH_FILE_IN_L2" ]; then
                        rm "$PATH_FILE_IN_L2"
                    fi
                    PATH_FILE_OUT="$PATH_FILE_OUT_L2"
                else
                    print_error "Module LPP2 failed!"
                    exit 1
                fi
            else
                print_error "L2 input file does not exist: $PATH_FILE_IN_L2"
                exit 1
            fi
        fi

        # --- RUN PLOTTING ---
        if [ -n "$PATH_FILE_OUT" ] && [ -f "$PATH_FILE_OUT" ]; then
            print_section "Generating Plots..."
            PATH_FILE_TO_LPP_PLOT="${PATH_TO_LPP_PLOT}plot_LPP.py"
            
            if [ -f "$PATH_FILE_TO_LPP_PLOT" ]; then
                # Determine correct target output folder for plots
                PLOT_OUT_DIR="$PATH_LPP_OUT"
                if [ -z "$PLOT_OUT_DIR" ]; then
                    PLOT_OUT_DIR="$(dirname "$PATH_FILE_OUT")"
                fi
                
                print_info "Command: $PYTHON_CMD \"$PATH_FILE_TO_LPP_PLOT\" \"$PATH_FILE_OUT\" \"$PLOT_OUT_DIR\" 0"
                $PYTHON_CMD "$PATH_FILE_TO_LPP_PLOT" "$PATH_FILE_OUT" "$PLOT_OUT_DIR" 0
                
                if [ $? -eq 0 ]; then
                    print_success "Plots generated successfully."
                else
                    print_warn "Plotting execution encountered warnings/errors."
                fi
            else
                print_warn "Plot script not found at: $PATH_FILE_TO_LPP_PLOT"
            fi
        fi
    fi
done

print_header "LPP PIPELINE RUN COMPLETED"
exit 0
