// game file paths packed in 56 chars.
// - rlyeh, public domain.
//
// goal:
// - limit game paths to 56 chars long (pak-style), but still allow for fast lookups + preserve path hierarchy somehow.
//
// idea:
// - format: [np:8][hash1:32][hash2:32][...][filename\0:utf8]
// - np: number of path stems (*). outer (most left) stems are truncated until path fits buffer capacity.
// - h1..hN: (normalized) path stems, stored as a chain of 32-bit hashes.
// - filename: (normalized) filename, stored as null-terminated utf8 strings.
//
// pros:
// - compact.
// - fixed-size entries.
// - relatively quick file lookups.
//
// cons:
// - long path files truncate outer path stems.
//
// stats:
// - 0.. 2 chars long filenames -> (54- 2)=52/4=13 path stems max)
// - 3.. 6 chars long filenames -> (54- 6)=48/4=12 path stems max)
// - 7..10 chars long filenames -> (54-10)=44/4=11 path stems max) \
// -11..14 chars long filenames -> (54-14)=40/4=10 path stems max) |
// -15..18 chars long filenames -> (54-18)=36/4= 9 path stems max) |
// -19..22 chars long filenames -> (54-22)=32/4= 8 path stems max) | common
// -23..26 chars long filenames -> (54-26)=28/4= 7 path stems max) | path
// -27..30 chars long filenames -> (54-30)=24/4= 6 path stems max) | lengths
// -31..34 chars long filenames -> (54-34)=20/4= 5 path stems max) |
// -35..38 chars long filenames -> (54-38)=16/4= 4 path stems max) |
// -39..42 chars long filenames -> (54-42)=12/4= 3 path stems max) /
// -43..46 chars long filenames -> (54-46)= 8/4= 2 path stems max)
// -47..50 chars long filenames -> (54-50)= 4/4= 1 path stems max)
// -51..54 chars long filenames -> (54-54)= 0/4= 0 path stems max)

#ifndef FILE56_H
#define FILE56_H

typedef struct file56 {
    unsigned char stems;
    char name[55];
} file56;

API file56  file56_create( const char *fname );
API char*   file56_name( file56 f );
API void    file56_print( file56 f );

API file56* file56_find( array(file56) files, const char *fname, unsigned num_stems );

API int     file56_compare_partial( file56 a, file56 b, unsigned num_stems );
API int     file56_compare_exact( file56 a, file56 b );
API int     file56_compare_fuzzy( file56 a, file56 b );

#endif

// ----------------------------------------------------------------------------

#ifdef FILE56_C
#pragma once

typedef int static_assert_sizeof_file56[ sizeof(file56) == 56 ];

int file56_get_maxstems( unsigned fname_len ) {
    // determine maximum path stems for given len (2=stems_byte+terminator; 4=sizeof hash32)
    return (56 - 2 - fname_len) / 4;
}

file56 file56_create( const char *fname ) {
    file56 f = {0};

    if( fname ) {
        char *fnorm = file_norm(fname)+1;
        array(char*) list = string_split(fnorm, "/");
        int stems = array_count(list) - 1;

        int flen = strlen(*array_back(list));
        int maxstems = file56_get_maxstems(flen);
        int count = stems > maxstems ? maxstems : stems;
        int index = stems - count;

        f.stems = count;
        char *buf = f.name;
        for( int i = 0; i < count; ++i ) {
            int len = strlen(list[index+i]);
            if( len <= 4 ) {
                char pad[5] = "     ";
                memcpy(buf, list[index+i], len);
                memcpy(buf+len, pad, 4-len);
            } else {
                int32_t hash = str64(list[index+i]);
                memcpy(buf, &hash, 4);
            }
            buf += 4;
        }
        sprintf(buf, "%s", *array_back(list));

        array_free(list);
    }

    return f;
}

char *file56_name( file56 f ) {
    return f.name + 4 * f.stems;
}

