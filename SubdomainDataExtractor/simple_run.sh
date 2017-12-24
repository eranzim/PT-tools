#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage: ./simple_run.sh <domain>"
    exit 1
fi
if [ ! -f ../Subdomainer/subdomains_nc.txt ]; then
    wget https://raw.githubusercontent.com/averagesecurityguy/axfr/master/subdomains_nc.txt
fi
echo -e "\n\nPress Ctrl-C to stop script..."
python ../Subdomainer/subdomainer.py -i subdomains_nc.txt -d $1 -n 1000 -o subdomains
./extractor.sh subdomains $1
