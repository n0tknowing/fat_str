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
class fat_str {
public:
    fat_str();
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

    char& at(size_t pos); // bound-checked
    char& operator[](size_t pos); // UB if pos >= size()
    char front() const; // copy not reference
    char back() const; // copy not reference
    const char* data() const; // NUL-terminated

    bool empty() const;
    size_t size() const;
    size_t length() const;
    size_t max_size() const;
    void reserve(size_t new_cap);
    size_t capacity() const;

    void clear();
    void push_back(char ch);
    void pop_back();
    void swap(fat_str& other);

    bool operator==(const char* c_str) const;
    bool operator==(const fat_str& other) const;
    bool operator!=(const char* c_str) const;
    bool operator!=(const fat_str& other) const;
};
```


## Limitations

- ASCII only.
- Hard coded max size (512 MiB), you can't change this.

Current limitations may change in the future, especially with ASCII only problem,
because I have limited knowledge about other character sets (UTF-8, etc.).
Another problem is ABI break, because it will change pointer structure and
lead to bug (see commit 4c9e81eac3fbd042e5d30081f168d703f14f3944).


## See also
- [antirez sds](https://github.com/antirez/sds) (C)
