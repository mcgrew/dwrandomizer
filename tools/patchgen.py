#!/usr/bin/env python3

from sys import argv,exit
from ips import Patch


def main():
    if len(argv) < 3:
        print(f"Usage: {argv[0]} <original rom> <modified rom>")
        exit(1)
    with open(argv[1], 'rb') as original_file:
        original = original_file.read()

    expansion = None
    with open(argv[2], 'rb') as changed_file:
        changed = changed_file.read()
        if len(changed) > 0x14010:
            expansion = changed[0xc010:0x1c010]
            changed = changed[:0xc010] + changed[0x1c010:]

    p = Patch.create(original, changed)
    for r in p.records:
        if r.address > 0x10:
            print("    vpatch(rom, 0x%05x, %4d," %
                    (r.address - 0x10, len(r.content)), end=" ")
        else:
            print("    pvpatch(&rom->header[0x%05x], %4d," %
                (r.address, len(r.content)), end=" ")
        for i,b in enumerate(r.content[:-1]):
            if not i % 12:
                print("\n       ", end="")
            print(" 0x%02x," % b, end="")
        print(" 0x%02x);" % r.content[-1])

    if expansion:
        empty = [0xff] * len(expansion)
        p = Patch.create(empty, expansion)
        for r in p.records:
            print("    pvpatch(&rom->expansion[0x%05x], %4d," %
                    (r.address, len(r.content)), end=" ")
            for i,b in enumerate(r.content[:-1]):
                if not i % 12:
                    print("\n       ", end="")
                print(" 0x%02x," % b, end="")
            print(" 0x%02x);" % r.content[-1])


if __name__ == "__main__":
    main()




