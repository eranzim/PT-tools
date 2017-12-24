#!/bin/python
import os
import sys
import bs4

PATH_INDEX = 1
SHOW_ERRORS = True
BS_PARSER = 'lxml'


def extract_from_file(file_path):
    try:
        with open(file_path, "rb") as f:
            soup = bs4.BeautifulSoup(f.read(), BS_PARSER)
        # Return a list of all the comments in the file
        return soup.findAll(text=lambda text: isinstance(text, bs4.Comment))
    except IOError:
        if SHOW_ERRORS:
            sys.stderr.write("Failed parsing file \"{}\"\n".format(file_path))
        return []


def print_comments(all_comments):
    # If we haven't found anything:
    if not all_comments:
        print "\nNo comments found... :("
        return
    
    # Strip, remove duplicates and sort
    all_comments = [comment.strip() for comment in all_comments]
    all_comments = list(set(all_comments))
    all_comments.sort()
    print "\nComments:"
    print "---------\n"
    for comment in all_comments:
        print comment


def main():
    all_comments = []
    
    if os.path.isdir(sys.argv[PATH_INDEX]):
        for directory, subdirectories, files in os.walk(sys.argv[PATH_INDEX]):
            for curr_file in files:
                file_path = os.path.join(directory, curr_file)
                all_comments += extract_from_file(file_path)
    else:
        all_comments = extract_from_file(sys.argv[PATH_INDEX])

    print_comments(all_comments)


if __name__ == "__main__":
    assert len(sys.argv) == 2, "USAGE: find_comments.py <dir_or_file_path>"
    main()
