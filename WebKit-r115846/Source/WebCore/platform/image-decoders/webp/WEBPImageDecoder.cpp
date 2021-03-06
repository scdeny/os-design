/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WEBPImageDecoder.h"

#if USE(WEBP)

#include "webp/decode.h"

#if PLATFORM(CHROMIUM)
#include "TraceEvent.h"
#endif

#if CPU(BIG_ENDIAN) || CPU(MIDDLE_ENDIAN)
inline WEBP_CSP_MODE outputMode() { return MODE_RGBA; }
#else // LITTLE_ENDIAN, output BGRA pixels.
inline WEBP_CSP_MODE outputMode() { return MODE_BGRA; }
#endif

namespace WebCore {

WEBPImageDecoder::WEBPImageDecoder(ImageSource::AlphaOption alphaOption,
                                   ImageSource::GammaAndColorProfileOption gammaAndColorProfileOption)
    : ImageDecoder(alphaOption, gammaAndColorProfileOption)
    , m_decoder(0)
{
}

WEBPImageDecoder::~WEBPImageDecoder()
{
    if (m_decoder)
        WebPIDelete(m_decoder);
    m_decoder = 0;
}

bool WEBPImageDecoder::isSizeAvailable()
{
    if (!ImageDecoder::isSizeAvailable())
         decode(true);

    return ImageDecoder::isSizeAvailable();
}

ImageFrame* WEBPImageDecoder::frameBufferAtIndex(size_t index)
{
    if (index)
        return 0;

    if (m_frameBufferCache.isEmpty()) {
        m_frameBufferCache.resize(1);
        m_frameBufferCache[0].setPremultiplyAlpha(m_premultiplyAlpha);
    }

    ImageFrame& frame = m_frameBufferCache[0];
    if (frame.status() != ImageFrame::FrameComplete)
        decode(false);
    return &frame;
}

bool WEBPImageDecoder::decode(bool onlySize)
{
#if PLATFORM(CHROMIUM)
    TRACE_EVENT("WEBPImageDecoder::decode", this, 0);
#endif
    if (failed())
        return false;

    const uint8_t* dataBytes = reinterpret_cast<const uint8_t*>(m_data->data());
    const size_t dataSize = m_data->size();

    if (!ImageDecoder::isSizeAvailable()) {
        static const size_t imageHeaderSize = 30;
        if (dataSize < imageHeaderSize)
            return false;
        int width, height;
        if (!WebPGetInfo(dataBytes, dataSize, &width, &height))
            return setFailed();
        if (!setSize(width, height))
            return setFailed();
    }

    ASSERT(ImageDecoder::isSizeAvailable());
    if (onlySize)
        return true;

    ASSERT(!m_frameBufferCache.isEmpty());
    ImageFrame& buffer = m_frameBufferCache[0];
    ASSERT(buffer.status() != ImageFrame::FrameComplete);

    if (buffer.status() == ImageFrame::FrameEmpty) {
        if (!buffer.setSize(size().width(), size().height()))
            return setFailed();
        buffer.setStatus(ImageFrame::FramePartial);
        buffer.setHasAlpha(false); // FIXME: webp does not support alpha yet.
        buffer.setOriginalFrameRect(IntRect(IntPoint(), size()));
    }

    if (!m_decoder) {
        int rowStride = size().width() * sizeof(ImageFrame::PixelData);
        uint8_t* output = reinterpret_cast<uint8_t*>(buffer.getAddr(0, 0));
        int outputSize = size().height() * rowStride;
        m_decoder = WebPINewRGB(outputMode(), output, outputSize, rowStride);
        if (!m_decoder)
            return setFailed();
    }

    switch (WebPIUpdate(m_decoder, dataBytes, dataSize)) {
    case VP8_STATUS_OK:
        buffer.setStatus(ImageFrame::FrameComplete);
        WebPIDelete(m_decoder);
        m_decoder = 0;
        return true;
    case VP8_STATUS_SUSPENDED:
        return false;
    default:
        WebPIDelete(m_decoder);
        m_decoder = 0;
        return setFailed();
    }
}

} // namespace WebCore

#endif
