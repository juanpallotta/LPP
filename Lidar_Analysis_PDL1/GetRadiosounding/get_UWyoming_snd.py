# Python3 script to retrieve directly soundng data from U. Wymoning data-base
#   after an script from A. L. Rolla, CIMA
#
## e.g. # python3 get_UWyoming_snd.py -y 2021 -m 9 -i 0100 -e 0200 -r samer -s 87576
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
  
  print ("Successfull writting of '" + snd_output + "' !!")
