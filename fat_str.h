// pointer shenanigans, but it's fun.
// alignment? well... let c++ allocator do the job :-)

#include <memory>
#include <stdexcept>
#include <utility>
#include <cstring>
#include <cstdint>

namespace fat_str {
class fat_str {
public:
    // ===== Member functions =====

    fat_str() {}

    fat_str(const char *str) {
        size_t size = str ? std::strlen(str) : 0;
        size_t cap = size < 32 ? 32 : size;
        if (cap > UINT32_C(536870912)) // 512 MiB
            throw std::length_error("fat_str::fat_str(const char*): string size reached max size: 536870912 bytes");
        m_ptr = new char[sizeof(uint32_t) * 2 + cap + 1]();
        std::memcpy(m_ptr, &cap, sizeof(uint32_t)); // capacity
        std::memcpy(m_ptr + sizeof(uint32_t), &size, sizeof(uint32_t)); // length
        if (str != nullptr) { // data
            size_t data_off = sizeof(uint32_t) * 2;
            std::memcpy(m_ptr + data_off, str, size);
        }
    }

    fat_str(const fat_str& other) {
        if (this != &other) {
            size_t other_size = other.size();
            size_t other_cap = other.capacity();
            other_cap = other_cap < 32 ? 32 : other_cap;
            m_ptr = new char[sizeof(uint32_t) * 2 + other_cap + 1]();
            if (other_size > 0) {
                const size_t copy_size = sizeof(uint32_t) * 2 + other.size();
                std::memcpy(m_ptr, other.m_ptr, copy_size);
            } else {
                std::memcpy(m_ptr, &other_cap, sizeof(uint32_t));
            }
        }
    }

    fat_str& operator=(const fat_str& other) {
        if (this != &other && other.m_ptr != nullptr) {
            size_t this_cap = capacity();
            size_t other_size = other.size();
            size_t other_cap = other.capacity();
            size_t copy_size = sizeof(uint32_t) * 2 + other_cap;
            if (this_cap < other_size) {
                delete[] m_ptr;
                m_ptr = new char[copy_size + 1]();
            }
            std::memcpy(m_ptr, other.m_ptr, copy_size);
        }
        return *this;
    }

    fat_str& operator=(const char *str) {
        size_t str_size = str ? std::strlen(str) : 0;
        if (str_size > max_size())
            throw std::length_error("fat_str::operator=(const char*): string size reached max_size(): 536870912 bytes");
        size_t this_cap = capacity();
        size_t this_size = size();
        size_t data_off = sizeof(uint32_t) * 2;
        if (this_cap < str_size) { // not enough space to hold new string
            delete[] m_ptr;
            size_t cap = str_size < 32 ? 32 : str_size;
            m_ptr = new char[sizeof(uint32_t) * 2 + cap + 1]();
            std::memcpy(m_ptr, &cap, sizeof(uint32_t)); // update capacity
        } else if (this_size > str_size) { // zero out unused bytes
            std::memset(m_ptr + data_off + str_size, 0, this_size - str_size);
        }
        std::memcpy(m_ptr + sizeof(uint32_t), &str_size, sizeof(uint32_t)); // update length
        if (str_size > 0) // update data if it's not empty
            std::memcpy(m_ptr + data_off, str, str_size);
        return *this;
    }

    fat_str(fat_str&& other) {
        if (this != &other) {
            std::swap(m_ptr, other.m_ptr);
            delete[] other.m_ptr;
            other.m_ptr = nullptr;
        }
    }

    fat_str& operator=(fat_str&& other) {
        if (this != &other) {
            std::swap(m_ptr, other.m_ptr);
            delete[] other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
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
            throw std::out_of_range("fat_str::at(size_t): index is out-of-range");
        size_t data_off = sizeof(uint32_t) * 2;
        return m_ptr[data_off + pos];
    }

    char& operator[](size_t pos) {
        // UB if m_ptr is nullptr and pos >= size()
        size_t data_off = sizeof(uint32_t) * 2;
        return m_ptr[data_off + pos];
    }

    char front() const {
        size_t data_off = sizeof(uint32_t) * 2;
        return m_ptr ? m_ptr[data_off] : '\0';
    }

    char back() const {
        char c = '\0';
        size_t sz = size(), data_off = sizeof(uint32_t) * 2;
        if (m_ptr != nullptr && sz > 0)
            c = m_ptr[data_off + sz - 1];
        return c;
    }

    const char *data() const {
        size_t data_off = sizeof(uint32_t) * 2;
        return m_ptr ? static_cast<const char*>(m_ptr + data_off) : "";
    }

    // ===== Capacity =====

    bool empty() const {
        return size() == 0;
    }

    size_t size() const {
        size_t size = 0;
        if (m_ptr != nullptr)
            std::memcpy(&size, m_ptr + sizeof(uint32_t), sizeof(uint32_t));
        return size;
    }

    size_t length() const {
        return size();
    }

    size_t max_size() const {
        return UINT32_C(536870912);
    }

    void reserve(size_t new_cap) {
        if (new_cap > max_size())
            throw std::length_error("fat_str::reserve(size_t): new capacity reached max_size(): 536870912 bytes");
        if (new_cap <= capacity())
            return;
        char *nptr = new char[sizeof(uint32_t) * 2 + new_cap + 1]();
        std::memcpy(nptr, &new_cap, sizeof(uint32_t)); // capacity
        if (m_ptr != nullptr) { // length and data
            size_t this_size = size();
            size_t data_off = sizeof(uint32_t) * 2;
            std::memcpy(nptr + sizeof(uint32_t), &this_size, sizeof(uint32_t));
            std::memcpy(nptr + data_off, m_ptr, this_size);
            delete[] m_ptr;
        }
        m_ptr = nptr;
    }

    size_t capacity() const {
        size_t cap = 0;
        if (m_ptr != nullptr)
            std::memcpy(&cap, m_ptr, sizeof(uint32_t));
        return cap;
    }

    // ===== Other =====

    bool operator==(const fat_str& other) const {
        bool result = true;
        if (this != &other) {
            const char *this_str = m_ptr ? m_ptr + sizeof(uint32_t) : "";
            const char *other_str = other.m_ptr ? other.m_ptr + sizeof(uint32_t) : "";
            result = strcmp(this_str, other_str) == 0;
        }
        return result;
    }

    bool operator==(const char *str) const {
        const char *this_str = m_ptr ? m_ptr + sizeof(uint32_t) : "";
        return strcmp(this_str, str ? str : "") == 0;
    }

    bool operator!=(const fat_str& other) const {
        return !(*this == other);
    }

    bool operator!=(const char *str) const {
        return !(*this == str);
    }

private:
    char *m_ptr = nullptr;
};
};
