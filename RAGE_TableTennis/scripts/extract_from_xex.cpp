// Standalone tool to extract function boundaries from XEX files
// Uses XenonRecomp's existing infrastructure

#include <cstdio>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <fstream>
#include "../XenonUtils/file.h"
#include "../XenonUtils/image.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input.xex> <output_functions.h> [GAME_NAME]\n", argv[0]);
        return 1;
    }
    
    const char* xexPath = argv[1];
    const char* outputPath = argv[2];
    const char* gameName = argc > 3 ? argv[3] : "GAME";
    
    printf("[*] Loading XEX: %s\n", xexPath);
    auto file = LoadFile(xexPath);
    if (file.empty()) {
        printf("[-] Failed to load XEX file\n");
        return 1;
    }
    
    auto image = Image::ParseImage(file.data(), file.size());
    printf("[*] Entry point: 0x%X\n", image.entry_point);
    
    std::vector<std::pair<uint32_t, uint32_t>> functions;
    
    // Extract from .pdata section
    auto* pdata = image.Find(".pdata");
    if (pdata) {
        size_t pdataCount = pdata->size / sizeof(IMAGE_CE_RUNTIME_FUNCTION);
        auto* pf = (IMAGE_CE_RUNTIME_FUNCTION*)pdata->data;
        
        printf("[*] Found .pdata with %zu entries\n", pdataCount);
        
        for (size_t i = 0; i < pdataCount; i++) {
            uint32_t addr = ByteSwap(pf[i].BeginAddress);
            uint32_t data = ByteSwap(pf[i].Data);
            uint32_t size = (data & 0xFF) * 4;
            
            if (addr >= 0x82000000 && addr < 0x90000000 && size > 0) {
                functions.emplace_back(addr, size);
            }
        }
    }
    
    printf("[*] Extracted %zu functions from .pdata\n", functions.size());
    
    // Sort and remove duplicates
    std::sort(functions.begin(), functions.end());
    functions.erase(std::unique(functions.begin(), functions.end()), functions.end());
    
    printf("[*] After dedup: %zu functions\n", functions.size());
    
    // Write output
    std::ofstream out(outputPath);
    out << "// AUTO-GENERATED: " << gameName << " function addresses\n";
    out << "// Total: " << functions.size() << " functions\n\n";
    out << "#pragma once\n";
    out << "#include <cstdint>\n";
    out << "#include <utility>\n\n";
    out << "constexpr size_t " << gameName << "_FUNCTION_COUNT = " << functions.size() << ";\n\n";
    out << "constexpr std::pair<uint32_t, uint32_t> " << gameName << "_FUNCTIONS[] = {\n";
    
    for (const auto& [addr, size] : functions) {
        char buf[64];
        snprintf(buf, sizeof(buf), "    {0x%08X, 0x%X},\n", addr, size);
        out << buf;
    }
    
    out << "};\n";
    out.close();
    
    printf("[+] Written to: %s\n", outputPath);
    return 0;
}
