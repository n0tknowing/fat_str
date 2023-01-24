# fat\_str - C++ string managed by fat pointer


## Why

Purely educational purpose (and having fun), improving my C++ skill by doing
some pointer magic.


## Pointer structure/layout

```
+----------------------+
| CCCC | LLLL |  DATA  |
+----------------------+
```

`C` = Capacity. (4 bytes, start at offset 0)<br/>
`L` = Current length. (4 bytes, start at offset 4)<br/>
`DATA` = NUL (`\0`) terminated ASCII string (0 to 512 MiB, start at offset 8).


## (Current) Public API

```cpp
namespace fat_str {
class fat_str {
public:
    fat_str() noexcept;
    fat_str(const char* c_str);
    fat_str(const char* c_str, size_t count);
    fat_str(char ch, size_t count);
    fat_str(const fat_str& other);
    fat_str(const fat_str& other, size_t count);
    fat_str(fat_str&& other);
    fat_str(std::nullptr_t) = delete;
    fat_str(std::nullptr_t, size_t) = delete;

    fat_str& operator=(const char* c_str);
    fat_str& operator=(const fat_str& other);
    fat_str& operator=(fat_str&& other);
    fat_str& operator=(std::nullptr_t) = delete;

    fat_str& assign(const char* c_str);
    fat_str& assign(const char* c_str, size_t count);
    fat_str& assign(char ch);
    fat_str& assign(char ch, size_t count);
    fat_str& assign(const char fat_str& other);
    fat_str& assign(const char fat_str& other, size_t count);
    fat_str& assign(std::nullptr_t) = delete;
    fat_str& assign(std::nullptr_t, size_t) = delete;

    constexpr char& at(size_t pos); // bound-checked
    constexpr char& operator[](size_t pos) noexcept; // UB if pos >= size()
    constexpr char front() const noexcept; // copy not reference
    constexpr char back() const noexcept; // copy not reference
    constexpr const char* data() const noexcept; // NUL-terminated

    constexpr bool empty() const noexcept;
    constexpr size_t size() const noexcept;
    constexpr size_t length() const noexcept;
    constexpr size_t max_size() const noexcept;
    constexpr size_t capacity() const noexcept;
    void reserve(size_t new_cap);

    constexpr void clear() noexcept;
    constexpr void push_back(char ch);
    constexpr void pop_back() noexcept;
    constexpr void swap(fat_str& other) noexcept;

    fat_str& append(const char* c_str);
    fat_str& append(const char* c_str, size_t count);
    fat_str& append(char ch);
    fat_str& append(char ch, size_t count);
    fat_str& append(const fat_str& other);
    fat_str& append(const fat_str& other, size_t count);
    fat_str& append(std::nullptr_t) = delete;
    fat_str& append(std::nullptr_t, size_t) = delete;

    fat_str& operator+=(const char* c_str);
    fat_str& operator+=(char ch);
    fat_str& operator+=(const fat_str& other);
    fat_str& operator+=(std::nullptr_t) = delete;

    constexpr bool operator==(const char* c_str) const noexcept;
    constexpr bool operator==(const fat_str& other) const noexcept;
    constexpr bool operator!=(const char* c_str) const noexcept;
    constexpr bool operator!=(const fat_str& other) const noexcept;
};
};

fat_str::fat_str operator""_fs(const char* c_str, size_t count);
```


## Limitations

- ASCII only.
- Hard coded max size (512 MiB), you can't change this.
- Not sure if this categorized as limitation, but if you trying to allocate
  small string, it always allocate 32 bytes even though your string is just
  8 bytes or 4 bytes or even just 1 byte, this applies to `reserve()` too.

Current limitations may change in the future, especially with ASCII only problem,
because I have limited knowledge about other character sets (UTF-8, etc.).
Another problem is ABI break, because it will change pointer structure and
lead to bug (see commit 4c9e81eac3fbd042e5d30081f168d703f14f3944).


## See also
- [antirez sds](https://github.com/antirez/sds) (C)
