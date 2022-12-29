// memcpy hell

#ifndef FAT_STR_H
#define FAT_STR_H

#include <memory>
#include <stdexcept>
#include <utility>
#include <cstring>
#include <cstdint>

namespace fat_str {
class fat_str {
public:
    // ===== Constructors and Destructors =====

    fat_str() noexcept {}
    fat_str(const char *str, size_t n) : fat_str(str, n, false) {}
    fat_str(char ch, size_t count) : fat_str(&ch, count, true) {}
    fat_str(const char *str) : fat_str(str, std::strlen(str), false) {}
    fat_str(const fat_str& other) : fat_str(other.data(), other.size(), false) {}
    fat_str(const fat_str& other, size_t count) : fat_str(other.data(), std::min(other.size(), count), false) {}

    fat_str(fat_str&& other) {
        if (this != &other) {
            std::swap(m_ptr, other.m_ptr);
            delete[] other.m_ptr;
            other.m_ptr = nullptr;
        }
    }

    ~fat_str() {
        if (m_ptr != nullptr) {
            delete[] m_ptr;
            m_ptr = nullptr;
        }
    }

    fat_str(std::nullptr_t) = delete;
    fat_str(std::nullptr_t, size_t) = delete;

    // ===== Assign =====

    fat_str& assign(const char *str, size_t count) {
        return assign_impl(str, count, /* check_count */ true, /* from_char */ false);
    }

    fat_str& assign(const char *str) {
        return assign_impl(str, std::strlen(str), false, false);
    }

    fat_str& assign(char ch) {
        return assign_impl(&ch, 1, false, false);
    }

    fat_str& assign(char ch, size_t count) {
        return assign_impl(&ch, count, false, true);
    }

    fat_str& assign(const fat_str& other) {
        if (this != &other)
            *this = assign_impl(other.data(), other.size(), false, false);
        return *this;
    }

    fat_str& assign(const fat_str& other, size_t count) {
        if (this != &other)
            *this = assign_impl(other.data(), std::min(other.size(), count), false, false);
        return *this;
    }

    fat_str& assign(std::nullptr_t) = delete;
    fat_str& assign(std::nullptr_t, size_t) = delete;

    // ===== Assignment =====

    fat_str& operator=(const char *str) {
        return assign(str, std::strlen(str));
    }

    fat_str& operator=(const fat_str& other) {
        const size_t count = other.size();
        if (this != &other && count > 0)
            *this = assign(other.data(), count);
        return *this;
    }

