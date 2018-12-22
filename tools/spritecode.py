#!/usr/bin/python

from sys import argv
from ips import Patch


def main():
    original = open(argv[1], 'rb')
    changed = open(argv[2], 'rb')
    p = Patch.create(original.read(), changed.read())
    original.close()
    changed.close()

    for r in p.records:
        print("vpatch(rom, 0x%05x, %4d, " %
                (r.address - 0x10, len(r.content)), end=" ")
        for b in r.content[:-1]:
            print("0x%02x, " % b, end=" ")
        print("0x%02x);" % r.content[-1])


if __name__ == "__main__":
    main()
        



