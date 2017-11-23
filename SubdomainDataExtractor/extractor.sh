#!/bin/bash
# Param #1: input file
# param #2: domain

dir_name="$2_$(date +%Y%m%d_%H%M%S)"
function to_file {
    (echo $1
	$2
	echo -e "\n--------------------\n\n") >> "$dir_name/$3"
}
mkdir -v "$dir_name"
for subdomain in $(cat $1);do
    to_file $subdomain "host $subdomain.$2" host.log
    # dig
    # whois
    # ping
    # dig +short
    # whois on address
    # host on address
done
