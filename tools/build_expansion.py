#!/usr/bin/env python3

from subprocess import run
from os.path import dirname, basename, realpath, join, exists
from os import chdir, remove
from shutil import which
from collections import defaultdict
from glob import glob
import re

from ips import Patch


def generate_header(patch_headers: dict[str, list[tuple[str, int]]]):
    print("Generating header file...")
    with open('../common/expansion.h', 'w') as h:
        h.write(f'/** This file is generated by {basename(__file__)} */\n')
        h.write('#ifndef _EXPANSION_H_\n')
        h.write('#define _EXPANSION_H_\n')
        h.write('\n')
        h.write('#include "dwr_types.h"\n')
        h.write('\n')
        h.write('enum hooktype {\n')
        h.write('    DIALOGUE,\n')
        h.write('    JSR = 0x20,\n')
        h.write('    JMP = 0x4c,\n')
        h.write('};\n')
        h.write('\n')
        h.write('enum subroutine {\n')
        for header, value in patch_headers.items():
            for name, addr in value:
                h.write(f'  {name.upper()} = 0x{addr:X},\n')
        h.write('};\n')
        h.write('\n')
        h.write('void add_hook(dw_rom *rom, enum hooktype type, uint16_t address,\n')
        h.write('              enum subroutine to_addr);\n')
        for name in patch_headers.keys():
            h.write(f'void {name}(dw_rom *rom);\n')
        h.write('void fill_expansion(dw_rom *rom);\n')
        h.write('\n')
        h.write('#endif\n')


def generate_c_file(patches: dict[str, list[tuple[int, bytes]]], expansion: bytes):
    print("Generating C file...")
    with open('../common/expansion.c', 'w') as c:
        c.write(f'/** This file is generated by {basename(__file__)} */\n')
        c.write('/** Assembly source available in the expansion directory */\n\n')
        c.write('#include <stdint.h>\n')
        c.write('#include <stddef.h>\n')
        c.write('#include "expansion.h"\n')
        c.write('#include "credit_music.h"\n')
        c.write('#include "dwr_types.h"\n')
        c.write('#include "patch.h"\n')
        c.write('\n')
        c.write('void add_hook(dw_rom *rom, enum hooktype type, uint16_t address,\n')
        c.write('              enum subroutine to_addr)\n')
        c.write('{\n')
        c.write('    switch(type) {\n')
        c.write('        case DIALOGUE:\n')
        c.write('            vpatch(rom, address, 4, JSR, to_addr & 0xff, to_addr >> 8, 0xea);\n')
        c.write('            break;\n')
        c.write('        case JSR:\n')
        c.write('        case JMP:\n')
        c.write('            vpatch(rom, address, 3, type, to_addr & 0xff, to_addr >> 8);\n')
        c.write('        default:\n')
        c.write('            break;\n')
        c.write('    }\n')
        c.write('}\n')
        c.write('\n')
        for name, value in patches.items():
            c.write(f'void {name}(dw_rom *rom)\n')
            c.write('{\n')
            for patch_addr, content in value:
                empty = [0xff] * len(content)
                p = Patch.create(empty, content)
                if not p.records:
                    c.write(f'    /* No patches */\n')
                for r in p.records:
                    c.write(f'    vpatch(rom, 0x{r.address+patch_addr:04x},'
                            f'{len(r.content):4d},')
                    for i, b in enumerate(r.content):
                        if i:
                            c.write(',')
                        if not i % 12:
                            c.write('\n       ')
                        c.write(f' 0x{b:02x}')
                    c.write('\n    );\n')
            c.write('}\n\n')

        c.write('void fill_expansion(dw_rom *rom)\n')
        c.write('{\n')
        empty = [0xff] * len(expansion)
        p = Patch.create(empty, expansion)
        for r in p.records:
            if r.address == 0x4000:
                continue  # single byte placeholder for music, ignore it
            c.write(f'    pvpatch(&rom->expansion[0x{r.address:04x}], '
                    f'{len(r.content):4d},')
            for i, b in enumerate(r.content):
                if i:
                    c.write(',')
                if not i % 12:
                    c.write('\n       ')
                c.write(f' 0x{b:02x}')
            c.write('\n    );\n')
        c.write('}\n\n')

def parse_listing(filename: str, limit: int = 0x10000):
    """ Parses the lst file generated by asm6 to find labels """
    labels = []
    with open(filename, 'r') as listing:
        # first find patches
        for line in listing.readlines():
            addr, name = line.strip()[1:-1].split('#')
            addr = int(addr, 16)
            if name[0].isalnum():
                labels.append((addr, name))
    yield from sorted(labels)

def main():
    chdir(join(dirname(realpath(__file__)), '..', 'expansion'))
    asm6 = which('asm6f') or which('asm6')
    if run([asm6, '-q', '-n', '-dDWR_BUILD', 'credits.asm', 'credits.nes']).returncode:
        return -1

    patches = defaultdict(list)
    patch_headers = {}
    patch_start = None
    with open('credits.nes', 'rb') as asm:
        asm.seek(0x10000 + 16, 0)
        expansion = (b'\xff' * 0x4000) + asm.read(0xc000)
        for i in range(4):
            if not exists(f'credits.nes.{i}.nl'):
                continue
            for addr, name in parse_listing(f'credits.nes.{i}.nl'):
                addr = (addr & 0x3fff) | (i * 0x4000) #addjust the address from PRG
                if re.match('patch_.*_start', name):
                    patch_name = name[:-6]
                    patch_headers[patch_name] = []
                    patch_start = addr
                elif re.match('patch_.*_end', name):
                    if patch_start is None:
                        print(f'Error: {name} found without start address!')
                        exit(-1)
                    print(f'Adding {name[:-4]} at 0x{patch_start & 0xffff:04X}')
                    asm.seek(patch_start + 16, 0)
                    patches[patch_name].append((patch_start & 0xffff, asm.read(addr - patch_start)))
                    patch_start = None
                elif patch_start is not None:
                    patch_headers[patch_name].append((name, (addr | 0x8000) & 0xffff))

    generate_header(patch_headers)
    generate_c_file(patches, expansion)
    remove('credits.nes')
    for filename in glob('*.nl'):
        remove(filename)


if __name__ == "__main__":
    main()
