// Original source at https://quakewiki.org/wiki/.pak (Public domain).
// - rlyeh, public domain.

// ## PAK
// - Header: 12 bytes
//   - "PACK"           4-byte
//   - directory offset int32_t
//   - directory size   int32_t (number of files by dividing this by 64, which is sizeof(pak_entry))
//
// - File Directory Entry (Num files * 64 bytes)
//   - Each Directory Entry: 64 bytes
//     - file name     56-byte null-terminated string. Includes path. Example: "maps/e1m1.bsp".
//     - file offset   int32_t from beginning of pak file.
//     - file size     int32_t

#ifndef PAK_H
#define PAK_H

char *pak_load(const char *pak_filename, const char *filename, int *out_filesize);

#endif

// ----------------------------------------------------------------------------

#ifdef PAK_C
#pragma once

typedef struct pak_header { // 4+4+4 == 12
    char id[4];
    int32_t offset;
    int32_t size;
} pak_header;

typedef struct pak_file { // 56+4+4 == 64
    char name[56];
    int32_t offset;
    int32_t size;
} pak_file;

typedef static_assert_pak_header[ sizeof(pak_header) == 12 ];
typedef static_assert_pak_file[ sizeof(pak_file) == 64 ];

pak_header pak_info(const char *pak_filename) {
    pak_header z = {0}, *t = (pak_header*)file_chunk(pak_filename, 0, sizeof(pak_header));
    if( t ) {
        z = *t;
        z.offset = swap32ifbe(z.offset);
        z.size = swap32ifbe(z.size) / sizeof(pak_file);
    }
    return z;
}

array(pak_file) pak_dir(const char *pak_filename) {
    array(pak_file) list = 0;

    pak_header info = pak_info(pak_filename);
    if( 0 == memcmp(info.id, "PACK", 4) )
    for (int i = 0; i < info.size; i++) {
        pak_file z, *p = (pak_file*)file_chunk(pak_filename, info.offset, sizeof(pak_file));
        info.offset += sizeof(pak_file);
        if( !p ) break;
        z = *p;
        z.offset = swap32ifbe(z.offset);
        z.size = swap32ifbe(z.size);
        array_push(list, z);
    }

    return list;
}

char *pak_load(const char *pak_filename, const char *filename, int *out_filesize) {
    char *chunk = 0;
    array(pak_file) p = pak_dir(pak_filename);

    for( int i = 0; i < array_count(p); ++i ) {
        if (!strcmp(p[i].name, filename)) {
            chunk = file_chunk(pak_filename, p[i].offset, p[i].size);
            if( out_filesize ) *out_filesize = (int)p[i].size;
            break;
        }
    }

    array_free(p);
    return chunk;
}

void pak_print(const char *pak_filename) {
    int total = 0;
    array(pak_file) pak = pak_dir(pak_filename);

    for(int i = 0; i < array_count(pak); i++) {
        printf("%12d %-56s\n", pak[i].size, pak[i].name);
        total += pak[i].size;
    }

    printf("%12d bytes (%d files)\n", total, array_count(pak));
}

#endif

// ----------------------------------------------------------------------------

#ifdef PAK_DEMO
#pragma once

int main(int argc, char **argv) {
    if( argc < 2 ) {
        printf("%s [file.pak] [file_to_extract]\n", argv[0]);
    }
    if( argc > 1 ) {
        pak_print(argv[1]);
    }
    if( argc > 2 ) {
        int sz; char *buf = pak_load(argv[1], argv[2], &sz);
        printf( "%.*s", sz, buf );
    }
}

#endif
