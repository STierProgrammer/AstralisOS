#pragma once

#define PSF1_FONT_MAGIC 0x0436

// for PSF1 glyph width is always 8 bits
// and glyph height is charSize
typedef struct {
    uint16_t magic;
    uint8_t  fontMode;
    uint8_t  charSize;
} psf1_header_t;

#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;
    uint32_t version; // zero
    uint32_t headerSize; // offset of bitmaps in the file
    uint32_t flags; // if there's no unicode table it's 0, if there is then it's 1
    uint32_t numGlyph;
    uint32_t bytesPerGlyph;
    uint32_t height;
    uint32_t width;
} psf_font_t;


