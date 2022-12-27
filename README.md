# fat\_str - C++ string managed by fat pointer

## Why

Purely educational purpose (and having fun), improving my C++ skill by doing
some pointer magic.


## Pointer structure

`L` = Current length. (4 bytes, Max is 512 MiB)<br/>
`DATA` = NUL (`\0`) terminated ASCII string.

```
+---------------+
| LLLL |  DATA  |
+---------------+
```


## See also
- [antirez sds](https://github.com/antirez/sds) (C)
