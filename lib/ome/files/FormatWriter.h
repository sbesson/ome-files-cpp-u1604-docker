/*
 * #%L
 * OME-FILES C++ library for image IO.
 * Copyright © 2006 - 2015 Open Microscopy Environment:
 *   - Massachusetts Institute of Technology
 *   - National Institutes of Health
 *   - University of Dundee
 *   - Board of Regents of the University of Wisconsin-Madison
 *   - Glencoe Software, Inc.
 * %%
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of any organization.
 * #L%
 */

#ifndef OME_FILES_FORMATWRITER_H
#define OME_FILES_FORMATWRITER_H

#include <array>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <map>

#include <boost/optional.hpp>

#include <ome/files/CoreMetadata.h>
#include <ome/files/FileInfo.h>
#include <ome/files/FormatHandler.h>
#include <ome/files/MetadataConfigurable.h>
#include <ome/files/MetadataMap.h>
#include <ome/files/Types.h>

#include <ome/xml/meta/MetadataRetrieve.h>

namespace ome
{
  namespace files
  {

    class VariantPixelBuffer;

    /**
     * Interface for all biological file format writers.
     *
     * @note No IOException in C++.
     *
     * @note getColorModel and setColorModel not implemented; it is
     * Java-specific and if needed will require reimplementing in C++.
     *
     * @note getCodecOptions and setCodec options not implemented;
     * these rely on the Java-specific ColorModel and its design
     * reflects that of Java c javax.imageio which we can't use here,
     * though bits of it might require reimplementing.
     *
     * @todo Implement saveBytes methods using 2D @c Region with
     * and/or in terms of Boost.MultiArray index ranges.
     */
    class FormatWriter : virtual public FormatHandler
    {
    public:
      /// Frame rate type.
      typedef uint16_t frame_rate_type;

    protected:
      /// Constructor.
      FormatWriter()
      {}

      /// @cond SKIP
      FormatWriter (const FormatWriter&) = delete;

      FormatWriter&
      operator= (const FormatWriter&) = delete;
      /// @endcond SKIP

    public:
      /// Destructor.
      virtual
      ~FormatWriter()
      {}

      /**
       * Set the color lookup table associated with the current
       * series.
       *
       * If the pixel type of the lookup table is unsupported by the
       * file format, this method will throw an exception.
       *
       * @param plane the plane index within the series.
       * @param buf the source pixel buffer.
       */
      virtual
      void
      setLookupTable(dimension_size_type       plane,
                     const VariantPixelBuffer& buf) = 0;

      /**
       * Save an image plane.
       *
       * Write an image plane from a VariantPixelBuffer
       * of size
       *
       * \code{.cpp}
       * getSizeX * getSizeY * bytesPerPixel * getRGBChannelCount()
       * \endcode
       *
       * to the current series in the current file.
       *
       * @param plane the plane index within the series.
       * @param buf the source pixel buffer.
       * @throws FormatException if any of the parameters are invalid.
       */
      virtual
      void
      saveBytes(dimension_size_type plane,
                VariantPixelBuffer& buf) = 0;

      /**
       * Save an image plane.
       *
       * Write an image plane from a VariantPixelBuffer
       * of size
       *
       * \code{.cpp}
       * getSizeX * getSizeY * bytesPerPixel * getRGBChannelCount()
       * \endcode
       *
       * to the current series in the current file.
       *
       * @param plane the plane index within the series.
       * @param buf the source pixel buffer.
       * @param x the @c X coordinate of the upper-left corner of the sub-image.
       * @param y the @c Y coordinate of the upper-left corner of the sub-image.
       * @param w the width of the sub-image.
       * @param h the height of the sub-image.
       * @throws FormatException if any of the parameters are invalid.
       */
      virtual
      void
      saveBytes(dimension_size_type plane,
                VariantPixelBuffer& buf,
                dimension_size_type x,
                dimension_size_type y,
                dimension_size_type w,
                dimension_size_type h) = 0;

      /**
       * Set the active series.
       *
       * @param series the series to activate.
       *
       * @todo Remove use of stateful API which requires use of
       * series switching in const methods.
       */
      virtual
      void
      setSeries(dimension_size_type series) const = 0;

      /**
       * Get the active series.
       *
       * @returns the active series.
       */
      virtual
      dimension_size_type
      getSeries() const = 0;

      /**
       * Set the active plane.
       *
       * @param plane the plane to activate.
       *
       * @todo Remove use of stateful API which requires use of
       * plane switching in const methods.
       */
      virtual void
      setPlane(dimension_size_type plane) const = 0;

      /**
       * Get the active plane.
       *
       * @returns the active plane.
       */
      virtual dimension_size_type
      getPlane() const = 0;

      /**
       * Get whether or not the writer can save multiple images in a
       * single file.
       *
       * @returns @c true if the writer supports multiple images, @c
       * false otherwise.
       */
      virtual
      bool
      canDoStacks() const = 0;

      /**
       * Set the default metadata store for this writer.
       *
       * @param retrieve a metadata retrieve implementation.
       */
      virtual
      void
      setMetadataRetrieve(std::shared_ptr<::ome::xml::meta::MetadataRetrieve>& retrieve) = 0;

