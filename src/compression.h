/*
 * compression.h
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

#ifndef SRC_COMPRESSION_H_
#define SRC_COMPRESSION_H_

#include <string>
#include <vector>

enum CompressionMethod {
    Gzip,
    Zlib,
    Zstandard
};

/**
 * Decompresses compressed memory. Returns an empty string
 * if decompressing failed.
 *
 * @param data         the compressed data
 * @param length       the expected size of the uncompressed data in bytes
 * @param method       the compression method
 * @return the uncompressed data, or an empty string if decompressing failed
 */
std::vector<char> decompress(const std::string &data, int length, CompressionMethod method = Zlib);

#endif /* SRC_COMPRESSION_H_ */
