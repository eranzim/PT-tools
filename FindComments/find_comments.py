#!/bin/python
import os
import sys
import bs4

DIR_INDEX = 1
SHOW_ERRORS = True

def main():
    all_comments = []
    
    for directory, subdirectories, files in os.walk(sys.argv[DIR_INDEX]):
        for file in files:
            file_path = os.path.join(directory, file)
            try:
                with open(file_path, "rb") as f:
                    soup = bs4.BeautifulSoup(f.read(), 'lxml')
                all_comments += soup.findAll(text=lambda text:isinstance(text, bs4.Comment))
            #except bs4.HTMLParser.HTMLParseError:
            except IOError:
                if SHOW_ERRORS:
                    sys.stderr.write("Failed parsing file \"{}\"\n".format(file_path))
    
    all_comments = list(set(all_comments))
    all_comments.sort()
    for comment in all_comments:
        print comment

if __name__ == "__main__":
    assert len(sys.argv) == 2, "USAGE: find_comments.py <containing_dir>"
    main()