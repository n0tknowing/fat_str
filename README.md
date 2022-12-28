# fat\_str - C++ string managed by fat pointer

## Why

Purely educational purpose (and having fun), improving my C++ skill by doing
some pointer magic.


## Pointer structure

`C` = Capacity. (4 bytes)<br/>
`L` = Current length. (4 bytes)<br/>
`DATA` = NUL (`\0`) terminated ASCII string (0 to 512 MiB).

```
+----------------------+
| CCCC | LLLL |  DATA  |
+----------------------+
```


## See also
- [antirez sds](https://github.com/antirez/sds) (C)
