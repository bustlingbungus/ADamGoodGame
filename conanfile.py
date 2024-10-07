from conan import ConanFile
from conan.tools.cmake import cmake_layout


class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("sdl_ttf/[~2.22]")
        # self.requires("sdl_image/[~2.6]")
        # self.requires("sdl_mixer/[~2.8]")

    def layout(self):
        cmake_layout(self)
