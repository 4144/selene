// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBJPEG)

#include <catch.hpp>

#include <cstdlib>

#include <boost/filesystem.hpp>

#include <selene/io/FileReader.hpp>
#include <selene/io/FileWriter.hpp>
#include <selene/io/FileUtils.hpp>
#include <selene/io/MemoryReader.hpp>
#include <selene/io/VectorWriter.hpp>

#include <selene/img/Image.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/ImageDataToImage.hpp>
#include <selene/img/ImageToImageData.hpp>
#include <selene/img/JPEGRead.hpp>
#include <selene/img/JPEGWrite.hpp>

#include <Utils.hpp>

using namespace selene;
using namespace selene::img;
using namespace selene::io;

namespace fs = boost::filesystem;

constexpr auto ref_width = 1024;
constexpr auto ref_height = 684;
constexpr std::array<std::array<int, 6>, 3> px = {
    {std::array<int, 6>{{226, 180, 244, 198, 0, 189}},
     std::array<int, 6>{{582, 415, 228, 227, 232, 228}},
     std::array<int, 6>{{878, 597, 57, 60, 69, 60}}}}; // {x, y}, {r, g, b}, {y}
constexpr auto compression_factor = 70;

namespace {

fs::path in_filename()
{
  const auto env_var = std::getenv("SELENE_DATA_PATH");
  return (env_var) ? fs::path(env_var) / "bike_duck.jpg" : fs::path("../data/bike_duck.jpg");
}

} // namespace _

