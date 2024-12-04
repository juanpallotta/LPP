
This code was developed and tested in Ubuntu 20.04 and gcc 

How to install      : Just run the shell script ./installRequisites.
How to compile      : Just run the shell script ./compileMergeLidarFiles
How to run          : Just run the shell script ./runMergeLidarFiles
How to compile & run: Just run the shell script ./compileRunMergeLidarFiles


* Configuration files:

- dirFile.sh:
Here you set the path to the Licel files to merge and save in a NetCDF file.

- mergingParameters.dat:
Here you can set all the parameters needed for merging, as the attributes of each of them. The file is self-explanatory, and te only has to follow the format: 
VARIABLE = VALUE
Being the value a string or a number depending on the variable type.

- runMergeLidarFiles:
This is a shell script wich only run the sofware with the parameters. To do this, first load the dirFile.sh and then execute:
./mergeLidarFiles pathToLicelFiles mergingParameters.dat

The variable 'pathToLicelFiles' is loaded in dirFile.sh script.





