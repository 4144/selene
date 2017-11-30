// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PIXEL_TRAITS_HPP
#define SELENE_IMG_PIXEL_TRAITS_HPP

/// @file

#include <selene/img/Pixel.hpp>
#include <selene/img/PixelFormat.hpp>

#include <cstdint>
#include <cstdlib>
#include <type_traits>

namespace selene {
namespace img {

/** \brief Class representing traits of a pixel.
 *
 * @tparam Element_ The pixel element type.
 */
template <typename Element_>
struct PixelTraits
{
  using Element = Element_;  ///< The pixel element type.
  static constexpr std::uint32_t nr_channels = 1;  ///< The number of channels per pixel.
  static constexpr std::uint8_t nr_bytes = sizeof(Element);  ///< The number of bytes per pixel.
  static constexpr std::uint8_t nr_bytes_per_channel = sizeof(Element);  ///< The number of bytes per pixel channel.

  /// True, if the pixel elements are integral values; false otherwise
  static constexpr bool is_integral = std::is_integral<Element>::value;

  /// True, if the pixel elements are floating point values; false otherwise
  static constexpr bool is_floating_point = std::is_floating_point<Element>::value;

  /// True, if the pixel elements are unsigned; false otherwise
  static constexpr bool is_unsigned = std::is_unsigned<Element>::value;

  /// The sample type (unsigned/signed integer or floating point number)
  static constexpr SampleType sample_type = \
      std::is_integral<Element>::value ? (std::is_unsigned<Element>::value ? SampleType::UnsignedInteger
                                                                           : SampleType::SignedInteger)
                                       : (std::is_floating_point<Element>::value ? SampleType::FloatingPoint
                                                                                 : SampleType::Unknown);
};

/** \brief Class representing traits of a pixel. Specialization for `Pixel<T, N>`.
 *
 * @tparam Element_ The pixel element type.
 */
template <typename T, std::uint32_t N>
struct PixelTraits<Pixel<T, N>>
{
  using Element = T;  ///< The pixel element type.
  static constexpr std::uint32_t nr_channels = N;  ///< The number of channels per pixel.
  static constexpr std::uint8_t nr_bytes = sizeof(Pixel<T, N>);  ///< The number of bytes per pixel.
  static constexpr std::uint8_t nr_bytes_per_channel = sizeof(Element);  ///< The number of bytes per pixel channel.

  /// True, if the pixel elements are integral values; false otherwise
  static constexpr bool is_integral = std::is_integral<T>::value;

  /// True, if the pixel elements are floating point values; false otherwise
  static constexpr bool is_floating_point = std::is_floating_point<T>::value;

  /// True, if the pixel elements are unsigned; false otherwise
  static constexpr bool is_unsigned = std::is_unsigned<T>::value;

  /// The sample type (unsigned/signed integer or floating point number)
  static constexpr SampleType sample_type = \
      std::is_integral<T>::value ? (std::is_unsigned<T>::value ? SampleType::UnsignedInteger
                                                               : SampleType::SignedInteger)
                                 : (std::is_floating_point<T>::value ? SampleType::FloatingPoint
                                                                     : SampleType::Unknown);
};

} // namespace img
} // namespace selene

#endif // SELENE_IMG_PIXEL_TRAITS_HPP