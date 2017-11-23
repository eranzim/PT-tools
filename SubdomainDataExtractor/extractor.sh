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
    to_file $subdomain "host -a $subdomain.$2" host-a.log
    to_file $subdomain "dig $subdomain.$2 any +all" dig.log
    to_file $subdomain "dig $subdomain.$2 +short" dig-short.log
    to_file $subdomain "whois -H $subdomain.$2" whois.log
    to_file $subdomain "ping -c 2 $subdomain.$2" ping.log
	dig $subdomain.$2 +short > "$dir_name/ips.tmp"
    for ip in $(cat "$dir_name/ips.tmp");do
		to_file $ip "whois $ip" netrange.log
		to_file $ip "whois -HBda $ip" netrange-hbda.log
		to_file $ip "host $ip" rdns.log
		to_file $ip "host -a $ip" rdns-a.log
	done
done

rm -f "$dir_name/ips.tmp"
