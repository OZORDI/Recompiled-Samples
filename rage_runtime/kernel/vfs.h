/**
 * VFS - Virtual File System for static recompilation
 * Maps Xbox 360 paths to host filesystem paths
 * 
 * Default asset path: /Users/Ozordi/Downloads/xenia/xenia/RAGE/extracted
 * Can be reconfigured via SetAssetRoot()
 */

#ifndef RAGE_RUNTIME_VFS_H_
#define RAGE_RUNTIME_VFS_H_

#include <string>
#include <unordered_map>
#include <vector>
#include <cstdio>
#include <sys/stat.h>

namespace xe {
namespace vfs {

class VirtualFileSystem {
public:
    VirtualFileSystem();
    ~VirtualFileSystem();
    
    // Singleton access
    static VirtualFileSystem* shared();
    static void set_shared(VirtualFileSystem* vfs);
    
    // Configure asset root (default: RAGE/extracted)
    void SetAssetRoot(const std::string& path);
    const std::string& asset_root() const { return asset_root_; }
    
    // Add mount point: maps Xbox path prefix to host path
    // e.g., Mount("game:", "/path/to/gta_iv") 
    void Mount(const std::string& xbox_prefix, const std::string& host_path);
    void Unmount(const std::string& xbox_prefix);
    
    // Path resolution
    std::string ResolvePath(const std::string& xbox_path);
    
    // File operations
    FILE* OpenFile(const std::string& xbox_path, const char* mode);
    bool FileExists(const std::string& xbox_path);
    uint64_t GetFileSize(const std::string& xbox_path);
    
    // Directory listing (returns Xbox-style paths)
    std::vector<std::string> ListDirectory(const std::string& xbox_path);
    
private:
    static VirtualFileSystem* shared_vfs_;
    
    std::string asset_root_;
    std::unordered_map<std::string, std::string> mount_points_;
    
    std::string NormalizePath(const std::string& path);
};

// Game-specific VFS setup helpers
namespace games {
    
// Set up VFS for GTA IV
inline void SetupGTAIV(VirtualFileSystem* vfs, const std::string& extracted_root) {
    vfs->SetAssetRoot(extracted_root);
    vfs->Mount("game:", extracted_root + "/gta_iv");
    vfs->Mount("d:", extracted_root + "/gta_iv");
    vfs->Mount("cache:", extracted_root + "/gta_iv/cache");
}

// Set up VFS for GTA V
inline void SetupGTAV(VirtualFileSystem* vfs, const std::string& extracted_root) {
    vfs->SetAssetRoot(extracted_root);
    vfs->Mount("game:", extracted_root + "/gta_v");
    vfs->Mount("d:", extracted_root + "/gta_v");
}

// Set up VFS for Max Payne 3
inline void SetupMaxPayne3(VirtualFileSystem* vfs, const std::string& extracted_root) {
    vfs->SetAssetRoot(extracted_root);
    vfs->Mount("game:", extracted_root + "/max_payne");
    vfs->Mount("d:", extracted_root + "/max_payne");
}

// Set up VFS for Midnight Club LA
inline void SetupMidnightClub(VirtualFileSystem* vfs, const std::string& extracted_root) {
    vfs->SetAssetRoot(extracted_root);
    vfs->Mount("game:", extracted_root + "/midnight_club");
    vfs->Mount("d:", extracted_root + "/midnight_club");
}

}  // namespace games

}  // namespace vfs
}  // namespace xe

#endif  // RAGE_RUNTIME_VFS_H_
