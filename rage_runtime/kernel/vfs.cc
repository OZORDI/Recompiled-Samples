/**
 * VFS Implementation
 */

#include "vfs.h"
#include <algorithm>
#include <dirent.h>

namespace xe {
namespace vfs {

VirtualFileSystem* VirtualFileSystem::shared_vfs_ = nullptr;

// Default asset root
static const char* kDefaultAssetRoot = "/Users/Ozordi/Downloads/xenia/xenia/RAGE/extracted";

VirtualFileSystem::VirtualFileSystem() 
    : asset_root_(kDefaultAssetRoot) {
    // Set up default mount points
    Mount("game:", asset_root_);
    Mount("d:", asset_root_);
}

VirtualFileSystem::~VirtualFileSystem() {
    if (shared_vfs_ == this) {
        shared_vfs_ = nullptr;
    }
}

VirtualFileSystem* VirtualFileSystem::shared() {
    return shared_vfs_;
}

void VirtualFileSystem::set_shared(VirtualFileSystem* vfs) {
    shared_vfs_ = vfs;
}

void VirtualFileSystem::SetAssetRoot(const std::string& path) {
    asset_root_ = path;
    printf("[VFS] Asset root set to: %s\n", path.c_str());
}

void VirtualFileSystem::Mount(const std::string& xbox_prefix, const std::string& host_path) {
    std::string normalized_prefix = NormalizePath(xbox_prefix);
    mount_points_[normalized_prefix] = host_path;
    printf("[VFS] Mounted %s -> %s\n", xbox_prefix.c_str(), host_path.c_str());
}

void VirtualFileSystem::Unmount(const std::string& xbox_prefix) {
    std::string normalized = NormalizePath(xbox_prefix);
    mount_points_.erase(normalized);
}

std::string VirtualFileSystem::NormalizePath(const std::string& path) {
    std::string result = path;
    
    // Convert backslashes to forward slashes
    std::replace(result.begin(), result.end(), '\\', '/');
    
    // Lowercase for case-insensitive matching
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    
    // Remove trailing slashes
    while (!result.empty() && result.back() == '/') {
        result.pop_back();
    }
    
    return result;
}

std::string VirtualFileSystem::ResolvePath(const std::string& xbox_path) {
    std::string normalized = NormalizePath(xbox_path);
    
    // Try each mount point
    for (const auto& [prefix, host_path] : mount_points_) {
        if (normalized.find(prefix) == 0) {
            // Found matching prefix
            std::string remainder = normalized.substr(prefix.length());
            if (!remainder.empty() && remainder[0] == '/') {
                remainder = remainder.substr(1);
            }
            
            std::string resolved = host_path;
            if (!remainder.empty()) {
                resolved += "/" + remainder;
            }
            
            return resolved;
        }
    }
    
    // No mount point found - try as relative path under asset root
    if (!normalized.empty() && normalized[0] != '/') {
        return asset_root_ + "/" + normalized;
    }
    
    // Return as-is (might be absolute host path)
    return xbox_path;
}

FILE* VirtualFileSystem::OpenFile(const std::string& xbox_path, const char* mode) {
    std::string host_path = ResolvePath(xbox_path);
    FILE* f = fopen(host_path.c_str(), mode);
    
    if (f) {
        printf("[VFS] Opened: %s -> %s\n", xbox_path.c_str(), host_path.c_str());
    } else {
        printf("[VFS] Failed to open: %s (tried: %s)\n", xbox_path.c_str(), host_path.c_str());
    }
    
    return f;
}

bool VirtualFileSystem::FileExists(const std::string& xbox_path) {
    std::string host_path = ResolvePath(xbox_path);
    struct stat st;
    return stat(host_path.c_str(), &st) == 0;
}

uint64_t VirtualFileSystem::GetFileSize(const std::string& xbox_path) {
    std::string host_path = ResolvePath(xbox_path);
    struct stat st;
    if (stat(host_path.c_str(), &st) == 0) {
        return static_cast<uint64_t>(st.st_size);
    }
    return 0;
}

std::vector<std::string> VirtualFileSystem::ListDirectory(const std::string& xbox_path) {
    std::vector<std::string> results;
    std::string host_path = ResolvePath(xbox_path);
    
    DIR* dir = opendir(host_path.c_str());
    if (!dir) {
        return results;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] == '.') continue;  // Skip hidden files
        results.push_back(entry->d_name);
    }
    
    closedir(dir);
    return results;
}

}  // namespace vfs
}  // namespace xe