void file56_print( file56 f ) {
    printf("file56: %s\t", file56_name(f));
    for( int i = 0; i < f.stems; ++i ) {
        unsigned char a = f.name[i*4+0], b = f.name[i*4+1];
        unsigned char c = f.name[i*4+2], d = f.name[i*4+3];
        if( isprint(a) ) printf(".%c", a); else printf("%02X", a);
        if( isprint(b) ) printf(".%c", b); else printf("%02X", b);
        if( isprint(c) ) printf(".%c", c); else printf("%02X", c);
        if( isprint(d) ) printf(".%c/",d); else printf("%02X/",d);
    }
    puts("");
}

int file56_shared_stems(file56 a, file56 b) {
    return a.stems < b.stems ? a.stems : b.stems;
}

int file56_compare_partial( file56 a, file56 b, unsigned num_stems ) {
    if( num_stems > a.stems ) return -1;
    if( num_stems > b.stems ) return +1;
    return strcmp( file56_name(a)-4*num_stems, file56_name(b)-4*num_stems);
}

int file56_compare_exact( file56 a, file56 b ) {
    return memcmp( &a, &b, 56 );
}

int file56_compare_fuzzy( file56 a, file56 b ) {
    return file56_compare_partial(a,b, file56_shared_stems(a,b));
}

file56* file56_find(array(file56) files, const char *fname, unsigned num_stems) {
    // @todo: add faster binary search if files are already sorted
    file56 target = file56_create(fname);
    if( num_stems != ~0u) for (int i = 0; i < array_count(files); ++i) {
        if (!file56_compare_partial(target, files[i], num_stems)) {
            return &files[i];
        }
    }
    if( num_stems == ~0u) for (int i = 0; i < array_count(files); ++i) {
        if (!file56_compare_fuzzy(target, files[i])) {
            return &files[i];
        }
    }
    return 0;
}

#ifdef FILE56_DEMO
#pragma once

int main() {
    file56 a = file56_create("hello");
    file56 b = file56_create( "/123/hello" );
    file56 c = file56_create( "/abc/123/hello" );

    file56_print(a);
    file56_print(b);
    file56_print(c);

    assert( 0 != file56_compare_exact(a,b) );
    assert( 0 != file56_compare_exact(b,c) );

    assert( 0 == file56_compare_fuzzy(a,b) );
    assert( 0 == file56_compare_fuzzy(b,c) );

    assert( 0 == file56_compare_partial(b,c,0) );
    assert( 0 == file56_compare_partial(b,c,1) );
    assert( 0 != file56_compare_partial(b,c,2) );
    assert( 0 != file56_compare_partial(b,c,3) );

    // ---
    array(file56) list = 0;
    array_push(list, file56_create("assets/data/characters/boy/diffuse.png"));
    array_push(list, file56_create("assets/data/characters/dog/diffuse.png"));
    array_push(list, file56_create("assets/data/characters/lamp/diffuse.png"));

    file56_print(list[0]);
    file56_print(list[1]);
    file56_print(list[2]);

    file56 *found0 = file56_find(list, "lamp/diffuse.png", 0); // returns first file match
    file56 *found1 = file56_find(list, "lamp/diffuse.png", 1); // returns first dir+file match
    file56 *found2 = file56_find(list, "lamp/diffuse.png", 2); // returns null (no 2dirs source to compare with)
    file56 *found4 = file56_find(list, "game/assets/data/characters/lamp/diffuse.png",  4); // returns first 4dirs+file match
    file56 *found9 = file56_find(list, "game/assets/data/characters/lamp/diffuse.png",  9); // returns null (no 9dirs matches)
    file56 *foundN = file56_find(list, "game/assets/data/characters/lamp/diffuse.png", -1); // returns best match

    assert(found0 == &list[0]);
    assert(found1 == &list[2]);
    assert(found2 == NULL    );
    assert(found4 == &list[2]);
    assert(found9 == NULL    );
    assert(foundN == &list[2]);

    // ---
    assert(~puts("ok"));
}

#endif
#endif
