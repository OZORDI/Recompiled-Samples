#!/usr/bin/env python3
"""
Extract function boundaries from Xbox 360 XEX files for XenonRecomp.
Parses the .pdata section which contains exception handling data with function boundaries.
"""

import struct
import sys
import os

def read_u32_be(data, offset):
    return struct.unpack('>I', data[offset:offset+4])[0]

def read_u32_le(data, offset):
    return struct.unpack('<I', data[offset:offset+4])[0]

def parse_pe_sections(data, pe_offset):
    """Parse PE sections from the image."""
    # COFF header is at pe_offset + 4
    coff_offset = pe_offset + 4
    num_sections = struct.unpack('<H', data[coff_offset+2:coff_offset+4])[0]
    optional_header_size = struct.unpack('<H', data[coff_offset+16:coff_offset+18])[0]
    
    # Section headers start after optional header
    section_table_offset = coff_offset + 20 + optional_header_size
    
    sections = []
    for i in range(num_sections):
        section_offset = section_table_offset + (i * 40)
        name = data[section_offset:section_offset+8].rstrip(b'\x00').decode('ascii', errors='ignore')
        virtual_size = read_u32_le(data, section_offset + 8)
        virtual_addr = read_u32_le(data, section_offset + 12)
        raw_size = read_u32_le(data, section_offset + 16)
        raw_offset = read_u32_le(data, section_offset + 20)
        
        sections.append({
            'name': name,
            'virtual_size': virtual_size,
            'virtual_addr': virtual_addr,
            'raw_size': raw_size,
            'raw_offset': raw_offset
        })
    
    return sections

def find_pdata_in_image(data):
    """Find .pdata section in the raw image binary."""
    # The image.bin files are already the loaded PE image
    # Look for PE signature
    if data[:2] == b'MZ':
        pe_offset = read_u32_le(data, 0x3C)
        if data[pe_offset:pe_offset+4] == b'PE\x00\x00':
            sections = parse_pe_sections(data, pe_offset)
            for sec in sections:
                if sec['name'] == '.pdata':
                    return sec
    return None

def extract_functions_from_pdata(data, pdata_section, base_addr=0x82000000):
    """Extract function boundaries from .pdata section."""
    functions = []
    
    offset = pdata_section['raw_offset']
    size = pdata_section['raw_size']
    
    # Each entry is 8 bytes: BeginAddress (4 bytes, BE) + Data (4 bytes, BE)
    entry_size = 8
    num_entries = size // entry_size
    
    print(f"[*] Parsing {num_entries} .pdata entries...")
    
    for i in range(num_entries):
        entry_offset = offset + (i * entry_size)
        begin_addr = read_u32_be(data, entry_offset)
        func_data = read_u32_be(data, entry_offset + 4)
        
        # Function size is in the lower byte, multiplied by 4
        func_size = (func_data & 0xFF) * 4
        
        # Validate address range
        if begin_addr >= 0x82000000 and begin_addr < 0x90000000 and func_size > 0:
            functions.append((begin_addr, func_size))
    
    return functions

def scan_bl_targets(data, text_section, known_functions):
    """Scan for bl (branch link) targets to find additional functions."""
    bl_targets = set()
    known_addrs = set(f[0] for f in known_functions)
    
    offset = text_section['raw_offset']
    size = text_section['raw_size']
    base = text_section['virtual_addr'] + 0x82000000  # Adjust base
    
    print(f"[*] Scanning .text section for bl targets ({size} bytes)...")
    
    for i in range(0, size, 4):
        insn = read_u32_be(data, offset + i)
        # bl instruction: opcode 18 (bits 26-31), LK=1 (bit 0)
        if (insn >> 26) == 18 and (insn & 1) == 1:
            # Extract offset (bits 2-25, sign-extended)
            li = (insn >> 2) & 0xFFFFFF
            if li & 0x800000:  # Sign extend
                li |= 0xFF000000
            li = struct.unpack('>i', struct.pack('>I', li << 2))[0]
            
            target = (base + i + li) & 0xFFFFFFFF
            
            if target >= 0x82000000 and target < 0x90000000:
                if target not in known_addrs:
                    bl_targets.add(target)
    
    return bl_targets

def main():
    if len(sys.argv) < 4:
        print(f"Usage: {sys.argv[0]} <input.bin> <output.h> <GAME_NAME>")
        sys.exit(1)
    
    input_path = sys.argv[1]
    output_path = sys.argv[2]
    game_name = sys.argv[3]
    
    print(f"[*] Loading: {input_path}")
    with open(input_path, 'rb') as f:
        data = f.read()
    
    print(f"[*] File size: {len(data)} bytes")
    
    pdata = find_pdata_in_image(data)
    if not pdata:
        print("[-] Could not find .pdata section")
        sys.exit(1)
    
    print(f"[*] Found .pdata: offset=0x{pdata['raw_offset']:X}, size={pdata['raw_size']}")
    
    functions = extract_functions_from_pdata(data, pdata)
    print(f"[*] Extracted {len(functions)} functions from .pdata")
    
    # Sort and deduplicate
    functions = sorted(set(functions))
    print(f"[*] After dedup: {len(functions)} functions")
    
    # Write output header
    with open(output_path, 'w') as f:
        f.write(f"// AUTO-GENERATED: {game_name} function addresses\n")
        f.write(f"// Total: {len(functions)} functions\n\n")
        f.write("#pragma once\n")
        f.write("#include <cstdint>\n")
        f.write("#include <utility>\n\n")
        f.write(f"constexpr size_t {game_name}_FUNCTION_COUNT = {len(functions)};\n\n")
        f.write(f"constexpr std::pair<uint32_t, uint32_t> {game_name}_FUNCTIONS[] = {{\n")
        
        for addr, size in functions:
            f.write(f"    {{0x{addr:08X}, 0x{size:X}}},\n")
        
        f.write("};\n")
    
    print(f"[+] Written to: {output_path}")

if __name__ == "__main__":
    main()
