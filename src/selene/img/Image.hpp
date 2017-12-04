// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_HPP
#define SELENE_IMG_IMAGE_HPP

/// @file

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MemoryBlock.hpp>

#include <selene/img/PixelFormat.hpp>
#include <selene/img/PixelTraits.hpp>
#include <selene/img/Types.hpp>

#include <algorithm>
#include <cstdint>
#include <memory>

namespace selene {
namespace img {

class ImageData;

template <typename PixelType>
class Image;

template <typename PixelType>
void clone(const Image<PixelType>& src, Image<PixelType>& dst);

template <typename PixelType>
ImageData to_image_data(Image<PixelType>&& img, PixelFormat pixel_format);


/** \brief Statically typed image class.
 *
 * An instance of `Image<PixelType>` represents a statically typed image with pixel elements of type `PixelType`.
 * Since the number of channels is determined by the pixel type (e.g. `Pixel<U, N>`), the storage of multiple
 * channels/samples is always interleaved, as opposed to planar.
 * Images are stored row-wise contiguous, with additional space after each row due to a custom stride in bytes.
 *
 * The memory of an `Image<PixelType>` instance may either be owned or non-owned; in the latter case, the instance is a
 * "view" on image data.
 */
template <typename PixelType>
class Image
{
public:
  Image();
  Image(Length width, Length height);
  Image(Length width, Length height, Stride stride_bytes);
  Image(std::uint8_t* data, Length width, Length height, Stride stride_bytes);
  Image(MemoryBlock<NewAllocator>&& data, Length width, Length height, Stride stride_bytes);

  Image(const Image<PixelType>& other);
  Image<PixelType>& operator=(const Image<PixelType>& other);

  Image(Image<PixelType>&& other) noexcept;
  Image<PixelType>& operator=(Image<PixelType>&& other) noexcept;

  ~Image();

  Length width() const;
  Length height() const;
  Stride stride_bytes() const;
  std::size_t row_bytes() const;
  std::size_t total_bytes() const;
  bool is_packed() const;
  bool is_view() const;
  bool is_empty() const;
  bool is_valid() const;

  void clear();
  void fill(PixelType value);
  void allocate(Length width, Length height);
  void allocate(Length width, Length height, Stride stride_bytes);
  void set_view(std::uint8_t* data, Length width, Length height, Stride stride_bytes);
  void set_data(MemoryBlock<NewAllocator>&& data, Length width, Length height, Stride stride_bytes);

  PixelType* data();
  const PixelType* data() const;

  PixelType* data(Index y);
  const PixelType* data(Index y) const;

  PixelType* data_row_end(Index y);
  const PixelType* data_row_end(Index y) const;

  PixelType* data(Index x, Index y);
  const PixelType* data(Index x, Index y) const;

  std::uint8_t* byte_ptr();
  const std::uint8_t* byte_ptr() const;

  std::uint8_t* byte_ptr(Index y);
  const std::uint8_t* byte_ptr(Index y) const;

  std::uint8_t* byte_ptr(Index x, Index y);
  const std::uint8_t* byte_ptr(Index x, Index y) const;

  PixelType& operator()(Index x, Index y);
  const PixelType& operator()(Index x, Index y) const;

private:
  std::uint8_t* data_;
  Stride stride_bytes_;
  Length width_;
  Length height_;
  bool owns_memory_;

  void allocate_bytes(std::size_t nr_bytes);
  void deallocate_bytes();
  void deallocate_bytes_if_owned();
  void reset();
  void copy_rows_from(const Image<PixelType>& src);
  std::uint32_t compute_data_offset(Index y) const;
  std::uint32_t compute_data_offset(Index x, Index y) const;

  MemoryBlock<NewAllocator> relinquish_data_ownership();

