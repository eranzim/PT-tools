#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage: ./simple_run.sh <domain>"
    exit 1
fi
chmod +x ./extractor.sh
if [ ! -f ../Subdomainer/subdomains_nc.txt ]; then
    wget -O ../Subdomainer/subdomains_nc.txt https://raw.githubusercontent.com/averagesecurityguy/axfr/master/subdomains_nc.txt
fi
echo -e "\n\nPress Ctrl-C to stop script..."
python3 ../Subdomainer/subdomainer.py -i ../Subdomainer/subdomains_nc.txt -d $1 -n 1000 -o ../Subdomainer/subdomains
./extractor.sh ../Subdomainer/subdomains $1
