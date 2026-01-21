#!/usr/bin/env python3
"""
Extract TLS data from Xbox 360 XEX files.
Based on Xenia's xex2_info.h structures.
"""

import struct
import sys
import os
import zlib

# XEX header constants
XEX2_MAGIC = b'XEX2'
XEX_HEADER_TLS_INFO = 0x00020104
XEX_HEADER_FILE_FORMAT_INFO = 0x000003FF
XEX_HEADER_COMPRESSION_INFO = 0x000005FF

def read_be32(data, offset):
    return struct.unpack('>I', data[offset:offset+4])[0]

def read_be16(data, offset):
    return struct.unpack('>H', data[offset:offset+2])[0]

def read_le32(data, offset):
    return struct.unpack('<I', data[offset:offset+4])[0]

def decompress_xex_block(compressed_data):
    """Decompress LZX or raw compressed XEX data"""
    try:
        return zlib.decompress(compressed_data, -15)
    except:
        return compressed_data

def extract_tls_from_xex(xex_path, output_path):
    print(f"[TLS Extractor] Reading: {xex_path}")
    
    with open(xex_path, 'rb') as f:
        data = f.read()
    
    # Verify XEX2 magic
    if data[:4] != XEX2_MAGIC:
        print(f"[ERROR] Not a valid XEX2 file (magic: {data[:4]})")
        return False
    
    # XEX2 header structure:
    # 0x00: magic (4 bytes) = "XEX2"
    # 0x04: module_flags (4 bytes)
    # 0x08: pe_data_offset (4 bytes)
    # 0x0C: reserved (4 bytes)
    # 0x10: security_offset (4 bytes)
    # 0x14: header_count (4 bytes)
    
    header_count = read_be32(data, 0x14)
    print(f"[TLS Extractor] Header count: {header_count}")
    
    # Optional headers start at offset 0x18
    offset = 0x18
    tls_info = None
    
    for i in range(header_count):
        # Each optional header entry is 8 bytes:
        # 0x00: key (4 bytes) - header type
        # 0x04: value/offset (4 bytes)
        header_key = read_be32(data, offset)
        header_value = read_be32(data, offset + 4)
        
        # Check if this is TLS_INFO header
        # The key format varies - check the high byte for type
        key_type = header_key & 0xFF
        key_id = (header_key >> 8) & 0xFFFFFF
        
        print(f"  Header {i}: key=0x{header_key:08X} value=0x{header_value:08X}")
        
        if header_key == XEX_HEADER_TLS_INFO:
            print(f"[TLS Extractor] Found TLS_INFO at offset 0x{header_value:08X}")
            # TLS info structure at header_value offset:
            # 0x00: slot_count (4 bytes BE)
            # 0x04: raw_data_address (4 bytes BE)
            # 0x08: data_size (4 bytes BE)
            # 0x0C: raw_data_size (4 bytes BE)
            tls_offset = header_value
            tls_info = {
                'slot_count': read_be32(data, tls_offset),
                'raw_data_address': read_be32(data, tls_offset + 4),
                'data_size': read_be32(data, tls_offset + 8),
                'raw_data_size': read_be32(data, tls_offset + 12),
            }
            break
        
        # Check for compressed header format (key & 0xFF == 0xFF means offset)
        if key_type == 0xFF:
            # This is an offset to extended header data
            pass
        elif key_type == 0x01:
            # Inline 32-bit value
            pass
        
        offset += 8
    
    if not tls_info:
        print("[TLS Extractor] No TLS_INFO header found in XEX")
        print("[TLS Extractor] This game may not use TLS variables")
        # Create empty TLS file
        with open(output_path, 'wb') as f:
            pass
        print(f"[TLS Extractor] Created empty TLS file: {output_path}")
        return True
    
    print(f"[TLS Extractor] TLS Info:")
    print(f"  Slot Count: {tls_info['slot_count']}")
    print(f"  Raw Data Address: 0x{tls_info['raw_data_address']:08X}")
    print(f"  Data Size: {tls_info['data_size']} bytes")
    print(f"  Raw Data Size: {tls_info['raw_data_size']} bytes")
    
    # The raw_data_address is a virtual address in the loaded image
    # We need to find the file offset for this address
    # This requires parsing the PE sections
    
    if tls_info['raw_data_size'] == 0:
        print("[TLS Extractor] TLS has no raw data (only slots)")
        # Just need slot count, create minimal TLS
        tls_data = b'\x00' * (tls_info['slot_count'] * 4)
        with open(output_path, 'wb') as f:
            f.write(tls_data)
        print(f"[TLS Extractor] Created TLS file with {len(tls_data)} zero bytes for {tls_info['slot_count']} slots")
        return True
    
    # Get PE data offset to find sections
    pe_data_offset = read_be32(data, 0x08)
    print(f"[TLS Extractor] PE data at offset: 0x{pe_data_offset:08X}")
    
    # For compressed XEX, the PE data may need decompression
    # For now, try to find TLS data in the raw file
    
    # The raw_data_address is typically a virtual address like 0x82XXXXXX
    # We need to convert this to a file offset
    # This requires understanding the XEX base address and section layout
    
    # Try to extract from security info
    security_offset = read_be32(data, 0x10)
    print(f"[TLS Extractor] Security info at offset: 0x{security_offset:08X}")
    
    # For simplicity, let's dump what we know about TLS
    # The recompiler should have access to the decompressed image
    
    # Create a TLS info file that the runtime can use
    # Format: slot_count (4 bytes LE) + raw_data_size (4 bytes LE) + raw_data
    with open(output_path, 'wb') as f:
        # Write header
        f.write(struct.pack('<I', tls_info['slot_count']))
        f.write(struct.pack('<I', tls_info['data_size']))
        f.write(struct.pack('<I', tls_info['raw_data_address']))
        # Write placeholder zeros for now
        f.write(b'\x00' * tls_info['data_size'])
    
    print(f"[TLS Extractor] Created TLS file: {output_path}")
    print(f"[TLS Extractor] NOTE: TLS data contains placeholder zeros")
    print(f"[TLS Extractor] Full extraction requires XEX decompression")
    
    return True

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <xex_file> [output.bin]")
        sys.exit(1)
    
    xex_path = sys.argv[1]
    output_path = sys.argv[2] if len(sys.argv) > 2 else 'tls_data.bin'
    
    if extract_tls_from_xex(xex_path, output_path):
        print("[TLS Extractor] Done")
    else:
        print("[TLS Extractor] Failed")
        sys.exit(1)
