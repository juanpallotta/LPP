#!/bin/bash

# ==============================================================================
# run_lpp_bulk.sh
# Automates the LPP analysis workflow across daily folders.
# Follows rules outlined in conf_lpp_bulk.conf
# ==============================================================================

# Colors for pretty terminal output
GREEN='\033[0;32m'
CYAN='\033[0;36m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color
BOLD='\033[1m'

# Locate and parse configuration file
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
CONF_FILE="${SCRIPT_DIR}/conf_lpp_bulk.conf"

if [ ! -f "$CONF_FILE" ]; then
    echo -e "${RED}${BOLD}ERROR:${NC} Configuration file not found at ${YELLOW}$CONF_FILE${NC}"
    exit 1
fi

# Ensure L0, L1, L2 exist in conf_lpp_bulk.conf (set to "yes" by default if missing)
for var in L0 L1 L2; do
    if ! grep -q "^[[:space:]]*${var}[[:space:]]*=" "$CONF_FILE"; then
        echo "${var}=\"yes\"" >> "$CONF_FILE"
    fi
done

# Initialize configurations
L0="yes"
L1="yes"
L2="yes"
PATH_TO_LPP=""
# PATH_IN variable initialization
PATH_IN=""
FILE_CONF=""
Path_To_Molecular_File=""
PATH_TO_OUTPUT=""

# Parse conf_lpp_bulk.conf
while IFS='=' read -r key val; do
    # Skip empty lines and comments
    [[ -z "$key" || "$key" =~ ^# ]] && continue
    
    # Trim whitespace
    key=$(echo "$key" | xargs)
    val=$(echo "$val" | xargs)
    
    # Strip enclosing quotes if present
    val="${val#\"}"
    val="${val%\"}"
    val="${val#\'}"
    val="${val%\'}"
    
    case "$key" in
        L0) L0="$val" ;;
        L1) L1="$val" ;;
        L2) L2="$val" ;;
        PATH_TO_LPP) PATH_TO_LPP="$val" ;;
        PATH_IN) PATH_IN="$val" ;;
        FILE_CONF) FILE_CONF="$val" ;;
        Path_To_Molecular_File) Path_To_Molecular_File="$val" ;;
        PATH_TO_OUTPUT) PATH_TO_OUTPUT="$val" ;;
    esac
done < "$CONF_FILE"

# Validation
if [ -z "$PATH_TO_LPP" ] || [ -z "$PATH_IN" ] || [ -z "$FILE_CONF" ] || [ -z "$Path_To_Molecular_File" ] || [ -z "$PATH_TO_OUTPUT" ]; then
    echo -e "${RED}${BOLD}ERROR:${NC} One or more required configuration variables are missing or empty in ${YELLOW}$CONF_FILE${NC}"
    exit 1
fi

if [ ! -d "$PATH_TO_LPP" ]; then
    echo -e "${RED}${BOLD}ERROR:${NC} PATH_TO_LPP directory does not exist: ${YELLOW}$PATH_TO_LPP${NC}"
    exit 1
fi

if [ ! -d "$PATH_IN" ]; then
    echo -e "${RED}${BOLD}ERROR:${NC} PATH_IN directory does not exist: ${YELLOW}$PATH_IN${NC}"
    exit 1
fi

if [ ! -d "$Path_To_Molecular_File" ]; then
    echo -e "${RED}${BOLD}ERROR:${NC} Path_To_Molecular_File directory does not exist: ${YELLOW}$Path_To_Molecular_File${NC}"
    exit 1
fi

