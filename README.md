# PT-tools
Various tools for Penetration Testing

## Combinator
Useful for generating passwords for a dictionary attack based on a target's name (for example).
### Usage examples:
* Basic usage:
```bash
echo "john
fitzgerald
kennedy" > names
python ./combinator.py -i names
```
* Using minimum and maximum for more lengths:
```bash
python ./combinator.py -i names -min 1 -max 3
```
* Returning only concatenation results without underscores (-c) and only lowercase (-l):
```bash
python ./combinator.py -i names -l -c
```

## FindComments
Finds comments inside web pages. These can sometimes contain useful information.
### Usage examples:
* Recursively scanning a directory containing a website:
```bash
python ./find_comments.py "/local/path/to/website/dir"
```
* Scanning a single file:
```bash
python ./find_comments.py "/local/path/to/some/index.html"
```
* No errors and no titles etc. in output:
```bash
python ./find_comments.py -e -s "/local/path/to/page_or_dir"
```

## SimpleDllInjector
Injects a DLL of our choice to a process of our choice, calling its DllMain function in the victim process.
### Usage:
```cmd
SDI.exe <Target PID> <DLL Path>
```

## Subdomainer
Finds subdomains of a given website. Works well with https://github.com/averagesecurityguy/axfr/blob/master/subdomains_nc.txt (entire runtime is likely to take a while, but it will find most results quickly, and the script allows for stopping and picking up from a previous spot)
### Usage examples:
* Using the one-click, no-questions-asked script:
```bash
./simple_run.sh example.com
```
* Basic usage:
```bash
echo "www
mail" > subdomain_options
python ./subdomainer.py -i subdomain_options -d example.com
```
* Using subdomains_nc and output to file:
```bash
python ./subdomainer.py -i subdomains_nc.txt -d example.com -o subdomains
```
* Scan only first 10,000 subdomain options in the list:
```bash
python ./subdomainer.py -i subdomains_nc.txt -d example.com -n 10000 -o subdomains
```
* Continue from line 10,001:
```bash
python ./subdomainer.py -i subdomains_nc.txt -d example.com -s 10001
```

## SubdomainDataExtractor
Collects data about a given list of subdomains. Useful to run on the output of Subdomainer.
### Usage examples:
* Basic usage, scanning www.example.com and mail.example.com:
```bash
echo "www
mail" > subdomains
./extractor.sh subdomains example.com
```
* Using the one-click, no-questions-asked script, including running Subdomainer:
```bash
./simple_run.sh example.com
```

## PHPObfuscator
Very simple obfuscator for php. I mainly wrote it for the sport, but it's not nearly as good as some of the other ones out there, like fopo.com.ar
### Usage:
```bash
python obfuscate.py my.php
```
