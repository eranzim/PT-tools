#!/bin/python
# Rate on my computer was about 10.06 subdomains/sec.
# For large input files, it is recommended to sort them from most to least probable (if possible), not alphabetically.
# Arg #1: input file
# Arg #2: domain
import sys
import socket


def main(argv):
    with open(argv[1], "rb") as f:
        options = f.read().splitlines()

    for option in options:
        try:
            socket.gethostbyname("{}.{}".format(option, argv[2]))
            print option
        except socket.gaierror:
            pass

if __name__ == "__main__":
    assert len(sys.argv) == 3
    main(sys.argv)
