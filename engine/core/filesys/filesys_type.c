// # file types ###############################################################

API const char *file_type(const char *buf, int len);

#ifdef FTYPE_C
#pragma once
#include <string.h>

const char *file_type(const char *buf, int len) {
    struct type {
        const char *ext; int len; const char *buf; int off;
    } types[] = { //// Descending priority order
        "opus",     8, "OpusHead", 28, // before 'ogg'
        "mxf",     14, "\x06\x0E\x2B\x34\x02\x05\x01\x01\x0D\x01\x02\x01\x01\x02", 0,
        "hdr",     11, "#?RADIANCE\n", 0,
        "m4v",     11, "\x00\x00\x00\x1C\x66\x74\x79\x70\x4D\x34\x56", 0,
        "wmv",     10, "\x30\x26\xB2\x75\x8E\x66\xCF\x11\xA6\xD9", 0,
        "mov",      8, "\x00\x00\x00\x14\x66\x74\x79\x70", 0,
        "msi",      8, "\xD0\xCF\x11\xE0\xA1\xB1\x1A\xE1", 0,
        "ar",       7, "!<arch>", 0,
        "hdr",      7, "#?RGBE\n", 0,
        "m4a",      7, "ftypM4A", 4,
        "7z",       6, "\x37\x7A\xBC\xAF\x27\x1C", 0,
        "xz",       6, "\xFD\x37\x7A\x58\x5A\x00", 0,
        "amr",      5, "#!AMR", 0,
        "rtf",      5, "{\rtf", 0,
        "tar",      5, "ustar", 257,
        "ttf",      5, "\x00\x01\x00\x00\x00", 0,
        "cab",      4, "ISc(", 0,
        "cab",      4, "MSCF", 0,
        "crx",      4, "Cr24", 0,
        "exr",      4, "v/1\x1", 0,
        "flac",     4, "fLaC", 0,
        "flif",     4, "FLIF", 0,
        "flv",      4, "FLV\1", 0,
        "ico",      4, "\x00\x00\x01\x00", 0,
        "lz",       4, "LZIP", 0,
        "m4a",      4, "M4A ", 0,
        "mid",      4, "MThd", 0,
        "mkv",      4, "\x1A\x45\xDF\xA3", 0,
        "mp4",      4, "3gp5", 0,
        "mp4",      4, "ftyp", 4,
        "mpg",      4, "\x00\x00\x01\x62", 0,
        "ogg",      4, "OggS", 0,
        "otf",      4, "OTTO", 0,
        "pdf",      4, "%PDF", 0,
        "png",      4, "\x89PNG", 0,
        "psd",      4, "8BPS", 0,
        "rar",      4, "Rar!", 0,
        "rpm",      4, "\xED\xAB\xEE\xDB", 0,
        "sqlite",   4, "SQLi", 0,
        "svg",      4, "<svg", 0,
        "tiff",     4, "\x49\x49\x2A\x00", 0,
        "tiff",     4, "\x4D\x4D\x00\x2A", 0,
        "wav",      4, "WAVE", 8,
        "webp",     4, "WEBP", 8,
        "woff",     4, "wOFF", 0,
        "woff2",    4, "wOF2", 0,
        "avi",      3, "AVI", 8,
        "bz2",      3, "BZh", 0,
        "gif",      3, "GIF", 0,
        "gz",       3, "\x1F\x8B\x08", 0,
        "jpg",      3, "\xFF\xD8\xFF", 0,
        "jxr",      3, "\x49\x49\xBC", 0,
        "mp3",      3, "ID3", 0,
        "bmp",      2, "BM", 0,
        "dmg",      2, "x\1", 0,
        "exe",      2, "MZ", 0,
        "ps",       2, "%!", 0,
        "swf",      2, "WS", 1,
        "z",        2, "\x1F\x9D", 0,
        "z",        2, "\x1F\xA0", 0,
        "zip",      2, "PK", 0,
        "json",     1, "{", 0, // @todo: improve this weak detection someday.
        "json",     1, "[", 0, // @todo: improve this weak detection someday.
        "xml",      1, "<", 0, // @todo: improve this weak detection someday.
        0
    };
    for( int i = 0; types[i].ext; ++i ) {
        if( (types[i].off + types[i].len) < len ) {
            if( 0 == memcmp( buf + types[i].off, types[i].buf, types[i].len ) ) {
                return types[i].ext;
            }
        }
    }
    return "";
}

#endif

#ifdef FTYPE_DEMO
#include <stdio.h>
int main() {
    // samples taken from https://github.com/mathiasbynens/small
    const unsigned char unknown1[] = {
        0x3C,0x3F,0x78,0x6D,0x6C,0x20,0x76,0x65,0x72,0x73,0x69,0x6F,0x6E,0x3D,0x22,0x31,
        0x2E,0x31,0x22,0x3F,0x3E,0x3C,0x5F,0x2F,0x3E
    };
    const unsigned char unknown2[] = {
        0x47,0x49,0x46,0x38,0x39,0x61,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x3B
    };
    const unsigned char unknown3[] = {
        0x42,0x4D,0x1E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1A,0x00,0x00,0x00,0x0C,0x00,
        0x00,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x18,0x00,0x00,0x00,0xFF,0x00
    };
    const unsigned char unknown4[] = {
        0x50,0x4B,0x05,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00
    };
    const unsigned char unknown5[] = {
        0x7B,0x5C,0x72,0x74,0x66,0x31,0x7D
    };
    const unsigned char unknown6[] = {
        0x52,0x49,0x46,0x46,0x12,0x00,0x00,0x00,0x57,0x45,0x42,0x50,0x56,0x50,0x38,0x4C,
        0x06,0x00,0x00,0x00,0x2F,0x41,0x6C,0x6F,0x00,0x6B
    };
    puts( file_type( unknown1, sizeof(unknown1) ) );
    puts( file_type( unknown2, sizeof(unknown2) ) );
    puts( file_type( unknown3, sizeof(unknown3) ) );
    puts( file_type( unknown4, sizeof(unknown4) ) );
    puts( file_type( unknown5, sizeof(unknown5) ) );
    puts( file_type( unknown6, sizeof(unknown6) ) );
}
#endif
