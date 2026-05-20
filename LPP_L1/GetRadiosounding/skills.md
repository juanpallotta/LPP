
# Agent task:
1. Read `get_UWyoming_snd.py` and understand how it works. Discard the others versions like `get_UWyoming_snd_ORIGINAL.py`, `get_UWyoming_snd_sil.py`.
2. Identify the issue: it only retrieves data for *one* station and *one* date range.
3. Write the modified/new script(s) to implement this enhanced functionality.
4. Create the requirements.txt file and write a brief documentation with clear usage examples. Name it README.md. Make a Linux script file to execute the installation of the dependencies.
5. The installation and the execution must create and use a Python virtual environment (venv).
6. Update the file `Airports_with_ACARS_Soundings.dat`.

# Output file
1. The `get_UWyoming_snd.py` script must create an output file with only the columns related to the heigh, temperature and pressure (in this mentioned order).