    fat_str& operator=(fat_str&& other) {
        if (this != &other) {
            std::swap(m_ptr, other.m_ptr);
            delete[] other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    fat_str& operator=(std::nullptr_t) = delete;

    // ===== Element access =====

    constexpr char& at(size_t pos) {
        if (pos >= this->size())
            throw std::out_of_range("fat_str::at(size_t): index is out-of-range");
        size_t data_off = sizeof(uint32_t) * 2;
        return m_ptr[data_off + pos];
    }

    constexpr char& operator[](size_t pos) noexcept {
        // UB if m_ptr is nullptr and pos >= size()
        size_t data_off = sizeof(uint32_t) * 2;
        return m_ptr[data_off + pos];
    }

    constexpr char front() const noexcept {
        size_t data_off = sizeof(uint32_t) * 2;
        return m_ptr ? m_ptr[data_off] : '\0';
    }

    constexpr char back() const noexcept {
        char c = '\0';
        size_t sz = this->size(), data_off = sizeof(uint32_t) * 2;
        if (sz > 0)
            c = m_ptr[data_off + sz - 1];
        return c;
    }

    constexpr const char *data() const noexcept {
        size_t data_off = sizeof(uint32_t) * 2;
        return m_ptr ? static_cast<const char*>(m_ptr + data_off) : "";
    }

    // ===== Capacity =====

    constexpr bool empty() const noexcept {
        return this->size() == 0;
    }

    constexpr size_t size() const noexcept {
        size_t size = 0;
        if (m_ptr != nullptr)
            std::memcpy(&size, m_ptr + sizeof(uint32_t), sizeof(uint32_t));
        return size;
    }

    constexpr size_t length() const noexcept {
        return this->size();
    }

    constexpr size_t max_size() const noexcept {
        return UINT32_C(536870912);
    }

    void reserve(size_t new_cap) {
        if (new_cap > this->max_size())
            throw std::length_error("fat_str::reserve(size_t): new capacity reached max_size(): 536870912 bytes");
        if (new_cap <= this->capacity())
            return;
        size_t data_off = sizeof(uint32_t) * 2;
        char *nptr = new char[data_off + new_cap + 1]();
        std::memcpy(nptr, &new_cap, sizeof(uint32_t)); // capacity
        if (m_ptr != nullptr) { // length and data
            size_t this_size = this->size();
            std::memcpy(nptr + sizeof(uint32_t), &this_size, sizeof(uint32_t));
            std::memcpy(nptr + data_off, m_ptr + data_off, this_size);
            delete[] m_ptr;
        }
        m_ptr = nptr;
    }

    constexpr size_t capacity() const noexcept {
        size_t cap = 0;
        if (m_ptr != nullptr)
            std::memcpy(&cap, m_ptr, sizeof(uint32_t));
        return cap;
    }

    // ===== Operations =====

    constexpr void clear() noexcept {
        size_t sz = size();
        if (sz > 0) { // don't clear capacity
            size_t clr_size = sizeof(uint32_t) + sz;
            std::memset(m_ptr + sizeof(uint32_t), 0, clr_size);
        }
    }

    constexpr void push_back(char ch) noexcept {
        size_t sz = this->size();
        if (sz == 0 || sz == capacity())
            this->reserve(sz + (sz ? 16 : 32));
        size_t pos = sizeof(uint32_t) * 2 + sz;
        m_ptr[pos] = ch;
        sz += 1;
        std::memcpy(m_ptr + sizeof(uint32_t), &sz, sizeof(uint32_t));
    }

    constexpr void pop_back() noexcept {
        size_t sz = size();
        if (sz > 0) {
            size_t pos = sizeof(uint32_t) * 2 + sz - 1;
            m_ptr[pos] = 0;
            sz -= 1;
            std::memcpy(m_ptr + sizeof(uint32_t), &sz, sizeof(uint32_t));
        }
    }

    constexpr void swap(fat_str& other) noexcept {
        if (this != &other)
            std::swap(m_ptr, other.m_ptr);
    }

    // ===== Other =====

    constexpr bool operator==(const fat_str& other) const noexcept {
        bool result = false;
        if (this == &other) {
            result = true;
        } else if (m_ptr != nullptr && other.m_ptr != nullptr) {
            size_t data_off = sizeof(uint32_t) * 2;
            const char *this_str = m_ptr + data_off;
            const char *other_str = other.m_ptr + data_off;
            result = strcmp(this_str, other_str) == 0;
        }
        return result;
    }

    constexpr bool operator==(const char *str) const noexcept {
        bool result = false;
        if (m_ptr != nullptr && str != nullptr) {
            const char *this_str = m_ptr + sizeof(uint32_t) * 2;
            result = strcmp(this_str, str) == 0;
        }
        return result;
    }

    constexpr bool operator!=(const fat_str& other) const noexcept {
        return !(*this == other);
    }

    constexpr bool operator!=(const char *str) const noexcept {
        return !(*this == str);
    }

private:
    char *m_ptr = nullptr;

    fat_str(const char *str, size_t n, bool from_char) {
        size_t cap = n < 32 ? 32 : n;
        if (cap > this->max_size())
            throw std::length_error("fat_str(): string size reached max size: 536870912 bytes");
        size_t data_off = sizeof(uint32_t) * 2;
        m_ptr = new char[data_off + cap + 1]();
        std::memcpy(m_ptr, &cap, sizeof(uint32_t)); // capacity
        std::memcpy(m_ptr + sizeof(uint32_t), &n, sizeof(uint32_t)); // length
        if (n > 0) { // after moved 'str' is nullptr, so check it
            if (from_char)
                std::memset(m_ptr + data_off, str[0], n);
            else
                std::memcpy(m_ptr + data_off, str, n); // data
        }
    }

    fat_str& assign_impl(const char *str, size_t count, bool check_count, bool from_char) {
        if (check_count)
            count = std::min(std::strlen(str), count);
        if (count > this->max_size())
            throw std::length_error("fat_str::assign(): string size reached max_size(): 536870912 bytes");
        size_t this_cap = this->capacity();
        size_t this_size = this->size();
        size_t data_off = sizeof(uint32_t) * 2;
        if (this_cap < count) { // not enough space to hold new string
            delete[] m_ptr;
            size_t cap = count < 32 ? 32 : count;
            m_ptr = new char[data_off + cap + 1]();
            std::memcpy(m_ptr, &cap, sizeof(uint32_t)); // update capacity
        } else if (this_size > count) { // zero out unused bytes
            std::memset(m_ptr + data_off + count, 0, this_size - count);
        }
        std::memcpy(m_ptr + sizeof(uint32_t), &count, sizeof(uint32_t)); // update length
        if (from_char)
            std::memset(m_ptr + data_off, str[0], count);
        else
            std::memcpy(m_ptr + data_off, str, count);
        return *this;
    }
};
};

#endif // FAT_STR_H
