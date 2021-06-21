#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage: ./simple_run.sh <domain>"
    exit 1
fi
if [ ! -f subdomains_nc.txt ]; then
    wget https://raw.githubusercontent.com/averagesecurityguy/axfr/master/subdomains_nc.txt
fi
echo -e "\n\nPress Ctrl-C to stop script..."
python3 ./subdomainer.py -i subdomains_nc.txt -d $1 -o subdomains
