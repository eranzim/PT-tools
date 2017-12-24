#!/bin/python
import os
import sys
import bs4

DIR_INDEX = 1
SHOW_ERRORS = True
BS_PARSER = 'lxml'


def main():
    all_comments = []
    
    for directory, subdirectories, files in os.walk(sys.argv[DIR_INDEX]):
        for curr_file in files:
            file_path = os.path.join(directory, curr_file)
            try:
                with open(file_path, "rb") as f:
                    soup = bs4.BeautifulSoup(f.read(), BS_PARSER)
                all_comments += soup.findAll(text=lambda text: isinstance(text, bs4.Comment))
            except IOError:
                if SHOW_ERRORS:
                    sys.stderr.write("Failed parsing file \"{}\"\n".format(file_path))
    
    all_comments = list(set(all_comments))
    all_comments.sort()
    print "Comments:"
    for comment in all_comments:
        print comment


if __name__ == "__main__":
    assert len(sys.argv) == 2, "USAGE: find_comments.py <containing_dir>"
    main()
