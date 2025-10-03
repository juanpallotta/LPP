import requests
import xml.etree.ElementTree as ET
import os
import argparse

def main():
    # Parse command-line arguments
    parser = argparse.ArgumentParser(description="Download GOES-16 ABI-L2-AODF data")
    parser.add_argument("--year", required=True, help="Year (e.g., 2019)")
    parser.add_argument("--day", required=True, help="Day of year [001–365] (e.g., 339)")
    parser.add_argument("--hour", required=True, help="Hour [00–23] (e.g., 15)")
    args = parser.parse_args()

    # Build prefix
    prefix = f"ABI-L2-AODF/{args.year}/{args.day.zfill(3)}/{args.hour.zfill(2)}/"
    url = f"https://noaa-goes16.s3.amazonaws.com/?prefix={prefix}"

    # Create output folder
    os.makedirs("goes16_data", exist_ok=True)

    # Fetch XML listing
    print(f"Fetching file list from {url} ...")
    r = requests.get(url)
    r.raise_for_status()

    # Parse XML
    root = ET.fromstring(r.text)
    ns = {"s3": "http://s3.amazonaws.com/doc/2006-03-01/"}

    # Download each .nc file
    for content in root.findall("s3:Contents", ns):
        key = content.find("s3:Key", ns).text
        if key.endswith(".nc"):
            file_url = f"https://noaa-goes16.s3.amazonaws.com/{key}"
            filename = os.path.join("goes16_data", os.path.basename(key))
            print(f"Downloading {filename} ...")

            with requests.get(file_url, stream=True) as resp:
                resp.raise_for_status()
                with open(filename, "wb") as f:
                    for chunk in resp.iter_content(chunk_size=8192):
                        f.write(chunk)

    print("Download complete!")

if __name__ == "__main__":
    main()