  friend void clone<PixelType>(const Image<PixelType>& src, Image<PixelType>& dst);
  friend ImageData to_image_data<PixelType>(Image<PixelType>&&, PixelFormat);
};

template <typename PixelType>
void clone(const Image<PixelType>& src, Image<PixelType>& dst);

template <typename PixelType>
void clone(const Image<PixelType>& src, Index x0, Index y0, Length width, Length height, Image<PixelType>& dst);

template <typename PixelType>
Image<PixelType> clone(const Image<PixelType>& src);

template <typename PixelType>
Image<PixelType> clone(const Image<PixelType>& src, Index x0, Index y0, Length width, Length height);

template <typename PixelType>
Image<PixelType> view(const Image<PixelType>& src);

template <typename PixelType>
Image<PixelType> view(const Image<PixelType>& src, Index x0, Index y0, Length width, Length height);

template <typename PixelType>
void crop(Image<PixelType>& img, Index x0, Index y0, Length width, Length height);

// ----------
// Aliases:

using Image_8u1 = Image<Pixel<std::uint8_t, 1>>;  ///< 8-bit unsigned 1-channel image.
using Image_8u2 = Image<Pixel<std::uint8_t, 2>>;  ///< 8-bit unsigned 2-channel image.
using Image_8u3 = Image<Pixel<std::uint8_t, 3>>;  ///< 8-bit unsigned 3-channel image.
using Image_8u4 = Image<Pixel<std::uint8_t, 4>>;  ///< 8-bit unsigned 4-channel image.

using Image_8s1 = Image<Pixel<std::int8_t, 1>>;  ///< 8-bit signed 1-channel image.
using Image_8s2 = Image<Pixel<std::int8_t, 2>>;  ///< 8-bit signed 2-channel image.
using Image_8s3 = Image<Pixel<std::int8_t, 3>>;  ///< 8-bit signed 3-channel image.
using Image_8s4 = Image<Pixel<std::int8_t, 4>>;  ///< 8-bit signed 4-channel image.

using Image_16u1 = Image<Pixel<std::uint16_t, 1>>;  ///< 16-bit unsigned 1-channel image.
using Image_16u2 = Image<Pixel<std::uint16_t, 2>>;  ///< 16-bit unsigned 2-channel image.
using Image_16u3 = Image<Pixel<std::uint16_t, 3>>;  ///< 16-bit unsigned 3-channel image.
using Image_16u4 = Image<Pixel<std::uint16_t, 4>>;  ///< 16-bit unsigned 4-channel image.

using Image_16s1 = Image<Pixel<std::int16_t, 1>>;  ///< 16-bit signed 1-channel image.
using Image_16s2 = Image<Pixel<std::int16_t, 2>>;  ///< 16-bit signed 2-channel image.
using Image_16s3 = Image<Pixel<std::int16_t, 3>>;  ///< 16-bit signed 3-channel image.
using Image_16s4 = Image<Pixel<std::int16_t, 4>>;  ///< 16-bit signed 4-channel image.

using Image_32u1 = Image<Pixel<std::uint32_t, 1>>;  ///< 32-bit unsigned 1-channel image.
using Image_32u2 = Image<Pixel<std::uint32_t, 2>>;  ///< 32-bit unsigned 2-channel image.
using Image_32u3 = Image<Pixel<std::uint32_t, 3>>;  ///< 32-bit unsigned 3-channel image.
using Image_32u4 = Image<Pixel<std::uint32_t, 4>>;  ///< 32-bit unsigned 4-channel image.

using Image_32s1 = Image<Pixel<std::int32_t, 1>>;  ///< 32-bit signed 1-channel image.
using Image_32s2 = Image<Pixel<std::int32_t, 2>>;  ///< 32-bit signed 2-channel image.
using Image_32s3 = Image<Pixel<std::int32_t, 3>>;  ///< 32-bit signed 3-channel image.
using Image_32s4 = Image<Pixel<std::int32_t, 4>>;  ///< 32-bit signed 4-channel image.

using Image_64u1 = Image<Pixel<std::uint64_t, 1>>;  ///< 64-bit unsigned 1-channel image.
using Image_64u2 = Image<Pixel<std::uint64_t, 2>>;  ///< 64-bit unsigned 2-channel image.
using Image_64u3 = Image<Pixel<std::uint64_t, 3>>;  ///< 64-bit unsigned 3-channel image.
using Image_64u4 = Image<Pixel<std::uint64_t, 4>>;  ///< 64-bit unsigned 4-channel image.

using Image_64s1 = Image<Pixel<std::int64_t, 1>>;  ///< 64-bit signed 1-channel image.
using Image_64s2 = Image<Pixel<std::int64_t, 2>>;  ///< 64-bit signed 2-channel image.
using Image_64s3 = Image<Pixel<std::int64_t, 3>>;  ///< 64-bit signed 3-channel image.
using Image_64s4 = Image<Pixel<std::int64_t, 4>>;  ///< 64-bit signed 4-channel image.

using Image_32f1 = Image<Pixel<float32_t, 1>>;  ///< 32-bit floating point 1-channel image.
using Image_32f2 = Image<Pixel<float32_t, 2>>;  ///< 32-bit floating point 2-channel image.
using Image_32f3 = Image<Pixel<float32_t, 3>>;  ///< 32-bit floating point 3-channel image.
using Image_32f4 = Image<Pixel<float32_t, 4>>;  ///< 32-bit floating point 4-channel image.

using Image_64f1 = Image<Pixel<float64_t, 1>>;  ///< 64-bit floating point 1-channel image.
using Image_64f2 = Image<Pixel<float64_t, 2>>;  ///< 64-bit floating point 2-channel image.
using Image_64f3 = Image<Pixel<float64_t, 3>>;  ///< 64-bit floating point 3-channel image.
using Image_64f4 = Image<Pixel<float64_t, 4>>;  ///< 64-bit floating point 4-channel image.

// ----------
// Implementation:

/** \brief Default constructor.
 *
 * Creates an empty image of width and height 0. The image data will be owned, i.e. `is_view() == false`.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType>
Image<PixelType>::Image() : data_(nullptr), stride_bytes_(0), width_(0), height_(0), owns_memory_(true)
{
}

/** \brief Constructs an image of the specified width and height.
 *
 * Image content will be undefined.
 * The image data will be owned, i.e. `is_view() == false`.
 *
 * @tparam PixelType The pixel type.
 * @param width Desired image width.
 * @param height Desired image height.
 */
template <typename PixelType>
Image<PixelType>::Image(Length width, Length height)
    : data_(nullptr)
    , stride_bytes_(PixelTraits<PixelType>::nr_bytes * width)
    , width_(width)
    , height_(height)
    , owns_memory_(true)
{
  allocate_bytes(stride_bytes_ * height_);
}

/** \brief Constructs an image of the specified width, height, and stride in bytes.
 *
 * Image content will be undefined.
 * The image data will be owned, i.e. `is_view() == false`.
 *
 * @tparam PixelType The pixel type.
 * @param width Desired image width.
 * @param height Desired image height.
 * @param stride_bytes The stride (row length) in bytes.
 */
template <typename PixelType>
Image<PixelType>::Image(Length width, Length height, Stride stride_bytes)
    : data_(nullptr)
    , stride_bytes_(std::max(stride_bytes, PixelTraits<PixelType>::nr_bytes * width))
    , width_(width)
    , height_(height)
    , owns_memory_(true)
{
  allocate_bytes(stride_bytes_ * height_);
}

/** \brief Constructs an image view (non-owned data) from supplied memory.
 *
 * @tparam PixelType The pixel type.
 * @param data Pointer to the existing image data.
 * @param width Image width.
 * @param height Image height.
 * @param stride_bytes The stride (row length) in bytes.
 */
template <typename PixelType>
Image<PixelType>::Image(std::uint8_t* data, Length width, Length height, Stride stride_bytes)
    : data_(data), stride_bytes_(stride_bytes), width_(width), height_(height), owns_memory_(false)
{
  SELENE_ASSERT(width_ > 0 && height_ > 0 && stride_bytes_ > 0);
}

/** \brief Constructs an image (owned data) from supplied memory.
 *
 * @tparam PixelType The pixel type.
 * @param data A `MemoryBlock<NewAllocator>` with the existing data.
 * @param width Image width.
 * @param height Image height.
 * @param stride_bytes The stride (row length) in bytes.
 */
template <typename PixelType>
Image<PixelType>::Image(MemoryBlock<NewAllocator>&& data, Length width, Length height, Stride stride_bytes)
    : data_(data.transfer_data()), stride_bytes_(stride_bytes), width_(width), height_(height), owns_memory_(true)
{
  SELENE_ASSERT(width_ > 0 && height_ > 0 && stride_bytes_ > 0);
}

/** \brief Copy constructor.
 *
 * Constructs an image instance from the supplied image.
 *
 * The ownership semantics will stay the same; i.e. if the supplied image has owned data, then so will the constructed
 * image (the data will be copied, s.t. `is_view() == false`), but if the supplied image points to non-owned data, then
 * the constructed image will be a view (`is_view() == true`).
 *
 * @tparam PixelType The pixel type.
 * @param other The source image.
 */
template <typename PixelType>
inline Image<PixelType>::Image(const Image<PixelType>& other)
    : data_(other.data_)
    , stride_bytes_(other.stride_bytes_)
    , width_(other.width_)
    , height_(other.height_)
    , owns_memory_(other.owns_memory_)
{
  // Keep the image semantics, i.e. owned will be owned, and non-owned will stay non-owned
  if (!other.owns_memory_ || other.is_empty())
  {
    return;
  }

  // Allocate memory to copy data from other image
  allocate_bytes(height_ * stride_bytes_);
  // Copy the image data
  copy_rows_from(other);
}

/** \brief Copy assignment operator
 *
 * Assigns another image instance.
 *
 * The ownership semantics will stay the same; i.e. if the supplied image has owned data, then so will the constructed
 * image (the data will be copied, s.t. `is_view() == false`), but if the supplied image points to non-owned data, then
 * the constructed image will be a view (`is_view() == true`).
 *
 * @tparam PixelType The pixel type.
 * @param other The image to assign from.
 * @return A reference to this image.
 */
template <typename PixelType>
inline Image<PixelType>& Image<PixelType>::operator=(const Image<PixelType>& other)
{
  // Check for self-assignment
  if (this == &other)
  {
    return *this;
  }

  const auto equal_size = (total_bytes() == other.total_bytes());

  // Deallocate own memory if we need to
  if (!equal_size || !other.owns_memory_)
  {
    deallocate_bytes_if_owned();
  }

  // Copy over values from other image
  data_ = other.data_;
  stride_bytes_ = other.stride_bytes_;
  width_ = other.width_;
  height_ = other.height_;
  owns_memory_ = other.owns_memory_;

  if (other.owns_memory_)
  {
    // Allocate if we deallocated
    if (!equal_size)
    {
      allocate_bytes(stride_bytes_ * height_);
    }

    // Copy the image data
    copy_rows_from(other);
  }

  return *this;
}

/** \brief Move constructor.
 *
 * @tparam PixelType The pixel type.
 * @param other The image to move from.
 */
template <typename PixelType>
inline Image<PixelType>::Image(Image<PixelType>&& other) noexcept
    : data_(other.data_)
    , stride_bytes_(other.stride_bytes_)
    , width_(other.width_)
    , height_(other.height_)
    , owns_memory_(other.owns_memory_)
{
  other.reset();
}

/**\brief Move assignment operator.
 *
 * @tparam PixelType The pixel type.
 * @param other The image to move assign from.
 * @return A reference to this image.
 */
template <typename PixelType>
inline Image<PixelType>& Image<PixelType>::operator=(Image<PixelType>&& other) noexcept
{
  // Check for self-assignment
  if (this == &other)
  {
    return *this;
  }

  // Clean up own data
  deallocate_bytes_if_owned();

  // Set local values
  data_ = other.data_;
  stride_bytes_ = other.stride_bytes_;
  width_ = other.width_;
  height_ = other.height_;
  owns_memory_ = other.owns_memory_;

  // Reset other image
  other.reset();

  return *this;
}

/** \brief Destructor.
 *
 * Owned data will be deallocated at destruction time.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType>
inline Image<PixelType>::~Image()
{
  deallocate_bytes_if_owned();
}

/** \brief Returns the image width.
 *
 * @tparam PixelType The pixel type.
 * @return Width of the image in pixels.
 */
template <typename PixelType>
inline Length Image<PixelType>::width() const
{
  return width_;
}

/** \brief Returns the image height.
 *
 * @tparam PixelType The pixel type.
 * @return Height of the image in pixels.
 */
template <typename PixelType>
inline Length Image<PixelType>::height() const
{
  return height_;
}

/** \brief Returns the row stride of the image in bytes.
 *
 * The row stride is the number of bytes that a row occupies in memory.
 * It has to be greater or equal to the width times the size of a pixel element:
 * `(stride_bytes() >= width() * PixelTraits::nr_bytes)`.
 * If it is equal, then `is_packed()` returns `true`, otherwise `is_packed()` returns `false`.
 *
 * @tparam PixelType The pixel type.
 * @return Row stride in bytes.
 */
template <typename PixelType>
inline Stride Image<PixelType>::stride_bytes() const
{
  return stride_bytes_;
}

/** \brief Returns the number of data bytes occupied by each image row.
 *
 * The value returned is equal to `(width() * PixelTraits::nr_bytes)`.
 * It follows that `stride_bytes() >= row_bytes()`, since `stride_bytes()` may include additional padding bytes.
 *
 * @tparam PixelType The pixel type.
 * @return Number of data bytes occupied by each image row.
 */
template <typename PixelType>
inline std::size_t Image<PixelType>::row_bytes() const
{
  return width_ * PixelTraits<PixelType>::nr_bytes;
}

/** \brief Returns the total number of bytes occupied by the image data in memory.
 *
 * The value returned is equal to `(stride_bytes() * height())`.
 *
 * @tparam PixelType The pixel type.
 * @return Number of bytes occupied by the image data in memory.
 */
template <typename PixelType>
inline std::size_t Image<PixelType>::total_bytes() const
{
  return stride_bytes_ * height_;
}

/** \brief Returns whether the image data is stored packed in memory.
 *
 * Returns the boolean expression `(stride_bytes() == width() * PixelTraits::nr_bytes)`.
 *
 * @tparam PixelType The pixel type.
 * @return True, if the image data stored packed; false otherwise.
 */
template <typename PixelType>
inline bool Image<PixelType>::is_packed() const
{
  return stride_bytes_ == PixelTraits<PixelType>::nr_bytes * width_;
}

/** \brief Returns whether the image is a view onto (non-owned) memory.
 *
 * @tparam PixelType The pixel type.
 * @return True, if the image data points to non-owned memory; false otherwise, i.e. if the instance owns its own
 *         memory.
 */
template <typename PixelType>
inline bool Image<PixelType>::is_view() const
{
  return !owns_memory_;
}

/** \brief Returns whether the image is empty.
 *
 * An image is considered empty if its internal data pointer points to `nullptr`, `width() == 0`, `height() == 0`, or
 * any combination of these.
 *
 * @tparam PixelType The pixel type.
 * @return True, if the image is empty; false if it is non-empty.
 */
template <typename PixelType>
inline bool Image<PixelType>::is_empty() const
{
  return data_ == nullptr || width_ == 0 || height_ == 0;
}

/** \brief Returns whether the instance represents a valid image.
 *
 * Semantically equal to `!is_empty()`.
 *
 * @tparam PixelType The pixel type.
 * @return True, if the image is valid; false otherwise.
 */
template <typename PixelType>
inline bool Image<PixelType>::is_valid() const
{
  return !is_empty();
}

/** \brief Resets the image instance by clearing the image data and resetting the internal state to the state after
 * default construction.
 *
 * Postconditions: `data() == nullptr && width() == 0 && height() == 0 && stride_bytes() == 0 && is_empty() &&
 * !is_valid() && !is_view()`.
 *
 * @tparam PixelType The pixel type.
 */
template <typename PixelType>
void Image<PixelType>::clear()
{
  deallocate_bytes_if_owned();
  reset();
}

/** \brief Fills the image data, i.e. each pixel, with the specified value.
 *
 * @tparam PixelType The pixel type.
 * @param value The value that each image pixel should assume.
 */
template <typename PixelType>
void Image<PixelType>::fill(PixelType value)
{
  for (std::uint32_t y = 0; y < height_; ++y)
  {
    std::fill(data(y), data_row_end(y), value);
  }
}

/** \brief Resizes the allocated image data to exactly fit an image of size (width x height).
 *
 * Precondition: `!is_view()`.
 *
 * Postconditions: `!is_view() && is_packed()`.
 *
 * Equivalent to `resize(width, height, width * PixelTraits::nr_bytes`.
 * Images that are views onto non-owned memory cannot be resized. In this case, a `std::runtime_error` exception will
 * be thrown.
 *
 * @tparam PixelType The pixel type.
 * @param width The new image width.
 * @param height The new image height.
 */
template <typename PixelType>
void Image<PixelType>::allocate(Length width, Length height)
{
  const auto stride_bytes = PixelTraits<PixelType>::nr_bytes * width;
  allocate(width, height, stride_bytes);
}

/** \brief Resizes the allocated image data to exactly fit an image of size (width x height), with user-defined row
 * stride.
 *
 * Precondition: `!is_view()`.
 *
 * Postconditions: `!is_view() && (stride_bytes() >= width() * PixelTraits::nr_bytes)`.
 *
 * Images that are views onto non-owned memory cannot be resized. In this case, a `std::runtime_error` exception will
 * be thrown.
 *
 * No memory reallocation will happen, if the specified `width`, `height`, and `stride_bytes` parameters already match
 * the internal state.
 *
 * @tparam PixelType The pixel type.
 * @param width The new image width.
 * @param height The new image height.
 * @param stride_bytes The desired row stride in bytes.
 */
template <typename PixelType>
void Image<PixelType>::allocate(Length width, Length height, Stride stride_bytes)
{
  // No need to act, if size parameters match
  if (width_ == width && height_ == height && stride_bytes_ == stride_bytes)
  {
    return;
  }

  if (!owns_memory_)
  {
    throw std::runtime_error("Cannot reallocate external data");
  }

  stride_bytes = std::max(stride_bytes, PixelTraits<PixelType>::nr_bytes * width);

  deallocate_bytes();
  allocate_bytes(stride_bytes * height);

  stride_bytes_ = stride_bytes;
  width_ = width;
  height_ = height;
  owns_memory_ = true;
}

/** \brief Sets the image data to be a view onto non-owned external memory.
 *
 * Postcondition: `is_view()`.
 *
 * @tparam PixelType The pixel type.
 * @param data Pointer to the external data.
 * @param width The image width.
 * @param height The image height.
 * @param stride_bytes The row stride in bytes.
 */
template <typename PixelType>
inline void Image<PixelType>::set_view(std::uint8_t* data, Length width, Length height, Stride stride_bytes)
{
  // Clean up own data
  deallocate_bytes_if_owned();

  // Set local values
  data_ = data;
  stride_bytes_ = stride_bytes;
  width_ = width;
  height_ = height;
  owns_memory_ = false;
}

/** \brief Sets the image data to the provided memory block, which will be owned by the `Image<>` instance.
 *
 * Precondition: `data.size() == stride_bytes * height`.
 *
 * Postcondition: `!is_view()`.
 *
 * @tparam PixelType The pixel type.
 * @param data Memory block of image data, to be owned by the image instance.
 * @param width The image width.
 * @param height The image height.
 * @param stride_bytes The row stride in bytes.
 */
template <typename PixelType>
inline void Image<PixelType>::set_data(MemoryBlock<NewAllocator>&& data, Length width, Length height,
                                       Stride stride_bytes)
{
  SELENE_ASSERT(data.size() == stride_bytes * height);

  // Clean up own data
  deallocate_bytes_if_owned();

  // Set local values
  data_ = data.transfer_data();
  stride_bytes_ = stride_bytes;
  width_ = width;
  height_ = height;
  owns_memory_ = true;
}

/** \brief Returns a pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType The pixel type.
 * @return Pointer to the first pixel element.
 */
template <typename PixelType>
inline PixelType* Image<PixelType>::data()
{
  return reinterpret_cast<PixelType*>(byte_ptr());
}

/** \brief Returns a constant pointer to the first pixel element (i.e. at row 0, column 0).
 *
 * @tparam PixelType The pixel type.
 * @return Constant pointer to the first pixel element.
 */
template <typename PixelType>
inline const PixelType* Image<PixelType>::data() const
{
  return reinterpret_cast<const PixelType*>(byte_ptr());
}

/** \brief Returns a pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the first pixel element of the y-th row.
 */
template <typename PixelType>
inline PixelType* Image<PixelType>::data(Index y)
{
  return reinterpret_cast<PixelType*>(byte_ptr(y));
}

/** \brief Returns a constant pointer to the first pixel element of the y-th row (i.e. at row y, column 0).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first pixel element of the y-th row.
 */
template <typename PixelType>
inline const PixelType* Image<PixelType>::data(Index y) const
{
  return reinterpret_cast<const PixelType*>(byte_ptr(y));
}

/** \brief Returns a pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y, column `width()`).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename PixelType>
inline PixelType* Image<PixelType>::data_row_end(Index y)
{
  return reinterpret_cast<PixelType*>(byte_ptr(y) + PixelTraits<PixelType>::nr_bytes * width_);
}

/** \brief Returns a constant pointer to the one-past-the-last pixel element of the y-th row (i.e. at row y,
 * column `width()`).
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the one-past-the-last pixel element of the y-th row.
 */
template <typename PixelType>
inline const PixelType* Image<PixelType>::data_row_end(Index y) const
{
  return reinterpret_cast<const PixelType*>(byte_ptr(y) + PixelTraits<PixelType>::nr_bytes * width_);
}

/** \brief Returns a pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the x-th pixel element of the y-th row.
 */
template <typename PixelType>
inline PixelType* Image<PixelType>::data(Index x, Index y)
{
  return reinterpret_cast<PixelType*>(byte_ptr(x, y));
}

/** \brief Returns a constant pointer to the x-th pixel element of the y-th row (i.e. at row y, column x).
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the x-th pixel element of the y-th row.
 */
template <typename PixelType>
inline const PixelType* Image<PixelType>::data(Index x, Index y) const
{
  return reinterpret_cast<const PixelType*>(byte_ptr(x, y));
}

/** \brief Returns a pointer to the first byte storing image data (in row 0).
 *
 * @tparam PixelType The pixel type.
 * @return Pointer to the first image data byte.
 */
template <typename PixelType>
inline std::uint8_t* Image<PixelType>::byte_ptr()
{
  return data_;
}

/** \brief Returns a constant pointer to the first byte storing image data (in row 0).
 *
 * @tparam PixelType The pixel type.
 * @return Constant pointer to the first image data byte.
 */
template <typename PixelType>
inline const std::uint8_t* Image<PixelType>::byte_ptr() const
{
  return data_;
}

/** \brief Returns a pointer to the first byte storing image data in row `y`.
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Pointer to the first image data byte of row `y`.
 */
template <typename PixelType>
inline std::uint8_t* Image<PixelType>::byte_ptr(Index y)
{
  return data_ + compute_data_offset(y);
}

/** \brief Returns a constant pointer to the first byte storing image data in row `y`.
 *
 * @tparam PixelType The pixel type.
 * @param y Row index.
 * @return Constant pointer to the first image data byte of row `y`.
 */
template <typename PixelType>
inline const std::uint8_t* Image<PixelType>::byte_ptr(Index y) const
{
  return data_ + compute_data_offset(y);
}

/** \brief Returns a pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename PixelType>
inline std::uint8_t* Image<PixelType>::byte_ptr(Index x, Index y)
{
  return data_ + compute_data_offset(x, y);
}

/** \brief Returns a constant pointer to the first byte of the pixel element at location `(x, y)`, i.e. row `y`, column
 * `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant pointer to the first byte of the pixel element at location `(x, y)`.
 */
template <typename PixelType>
inline const std::uint8_t* Image<PixelType>::byte_ptr(Index x, Index y) const
{
  return data_ + compute_data_offset(x, y);
}

/** \brief Returns a reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Reference to the pixel element at location `(x, y)`.
 */
template <typename PixelType>
inline PixelType& Image<PixelType>::operator()(Index x, Index y)
{
  return *data(x, y);
}

/** \brief Returns a constant reference to the pixel element at location `(x, y)`, i.e. row `y`, column `x`.
 *
 * @tparam PixelType The pixel type.
 * @param x Column index.
 * @param y Row index.
 * @return Constant reference to the pixel element at location `(x, y)`.
 */
template <typename PixelType>
inline const PixelType& Image<PixelType>::operator()(Index x, Index y) const
{
  return *data(x, y);
}

template <typename PixelType>
void Image<PixelType>::allocate_bytes(std::size_t nr_bytes)
{
  SELENE_ASSERT(owns_memory_);

  auto memory = NewAllocator::allocate(nr_bytes);
  SELENE_ASSERT(memory.size() == nr_bytes);
  data_ = memory.transfer_data();
}

template <typename PixelType>
void Image<PixelType>::deallocate_bytes()
{
  SELENE_ASSERT(owns_memory_);

  if (data_)
  {
    NewAllocator::deallocate(data_);
    SELENE_ASSERT(data_ == nullptr);
  }
}

template <typename PixelType>
void Image<PixelType>::deallocate_bytes_if_owned()
{
  if (owns_memory_)
  {
    deallocate_bytes();
  }
}

template <typename PixelType>
void Image<PixelType>::reset()
{
  // reset to default constructed state
  data_ = nullptr;
  stride_bytes_ = Stride{0};
  width_ = Length{0};
  height_ = Length{0};
  owns_memory_ = true;
}

template <typename PixelType>
void Image<PixelType>::copy_rows_from(const Image<PixelType>& src)
{
  SELENE_ASSERT(data_ && src.data_);
  SELENE_ASSERT(width_ == src.width_ && height_ == src.height_);

  for (Index y = 0; y < height_; ++y)
  {
    std::copy(src.data(y), src.data_row_end(y), data(y));
  }
}

template <typename PixelType>
inline std::uint32_t Image<PixelType>::compute_data_offset(Index y) const
{
  return stride_bytes_ * y;
}

template <typename PixelType>
inline std::uint32_t Image<PixelType>::compute_data_offset(Index x, Index y) const
{
  return stride_bytes_ * y + PixelTraits<PixelType>::nr_bytes * x;
}

template <typename PixelType>
inline MemoryBlock<NewAllocator> Image<PixelType>::relinquish_data_ownership()
{
  SELENE_FORCED_ASSERT(owns_memory_);
  const auto ptr = data_;
  const auto len = total_bytes();

  owns_memory_ = false;
  clear();
  return construct_memory_block_from_existing_memory<NewAllocator>(ptr, len);
}

// ----------

/** \brief Copies the image represented by `src` into the image `dst`.
 *
 * After copying, `dst` will be owning its image data memory.
 *
 * @tparam PixelType The pixel type.
 * @param src Source image.
 * @param dst Destination image.
 */
template <typename PixelType>
void clone(const Image<PixelType>& src, Image<PixelType>& dst)
{
  if (&src == &dst)
  {
    throw std::runtime_error("Destination cannot be the same as the source for image cloning");
  }

  dst.allocate(src.width(), src.height(), src.stride_bytes());
  dst.copy_rows_from(src);
}

/** \brief Copies the specified part of the source image `src` into the image `dst`.
 *
 * There is no explicit check whether the region to be copied is within the source image bounds. Specifying an invalid
 * region results in undefined behavior.
 *
 * @tparam PixelType The pixel type.
 * @param src Source image.
 * @param x0 The top-left x-coordinate of the region to be copied.
 * @param y0 The top-left y-coordinate of the region to be copied.
 * @param width The width of the region to be copied.
 * @param height The height of the region to be copied.
 * @param dst Destination image.
 */
template <typename PixelType>
void clone(const Image<PixelType>& src, Index x0, Index y0, Length width, Length height, Image<PixelType>& dst)
{
  auto src_sub_view = view(src, x0, y0, width, height);
  clone(src_sub_view, dst);
}

/** \brief Returns a copy of the provided source image.
 *
 * After copying, the returned image will be owning its image data memory.
 *
 * @tparam PixelType The pixel type.
 * @param src Source image.
 * @return Copied image.
 */
template <typename PixelType>
Image<PixelType> clone(const Image<PixelType>& src)
{
  Image<PixelType> dst;
  clone(src, dst);
  return dst;
}

/** \brief Returns a copy of the specified sub-region of the source image `src`.
 *
 * There is no explicit check whether the region to be copied is within the source image bounds. Specifying an invalid
 * region results in undefined behavior.
 *
 * @tparam PixelType The pixel type.
 * @param src Source image.
 * @param x0 The top-left x-coordinate of the sub-region to be copied.
 * @param y0 The top-left y-coordinate of the sub-region to be copied.
 * @param width The width of the sub-region to be copied.
 * @param height The height of the sub-region to be copied.
 * @return Copied image, containing the sub-region.
 */
template <typename PixelType>
Image<PixelType> clone(const Image<PixelType>& src, Index x0, Index y0, Length width, Length height)
{
  Image<PixelType> dst;
  clone(src, x0, y0, width, height, dst);
  return dst;
}

/** \brief Returns an image representing a view onto the provided source image.
 *
 * @tparam PixelType The pixel type.
 * @param src Source image.
 * @return An image (pointing to non-owned memory), which represents a view onto the source image.
 */
template <typename PixelType>
Image<PixelType> view(const Image<PixelType>& src)
{
  return Image<PixelType>(src.byte_ptr(), src.width(), src.height(), src.stride_bytes());
}

/** \brief Returns an image representing a view onto the specified sub-region of the provided source image.
 *
 * @tparam PixelType The pixel type.
 * @param src Source image.
 * @param x0 The top-left x-coordinate of the sub-region.
 * @param y0 The top-left y-coordinate of the sub-region.
 * @param width The width of the sub-region.
 * @param height The height of the sub-region.
 * @return An image (pointing to non-owned memory), which represents a view onto the source image sub-region.
 */
template <typename PixelType>
Image<PixelType> view(const Image<PixelType>& src, Index x0, Index y0, Length width, Length height)
{
  return Image<PixelType>(src.byte_ptr(x0, y0), width, height, src.stride_bytes());
}

/** \brief Crops the image `img` to the specified sub-region.
 *
 * There is no explicit check whether the crop region is within the image bounds. Specifying an invalid region results
 * in undefined behavior.
 *
 * @tparam PixelType The pixel type.
 * @param img The image to be cropped.
 * @param x0 The top-left x-coordinate of the sub-region.
 * @param y0 The top-left y-coordinate of the sub-region.
 * @param width The width of the sub-region.
 * @param height The height of the sub-region.
 */
template <typename PixelType>
void crop(Image<PixelType>& img, Index x0, Index y0, Length width, Length height)
{
  auto cropped_clone = clone(img, x0, y0, width, height);
  img = std::move(cropped_clone);
}

}  // namespace img
}  // namespace selene

#endif  // SELENE_IMG_IMAGE_HPP
