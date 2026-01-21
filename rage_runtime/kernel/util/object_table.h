/**
 * Object Table - Handle to Object mapping
 * Adapted from Xenia for static recompilation
 */

#ifndef RAGE_RUNTIME_OBJECT_TABLE_H_
#define RAGE_RUNTIME_OBJECT_TABLE_H_

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "../xobject.h"

namespace xe {
namespace kernel {
namespace util {

class ObjectTable {
public:
    ObjectTable();
    ~ObjectTable();
    
    void Reset();
    
    X_STATUS AddHandle(XObject* object, X_HANDLE* out_handle);
    X_STATUS DuplicateHandle(X_HANDLE orig, X_HANDLE* out_handle);
    X_STATUS RetainHandle(X_HANDLE handle);
    X_STATUS ReleaseHandle(X_HANDLE handle);
    X_STATUS RemoveHandle(X_HANDLE handle);
    
    template <typename T>
    object_ref<T> LookupObject(X_HANDLE handle) {
        auto object = LookupObject(handle, false);
        if (object) {
            // Could add type checking here
        }
        return object_ref<T>(reinterpret_cast<T*>(object));
    }
    
    X_STATUS AddNameMapping(const std::string& name, X_HANDLE handle);
    void RemoveNameMapping(const std::string& name);
    X_STATUS GetObjectByName(const std::string& name, X_HANDLE* out_handle);
    
    std::vector<object_ref<XObject>> GetAllObjects();
    
private:
    struct ObjectTableEntry {
        int handle_ref_count = 0;
        XObject* object = nullptr;
    };
    
    ObjectTableEntry* LookupTable(X_HANDLE handle);
    XObject* LookupObject(X_HANDLE handle, bool already_locked);
    
    X_HANDLE TranslateHandle(X_HANDLE handle);
    
    static constexpr uint32_t GetHandleSlot(X_HANDLE handle) {
        return (handle - XObject::kHandleBase) >> 2;
    }
    
    X_STATUS FindFreeSlot(uint32_t* out_slot);
    bool Resize(uint32_t new_capacity);
    
    std::recursive_mutex mutex_;
    uint32_t table_capacity_ = 0;
    ObjectTableEntry* table_ = nullptr;
    uint32_t last_free_entry_ = 0;
    std::unordered_map<std::string, X_HANDLE> name_table_;
};

}  // namespace util
}  // namespace kernel
}  // namespace xe

#endif  // RAGE_RUNTIME_OBJECT_TABLE_H_
