import argparse
import urllib.request
from urllib.error import URLError, HTTPError
from bs4 import BeautifulSoup
from datetime import datetime
import os

def gets_html(url):
    try:
        req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Win64; x64)'})
        html = urllib.request.urlopen(req).read().decode("utf-8")
    except HTTPError as e:
        print('The server couldn\'t fulfill the SOUNDING requested.')
        print('Error code: ', e.code)
        html = ""
    except URLError as e:
        print('We failed to reach the server /weather.uwyo.edu.')
        print('Reason: ', e.reason)
        html = ""
    return html

def main():
    parser = argparse.ArgumentParser(description="Retrieve sounding data from U. Wyoming data-base.")
    parser.add_argument("-y", "--year", help="year(s) to retrieve from, comma-separated", required=True)
    parser.add_argument("-m", "--month", help="month(s) to retrieve from, comma-separated", required=True)
    parser.add_argument("-i", "--InitialDate", help="Initial date(s) of the period to retrieve from [DD][HH], comma-separated", required=True)
    parser.add_argument("-e", "--EndDate", help="End date(s) of the period to retrieve from [DD][HH], comma-separated", required=True)
    parser.add_argument("-r", "--Region", help="region", required=True)
    parser.add_argument("-s", "--station", help="station number(s), comma-separated")
    parser.add_argument("-f", "--station-file", help="file containing stations to retrieve")
    parser.add_argument("-o", "--output", help="output folder for radiosoundings (default: ./radiosondes)", default="./radiosondes")
    
    args = parser.parse_args()

    # Create output directory if it doesn't exist
    if not os.path.exists(args.output):
        print(f"Creating directory: {args.output}")
        os.makedirs(args.output)

    stations = []
    if args.station:
        stations.extend([s.strip() for s in args.station.split(',')])
    if args.station_file:
        with open(args.station_file, 'r') as sf:
            for line in sf:
                line = line.strip()
                if line and not line.startswith('#'):
                    # Read WMO IDs from our updated TSV/CSV format
                    parts = line.split('\t')
                    if len(parts) >= 3 and parts[2].isdigit() and parts[2] != '0':
                        stations.append(parts[2])
                    elif line.isdigit():
                        stations.append(line)

    if not stations:
        print("Error: No stations provided. Use -s or -f.")
        return

    years = [y.strip() for y in args.year.split(',')]
    months = [str(int(m.strip())).zfill(2) for m in args.month.split(',')]
    idates = [d.strip() for d in args.InitialDate.split(',')]
    edates = [d.strip() for d in args.EndDate.split(',')]

    max_len = max(len(years), len(months), len(idates), len(edates))
    
    if any(len(lst) not in (1, max_len) for lst in [years, months, idates, edates]):
        print("Error: If multiple dates are provided, year, month, InitialDate, and EndDate must have the same number of elements (or exactly 1).")
        return

    if len(years) == 1: years *= max_len
    if len(months) == 1: months *= max_len
    if len(idates) == 1: idates *= max_len
    if len(edates) == 1: edates *= max_len

    iurl = "http://weather.uwyo.edu/cgi-bin/sounding?"
    ftype = "TYPE=TEXT%3ALIST&"
    reg = "region=" + args.Region + "&"

    for stat in set(stations):
        for idx in range(max_len):
            yr = "YEAR=" + years[idx] + "&"
            mon = "MONTH=" + months[idx] + "&"
            beg = "FROM=" + idates[idx] + "&"
            end = "TO=" + edates[idx] + "&"
            st = "STNM=" + stat

            url = iurl + reg + ftype + yr + mon + beg + end + st
            print(f"Retrieving data for station {stat} from {years[idx]}-{months[idx]}-{idates[idx]} to {edates[idx]}...")
            
            pagina = gets_html(url)
            if not pagina:
                continue
                
            soup = BeautifulSoup(pagina, 'html.parser')
            ofilen = f"rad_{stat}_"
            
            headers = soup.find_all("h2")
            if not headers:
                print(f"No data found for station {stat} in this period.")
                continue
                
            Nvals = len(headers)
            print(f"Found: {Nvals} soundings for station {stat}")
            
            pres = soup.find_all("pre")
            for isnd in range(Nvals):
                try:
                    snd_h2 = headers[isnd].text.strip().split(' at ')
                    if len(snd_h2) > 1:
                        snd_time = datetime.strptime(snd_h2[1], '%HZ %d %b %Y').strftime("%Y%m%d%H")
                        snd_filename = ofilen + snd_time + '.csv'
                        snd_output = os.path.join(args.output, snd_filename)
                        
                        # Get the raw text
                        raw_text = pres[isnd*2].text.split('\n')
                        # The data starts from index 5
                        data_lines = [line for line in raw_text[5:] if line.strip()]
                        
                        if not data_lines:
                            print(f"No data lines found in sounding for station {stat}.")
                            continue

                        # Determine the most frequent column count
                        col_counts = [len(line.strip().split()) for line in data_lines]
                        if not col_counts:
                            continue
                        
                        from collections import Counter
                        most_common_count = Counter(col_counts).most_common(1)[0][0]
                        
                        # Updated Wyoming Sounding columns: HGHT[m], TEMP[K], PRES[Pa]
                        header = "HGHT[m],TEMP[K],PRES[Pa]"
                        csv_lines = [header]

                        for line in data_lines:
                            cols = line.strip().split()
                            if len(cols) == most_common_count:
                                try:
                                    # cols[0]: PRES (hPa), cols[1]: HGHT (m), cols[2]: TEMP (C)
                                    p_pa = float(cols[0]) * 100
                                    hght = float(cols[1])
                                    t_k = float(cols[2]) + 273.15
                                    
                                    csv_lines.append(f"{hght:.1f},{t_k:.2f},{p_pa:.1f}")
                                except (ValueError, IndexError):
                                    pass
                            else:
                                print(f"Warning: Removing malformed line in {snd_filename} (expected {most_common_count} cols): {line.strip()}")

                        with open(snd_output, 'w') as of:
                            of.write('\n'.join(csv_lines))
                        
                        print(f"Successfully wrote '{snd_output}' !!")
                except Exception as e:
                    print(f"Error parsing sounding {isnd} for station {stat}: {e}")

if __name__ == "__main__":
    main()
