# Python3 script to retrieve directly soundng data from U. Wymoning data-base
#   after an script from A. L. Rolla, CIMA
#
## e.g. # python get_UWyoming_snd_sil.py -y 2019 -m 6 -i 2700 -e 2800 -r samer -s 83746
from optparse import OptionParser
import urllib.request
from urllib.error import URLError, HTTPError
from bs4 import BeautifulSoup
import pandas as pd 
import re
from datetime import datetime

# Aca habria que armar el string de request
# poniendo las variables
# YEAR , MONTH, FROM, TO , STNM, etc

regions = {'europe': 'Europe', 'mideast': 'Middle East', 'samer': 'South America'}

parser = OptionParser()
parser.add_option("-y", "--year", dest="year", help="year to retrieve from", 
  metavar="VALUE")
parser.add_option("-m", "--month", dest="mon", help="month to retrieve from", 
  metavar="VALUE")
parser.add_option("-i", "--InitialDate", dest="idate", help="Initial date of the period to retrieve from [DD][HH]", 
  metavar="VALUE")
parser.add_option("-e", "--EndDate", dest="edate", help="End date of the period to retrieve from [DD][HH]", 
  metavar="VALUE")
parser.add_option("-r", "--Region", dest="region", help="region", 
  metavar="VALUE")
parser.add_option("-s", "--station", dest="stat", help="station number", 
  metavar="VALUE")
(opts, args) = parser.parse_args()

#######    #######
## MAIN
    #######

#url = "http://weather.uwyo.edu/cgi-bin/sounding?" + "region=samer&" \
#      "TYPE=TEXT%3ALIST&" \
#      "YEAR=2019&" \
#      "MONTH=06&" \
#      "FROM=2712&" \
 #     "TO=2712&" \
  #    "STNM=87576"

iurl = "http://weather.uwyo.edu/cgi-bin/sounding?"
ftype = "TYPE=TEXT%3ALIST&"

monS = str(int(opts.mon)).zfill(2)

reg = "region=" + opts.region + "&"
yr = "YEAR=" + opts.year + "&"
mon = "MONTH=" + monS + "&"
beg = "FROM=" + opts.idate + "&"
end = "TO=" + opts.edate + "&"
st = "STNM=" + opts.stat

url = iurl + reg + ftype + yr + mon + beg + end + st

# function to read html code from url =================================================================
def gets_html(url):
    # Read  the url and return  html code
    try:
        req = urllib.request.Request(url,
                                     headers={'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Win64; x64)'}
                                     )
        html = urllib.request.urlopen(req).read().decode("utf-8")
    except HTTPError as e:
        print('The server couldn\'t fulfill the SOUNDING requested.')
        print('Error code: ', e.code)
        html = ""
        pass
    except URLError as e:
        print('We failed to reach the server /weather.uwyo.edu.')
        print('Reason: ', e.reason)
        html = ""
        pass
    finally:
        pass
    return html


pagina = gets_html(url)

soup = BeautifulSoup(pagina, 'html.parser')

# ofilen = 'UWyoming_' + opts.year + monS + '_' + opts.stat + '.snd'
ofilen = 'rad_' + opts.stat + '_'

Nvals = len(soup.find_all("h2"))
print ("Found:", Nvals, " soundngs")

for isnd in range(Nvals):
  snd_h2=soup.find_all("h2")[isnd].text.strip().split(' at ')
  snd_time=datetime.strptime(snd_h2[1], '%HZ %d %b %Y').strftime("%Y%m%d%H")

  snd_output=ofilen + snd_time + '.dat'
  with open(snd_output, 'w') as of:
    itemlist=soup.find_all("pre")[isnd*2].text.split('\n')
    outfile='\n'.join(itemlist[5:])
    of.write(outfile)


  # Step 1: Load the .dat file into a DataFrame
  # Replace snd_output with your actual file name and specify the delimiter if needed
  df = pd.read_csv(snd_output, delim_whitespace=True, header=None)

  # Step 2: Define the factors
  factor_first = 100  # Factor for the first column
  factor_two = 1  # Factor for the second column
  factor_third = 273      # Factor for the third column

  # Step 3: Apply the factors to the specified columns
  df.iloc[:, :1] = df.iloc[:, :1] * factor_first  # Multiply the first column
  df.iloc[:, :2] = df.iloc[:, :2] * factor_two  # Multiply the second column
  df.iloc[:, 2] = df.iloc[:, 2] + factor_third        # Multiply the third column

  # Step 4: Select only the first three columns to keep in the final output
  df_modified = df.iloc[:, :3]

  # Step 5: Save the modified DataFrame with only the first three columns to a CSV file
  df_modified.to_csv('output'+ str(isnd) +'.csv', index=False, header=False)
 
  
  print ("Successfull writting of '" + snd_output + "' !!")

