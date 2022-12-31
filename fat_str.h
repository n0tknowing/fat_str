// memcpy hell

#ifndef FAT_STR_H
#define FAT_STR_H

#include <memory>
#include <stdexcept>
#include <utility>
#include <cstdint>

namespace fat_str {
class fat_str {
public:
    // ===== Constructors and Destructors =====

    fat_str() noexcept {}
    fat_str(const char *str, size_t n) : fat_str(str, n, false) {}
    fat_str(char ch, size_t count) : fat_str(&ch, count, true) {}
    fat_str(const char *str) : fat_str(str, __builtin_strlen(str), false) {}
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
        return assign_impl(str, __builtin_strlen(str), false, false);
    }

    fat_str& assign(char ch) {
        return assign_impl(&ch, 1, false, true);
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
        return assign(str, __builtin_strlen(str));
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
        return m_ptr[m_data_offset + pos];
    }

    constexpr char& operator[](size_t pos) noexcept {
        // UB if m_ptr is nullptr and pos >= size()
        return m_ptr[m_data_offset + pos];
    }

    constexpr char front() const noexcept {
        return m_ptr ? m_ptr[m_data_offset] : '\0';
    }

    constexpr char back() const noexcept {
        char c = '\0';
        const size_t sz = this->size();
        if (sz > 0)
            c = m_ptr[m_data_offset + sz - 1];
        return c;
    }

    constexpr const char *data() const noexcept {
        return m_ptr ? static_cast<const char*>(m_ptr + m_data_offset) : "";
    }

    // ===== Capacity =====

    constexpr bool empty() const noexcept {
        return this->size() == 0;
    }

    constexpr size_t size() const noexcept {
        size_t size = 0;
        if (m_ptr != nullptr)
            __builtin_memcpy(&size, m_ptr + sizeof(uint32_t), sizeof(uint32_t));
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
        new_cap = new_cap < 32 ? 32 : new_cap;
        char *nptr = new char[m_data_offset + new_cap + 1]();
        __builtin_memcpy(nptr, &new_cap, sizeof(uint32_t)); // capacity
        if (m_ptr != nullptr) { // length and data
            const size_t this_old_size = this->size();
            __builtin_memcpy(nptr + sizeof(uint32_t), &this_old_size, sizeof(uint32_t));
            __builtin_memcpy(nptr + m_data_offset, m_ptr + m_data_offset, this_old_size);
            delete[] m_ptr;
        }
        m_ptr = nptr;
    }

    constexpr size_t capacity() const noexcept {
        size_t cap = 0;
        if (m_ptr != nullptr)
            __builtin_memcpy(&cap, m_ptr, sizeof(uint32_t));
        return cap;
    }

    // ===== Operations =====

    constexpr void clear() noexcept {
        const size_t sz = size();
        if (sz > 0) { // don't clear capacity
            const size_t clr_size = sizeof(uint32_t) + sz;
            __builtin_memset(m_ptr + sizeof(uint32_t), 0, clr_size);
        }
    }

    constexpr void push_back(char ch) noexcept {
        size_t sz = this->size();
        if (sz == 0 || sz == capacity())
            this->reserve(sz + 32);
        const size_t pos = m_data_offset + sz;
        m_ptr[pos] = ch;
        sz += 1;
        __builtin_memcpy(m_ptr + sizeof(uint32_t), &sz, sizeof(uint32_t));
    }

    constexpr void pop_back() noexcept {
        size_t sz = size();
        if (sz > 0) {
            const size_t pos = m_data_offset + sz - 1;
            m_ptr[pos] = 0;
            sz -= 1;
            __builtin_memcpy(m_ptr + sizeof(uint32_t), &sz, sizeof(uint32_t));
        }
    }

    fat_str& append(const char *str) {
        bool is_this = false;
        if (str == this->data())
            is_this = true;
        return append_impl(str, __builtin_strlen(str), false, false, is_this);
    }

    fat_str& append(const char *str, size_t count) {
        bool is_this = false;
        if (str == this->data())
            is_this = true;
        return append_impl(str, count, true, false, is_this);
    }

    fat_str& append(char ch) {
        return append_impl(&ch, 1, false, true, false);
    }

    fat_str& append(char ch, size_t count) {
        return append_impl(&ch, count, false, true, false);
    }

    fat_str& append(const fat_str& other) {
        bool is_this = false;
        if (this == &other)
            is_this = true;
        return append_impl(other.data(), other.size(), false, false, is_this);
    }

    fat_str& append(const fat_str& other, size_t count) {
        bool is_this = false;
        count = std::min(other.size(), count);
        if (this == &other)
            is_this = true;
        return append_impl(other.data(), count, false, false, is_this);
    }

