
clear

comando_1="sshpass -p "Citedef17" rsync -arvz -e 'ssh -p 42044' citedef@savernet.smn.gob.ar:/rawdata/aeroparque/"

dateFolder=$(date +'%Y%m%d/')

# -p = NO HACE NADA SI EL DIRECTORIO EXISTE
mkdir -p "/home/juan/data/"$dateFolder

comando_2="$dateFolder""* /home/juan/data/$dateFolder"

comando="$comando_1$comando_2"

eval "$comando"

# sshpass -p "Citedef17" rsync -arvz -e 'ssh -p 42044' citedef@savernet.smn.gob.ar:rawdata/aeroparque/20200817/* ./data/20200817/






PARA QUE SE EJECUTE RUTINARIAMENTE:

- watch -n 60*15 runLidarAnalysis.sh


