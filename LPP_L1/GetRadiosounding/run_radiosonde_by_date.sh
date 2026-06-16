#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="/Users/veronicacheaz/Downloads/job/IISTA"
GET_RADIO_DIR="$ROOT_DIR/LPP-main/LPP_L1/GetRadiosounding"
DATA_ROOT="$ROOT_DIR/Data/2020_Sao_Paulo_Dataset/data"
OUTPUT_DIR="$ROOT_DIR/Data/Radiosonde"
EXECUTE_SCRIPT="execute.sh"
STATION="83779"
REGION="samer"
DEFAULT_START_HOUR="00"
DEFAULT_END_HOUR="23"

if ! [ -d "$GET_RADIO_DIR" ]; then
  echo "Error: GetRadiosounding directory not found: $GET_RADIO_DIR" >&2
  exit 1
fi

if ! [ -x "$GET_RADIO_DIR/$EXECUTE_SCRIPT" ]; then
  echo "Error: $EXECUTE_SCRIPT not found or not executable in $GET_RADIO_DIR" >&2
  exit 1
fi

if ! [ -d "$DATA_ROOT" ]; then
  echo "Error: date folder root not found: $DATA_ROOT" >&2
  exit 1
fi

mkdir -p "$OUTPUT_DIR"
cd "$GET_RADIO_DIR"

for date_dir in "$DATA_ROOT"/*/; do
  [ -d "$date_dir" ] || continue
  folder_name="$(basename "$date_dir")"

  if [[ "$folder_name" =~ ^([0-9]{4})([0-9]{2})([0-9]{2})$ ]]; then
    year="${BASH_REMATCH[1]}"
    month="${BASH_REMATCH[2]}"
    day="${BASH_REMATCH[3]}"
    initial_date="${day}${DEFAULT_START_HOUR}"
    end_date="${day}${DEFAULT_END_HOUR}"

    echo "\n=== Processing $folder_name ==="
    echo "Year: $year, Month: $month, Day: $day"
    echo "Command: ./execute.sh -y $year -m $month -i $initial_date -e $end_date -r $REGION -s $STATION -o '$OUTPUT_DIR'"
    ./execute.sh -y "$year" -m "$month" -i "$initial_date" -e "$end_date" -r "$REGION" -s "$STATION" -o "$OUTPUT_DIR"
  else
    echo "Skipping non-date folder: $folder_name"
  fi
done

echo "\nAll done. Output saved to: $OUTPUT_DIR"
