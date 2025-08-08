/*
 * compression.cpp
 * Copyright 2025, Brandon Barnes <winterknight@email.com>
 * Copyright 2008, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 *
 * This file comes from a part of libtiled.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <zlib.h>
#include <zstd.h>

#include "compression.h"

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)
#define LOGE(...) fprintf(stderr, "E/" QUOTEME(LOG_TAG) "(" ")" __VA_ARGS__ ); fprintf(stderr, "\n");


static void logZlibError(int error)
{
	switch (error)
	{
		case Z_MEM_ERROR:
			LOGE("Out of memory while (de)compressing data!");
			break;
		case Z_VERSION_ERROR:
			LOGE("Incompatible zlib version!");
			break;
		case Z_NEED_DICT:
		case Z_DATA_ERROR:
			LOGE("Incorrect zlib compressed data!");
			break;
		default:
			LOGE("Unknown error while (de)compressing data!");
	}
}

std::vector<char> decompress(const std::string &data,
                             int length,
                             CompressionMethod method)
{
	std::vector<char> out(length);
	std::vector<char> emptyVector;
	if (data.empty())
		return emptyVector;

	if (method == Zlib || method == Gzip) {
		z_stream strm;

		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.next_in = (Bytef *) data.data();
		strm.avail_in = data.length();
		strm.next_out = (Bytef *) out.data();
		strm.avail_out = out.size();

		int ret = inflateInit2(&strm, 15 + 32);

		if (ret != Z_OK) {
			logZlibError(ret);
			return emptyVector;
		}

		do {
			ret = inflate(&strm, Z_SYNC_FLUSH);
//			assert(ret != Z_STREAM_ERROR);

			switch (ret) {
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					inflateEnd(&strm);
					logZlibError(ret);
					return emptyVector;
			}

			if (ret != Z_STREAM_END) {
				int oldSize = out.size();
				out.resize(oldSize * 2);

				strm.next_out = (Bytef *)(out.data() + oldSize);
				strm.avail_out = oldSize;
			}
		}
		while (ret != Z_STREAM_END);

		if (strm.avail_in != 0) {
			logZlibError(Z_DATA_ERROR);
			return emptyVector;
		}

		const int outLength = out.size() - strm.avail_out;
		inflateEnd(&strm);

		out.resize(outLength);
		return out;
	} else if (method == Zstandard) {
		size_t const dSize = ZSTD_decompress(out.data(), out.size(), data.data(), data.size());
		if (ZSTD_isError(dSize)) {
			LOGE("error decoding: %s", ZSTD_getErrorName(dSize));
			return emptyVector;
		}
		out.resize(dSize);
		return out;
	} else {
		LOGE("compression method not supported: %d", method);
		return emptyVector;
	}
}
