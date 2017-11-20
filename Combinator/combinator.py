#!/bin/python
#####################################################
#                   Combinator.py                   #
#             Written by: Eran Zimmerman            #
#                  Date:29/11/2013                  #
#              Last modified:29/11/2013             #
#####################################################

import sys
import argparse
import itertools


DESCRIPTION = """\
Combine given words in different ways.
Useful for generating passwords.

Example:
    If the input contains the words "john" and "doe",
    Combinator will by default create the following words, and more: johndoe, JohnDoe, JOHNDOE, john_doe, John_Doe, JOHN_DOE, JohnDOE, doeJohn, doe_JOHN,...


If none of -l, -U, -C will be specified, all will be used.

If none of -u, -c will be specified, all will be used.

"""


def args_post_processing(args):
    if not (args.l or args.U or args.C):
        # All three are False
        args.l = args.U = args.C = True
    if not (args.u or args.c):
        # Both are False
        args.u = args.c = True
    if args.min > args.max:
        raise argparse.ArgumentTypeError("min argument (%d) is bigger than max argument (%d)" % (args.min, args.max, ))
    
    if 0 == args.min:
        print  # Empty string
        args.min = 1
    if 0 == args.max:
        exit(0)  # Nothing more to do here
    
    return args


def unsigned(s):
    i = int(s)
    if 0 > i:
        raise argparse.ArgumentTypeError("%r is negative" % s)
    return i


def parse_args():
    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, description=DESCRIPTION)

    parser.add_argument('-i', '--input', dest='i', metavar='infile', required=True, type=argparse.FileType('r'), help='Input file path')

    parser.add_argument('-l', '--lowercase', dest='l', action='store_true', default=False, help='Use lowercase version of the words')
    parser.add_argument('-U', '--UPPERCASE', dest='U', action='store_true', default=False, help='Use UPPERCASE version of the words')
    parser.add_argument('-C', '--CamelCase', dest='C', action='store_true', default=False, help='Use CamelCase version of the words')

    parser.add_argument('-u', '--use_underscores', dest='u', action='store_true', help='Use underscores to connect words')
    parser.add_argument('-c', '--concat', dest='c', action='store_true', help='Concatenate words without underscores')

    parser.add_argument('-min', '--min_length', dest='min', metavar='len', default=2, type=unsigned, help='Min words to concatenate together to a single result (defaults to 2)')
    parser.add_argument('-max', '--max_length', dest='max', metavar='len', default=2, type=unsigned, help='Max words to concatenate together to a single result (defaults to 2)')

    args = parser.parse_args()

    args = args_post_processing(args)
    
    return args


def exhaust_args(args):
    base_words_init = list(set([s.strip().lower() for s in args.i.readlines() if 0 < len(s.strip())]))
    if 0 == len(base_words_init):
        raise argparse.ArgumentTypeError("input file '%s' is empty!" % args.i.name)
    if len(base_words_init) < args.min:
        raise argparse.ArgumentTypeError("input file '%s' doesn't have enough words - has %d words, but minimum words in each single result is %d" % (args.i.name, len(base_words_init), args.min, ))
    args.max = min(args.max, len(base_words_init))
    
    base_words_list_of_lists = [[] for _ in range(len(base_words_init))]
    if args.l:
        for i, l in enumerate(base_words_list_of_lists):
            l.append(base_words_init[i])
    if args.U:
        for i, l in enumerate(base_words_list_of_lists):
            l.append(base_words_init[i].upper())
    if args.C:
        for i, l in enumerate(base_words_list_of_lists):
            l.append(base_words_init[i].capitalize())

    for i in range(len(base_words_list_of_lists)):
        base_words_list_of_lists[i] = list(set(base_words_list_of_lists[i]))
    
    separators = []
    if args.c:
        separators.append('')
    if args.u:
        separators.append('_')
    
    assert 0 < len(separators)
    
    return base_words_list_of_lists, separators


def intersperse(iterable, delimiter):
    it = iter(iterable)
    yield next(it)
    for x in it:
        yield delimiter
        yield x


def generate_combinations(base_words, separators, args):
    # Foreach required length
    for length in range(args.min, args.max+1):
        # itertools.permutations([[1,2], [3,4], [5,6]], 2) --> ([1, 2], [3, 4]) ([1, 2], [5, 6]) ([3, 4], [1, 2]) ([3, 4], [5, 6]) ([5, 6], [1, 2]) ([5, 6], [3, 4])
        # Warning: permutations is only iterable once
        permutations = itertools.permutations(base_words, length)
        # Foreach ordered selection of word lists ("johns" and "does")
        for perm in permutations:
            interspersed_perm = intersperse(perm, separators)
            prod = itertools.product(*interspersed_perm)
            # Finally, foreach result, print it to the output file
            for res in prod:
                # Each res is a tuple of words and separators, so we need to join it
                print ''.join(res)


def main():
    args = parse_args()
    
    base_words, separators = exhaust_args(args)
    
    generate_combinations(base_words, separators, args)

    return 0

if "__main__" == __name__:
    main()
