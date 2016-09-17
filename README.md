
## Enum helpers

C++ enum/enum-class iterator helper and enum-sized C-style array wrapper.

```cpp
enum class Foo
{
    Bar, Baz, Fooz,
    Count
};

int main()
{
    //
    // enum_iterator:
    //

    enum_iterator<Foo> iter;

    for (auto c : iter)
    {
        std::cout << "Foo = " << static_cast<int>(c) << "\n";
    }

    std::cout << "\n";

    for (auto c = iter.begin(); c != iter.end(); ++c)
    {
        std::cout << "Foo = " << static_cast<int>(*c) << "\n";
    }

    std::cout << "\n";

    auto a = std::begin(iter);
    auto b = std::begin(iter);

    assert(a == b);
    assert(*(a++) == Foo::Bar);
    assert(*(++b) == Foo::Baz);

    //
    // enum_array:
    //

    const enum_array<Foo, std::string> FooStrings{ { "Bar", "Baz", "Fooz" } };

    // Keys give you the constants associated with each array index.
    for (auto k : FooStrings.keys())
    {
        std::cout << "Foo = " << static_cast<int>(k) << "\n";
    }

    std::cout << "\n";

    // Using iterators or [] give the array values.
    for (const auto & s : FooStrings)
    {
        std::cout << "Foo as string = " << s.c_str() << "\n";
    }

    std::cout << "\n";

    for (auto i = FooStrings.rbegin(); i != FooStrings.rend(); ++i)
    {
        std::cout << "Foo as string (reverse iteration) = " << (*i).c_str() << "\n";
    }

    auto x = std::begin(FooStrings);
    assert(*x == "Bar");
    ++x;
    assert(*x == "Baz");
    ++x;
    assert(*x == "Fooz");

    assert(FooStrings[Foo::Bar]  == "Bar");
    assert(FooStrings[Foo::Baz]  == "Baz");
    assert(FooStrings[Foo::Fooz] == "Fooz");
}
```

### License

This software is in the *public domain*. Where that dedication is not recognized,
you are granted a perpetual, irrevocable license to copy, distribute, and modify
the code as you see fit.

Source code is provided "as is", without warranty of any kind, express or implied.
No attribution is required, but a mention about the author is appreciated.

