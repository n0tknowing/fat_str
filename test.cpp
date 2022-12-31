#include <stdexcept>
#include <utility>
#include <cstdio>
#include <vector>
#include "fat_str.h"

int main() {
    printf("sizeof(fat_str::fat_str) = %zu\n\n", sizeof(fat_str::fat_str));

    fat_str::fat_str str{"Hello fat_str"}, str2{"Hello world"};
    if (str != str2)
        printf("str=%s, str2=%s\n", str.data(), str2.data());

    fat_str::fat_str str3{str};
    if (str3 == str)
        printf("[OK] str=%s, str3=%s\n", str.data(), str3.data());

    fat_str::fat_str str4 = "Goodbye world";

    printf("str = %s, %zu, %zu\n", str.data(), str.size(), str.capacity());
    printf("str2 = %s, %zu, %zu\n", str2.data(), str2.size(), str2.capacity());
    printf("str3 = %s, %zu, %zu\n", str3.data(), str3.size(), str3.capacity());
    printf("str4 = %s, %zu, %zu\n", str4.data(), str4.size(), str4.capacity());

    printf("str = f=%c, b=%c\n", str.front(), str.back());
    printf("str2 = f=%c, b=%c\n", str2.front(), str2.back());
    printf("str3 = f=%c, b=%c\n", str3.front(), str3.back());
    printf("str4 = f=%c, b=%c\n", str4.front(), str4.back());

    // test copy ctor from empty str
    fat_str::fat_str empty, empty2{empty};
    if (empty.empty() && empty2.empty()) {
        printf("f=%c, b=%c\n", empty2.front(), empty2.back());
        printf("Empty!\n");
    }

    fat_str::fat_str cp{"12345678910"};
    printf("cp = %s, %zu, %zu\n", cp.data(), cp.size(), cp.capacity());

    // copy from 'str2' to test copy assignment with same size
    cp = str2;
    if (cp == str2) {
        printf("[OK], 'cp' is same as 'str2'\n");
        printf("cp = %s, %zu, %zu\n", cp.data(), cp.size(), cp.capacity());
        printf("cp = f=%c, b=%c\n", cp.front(), cp.back());
    }

    // copy from 'str3' to test copy assignment with size less than str3
    cp = str3;
    if (cp == str3) {
        printf("[OK], 'cp' is same as 'str3'\n");
        printf("cp = %s, %zu, %zu\n", cp.data(), cp.size(), cp.capacity());
        printf("cp = f=%c, b=%c\n", cp.front(), cp.back());
    }

    // do nothing
    cp = empty;
    if (cp == str3) {
        printf("[OK], 'cp' do nothing with empty fat_str\n");
        printf("cp = %s, %zu, %zu\n", cp.data(), cp.size(), cp.capacity());
        printf("cp = f=%c, b=%c\n", cp.front(), cp.back());
    }

    // test move ctor and assignment
    fat_str::fat_str mv1{"from mv1"}, mv2;
    printf("Before:\n");
    printf("  mv1=%s, mv2=%s\n", mv1.data(), mv2.data());
    mv2 = std::move(mv1);
    printf("After move ctor:\n");
    printf("  mv1=%s, mv2=%s\n", mv1.data(), mv2.data());

    fat_str::fat_str mv3{std::move(mv2)};
    printf("After move assignment:\n");
    printf("  mv1=%s, mv2=%s, mv3=%s\n", mv1.data(), mv2.data(), mv3.data());

    if (mv3 == "from mv1")
        printf("[OK], 'mv3' is from 'mv1'\n");

    fat_str::fat_str n;
    n.reserve(512);
    printf("n = %s, %zu, %zu\n", n.data(), n.size(), n.capacity());
    n = "does this work?";
    printf("n = %s, %zu, %zu\n", n.data(), n.size(), n.capacity());
    n = "how about this?";
    printf("n = %s, %zu, %zu\n", n.data(), n.size(), n.capacity());
    n = "and this?";
    printf("n = %s, %zu, %zu\n", n.data(), n.size(), n.capacity());
    n = "oooooooooooooooooooooohhhhh how about string longer than 32 bytes?";
    printf("n = %s, %zu, %zu\n", n.data(), n.size(), n.capacity());
    n = "how about exceed current capacity? aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    printf("n = %s, %zu, %zu\n", n.data(), n.size(), n.capacity());

    std::vector<fat_str::fat_str> fv{};
    for (int i = 0; i < 5; i++)
        fv.push_back("vector and fat_str");
    printf("vector size = %zu\n", fv.size());
    for (const fat_str::fat_str &str : fv)
        printf("%s, %zu, %zu\n", str.data(), str.size(), str.capacity());

    fat_str::fat_str a{"from a"}, b{"from b"};
    printf("a = %s\n", a.data());
    printf("b = %s\n", b.data());
    a = std::move(b);
    printf("a = %s\n", a.data());
    printf("b = %s\n", b.data()); // empty() == true
    fat_str::fat_str c, d{std::move(c)};
    printf("c = %s\n", c.data()); // empty() == true
    printf("d = %s\n", d.data()); // empty() == true
    try {
        printf("char at 0 in c is %c\n", c.at(0));
        printf("char at 0 in d is %c\n", d.at(0));
    } catch (std::exception& e) {
        printf("fail? %s\n", e.what());
    }
    printf("a = size(%zu), cap(%zu)\n", a.size(), a.capacity());
    a.clear();
    printf("a = size(%zu), cap(%zu)\n", a.size(), a.capacity());
    a = "it's a again";
    b = "from b (new)";
    printf("a = %s\n", a.data());
    printf("b = %s\n", b.data());
    a.swap(b);
    printf("a = %s\n", a.data());
    printf("b = %s\n", b.data());
    for (size_t i = 0; i < 5; i++) {
        a.pop_back();
        printf("a = %s, %zu, %zu\n", a.data(), a.size(), a.capacity());
    }
    for (size_t i = 0; i < 50; i++) {
        a.push_back(n[i]);
        printf("a = %s, %zu, %zu\n", a.data(), a.size(), a.capacity());
    }

    fat_str::fat_str e('e', 16);
    printf("e = %s, %zu, %zu\n", e.data(), e.size(), e.capacity());

    // compile error
    //e = nullptr;
    //printf("e = %s, %zu, %zu\n", e.data(), e.size(), e.capacity());

    e = ""; // clear
    printf("e = %s, %zu, %zu\n", e.data(), e.size(), e.capacity());

    if (e == nullptr)
        printf("???\n");

    // compile error
    //fat_str::fat_str f{nullptr};
    //printf("f = %s, %zu, %zu\n", f.data(), f.size(), f.capacity());

    fat_str::fat_str *g = new fat_str::fat_str();
    printf("g = %s, %zu, %zu\n", g->data(), g->size(), g->capacity());
    g->assign('g');
    printf("g = %s, %zu, %zu\n", g->data(), g->size(), g->capacity());
    g->assign("from assign()");
    printf("g = %s, %zu, %zu\n", g->data(), g->size(), g->capacity());
    g->assign(e);
    printf("g = %s, %zu, %zu\n", g->data(), g->size(), g->capacity());
    g->assign('g', 16);
    printf("g = %s, %zu, %zu\n", g->data(), g->size(), g->capacity());
    g->assign(d); // do nothing
    printf("g = %s, %zu, %zu\n", g->data(), g->size(), g->capacity());
    g->assign(d, 1000000); // do nothing
    printf("g = %s, %zu, %zu\n", g->data(), g->size(), g->capacity());

    //g = nullptr; works but you will leak memory
    delete g;

    // compile error
    //fat_str::fat_str h(nullptr, 12);
    //printf("h = %s, %zu, %zu\n", h.data(), h.size(), h.capacity());

    fat_str::fat_str h("from hhhh", 6);
    printf("h = %s, %zu, %zu\n", h.data(), h.size(), h.capacity());

    fat_str::fat_str i(h, 60);
    if (i == h)
        printf("i = %s, %zu, %zu\n", i.data(), i.size(), i.capacity());

    // compiler error
    //i.assign(nullptr);
    //i.assign(nullptr, 1000000);

    i.append("test append 1 ").append("test append 2");
    printf("i = %s, %zu, %zu\n", i.data(), i.size(), i.capacity());
    i.append('+');
    i.append('i', 30);
    printf("i = %s, %zu, %zu\n", i.data(), i.size(), i.capacity());

    fat_str::fat_str j{std::move(i)};
    printf("i = %s, %zu, %zu\n", i.data(), i.size(), i.capacity());
    printf("j = %s, %zu, %zu\n", j.data(), j.size(), j.capacity());

    i.append("test append again after moved");
    printf("i = %s, %zu, %zu\n", i.data(), i.size(), i.capacity());
    i.append(", only c++ that appended", 10);
    printf("i = %s, %zu, %zu\n", i.data(), i.size(), i.capacity());
    i.append(i);
    printf("i = %s, %zu, %zu\n", i.data(), i.size(), i.capacity());
    i += i;
    printf("i = %s, %zu, %zu\n", i.data(), i.size(), i.capacity());
    i.append(i.data());
    printf("i = %s, %zu, %zu\n", i.data(), i.size(), i.capacity());
    i += i.data();
    printf("i = %s, %zu, %zu\n", i.data(), i.size(), i.capacity());
    i += 'o';
    i += n;
    printf("i = %s, %zu, %zu\n", i.data(), i.size(), i.capacity());
}
