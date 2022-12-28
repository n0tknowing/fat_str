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

`C` = Capacity. (4 bytes)<br/>
`L` = Current length. (4 bytes)<br/>
`DATA` = NUL (`\0`) terminated ASCII string (0 to 512 MiB).


## (Current) Public API

```cpp
class fat_str {
public:
    fat_str();
    fat_str(const char*);
    fat_str(const fat_str&);
    fat_str(fat_str&&);

    fat_str& operator=(const char*);
    fat_str& operator=(const fat_str&);
    fat_str& operator=(fat_str&&);

    char& at(size_t); // bound-checked
    char& operator[](size_t);
    char front() const;
    char back() const;
    const char *data() const;

    bool empty() const;
    size_t size() const;
    size_t length() const;
    size_t max_size() const;
    void reserve(size_t);
    size_t capacity() const;

    bool operator==(const char*) const;
    bool operator==(const fat_str&) const;
    bool operator!=(const char*) const;
    bool operator!=(const fat_str&) const;
};
```


## See also
- [antirez sds](https://github.com/antirez/sds) (C)