# Iterate through daily folders (YYYYMMDD) in PATH_IN
for day_dir in "${PATH_IN%/}"/*/; do
    [ -d "$day_dir" ] || continue
    day_folder=$(basename "${day_dir%/}")
    
    # Process only folders with an 8-digit date format
    if [[ ! "$day_folder" =~ ^[0-9]{8}$ ]]; then
        continue
    fi
    
    echo -e "${CYAN}┌────────────────────────────────────────────────────────────────────────┐${NC}"
    echo -e "${CYAN}│                                                                        │${NC}"
    echo -e "${CYAN}│  ${BOLD}Processing Date:${NC} ${GREEN}${BOLD}${day_folder}${NC}${CYAN}                                              │${NC}"
    echo -e "${CYAN}│                                                                        │${NC}"
    echo -e "${CYAN}└────────────────────────────────────────────────────────────────────────┘${NC}"
    
    # 1. Find one of the folder paths inside folder PATH_IN/YYYYMMDD/data/
    data_subpath=$(find "${day_dir}data" -mindepth 1 -maxdepth 1 -type d | head -n 1)
    if [ -z "$data_subpath" ]; then
        # Fallback to the data folder itself if no subdirectories exist
        data_subpath="${day_dir}data"
    fi
    
    # 2. Get the dark files folder path: PATH_IN/YYYYMMDD/dark_current/
    dark_folder="${day_dir}dark_current"
    
    # 3. Find the radiosonde file (take the first one found if multiple exist)
    radio_file=$(find "$Path_To_Molecular_File" -name "rad_*_${day_folder}*.csv" | head -n 1)
    if [ -z "$radio_file" ]; then
        echo -e "${YELLOW}${BOLD}WARNING:${NC} No radiosonde file found for day ${CYAN}$day_folder${NC} in ${YELLOW}$Path_To_Molecular_File${NC}. Skipping."
        mkdir -p "$PATH_TO_OUTPUT"
        cat <<EOF > "${PATH_TO_OUTPUT}/${day_folder}_error.md"
# Error Processing Date $day_folder

- **Error Type:** Missing Radiosonde File
- **Details:** No radiosonde CSV file matching \`rad_*_${day_folder}*.csv\` was found in directory \`$Path_To_Molecular_File\`.
- **Timestamp:** $(date)
EOF
        continue
    fi
    
    # ── Update File 1: PATH_TO_LPP/Lidar_Configuration_Files/LPP_Run_Settings.sh ──
    RUN_SETTINGS="${PATH_TO_LPP}/Lidar_Configuration_Files/LPP_Run_Settings.sh"
    if [ ! -f "$RUN_SETTINGS" ]; then
        echo -e "${RED}${BOLD}ERROR:${NC} Run settings file not found at ${YELLOW}$RUN_SETTINGS${NC}"
        exit 1
    fi
    
    sed -i \
        -e "s|^L0=.*|L0=\"${L0}\"|" \
        -e "s|^L1=.*|L1=\"${L1}\"|" \
        -e "s|^L2=.*|L2=\"${L2}\"|" \
        -e "s|^PATH_IN=.*|PATH_IN=\"${data_subpath}\"|" \
        -e "s|^FILE_CONF=.*|FILE_CONF=\"${FILE_CONF}\"|" \
        "$RUN_SETTINGS"
        
    # ── Update File 2: FILE_CONF (analysisParameters_SPU.conf) ──
    if [ ! -f "$FILE_CONF" ]; then
        echo -e "${RED}${BOLD}ERROR:${NC} Configuration file not found at ${YELLOW}$FILE_CONF${NC}"
        exit 1
    fi
    
    sed -i \
        -e "s|^[[:space:]]*PATH_DARK_FILES[[:space:]]*=[[:space:]]*.*|PATH_DARK_FILES = ${dark_folder}/|" \
        -e "s|^[[:space:]]*Path_To_Molecular_File[[:space:]]*=[[:space:]]*.*|Path_To_Molecular_File = ${radio_file}|" \
        "$FILE_CONF"
        
    # ── Read indxWL_PDL2 from FILE_CONF to determine channel index ──
    indxWL_PDL2=$(grep -E "^[[:space:]]*indxWL_PDL2[[:space:]]*=" "$FILE_CONF" | cut -d'=' -f2 | xargs)
    WLindex=$(printf "%02d" "$indxWL_PDL2")

    # Determine the expected final NetCDF file based on L0, L1, L2 config
    data_subpath_base=$(basename "$data_subpath")
    LPP_OUT_DIR="${day_dir}data/LPP_OUT"
    if [ "$L2" = "yes" ]; then
        EXPECTED_FILE="${LPP_OUT_DIR}/${data_subpath_base}_L0_L1_L2.nc"
    elif [ "$L1" = "yes" ]; then
        EXPECTED_FILE="${LPP_OUT_DIR}/${data_subpath_base}_L0_L1.nc"
    else
        EXPECTED_FILE="${LPP_OUT_DIR}/${data_subpath_base}_L0.nc"
    fi

    # ── Execute LPP ──
    echo -e "${BLUE}Executing LPP analysis...${NC}"
    LPP_LOG="${SCRIPT_DIR}/lpp_run.log"
    (cd "$PATH_TO_LPP" && ./run_LPP_Analysis.sh) 2>&1 | tee "$LPP_LOG"
    lpp_exit_status=${PIPESTATUS[0]}
    
    # ── Move and rename LPP_OUT if it exists ──
    TARGET_DIR="${PATH_TO_OUTPUT}/${day_folder}_LPP_OUT_${WLindex}"
    if [ -d "$LPP_OUT_DIR" ]; then
        echo -e "${GREEN}Moving LPP_OUT output to ${YELLOW}$TARGET_DIR${GREEN}...${NC}"
        mkdir -p "$PATH_TO_OUTPUT"
        rm -rf "$TARGET_DIR"
        mv "$LPP_OUT_DIR" "$TARGET_DIR"
    else
        echo -e "${YELLOW}${BOLD}WARNING:${NC} Expected LPP_OUT folder at ${RED}$LPP_OUT_DIR${NC} was not found."
    fi
    
    # ── Verify execution and save error report if failed ──
    TARGET_EXPECTED_FILE="${TARGET_DIR}/$(basename "$EXPECTED_FILE")"
    success=true
    error_reason=""

    if [ $lpp_exit_status -ne 0 ]; then
        success=false
        error_reason="run_LPP_Analysis.sh exited with non-zero status ($lpp_exit_status)."
    elif [ ! -d "$TARGET_DIR" ]; then
        success=false
        error_reason="Expected LPP_OUT folder was not created/moved."
    elif [ ! -f "$TARGET_EXPECTED_FILE" ]; then
        success=false
        error_reason="Expected NetCDF output file '$(basename "$EXPECTED_FILE")' was not created."
    fi

    if [ "$success" = "true" ]; then
        # Delete the temporary log file on successful run
        rm -f "$LPP_LOG"

        # ── Clean up remaining folders inside the data folder ──
        echo -e "${BLUE}Cleaning up remaining folders inside ${YELLOW}${day_dir}data/${NC}..."
        find "${day_dir}data" -mindepth 1 -maxdepth 1 -type d -exec rm -rf {} +
        
        echo -e "${GREEN}${BOLD}Successfully completed processing for day $day_folder.${NC}"
    else
        echo -e "${RED}${BOLD}ERROR:${NC} LPP analysis failed for day ${CYAN}$day_folder${NC}. Reason: ${RED}$error_reason${NC}"
        # Ensure TARGET_DIR (the LPP_OUT folder in results) exists to save the log
        mkdir -p "$TARGET_DIR"
        
        # Save/move the log file inside the LPP_OUT folder (TARGET_DIR)
        if [ -f "$LPP_LOG" ]; then
            mv "$LPP_LOG" "$TARGET_DIR/"
        fi

        mkdir -p "$PATH_TO_OUTPUT"
        cat <<EOF > "${PATH_TO_OUTPUT}/${day_folder}_error.md"
# Error Processing Date $day_folder

- **Error Type:** LPP Execution Failure
- **Details:** $error_reason
- **Timestamp:** $(date)

## Last 50 lines of LPP Log:
\`\`\`
$(tail -n 50 "${TARGET_DIR}/$(basename "$LPP_LOG")" 2>/dev/null)
\`\`\`
EOF
        if [ -d "$TARGET_DIR" ]; then
            echo -e "${BLUE}Cleaning up remaining folders inside ${YELLOW}${day_dir}data/${NC}..."
            find "${day_dir}data" -mindepth 1 -maxdepth 1 -type d -exec rm -rf {} +
        fi
    fi
    echo ""
done

echo -e "${GREEN}${BOLD}Bulk processing script automation finished.${NC}"
