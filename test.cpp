#include <cstdio>
#include "fat_str.h"

int main(void)
{
    fat_str::fat_str str{"Hello fat_str"}, str2{"Hello world"};
    if (str != str2)
        printf("%s, %s\n", str.data(), str2.data());

    fat_str::fat_str str3{str};
    if (str3 == str)
        printf("OK\n");

    printf("%s, %zu\n", str.data(), str.size());
    printf("%s, %zu\n", str2.data(), str2.size());
    printf("%s, %zu\n", str3.data(), str3.size());

    printf("f=%c, b=%c\n", str.front(), str.back());
    printf("f=%c, b=%c\n", str2.front(), str2.back());
    printf("f=%c, b=%c\n", str3.front(), str3.back());

    // test copy ctor from empty str
    fat_str::fat_str empty, empty2{empty};
    if (empty.empty() && empty2.empty()) {
        printf("f=%c, b=%c\n", empty2.front(), empty2.back());
        printf("Empty!\n");
    }

    fat_str::fat_str cp{"12345678910"};
    printf("%s, %zu\n", cp.data(), cp.size());

    // copy from 'str2' to test copy operator with same size
    cp = str2;
    if (cp == str2) {
        printf("OK\n");
        printf("%s, %zu\n", cp.data(), cp.size());
        printf("f=%c, b=%c\n", cp.front(), cp.back());
    }

    // copy from 'str3' to test copy operator with size less than str3
    cp = str3;
    if (cp == str3) {
        printf("OK\n");
        printf("%s, %zu\n", cp.data(), cp.size());
        printf("f=%c, b=%c\n", cp.front(), cp.back());
    }

    // do nothing
    cp = empty;
    if (cp == str3) {
        printf("OK\n");
        printf("%s, %zu\n", cp.data(), cp.size());
        printf("f=%c, b=%c\n", cp.front(), cp.back());
    }
}
