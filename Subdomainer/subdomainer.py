#!/bin/python
import sys
import socket
import argparse
import traceback

DESCRIPTION = """\
Subdomain enumeration tool - find subdomains for a given domain.
Sends DNS queries for each subdomain in a given input list, outputs it if it exists.
The input file should contain optional subdomains (such as www), separated by newlines.

Note: For large input files, it is recommended to sort them from most to least probable (if possible),
        not alphabetically.
"""


def args_post_processing(args):
    # Make it 0-based
    args.s -= 1

    if not args.to_stdout and args.o is None:
        print("WARNING: no output is generated from this script (-q specified without -o)")

    return args


def positive_int(s):
    i = int(s)
    if i <= 0:
        raise argparse.ArgumentTypeError("{} is negative".format(repr(s)))
    return i


def parse_args():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, description=DESCRIPTION)

    parser.add_argument('-i', '--input', required=True, help='Input file path', metavar="IN_FILE", dest='i')
    parser.add_argument('-d', '--domain', required=True, help='Domain to search, e.g. example.com', dest='domain')

    parser.add_argument('-s', '--start-line', type=positive_int, default=1,
                        help='Start from this line (1-based) in input file', dest='s')
    parser.add_argument('-n', '--num-lines', type=positive_int,
                        help='Read (at most) this many lines from input file', dest='n')

    parser.add_argument('-q', '--quiet', action='store_false', default=True,
                        help='Don\'t output to screen', dest='to_stdout')

    parser.add_argument('-o', '--out', help='Output file path', metavar="OUT_FILE", dest='o')

    args = parser.parse_args()

    args = args_post_processing(args)

    return args


def main(args):
    with open(args.i, "rb") as f:
        options = f.read().splitlines()

    if args.s > 0:
        options = options[args.s:]
    if args.n is not None:
        options = options[:args.n]

    if args.o is not None:
        # Truncate output file
        open(args.o, 'w').close()

    # Initialize to avoid warning: Unbound local variable - Local variable 'option' might be referenced before
    # assignment (in except block)
    option = None
    # Rate on my computer was about 10.06 subdomains/sec.
    try:
        for option in options:
            try:
                socket.gethostbyname("{}.{}".format(option, args.domain))
                if args.to_stdout:
                    print(option)
                if args.o is not None:
                    with open(args.o, "a") as o:
                        o.write(option + "\n")
            except socket.gaierror:
                # Domain doesn't exist
                pass
            except Exception:
                # e.g. if opening output file fails, don't stop the script, but do print the error (to stderr).
                sys.stderr.write("\nError during subdomain {}: \n".format(option))
                traceback.print_exc()
    except KeyboardInterrupt:
        # If we're interrupted in the middle, print where we stopped
        sys.stderr.write("\nInterrupted during check of subdomain \"{}\"\n\n".format(option))
        raise


if __name__ == "__main__":
    parsed_args = parse_args()
    main(parsed_args)
