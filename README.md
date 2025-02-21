# msgpack23  
A modern, header-only C++ library for MessagePack serialization and deserialization.

## Overview
msgpack23 is a lightweight library that provides a straightforward approach to serializing and deserializing C++ data structures into the [MessagePack](https://msgpack.org/) format. It is written in modern C++ (targeting C++20 and beyond) and leverages templates and type traits to provide a flexible, zero-dependency solution for packing and unpacking various data types.

## Key Features
- **Header-only**: Simply include the header and start using it—no additional build steps or dependencies.
- **Modern C++**: Uses C++ features like concepts to handle containers, maps, enums, time points, and user-defined types.
- **Extensible**: Allows you to define custom types by implementing `pack` and `unpack` member functions, automatically integrating them into the serialization pipeline.
- **Collection and Map Support**: Automatically detects and serializes STL containers (e.g., `std::vector`, `std::map`) without extra work.
- **Time Point Support**: Native support for serializing `std::chrono::time_point` objects.
- **Variety of Primitive Types**: Integers (signed/unsigned), booleans, floating-point, `std::string`, byte arrays, and `nullptr` are all supported out-of-the-box.
- **Endian-Aware**: Properly handles endianness using `std::endian` and `std::byteswap` to ensure portability.

## Getting Started

1. **Clone the Repository**
   ```bash
   git clone https://github.com/rwindegger/msgpack23.git
   ```

2. **Include the Header**  
   Since this is a header-only library, just include the main header in your project:
   ```cpp
   #include "msgpack23.hpp"
   ```

3. **Pack and Unpack**
   ```cpp
   #include <iostream>
   #include <map>
   #include "msgpack23.hpp"
   
   int main() {
       // Create a map of some data
       std::map<std::string, int> original {{"apple", 1}, {"banana", 2}};
       
       // 1) Pack into a vector of std::byte
       msgpack23::Packer packer;
       auto packedData = packer(original); 
       
       // 2) Unpack back into a map
       std::map<std::string, int> unpacked;
       msgpack23::Unpacker unpacker(packedData);
       unpacker(unpacked);
       
       // Verify the result
       for (auto const& [key, value] : unpacked) {
           std::cout << key << ": " << value << "\n";
       }
       return 0;
   }
   ```

## Custom Types

To serialize your own types, define a `pack` and `unpack` function. The `pack` should accept a `T &` and the `unpack` should accept a `T &`.

```cpp
struct MyData {
   int64_t my_integer;
   std::string my_string;
   
   template<typename T>
   std::vector<std::byte> pack(T &packer) const {
      return packer(my_integer, my_string);
   }
   
   template<typename T>
   void unpack(T &unpacker) {
      unpacker(my_integer, my_string);
   }
};
```

Now you can use `MyData` with `msgpack23` just like any built-in type:
```cpp
MyData const my_data {42, "Hello" };
auto const data = msgpack23::pack(my_data);
auto obj = msgpack23::unpack<MyData>(data);
```

## Why msgpack23?

- **Simplicity**: A single header with clearly structured pack/unpack logic.
- **Performance**: Minimal overhead by using direct memory operations and compile-time type deductions.
- **Flexibility**: From primitive types and STL containers to custom structures, everything can be serialized with minimal boilerplate.

## Contributing

Contributions, bug reports, and feature requests are welcome! Feel free to open an [issue](https://github.com/rwindegger/msgpack23/issues) or submit a pull request.

1. Fork it!
2. Create your feature branch: `git checkout -b feature/my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin feature/my-new-feature`
5. Submit a pull request

## License

This project is licensed under the [MIT License](LICENSE).

---

Happy packing (and unpacking)! If you have any questions or feedback, please open an issue or start a discussion.