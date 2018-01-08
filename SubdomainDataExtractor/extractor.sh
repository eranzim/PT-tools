#!/bin/bash
# Param #1: input file
# Param #2: domain

dir_name="$2_$(date +%Y%m%d_%H%M%S)"

function to_file {
    (echo $1
    $2
    echo -e "\n--------------------\n\n") >> "$dir_name/$3"
}

mkdir -v "$dir_name"
mkdir -v "$dir_name/websites"
truncate -s 0 $dir_name/html_comments_u.log

for subdomain in $(cat $1);do
    mkdir -v "$dir_name/websites/$subdomain"
    # Create a directory, download the website into it, then scan it for comments
    mkdir -v "$dir_name/websites/$subdomain/80" && timeout 20 wget -P "$dir_name/websites/$subdomain/80" -q -r --no-host-directories --user-agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.75 Safari/537.36" http://$subdomain.$2 && to_file $subdomain "python ../FindComments/find_comments.py -s $dir_name/websites/$subdomain/80" html_comments.log && python ../FindComments/find_comments.py -s -e $dir_name/websites/$subdomain/80 >> $dir_name/html_comments_u.log &
    mkdir -v "$dir_name/websites/$subdomain/443" && timeout 20 wget -P "$dir_name/websites/$subdomain/443" -q -r --no-host-directories --user-agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.75 Safari/537.36" https://$subdomain.$2 && to_file $subdomain "python ../FindComments/find_comments.py -s $dir_name/websites/$subdomain/443" html_comments.log && python ../FindComments/find_comments.py -s -e $dir_name/websites/$subdomain/443 >> $dir_name/html_comments_u.log &
    to_file $subdomain "host $subdomain.$2" host.log &
    to_file $subdomain "host -a $subdomain.$2" host-a.log &
    to_file $subdomain "dig $subdomain.$2 any +all" dig.log &
    to_file $subdomain "dig $subdomain.$2 +short" dig-short.log &
    to_file $subdomain "whois -H $subdomain.$2" whois.log &
    to_file $subdomain "ping -c 2 $subdomain.$2" ping.log &
    # If files in array exists, do a download
    files=("robots.txt" "crossdomain.xml")
    for file in ${files[@]};do
    wget -q --spider http://$subdomain.$2/$file
    if [[ $? == 0 ]];then
        timeout 20 wget -P "$dir_name/websites/$subdomain/80" -q --no-host-directories --user-agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.75 Safari/537.36" http://$subdomain.$2/$file
    fi
    wget -q --spider https://$subdomain.$2/$file
    if [[ $? == 0 ]];then
        timeout 20 wget -P "$dir_name/websites/$subdomain/443" -q --no-host-directories --user-agent="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.75 Safari/537.36" https://$subdomain.$2/$file
    fi
    done
    wait
    # This line must run synchronously, since following lines depend on it!
    dig $subdomain.$2 +short > "$dir_name/ips.tmp"
    for ip in $(cat "$dir_name/ips.tmp");do
        to_file $ip "whois $ip" netrange.log &
        to_file $ip "whois -HBda $ip" netrange-hbda.log &
        to_file $ip "host $ip" rdns.log &
        to_file $ip "host -a $ip" rdns-a.log &
    done
    wait
done

sort -u $dir_name/html_comments_u.log -o $dir_name/html_comments_u.log

rm -f "$dir_name/ips.tmp"
find ./wget-log* -size 0 -type f -delete 2>/dev/null

# Find e-mail addresses by searching for one of the following: @, (at), [at] or similar
egrep -r --exclude=emails-grep.log '@|\([[:space:]]*at[[:space:]]*\)|\[[[:space:]]*at[[:space:]]*\]' "$dir_name" > "$dir_name/emails-grep.log"
