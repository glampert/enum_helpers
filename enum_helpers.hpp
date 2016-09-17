
// ================================================================================================
// -*- C++ -*-
// File: enum_helpers.hpp
// Author: Guilherme R. Lampert
// Created on: 17/09/16
//
// About:
//  C++ enum/enum-class iterator helper and enum-sized C-style array wrapper.
//
// License:
//  This software is in the public domain. Where that dedication is not recognized,
//  you are granted a perpetual, irrevocable license to copy, distribute, and modify
//  this file as you see fit. Source code is provided "as is", without warranty of any
//  kind, express or implied. No attribution is required, but a mention about the author
//  is appreciated.
// ================================================================================================

#ifndef ENUM_HELPERS_HPP
#define ENUM_HELPERS_HPP

// Defining this before including the file prevents pulling the Standard headers.
// Useful to be able to place this file inside a user-defined namespace or to simply
// avoid redundant inclusions. User is responsible for providing all the necessary
// Standard headers before #including this one.
#ifndef ENUM_HELPERS_NO_STD_INCLUDES
    #include <cstddef>
    #include <iterator>
    #include <type_traits>
#endif // ENUM_HELPERS_NO_STD_INCLUDES

// Hook to allow providing a custom assert() before including this file.
#ifndef ENUM_HELPERS_ASSERT
    #ifndef ENUM_HELPERS_NO_STD_INCLUDES
        #include <cassert>
    #endif // ENUM_HELPERS_NO_STD_INCLUDES
    #define ENUM_HELPERS_ASSERT assert
#endif // ENUM_HELPERS_ASSERT

// ========================================================
// template class enum_iterator:
// ========================================================

//
// Allows iterating sequential enumerators.
// You can also provide an increment step > 1
// if each constant is spaced by a larger value.
// Terminating constant is assumed to be a 'Count'
// member, which is never iterated. A different
// name for the terminating constant can also be
// specified on declaration.
//
// Usage example:
//
//  enum class Foo
//  {
//      Bar, Baz, Fooz,
//      Count
//  };
//
//  enum_iterator<Foo> iter;
//
//  for (auto c : iter)
//  {
//      'c' is a constant from Foo
//  }
//
template
<
    typename EnumType,               // The enum (class) type.
    EnumType Last = EnumType::Count, // Last constant or number thereof (assumes a default 'Count' member).
    unsigned int Step = 1            // Step increment per constant.
>
class enum_iterator final
    : public std::iterator<std::forward_iterator_tag, EnumType>
{
public:

    using integer_type = typename std::underlying_type<EnumType>::type;

    constexpr enum_iterator() noexcept
        : m_current{ 0 } // Assumes 0 is the first constant
    { }

    constexpr enum_iterator(const EnumType index) noexcept
        : m_current{ static_cast<integer_type>(index) }
    { }

    enum_iterator operator++() // pre-increment
    {
        m_current += Step;
        return *this;
    }

    enum_iterator operator++(int) // post-increment
    {
        enum_iterator old_val{ *this };
        m_current += Step;
        return old_val;
    }

    EnumType operator*() const
    {
        ENUM_HELPERS_ASSERT(m_current < static_cast<integer_type>(Last));
        return static_cast<EnumType>(m_current);
    }

    bool operator == (const enum_iterator other) const noexcept { return m_current == other.m_current; }
    bool operator != (const enum_iterator other) const noexcept { return m_current != other.m_current; }
    bool operator <  (const enum_iterator other) const noexcept { return m_current <  other.m_current; }
    bool operator >  (const enum_iterator other) const noexcept { return m_current >  other.m_current; }
    bool operator <= (const enum_iterator other) const noexcept { return m_current <= other.m_current; }
    bool operator >= (const enum_iterator other) const noexcept { return m_current >= other.m_current; }

    enum_iterator begin() const { return enum_iterator{}; }
    enum_iterator end()   const { return enum_iterator{ Last }; }

private:

    integer_type m_current;
};

// ========================================================
// template struct enum_array:
// ========================================================

//
// Wrapper for a C-style array with size and indexing
// defined by an enum. Useful for declaring arrays of
// enum names for debug printing.
//
// Usage example:
//
//  enum class Foo
//  {
//      Bar, Baz, Fooz,
//      Count
//  };
//
//  const enum_array<Foo, std::string> FooStrings = { { "Bar", "Baz", "Fooz" } };
//
//  print(FooStrings[Foo::Bar]);
//  print(FooStrings[Foo::Baz]);
//  ...
//
template
<
    typename EnumType,                   // The enum (class) type.
    typename DataType,                   // Type of the data stored in the array.
    EnumType ArraySize = EnumType::Count // Size in entries of the array.
>
struct enum_array final
{
    using key_iterator = enum_iterator<EnumType, ArraySize>;

    // Data is kept public so we can use direct aggregate
    // initialization just like in a plain C-style array.
    DataType m_elements[std::size_t(ArraySize)];

    static constexpr key_iterator keys() { return key_iterator{}; }
    static constexpr std::size_t  size() { return std::size_t(ArraySize); }

    //
    // Array access with asserts:
    //
    DataType & operator[](const std::size_t index)
    {
        ENUM_HELPERS_ASSERT(index < size());
        return m_elements[index];
    }
    const DataType & operator[](const std::size_t index) const
    {
        ENUM_HELPERS_ASSERT(index < size());
        return m_elements[index];
    }

    DataType & operator[](const EnumType index)
    {
        ENUM_HELPERS_ASSERT(std::size_t(index) < size());
        return m_elements[std::size_t(index)];
    }
    const DataType & operator[](const EnumType index) const
    {
        ENUM_HELPERS_ASSERT(std::size_t(index) < size());
        return m_elements[std::size_t(index)];
    }

    //
    // Range iterators (unchecked):
    //
    using iterator               = DataType *;
    using const_iterator         = const DataType *;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator               begin()        { return &m_elements[0];      }
    iterator               end()          { return &m_elements[size()]; }
    const_iterator         begin()  const { return &m_elements[0];      }
    const_iterator         end()    const { return &m_elements[size()]; }

    reverse_iterator       rbegin()       { return reverse_iterator{ end() }; }
    reverse_iterator       rend()         { return reverse_iterator{ begin() }; }
    const_reverse_iterator rbegin() const { return const_reverse_iterator{ end() }; }
    const_reverse_iterator rend()   const { return const_reverse_iterator{ begin() }; }
};

// ========================================================
// Small usage sample:
// ========================================================

#if 0
enum class Foo
{
    Bar, Baz, Fooz,
    Count
};
void enum_helpers_sample()
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

    ENUM_HELPERS_ASSERT(a == b);
    ENUM_HELPERS_ASSERT(*(a++) == Foo::Bar);
    ENUM_HELPERS_ASSERT(*(++b) == Foo::Baz);

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
    ENUM_HELPERS_ASSERT(*x == "Bar");
    ++x;
    ENUM_HELPERS_ASSERT(*x == "Baz");
    ++x;
    ENUM_HELPERS_ASSERT(*x == "Fooz");

    ENUM_HELPERS_ASSERT(FooStrings[Foo::Bar]  == "Bar");
    ENUM_HELPERS_ASSERT(FooStrings[Foo::Baz]  == "Baz");
    ENUM_HELPERS_ASSERT(FooStrings[Foo::Fooz] == "Fooz");
}
#endif // 0

#endif // ENUM_HELPERS_HPP
