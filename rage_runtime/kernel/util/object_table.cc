/**
 * Object Table Implementation
 */

#include "object_table.h"
#include <algorithm>
#include <cstring>

namespace xe {
namespace kernel {
namespace util {

ObjectTable::ObjectTable() {}

ObjectTable::~ObjectTable() { Reset(); }

void ObjectTable::Reset() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    for (uint32_t n = 0; n < table_capacity_; n++) {
        ObjectTableEntry& entry = table_[n];
        if (entry.object) {
            entry.object->Release();
        }
    }
    
    table_capacity_ = 0;
    last_free_entry_ = 0;
    free(table_);
    table_ = nullptr;
    name_table_.clear();
}

X_STATUS ObjectTable::FindFreeSlot(uint32_t* out_slot) {
    uint32_t slot = last_free_entry_;
    uint32_t scan_count = 0;
    
    while (scan_count < table_capacity_) {
        ObjectTableEntry& entry = table_[slot];
        if (!entry.object) {
            *out_slot = slot;
            return X_STATUS_SUCCESS;
        }
        scan_count++;
        slot = (slot + 1) % table_capacity_;
        if (slot == 0) {
            scan_count++;
            slot++;
        }
    }
    
    uint32_t new_table_capacity = std::max(16u * 1024u, table_capacity_ * 2);
    if (!Resize(new_table_capacity)) {
        return X_STATUS_NO_MEMORY;
    }
    
    slot = ++last_free_entry_;
    *out_slot = slot;
    return X_STATUS_SUCCESS;
}

bool ObjectTable::Resize(uint32_t new_capacity) {
    uint32_t new_size = new_capacity * sizeof(ObjectTableEntry);
    uint32_t old_size = table_capacity_ * sizeof(ObjectTableEntry);
    
    auto new_table = reinterpret_cast<ObjectTableEntry*>(realloc(table_, new_size));
    if (!new_table) {
        return false;
    }
    
    if (new_size > old_size) {
        std::memset(reinterpret_cast<uint8_t*>(new_table) + old_size, 0,
                    new_size - old_size);
    }
    
    last_free_entry_ = table_capacity_;
    table_capacity_ = new_capacity;
    table_ = new_table;
    return true;
}

X_STATUS ObjectTable::AddHandle(XObject* object, X_HANDLE* out_handle) {
    X_STATUS result = X_STATUS_SUCCESS;
    uint32_t handle = 0;
    
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        
        uint32_t slot = 0;
        result = FindFreeSlot(&slot);
        
        if (XSUCCEEDED(result)) {
            ObjectTableEntry& entry = table_[slot];
            entry.object = object;
            entry.handle_ref_count = 1;
            handle = XObject::kHandleBase + (slot << 2);
            object->handles().push_back(handle);
            object->Retain();
        }
    }
    
    if (XSUCCEEDED(result) && out_handle) {
        *out_handle = handle;
    }
    
    return result;
}

X_STATUS ObjectTable::DuplicateHandle(X_HANDLE handle, X_HANDLE* out_handle) {
    handle = TranslateHandle(handle);
    XObject* object = LookupObject(handle, false);
    if (object) {
        X_STATUS result = AddHandle(object, out_handle);
        object->Release();
        return result;
    }
    return X_STATUS_INVALID_HANDLE;
}

X_STATUS ObjectTable::RetainHandle(X_HANDLE handle) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    ObjectTableEntry* entry = LookupTable(handle);
    if (!entry) {
        return X_STATUS_INVALID_HANDLE;
    }
    
    entry->handle_ref_count++;
    return X_STATUS_SUCCESS;
}

X_STATUS ObjectTable::ReleaseHandle(X_HANDLE handle) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    ObjectTableEntry* entry = LookupTable(handle);
    if (!entry) {
        return X_STATUS_INVALID_HANDLE;
    }
    
    if (--entry->handle_ref_count == 0) {
        return RemoveHandle(handle);
    }
    
    return X_STATUS_SUCCESS;
}

X_STATUS ObjectTable::RemoveHandle(X_HANDLE handle) {
    handle = TranslateHandle(handle);
    if (!handle) {
        return X_STATUS_INVALID_HANDLE;
    }
    
    ObjectTableEntry* entry = LookupTable(handle);
    if (!entry) {
        return X_STATUS_INVALID_HANDLE;
    }
    
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    if (entry->object) {
        auto object = entry->object;
        entry->object = nullptr;
        entry->handle_ref_count = 0;
        
        auto handle_entry = std::find(object->handles().begin(), 
                                       object->handles().end(), handle);
        if (handle_entry != object->handles().end()) {
            object->handles().erase(handle_entry);
        }
        
        if (!object->name().empty()) {
            RemoveNameMapping(object->name());
        }
        
        object->Release();
    }
    
    return X_STATUS_SUCCESS;
}

ObjectTable::ObjectTableEntry* ObjectTable::LookupTable(X_HANDLE handle) {
    handle = TranslateHandle(handle);
    if (!handle) {
        return nullptr;
    }
    
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    
    uint32_t slot = GetHandleSlot(handle);
    if (slot < table_capacity_) {
        return &table_[slot];
    }
    
    return nullptr;
}

XObject* ObjectTable::LookupObject(X_HANDLE handle, bool already_locked) {
    handle = TranslateHandle(handle);
    if (!handle) {
        return nullptr;
    }
    
    XObject* object = nullptr;
    
    if (!already_locked) {
        mutex_.lock();
    }
    
    uint32_t slot = GetHandleSlot(handle);
    if (slot < table_capacity_) {
        ObjectTableEntry& entry = table_[slot];
        if (entry.object) {
            object = entry.object;
        }
    }
    
    if (object) {
        object->Retain();
    }
    
    if (!already_locked) {
        mutex_.unlock();
    }
    
    return object;
}

X_HANDLE ObjectTable::TranslateHandle(X_HANDLE handle) {
    if (handle == 0xFFFFFFFF) {
        return 0;  // CurrentProcess - not supported
    } else if (handle == 0xFFFFFFFE) {
        // CurrentThread - would need thread lookup
        return 0;
    }
    return handle;
}

X_STATUS ObjectTable::AddNameMapping(const std::string& name, X_HANDLE handle) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (name_table_.count(name)) {
        return X_STATUS_OBJECT_NAME_COLLISION;
    }
    name_table_[name] = handle;
    return X_STATUS_SUCCESS;
}

void ObjectTable::RemoveNameMapping(const std::string& name) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    name_table_.erase(name);
}

X_STATUS ObjectTable::GetObjectByName(const std::string& name, X_HANDLE* out_handle) {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto it = name_table_.find(name);
    if (it == name_table_.end()) {
        *out_handle = X_INVALID_HANDLE_VALUE;
        return X_STATUS_OBJECT_NAME_NOT_FOUND;
    }
    *out_handle = it->second;
    return X_STATUS_SUCCESS;
}

std::vector<object_ref<XObject>> ObjectTable::GetAllObjects() {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    std::vector<object_ref<XObject>> results;
    
    for (uint32_t slot = 0; slot < table_capacity_; slot++) {
        auto& entry = table_[slot];
        if (entry.object) {
            entry.object->Retain();
            results.push_back(object_ref<XObject>(entry.object));
        }
    }
    
    return results;
}

}  // namespace util
}  // namespace kernel
}  // namespace xe
