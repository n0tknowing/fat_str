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

    // NO!
    //fat_str::fat_str dont_run_this;
    //printf("f=%c,b=%c\n", dont_run_this.front(), dont_run_this.back());

    fat_str::fat_str empty{""};
    if (empty.empty()) {
        printf("f=%c, b=%c\n", empty.front(), empty.back());
        printf("Empty!\n");
    }
}
