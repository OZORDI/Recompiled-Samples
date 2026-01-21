#!/usr/bin/env python3
"""
Find save/restore function addresses in Xbox 360 binaries.
Uses byte patterns from XenonRecomp documentation.
"""

import struct
import sys

def read_u32_be(data, offset):
    return struct.unpack('>I', data[offset:offset+4])[0]

def find_pattern(data, pattern, start=0):
    """Find byte pattern in data, return all offsets."""
    results = []
    offset = start
    while True:
        idx = data.find(pattern, offset)
        if idx == -1:
            break
        results.append(idx)
        offset = idx + 1
    return results

def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <input.bin>")
        sys.exit(1)
    
    with open(sys.argv[1], 'rb') as f:
        data = f.read()
    
    # Base address for Xbox 360 executables
    base = 0x82000000
    
    # Patterns from README.md documentation:
    patterns = {
        'restgprlr_14': bytes.fromhex('e9c1ff68'),  # ld r14, -0x98(r1)
        'savegprlr_14': bytes.fromhex('f9c1ff68'),  # std r14, -0x98(r1)
        'restfpr_14':   bytes.fromhex('c9ccff70'),  # lfd f14, -0x90(r12)
        'savefpr_14':   bytes.fromhex('d9ccff70'),  # stfd r14, -0x90(r12)
        'restvmx_14':   bytes.fromhex('3960fee07dcb60ce'),  # li r11, -0x120; lvx v14, r11, r12
        'savevmx_14':   bytes.fromhex('3960fee07dcb61ce'),  # li r11, -0x120; stvx v14, r11, r12
        'restvmx_64':   bytes.fromhex('3960fc00100b60cb'),  # li r11, -0x400; lvx128 v64, r11, r12
        'savevmx_64':   bytes.fromhex('3960fc00100b61cb'),  # li r11, -0x400; stvx128 v64, r11, r12
    }
    
    print("[*] Searching for save/restore function patterns...")
    print()
    
    results = {}
    for name, pattern in patterns.items():
        offsets = find_pattern(data, pattern)
        if offsets:
            # Take the first match as the function start
            addr = base + offsets[0]
            results[name] = addr
            print(f"{name}_address = 0x{addr:08X}  # Found at offset 0x{offsets[0]:X}")
        else:
            print(f"# {name} NOT FOUND")
    
    print()
    print("[*] TOML config snippet:")
    print()
    for name, addr in results.items():
        print(f'{name}_address = 0x{addr:08X}')

if __name__ == "__main__":
    main()
