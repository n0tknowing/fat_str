// pointer shenanigans, but it's fun.
// alignment? well... let c++ allocator do the job :-)

#include <memory>
#include <stdexcept>
#include <cstring>
#include <cstdint>

namespace fat_str {
class fat_str {
public:
    // ===== ctor, dtor, copy and move ctor, copy and move assignment, assign =====

    fat_str() {}

    fat_str(const char *str) {
        size_t size = str ? std::strlen(str) : 0;
        if (size > UINT32_C(536870912)) // 512 MiB
            size = UINT32_C(536870912);
        m_ptr = new char[sizeof(uint32_t) + size + 1]();
        std::memcpy(static_cast<void*>(m_ptr), static_cast<void*>(&size), sizeof(uint32_t));
        if (str != nullptr) {
            m_ptr += sizeof(uint32_t);
            std::memcpy(static_cast<void*>(m_ptr), static_cast<const void*>(str), size);
            m_ptr -= sizeof(uint32_t);
        }
    }

    fat_str(const fat_str& other) {
        if (this != &other && other.m_ptr != nullptr) {
            size_t this_size = size();
            size_t other_size = other.size();
            if (this_size < other_size) {
                if (this_size > 0)
                    delete[] m_ptr;
                m_ptr = new char[sizeof(uint32_t) + other_size + 1]();
            } else {
                // this_size >= other_size
                //
                // zero out unused bytes. (0 is '\0' here)
                //   m_ptr       = |abcdefghijk0|  <-- before
                //   other.m_ptr = |qwerty0|
                // to
                //   m_ptr       = |abcdef000000|  <-- after
                //   other.m_ptr = |qwerty0|
                m_ptr += sizeof(uint32_t);
                std::memset(static_cast<void*>(m_ptr + other_size), 0, this_size - other_size);
                m_ptr -= sizeof(uint32_t);
            }
            // copy other's size() and data()
            size_t copy_size = sizeof(uint32_t) + other_size;
            std::memcpy(static_cast<void*>(m_ptr), static_cast<void*>(other.m_ptr), copy_size);

            // result should be like this if this_size >= other_size:
            //   m_ptr       = |qwerty000000|
            //   other.m_ptr = |qwerty0|
        }
    }

    ~fat_str() {
        if (m_ptr != nullptr) {
            delete[] m_ptr;
            m_ptr = nullptr;
        }
    }

    // ===== Element access =====

    char& at(size_t pos) {
        if (pos >= size())
            throw std::out_of_range("");
        return m_ptr[sizeof(uint32_t) + pos];
    }

    char& operator[](size_t pos) {
        // UB if m_ptr is nullptr and pos >= size()
        return m_ptr[sizeof(uint32_t) + pos];
    }

    char& front() {
        // UB if m_ptr is nullptr but it will returns '\0' if empty()
        return m_ptr[sizeof(uint32_t)];
    }

    char& back() {
        // UB if m_ptr is nullptr but it will returns '\0' if empty()
        size_t sz = size();
        sz = sz > 0 ? sz - 1 : 1;
        return m_ptr[sizeof(uint32_t) + sz];
    }

    const char *data() const {
        return static_cast<const char*>(m_ptr + sizeof(uint32_t));
    }

    // ===== Capacity =====

    bool empty() const {
        return size() == 0;
    }

    size_t size() const {
        size_t size = 0;
        if (m_ptr != nullptr)
            std::memcpy(static_cast<void*>(&size), static_cast<void*>(m_ptr), sizeof(uint32_t));
        return size;
    }

    size_t max_size() const {
        return UINT32_C(536870912);
    }

    size_t length() const {
        return size();
    }

    // ===== Other =====

    bool operator==(const fat_str& other) const {
        bool result = false;
        if (m_ptr != nullptr && other.m_ptr != nullptr) {
            if (this != &other)
                result = strcmp(m_ptr + sizeof(uint32_t), other.m_ptr + sizeof(uint32_t)) == 0;
            else
                result = true;
        }
        return result;
    }

    bool operator!=(const fat_str& other) const {
        return !(*this == other);
    }

private:
    char *m_ptr = nullptr;
};
};