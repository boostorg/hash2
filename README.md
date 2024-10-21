# Hash2, a hash function library

This library contains an extensible framework for implementing
hashing algorithms that can support user-defined types, based on the paper
["Types don't know #"](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3980.html)
by Howard Hinnant, Vinnie Falco and John Bytheway.

It also contains implementations of several popular hashing algorithms:

* [FNV-1a](https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function)
* [xxHash](https://cyan4973.github.io/xxHash/)
* [SipHash](https://131002.net/siphash/)
* [MD5](https://tools.ietf.org/html/rfc1321)
* [SHA-1](https://tools.ietf.org/html/rfc3174)
* [SHA-2](https://tools.ietf.org/html/rfc6234)
* [HMAC-MD5, HMAC-SHA1, HMAC-SHA2](https://tools.ietf.org/html/rfc2104)
* [RIPEMD-160 and HMAC-RIPEMD-160](https://homes.esat.kuleuven.be/%7Ebosselae/ripemd160.html)

The hashing algorithms conform to the following concept:

```
struct HashAlgorithm
{
    using result_type = /*integral or array<unsigned char, N>*/; // result type
    using size_type = /*unsigned integral*/; // how is container.size() hashed

    HashAlgorithm(); // default-constructible
    explicit HashAlgorithm( uint64_t seed ); // seed-constructible
    HashAlgorithm( unsigned char const* seed, size_t n ); // seed-constructible

    HashAlgorithm( HashAlgorithm const& r ); // copy-constructible
    HashAlgorithm& operator=( HashAlgorithm const& r ); // assignable

    void update( void const* data, size_t n ); // feed bytes

    result_type result(); // obtain result; also advances state,
                          // and can be called multiple times
};
```

## Supported compilers

The library requires C++11. The following compilers:

* g++ 4.8 or later
* clang++ 3.9 or later
* Visual Studio 2015 and above

are being tested on [Github Actions](https://github.com/pdimov/hash2/actions/) and [Appveyor](https://ci.appveyor.com/project/pdimov/hash2/).

## Documentation

[Work in progress](https://pdimov.github.io/hash2/).

## Planned Additions

* Support for endian-independent hashing
* A `std::hash`-compatible adaptor

## License

Distributed under the [Boost Software License, Version 1.0](http://boost.org/LICENSE_1_0.txt).