    fat_str& append(std::nullptr_t) = delete;
    fat_str& append(std::nullptr_t, size_t) = delete;

    fat_str& operator+=(const char *str) {
        bool is_this = false;
        if (str == this->data())
            is_this = true;
        return append_impl(str, __builtin_strlen(str), false, false, is_this);
    }

    fat_str& operator+=(char ch) {
        return append_impl(&ch, 1, false, true, false);
    }

    fat_str& operator+=(fat_str& other) {
        bool is_this = false;
        if (this == &other)
            is_this = true;
        return append_impl(other.data(), other.size(), false, false, is_this);
    }

    fat_str& operator+=(std::nullptr_t) = delete;

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
            const char *this_str = m_ptr + m_data_offset;
            const char *other_str = other.m_ptr + m_data_offset;
            result = __builtin_strcmp(this_str, other_str) == 0;
        }
        return result;
    }

    constexpr bool operator==(const char *str) const noexcept {
        bool result = false;
        if (m_ptr != nullptr && str != nullptr) {
            const char *this_str = m_ptr + m_data_offset;
            result = __builtin_strcmp(this_str, str) == 0;
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
    static constexpr size_t m_data_offset = sizeof(uint32_t) + sizeof(uint32_t);

    fat_str(const char *str, size_t n, bool from_char) {
        const size_t cap = n < 32 ? 32 : n;
        if (cap > this->max_size())
            throw std::length_error("fat_str(): string size reached max size: 536870912 bytes");
        m_ptr = new char[m_data_offset + cap + 1]();
        __builtin_memcpy(m_ptr, &cap, sizeof(uint32_t)); // capacity
        __builtin_memcpy(m_ptr + sizeof(uint32_t), &n, sizeof(uint32_t)); // length
        if (n > 0) { // after moved 'str' is nullptr, so check it
            if (from_char)
                __builtin_memset(m_ptr + m_data_offset, str[0], n);
            else
                __builtin_memcpy(m_ptr + m_data_offset, str, n); // data
        }
    }

    fat_str& assign_impl(const char *str, size_t count, bool check_count, bool from_char) {
        if (check_count)
            count = std::min(__builtin_strlen(str), count);
        if (count > this->max_size())
            throw std::length_error("fat_str::assign(): string size reached max_size(): 536870912 bytes");
        const size_t this_cap = this->capacity();
        const size_t this_size = this->size();
        if (this_cap < count) { // not enough space to hold new string
            const size_t cap = count < 32 ? 32 : count;
            char *nptr = new char[m_data_offset + cap + 1]();
            std::memcpy(nptr, &cap, sizeof(uint32_t)); // update capacity
            delete[] m_ptr;
            m_ptr = nptr;
        } else if (this_size > count) { // zero out unused bytes
            __builtin_memset(m_ptr + m_data_offset + count, 0, this_size - count);
        }
        __builtin_memcpy(m_ptr + sizeof(uint32_t), &count, sizeof(uint32_t)); // update length
        if (from_char) {
            if (count == 1)
                m_ptr[m_data_offset] = str[0];
            else
                __builtin_memset(m_ptr + m_data_offset, str[0], count);
        } else {
            __builtin_memcpy(m_ptr + m_data_offset, str, count);
        }
        return *this;
    }

    // naive implementation, there's a case when fat_str ended up reallocated
    // new memory every append_impl() call...
    fat_str& append_impl(const char *str, size_t count, bool check_count, bool from_char, bool is_this) {
        if (check_count)
            count = std::min(__builtin_strlen(str), count);
        if (count > this->max_size())
            throw std::length_error("fat_str::append(): string size reached max_size(): 536870912 bytes");
        size_t this_size = this->size();
        const size_t this_cap = this->capacity();
        const size_t total_size = this_size + count;
        char old_ch{}, *old_str{};
        if (this_cap < total_size) {
            if (is_this) { // save old string if 'str' is from 'this'
                if (count == 1) {
                    old_ch = str[0];
                } else {
                    old_str = __builtin_strndup(str, count);
                    if (old_str == nullptr)
                        throw std::runtime_error("fat_str::append(): unable to allocate more memory");
                }
            }
            this->reserve(total_size + this_cap);
        }
        __builtin_memcpy(m_ptr + sizeof(uint32_t), &total_size, sizeof(uint32_t));
        if (from_char) {
            if (count == 1)
                m_ptr[m_data_offset + this_size] = is_this ? old_ch : str[0];
            else
                __builtin_memset(m_ptr + m_data_offset + this_size, is_this ? old_ch : str[0], count);
        } else {
            __builtin_memcpy(m_ptr + m_data_offset + this_size, is_this ? old_str : str, count);
        }
        if (is_this)
            std::free(old_str);
        return *this;
    }
};
};

#endif // FAT_STR_H
