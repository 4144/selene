# Selene

[![Build Status](https://travis-ci.org/kmhofmann/selene.svg?branch=master)](https://travis-ci.org/kmhofmann/selene)
[![Build status](https://ci.appveyor.com/api/projects/status/7usv92g9ot12v656?svg=true)](https://ci.appveyor.com/project/kmhofmann/selene)

**Selene** is a C++14 image representation, processing, and I/O library, focusing on ease of use and a clean, modern,
type-safe API.

  * [Overview](docs/overview.md): Brief summary of the current feature set
  * [Rationale](docs/rationale.md): Why implement a new library for image representation, processing, and I/O?
  * [Installation](docs/installation.md): Installation using package managers
  * [Building the library](docs/building.md): How to build from source, and description of dependencies
  * [API documentation and status](docs/documentation.md): API documentation, examples, supported compilers
  * [Known issues](docs/known_issues.md): Known issues and possible workarounds
  * [Contribution guidelines](CONTRIBUTING.md): How to contribute to the library

### Summary

  * Strongly typed image and multi-channel pixel representations, and functions for image data access.
  * Easy-to-use APIs to read and write images in JPEG and PNG formats (leveraging *libjpeg* and *libpng*).
  * Basic image processing algorithms such as color conversions, pixel-wise operations, rotation, flipping, etc.
  * Lightweight and easy to build using *CMake* on Linux, MacOS, Windows.

### Example

A first impression of the API:

```
// Decode JPEG image data from disk
ImageData<> img_data = read_image(FileReader("example.jpg"));
assert(img_data.nr_channels() == 3 && img_data.nr_bytes_per_channel() == 1);

// Convert to strongly typed RGB image
Image<Pixel_8u3> img_rgb = to_image<Pixel_8u3>(std::move(img_data));
assert(img_rgb.width() > 400_px && img_rgb.height() > 350_px);

// Create non-owning view on part of the image
Image<Pixel_8u3> img_part = view(img_rgb, 100_idx, 100_idx, 300_px, 250_px);

// Darken this part
for_each_pixel(img_part, [](auto& px){ px /= 4; });

// Flip this part horizontally
flip_horizontally_in_place(img_part);

// Convert whole image to RGBA, adding semi-transparent alpha channel
const Image<Pixel_8u4> img_rgba =
  convert_image<PixelFormat::RGB, PixelFormat::RGBA>(img_rgb, std::uint8_t{128});

// Encode in-memory to PNG
std::vector<std::uint8_t> encoded_png_data;
write_image(to_image_data_view(img_rgba, PixelFormat::RGBA), ImageFormat::PNG,
          VectorWriter(encoded_png_data));

// Write encoded binary data to disk (or do something else with it...)
write_data_contents("example_out.png", encoded_png_data);
```

### Introductory presentation

Here is a [PDF format presentation](https://selene-lib.org/selene_presentation_20180524.pdf) that introduces Selene
(~v.0.1.0) and some of its features, given at a meetup of the
[Dutch C++ Group](https://www.meetup.com/The-Dutch-Cpp-Group/).

[![Selene: Image representation and I/O in C++14](docs/selene_presentation_preview_small.png)](https://selene-lib.org/selene_presentation_20180524.pdf)

### Package managed

Building from source "at head" using *CMake* is the recommended way to use **Selene**
(see [Building the library](docs/building.md)).
But the library is also available as a

  * [Conan package](https://bintray.com/kmhofmann/conan-repo/selene%3Aselene) on Bintray
  * [vcpkg](https://github.com/Microsoft/vcpkg) port

**Selene**'s own dependencies can be installed using

  * [Conan](https://conan.io/) (some)
  * [vcpkg](https://github.com/Microsoft/vcpkg) (all)

besides OS level package managers or from source.