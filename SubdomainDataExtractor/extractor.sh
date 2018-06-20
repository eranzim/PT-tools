#!/bin/bash
# Param #1: input file
# Param #2: domain

dir_name="$2_$(date +%Y%m%d_%H%M%S)"
user_agent_string="Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.75 Safari/537.36"
# This isn't the "full" e-mail regex, but a relatively simplified, usually-good-enough one. See here: https://stackoverflow.com/questions/201323/how-to-validate-an-email-address-using-a-regular-expression
email_regex='([a-zA-Z0-9_.+-]+([[:space:]]*@[[:space:]]*|[[:space:]]*\([[:space:]]*[aA][tT][[:space:]]*\)[[:space:]]*|[[:space:]]*\[[[:space:]]*[aA][tT][[:space:]]*\][[:space:]]*|[[:space:]]+[aA][tT][[:space:]]+)[a-zA-Z0-9-]+(\.[a-zA-Z0-9-]+)+)'

function to_file {
    (echo $1
    $2
    echo -e "\n--------------------\n\n") >> "$dir_name/$3"
}

mkdir -v "$dir_name"
mkdir -v "$dir_name/websites"
truncate -s 0 $dir_name/html-comments-u.log
# Using & (run in background), output got mixed with the next write to the file, so doing it synchronously
to_file $2 "whois -H $2" whois.log

for subdomain in $(cat $1);do
    mkdir -v "$dir_name/websites/$subdomain"
    # Create a directory, download the website into it, then scan it for comments
    protocols=("80" "443")
    files=("robots.txt" "crossdomain.xml")
    for protocol in ${protocols[@]};do
        mkdir -v "$dir_name/websites/$subdomain/$protocol"
        timeout 20 wget -P "$dir_name/websites/$subdomain/$protocol" -q -e robots=off -r --no-host-directories --user-agent="$user_agent_string" http://$subdomain.$2 && to_file $subdomain "python ../FindComments/find_comments.py -s $dir_name/websites/$subdomain/$protocol" html-comments.log && python ../FindComments/find_comments.py -s -e $dir_name/websites/$subdomain/$protocol >> $dir_name/html-comments-u.log &
        for file in ${files[@]};do
            timeout 20 wget -P "$dir_name/websites/$subdomain/$protocol" -q -e robots=off -r --no-host-directories --user-agent="$user_agent_string" http://$subdomain.$2/$file &
        done
    done
    to_file $subdomain "host $subdomain.$2" host.log &
    to_file $subdomain "host -a $subdomain.$2" host-a.log &
    to_file $subdomain "dig $subdomain.$2 any +all" dig.log &
    to_file $subdomain "dig $subdomain.$2 +short" dig-short.log &
    to_file $subdomain "whois -H $subdomain.$2" whois.log &
    to_file $subdomain "ping -c 2 $subdomain.$2" ping.log &
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

sort -u $dir_name/html-comments-u.log -o $dir_name/html-comments-u.log

rm -f "$dir_name/ips.tmp"
find ./wget-log* -size 0 -type f -delete 2>/dev/null

# Find e-mail addresses by searching for e-mails containing one of the following: @, (at), [at], at
egrep -r --exclude=emails-grep.log "$email_regex" "$dir_name" | sort -u > "$dir_name/emails-grep.log"
egrep -o "$email_regex" "$dir_name/emails-grep.log" | sort -u > "$dir_name/emails-grep-u.log"