      /**
       * Get the current metadata store for this writer.
       *
       * @returns the metadata store, which will never be @c null.
       */
      virtual
      const std::shared_ptr<::ome::xml::meta::MetadataRetrieve>&
      getMetadataRetrieve() const = 0;

      /**
       * Get the current metadata store for this writer.
       *
       * @returns the metadata store, which will never be @c null.
       */
      virtual
      std::shared_ptr<::ome::xml::meta::MetadataRetrieve>&
      getMetadataRetrieve() = 0;

      /**
       * Set the frame rate to use when writing.
       *
       * @param rate the frame rate (number of frames per second).
       */
      virtual
      void
      setFramesPerSecond(frame_rate_type rate) = 0;

      /**
       * Get the frame rate to use when writing.
       *
       * @returns the frame rate (number of frames per second).
       */
      virtual
      frame_rate_type
      getFramesPerSecond() const = 0;

      /**
       * Get supported pixel types.
       *
       * @returns the supported pixel types.
       */
      virtual
      const std::set<ome::xml::model::enums::PixelType>
      getPixelTypes() const = 0;

      /**
       * Get supported pixel types for the given codec.
       *
       * @param codec the codec to check.
       * @returns the supported pixel types.
       */
      virtual
      const std::set<ome::xml::model::enums::PixelType>
      getPixelTypes(const std::string& codec) const = 0;

      /**
       * Check if the pixel type is supported.
       *
       * @param type the pixel type to check.
       * @returns @c true if supported, @c false otherwise.
       */
      virtual
      bool
      isSupportedType(ome::xml::model::enums::PixelType type) const = 0;

      /**
       * Check if the pixel type is supported by the given codex.
       *
       * @param type the pixel type to check.
       * @param codec the codec to check.
       * @returns @c true if supported, @c false otherwise.
       */
      virtual
      bool
      isSupportedType(ome::xml::model::enums::PixelType type,
		      const std::string&                codec) const = 0;

      /**
       * Get supported compression types.
       *
       * @returns the supported compression types.
       */
      virtual
      const std::set<std::string>&
      getCompressionTypes() const  = 0;

      /**
       * Get supported compression types for a given pixel type.
       *
       * @param type the pixel type to use.
       * @returns the supported compression types.
       */
      virtual
      const std::set<std::string>&
      getCompressionTypes(ome::xml::model::enums::PixelType type) const  = 0;

      /**
       * Set the compression type to use when writing.
       *
       * @param compression the compression type.
       */
      virtual
      void
      setCompression(const std::string& compression) = 0;

      /**
       * Get the compression type to use when writing.
       *
       * @returns the compression type.
       */
      virtual
      const boost::optional<std::string>&
      getCompression() const = 0;

      /**
       * Set subchannel interleaving.
       *
       * @param interleaved @c true to enable interleaving (chunky) or
       * @c false to disable interleaving (planar).
       */
      virtual
      void
      setInterleaved(bool interleaved) = 0;

      /**
       * Set subchannel interleaving.
       *
       * @returns the current interleaving setting; @c false if unset.
       */
      virtual
      const boost::optional<bool>&
      getInterleaved() const = 0;

      /**
       * Switch the output file for the current dataset.
       *
       * @param id the new file name.
       */
      virtual
      void
      changeOutputFile(const boost::filesystem::path& id) = 0;

      /**
       * Write planes sequentially.
       *
       * Set if planes will be written sequentially.  If planes are
       * written sequentially and this flag is set, then performance
       * will be slightly improved.
       *
       * @param sequential @c true if sequential, @c false if not.
       */
      virtual
      void
      setWriteSequentially(bool sequential = true) = 0;

      /**
       * Check if planes are written sequentially.
       *
       * @returns @c true if sequential, @c false if not.
       */
      virtual
      bool
      getWriteSequentially() const = 0;

      /**
       * Set the requested tile width.
       *
       * The requested tile width may not be supported by the
       * underlying file format.  If the width requested is
       * unsupported, the writer may set the nearest supported size,
       * or the full image width or greater if no smaller tile sizes
       * are supported.  Call getTileSizeX() to get the effective size
       * in use by the writer, or use the return value.
       *
       * @param size the requested tile width.
       * @returns the effective tile width.
       **/
      virtual
      dimension_size_type
      setTileSizeX(boost::optional<dimension_size_type> size) = 0;

      /**
       * Get the effective tile width.
       *
       * This is intended for use with saveBytes().
       *
       * @returns the effective tile width.
       **/
      virtual
      dimension_size_type
      getTileSizeX() const = 0;

      /**
       * Set the requested tile height.
       *
       * The requested tile height may not be supported by the
       * underlying file format.  If the height requested is
       * unsupported, the writer may set the nearest supported size,
       * or the full image height or greater if no smaller tile sizes
       * are supported.  Call getTileSizeY() to get the effective size
       * in use by the writer, or use the return value.
       *
       * @param size the requested tile height.
       * @returns the effective tile height.
       **/
      virtual
      dimension_size_type
      setTileSizeY(boost::optional<dimension_size_type> size) = 0;

      /**
       * Get the effective tile height.
       *
       * This is intended for use with saveBytes().
       *
       * @returns the effective tile height.
       **/
      virtual
      dimension_size_type
      getTileSizeY() const = 0;
    };

  }
}

#endif // OME_FILES_FORMATWRITER_H

/*
 * Local Variables:
 * mode:C++
 * End:
 */
