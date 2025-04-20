from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain, CMakeDeps, CMake

class msgpack23(ConanFile):
    name = "msgpack23"
    version = "3.0"

    license = "MIT"
    author = "Rene Windegger <rene@windegger.wtf>"
    url = "https://github.com/rwindegger/msgpack23"
    description = "A modern, header-only C++ library for MessagePack serialization and deserialization."
    topics = ("msgpack", "serialization", "MessagePack")

    settings = "os", "compiler", "build_type", "arch"

    exports_sources = ( "CMakeLists.txt", "include/*", "tests/*", "cmake/*" )

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def requirements(self):
        requirements = self.conan_data.get('requirements', [])
        for requirement in requirements:
            self.requires(requirement)