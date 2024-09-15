#pragma once

#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

#include <system_error>

template <typename T>
class SHMStructWrapper {
    HANDLE m_handle;
    const T* m_ptr;

public:
    SHMStructWrapper(LPCWSTR shm_name) {
        HANDLE handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READONLY, 0, sizeof(T), shm_name);
        if (handle == nullptr)
            throw std::system_error(std::error_code(GetLastError(), std::system_category()), "File mapping object creation failed.");

        const void* ptr = MapViewOfFile(handle, FILE_MAP_READ, 0, 0, sizeof(T));
        if (ptr == nullptr)
            throw std::system_error(std::error_code(GetLastError(), std::system_category()), "File mmapping failed.");

        m_handle = handle;
        m_ptr = static_cast<const T*>(ptr);
    }

    ~SHMStructWrapper() {
        UnmapViewOfFile(m_ptr);
        CloseHandle(m_handle);
    }

    inline const T* operator->() const { return m_ptr; }
};