TEST_CASE("JPEG image reading and writing, no conversion", "[img]")
{
  const auto tmp_path = get_tmp_path();
  FileReader source(in_filename().c_str());
  REQUIRE(source.is_open());

  // Test reading without conversion
  MessageLog messages_read;
  auto img_data = read_jpeg(source, JPEGDecompressionOptions(), &messages_read);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(img_data.width() == ref_width);
  REQUIRE(img_data.height() == ref_height);
  REQUIRE(img_data.stride_bytes() == ref_width * 3);
  REQUIRE(img_data.nr_channels() == 3);
  REQUIRE(img_data.nr_bytes_per_channel() == 1);
  REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(!img_data.is_empty());
  REQUIRE(img_data.is_valid());

  auto img = to_image<Pixel_8u3>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 3);
  for (int i = 0; i < 3; ++i)
  {
    REQUIRE(img(px[i][0], px[i][1]) == Pixel_8u3(px[i][2], px[i][3], px[i][4]));
  }

  FileWriter sink((tmp_path / "test_duck.jpg").c_str());
  REQUIRE(sink.is_open());

  // Test writing of RGB image
  MessageLog messages_write;
  bool status_write = write_jpeg(sink, to_image_data_view(img, PixelFormat::RGB),
                                 JPEGCompressionOptions(compression_factor), &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

TEST_CASE("JPEG image reading and writing, conversion to grayscale", "[img]")
{
  const auto tmp_path = get_tmp_path();
  FileReader source(in_filename().c_str());
  REQUIRE(source.is_open());

  // Test reading with conversion to grayscale
  MessageLog messages_read;
  auto img_data = read_jpeg(source, JPEGDecompressionOptions(JPEGColorSpace::Grayscale), &messages_read);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(img_data.width() == ref_width);
  REQUIRE(img_data.height() == ref_height);
  REQUIRE(img_data.stride_bytes() == ref_width * 1);
  REQUIRE(img_data.nr_channels() == 1);
  REQUIRE(img_data.nr_bytes_per_channel() == 1);
  REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(!img_data.is_empty());
  REQUIRE(img_data.is_valid());

  auto img = to_image<Pixel_8u1>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 1);
  for (int i = 0; i < 3; ++i)
  {
    REQUIRE(static_cast<int>(img(px[i][0], px[i][1])) == static_cast<int>(Pixel_8u1(px[i][5])));
  }

  FileWriter sink((tmp_path / "test_duck_gray.jpg").c_str());
  REQUIRE(sink.is_open());

  // Test writing of grayscale image
  MessageLog messages_write;
  bool status_write = write_jpeg(sink, to_image_data_view(img, PixelFormat::Y),
                                 JPEGCompressionOptions(compression_factor), &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

TEST_CASE("JPEG image reading and writing, reusing decompression object", "[img]")
{
  const auto tmp_path = get_tmp_path();
  FileReader source(in_filename().c_str());
  REQUIRE(source.is_open());

  // Test reading of header...
  JPEGDecompressionObject decompression_object;
  const auto header = read_jpeg_header(decompression_object, source);

  REQUIRE(header.width == ref_width);
  REQUIRE(header.height == ref_height);
  REQUIRE(header.nr_channels == 3);
  REQUIRE(header.color_space == JPEGColorSpace::YCbCr);

  // ...and then reusing decompression object/header info for reading the image
  MessageLog messages_read;
  auto img_data = read_jpeg(decompression_object, source, JPEGDecompressionOptions(), &messages_read, &header);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(img_data.width() == ref_width);
  REQUIRE(img_data.height() == ref_height);
  REQUIRE(img_data.stride_bytes() == ref_width * 3);
  REQUIRE(img_data.nr_channels() == 3);
  REQUIRE(img_data.nr_bytes_per_channel() == 1);
  REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(!img_data.is_empty());
  REQUIRE(img_data.is_valid());

  auto img = to_image<Pixel_8u3>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 3);
  for (int i = 0; i < 3; ++i)
  {
    REQUIRE(img(px[i][0], px[i][1]) == Pixel_8u3(px[i][2], px[i][3], px[i][4]));
  }
}

#if defined(SELENE_LIBJPEG_PARTIAL_DECODING)
TEST_CASE("JPEG image reading and writing, partial image reading", "[img]")
{
  const auto tmp_path = get_tmp_path();
  FileReader source(in_filename().c_str());
  REQUIRE(source.is_open());

  // Test reading of partial image
  BoundingBox<Index> region(100, 100, 400, 350);
  const auto expected_width = 404;

  MessageLog messages_read;
  auto img_data = read_jpeg(source, JPEGDecompressionOptions(JPEGColorSpace::Auto, region), &messages_read);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(img_data.width() == expected_width);
  REQUIRE(img_data.height() == 350);
  REQUIRE(img_data.nr_channels() == 3);
  REQUIRE(img_data.nr_bytes_per_channel() == 1);
  REQUIRE(img_data.stride_bytes() == expected_width * 3);
  REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(!img_data.is_empty());
  REQUIRE(img_data.is_valid());

  auto img = to_image<Pixel_8u3>(std::move(img_data));

  REQUIRE(img.width() == expected_width);
  REQUIRE(img.height() == 350);
  REQUIRE(img.stride_bytes() == expected_width * 3);

  FileWriter sink((tmp_path / "test_duck_crop.jpg").c_str());
  REQUIRE(sink.is_open());

  // Test writing of RGB image
  MessageLog messages_write;
  bool status_write = write_jpeg(sink, to_image_data_view(img, PixelFormat::RGB),
                                 JPEGCompressionOptions(compression_factor), &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}
#endif

TEST_CASE("JPEG image reading and writing, reading/writing from/to memory", "[img]")
{
  const auto tmp_path = get_tmp_path();
  const auto file_contents = read_file_contents(in_filename().c_str());
  REQUIRE(!file_contents.empty());

  // Test reading from memory
  MemoryReader source(file_contents.data(), file_contents.size());
  REQUIRE(source.is_open());

  MessageLog messages_read;
  auto img_data = read_jpeg(source, JPEGDecompressionOptions(), &messages_read);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(img_data.width() == ref_width);
  REQUIRE(img_data.height() == ref_height);
  REQUIRE(img_data.stride_bytes() == ref_width * 3);
  REQUIRE(img_data.nr_channels() == 3);
  REQUIRE(img_data.nr_bytes_per_channel() == 1);
  REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(!img_data.is_empty());
  REQUIRE(img_data.is_valid());

  auto img = to_image<Pixel_8u3>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 3);
  for (int i = 0; i < 3; ++i)
  {
    REQUIRE(img(px[i][0], px[i][1]) == Pixel_8u3(px[i][2], px[i][3], px[i][4]));
  }

  // Test writing to memory
  std::vector<std::uint8_t> compressed_data;
  VectorWriter sink(compressed_data);
  REQUIRE(sink.is_open());

  // Test writing of RGB image
  MessageLog messages_write;
  bool status_write = write_jpeg(sink, to_image_data_view(img, PixelFormat::RGB), JPEGCompressionOptions(),
                                 &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

#endif // defined(SELENE_WITH_LIBJPEG)
