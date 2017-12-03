// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PNG_WRITE_HPP
#define SELENE_IMG_PNG_WRITE_HPP

/// @file

#if defined(SELENE_WITH_LIBPNG)

#include <selene/base/Allocators.hpp>
#include <selene/base/Assert.hpp>
#include <selene/base/MessageLog.hpp>
#include <selene/base/Utils.hpp>

#include <selene/img/BoundingBox.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/RowPointers.hpp>
#include <selene/img/detail/Util.hpp>

#include <selene/io/FileWriter.hpp>
#include <selene/io/VectorWriter.hpp>

#include <array>
#include <csetjmp>
#include <cstdio>
#include <memory>

namespace selene {
namespace img {

// Forward declarations

struct PNGCompressionOptions;
class PNGCompressionObject;

namespace detail
{
class PNGCompressionCycle;
void set_destination(PNGCompressionObject&, io::FileWriter&);
void set_destination(PNGCompressionObject&, io::VectorWriter&);
} // namespace detail

/** \brief PNG compression options.
 *
 * For more detailed information, consult the libpng manual (libpng-manual.txt) provided with every libpng source
 * distribution, or available here: http://www.libpng.org/pub/png/libpng-manual.txt
 */
struct PNGCompressionOptions
{
  int compression_level;  ///< Compression level; may take values from 0 (no compression) to 9 ("maximal" compression).
  bool interlaced;  ///< If true, write PNG image as interlaced.
  bool set_bgr;  ///< If true, convert BGR (supplied) to RGB (written).
  bool invert_alpha_channel;  ///< If true, invert values in alpha channel (e.g. 0 -> 255).
  bool invert_monochrome;  ///< If true, invert grayscale or grayscale_alpha image values.

  /** \brief Constructor, setting the respective JPEG compression options.
   *
   * @param compression_level_ Compression level; may take values from 0 (no compression) to 9 ("maximal" compression).
   * Defaults to the default zlib compression level.
   * @param interlaced_ If true, write PNG image as interlaced.
   * @param set_bgr_ If true, convert BGR (supplied) to RGB (written).
   * @param invert_alpha_channel_ If true, invert values in alpha channel (e.g. 0 -> 255).
   * @param invert_monochrome_ If true, invert grayscale or grayscale_alpha image values.
   */
  explicit PNGCompressionOptions(int compression_level_ = -1, bool interlaced_ = false, bool set_bgr_ = false,
                                 bool invert_alpha_channel_ = false, bool invert_monochrome_ = false)
      : compression_level(compression_level_), interlaced(interlaced_), set_bgr(set_bgr_),
        invert_alpha_channel(invert_alpha_channel_), invert_monochrome(invert_monochrome_)
  {
  }
};

/** Opaque PNG compression object, holding internal state.
 *
 */
class PNGCompressionObject
{
public:
  /// \cond INTERNAL
  PNGCompressionObject();
  ~PNGCompressionObject();

  bool valid() const;
  bool error_state() const;
  const MessageLog& message_log() const;

  bool set_image_info(int width, int height, int nr_channels, int bit_depth, bool interlaced, PixelFormat pixel_format);
  bool set_compression_parameters(int, bool);
  /// \endcond

private:
  struct Impl;
  std::unique_ptr <Impl> impl_;

  friend class detail::PNGCompressionCycle;
  friend void detail::set_destination(PNGCompressionObject&, io::FileWriter&);
  friend void detail::set_destination(PNGCompressionObject&, io::VectorWriter&);
};


template <typename SinkType>
bool write_png(const ImageData& img_data, SinkType& sink, PNGCompressionOptions options = PNGCompressionOptions(),
               MessageLog* messages = nullptr);

template <typename SinkType>
bool write_png(const ImageData& img_data, PNGCompressionObject& obj, SinkType& sink,
               PNGCompressionOptions options = PNGCompressionOptions(), MessageLog* messages = nullptr);

} // namespace img
} // namespace selene

#endif // defined(SELENE_WITH_LIBPNG)

#endif // SELENE_IMG_PNG_WRITE_HPP
