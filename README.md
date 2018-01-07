# Selene

**Selene** is a C++14 image representation, processing, and I/O library, focusing on ease of use and a clean, modern,
type-safe API.

  * [Overview](docs/overview.md): Brief summary of the current feature set
  * [Rationale](docs/rationale.md): Why implement a new library for image representation, processing, and I/O?
  * [Building the library](docs/building.md): How to build from source, and description of dependencies
  * [Documentation and status](docs/documentation.md): API documentation, examples, supported compilers

### Quick summary

  * Offers flexible classes for image and multi-channel pixel representations, and functions for image data access.
  * Provides easy-to-use APIs to read and write images in JPEG and PNG formats (leveraging *libjpeg* and *libpng*).
  * Offers basic image processing algorithms such as color conversions, pixel-wise operations, etc.
  * Lightweight and easy to build using *CMake* on Linux, MacOS, Windows.

### Example code

A first impression of the API:

```
// Decode JPEG image data from disk
ImageData<> img_data = read_image(FileReader("example.jpg"));
assert(img_data.nr_channels() == 3 && img_data.nr_bytes_per_channel() == 1);

// Convert to strongly typed RGB image
Image<Pixel_8u3> img_rgb = to_image<Pixel_8u3>(std::move(img_data));
assert(img_rgb.width() > 400_px && img_rgb.height() > 350_px);

// Create non-owning view on part of the image
Image<Pixel_8u3> img_part = view(img_rgb, 100_px, 100_px, 300_px, 250_px);

// Darken this part
for_each_pixel(img_part, [](auto& px){ px /= 4; });

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