#ifndef CODEC_H
#define CODEC_H

void base64_encode(const uint8_t* data, size_t len, String& output);
void base64_decode(const String& input, uint8_t* output, size_t& outputLen);

#endif // CODEC_H