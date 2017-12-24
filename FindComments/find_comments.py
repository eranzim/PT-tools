#!/bin/python
import os
import sys
import bs4
import argparse

DESCRIPTION = """\
Finds comments in web pages.
Can search in either a single file or recursively through a whole directory.
"""
PATH_INDEX = 1
BS_PARSER = 'lxml'


def extract_from_file(file_path, show_errors):
    try:
        with open(file_path, "rb") as f:
            soup = bs4.BeautifulSoup(f.read(), BS_PARSER)
        # Return a list of all the comments in the file
        return soup.findAll(text=lambda text: isinstance(text, bs4.Comment))
    except IOError:
        if show_errors:
            sys.stderr.write("Failed parsing file \"{}\"\n".format(file_path))
        return []


def print_comments(all_comments, simple_output):
    # If we haven't found anything:
    if not all_comments:
        if not simple_output:
            print "\nNo comments found... :("
        return

    # Strip, remove duplicates and sort
    all_comments = [comment.strip() for comment in all_comments]
    all_comments = list(set(all_comments))
    all_comments.sort()
    if not simple_output:
        print "\nComments:"
        print "---------\n"
    for comment in all_comments:
        print comment


def parse_args():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, description=DESCRIPTION)

    parser.add_argument('-s', '--simple', action='store_true', default=False,
                        help='Output comments only (without titles and no-comments-found messages)', dest='simple')
    parser.add_argument('-e', '--no-errors', action='store_false', default=True, help='Don\'t print errors',
                        dest='errors')
    parser.add_argument('path', help='Path to dir or file to scan', metavar="dir_or_file_path")

    args = parser.parse_args()

    return args


def main(args):
    all_comments = []
    
    if os.path.isdir(args.path):
        for directory, subdirectories, files in os.walk(args.path):
            for curr_file in files:
                file_path = os.path.join(directory, curr_file)
                all_comments += extract_from_file(file_path, args.errors)
    else:
        all_comments = extract_from_file(args.path, args.errors)

    print_comments(all_comments, args.simple)


if __name__ == "__main__":
    parsed_args = parse_args()
    main(parsed_args)
