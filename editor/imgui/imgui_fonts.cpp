#pragma once

/*
#include "3rd/fonts/fs3.c"  // monospaced
#include "3rd/fonts/djv.c"  //               , medium  (18.f)
#include "3rd/fonts/mp1r.c" //   formal style, regular (21.f)
#include "3rd/fonts/mp1m.c" //   formal style, medium
#include "3rd/fonts/mp1b.c" //   formal style, bold
#include "3rd/fonts/mp1h.c" //   formal style, heavy
#include "3rd/fonts/mp1a.c" //   formal style, black
#include "3rd/fonts/mp2r.c" // informal style, regular (21.f)
#include "3rd/fonts/mp2m.c" // informal style, medium
#include "3rd/fonts/mp2b.c" // informal style, bold
#include "3rd/fonts/mp2h.c" // informal style, heavy
#include "3rd/fonts/mp2a.c" // informal style, black
*/

#include "3rd/@juliettef/IconsMaterialDesign_c.h"
#include "3rd/@juliettef/IconsMaterialDesignRanges.h"

#include "3rd/fonts/mp1m.c" //   formal style, medium
#define font_buf  mp1m_compressed_data
#define font_len  mp1m_compressed_size
#define font_size 18.f // 21.f
#define font_chn  1
#define font_jap  1
#define font_kor  0
#define font_cyr  1
#define font_tha  0

#include "3rd/fonts/ntar.c"   // noto sans cjk (arabic), regular
#define extra_font_ar_buf     ntar_compressed_data
#define extra_font_ar_len     ntar_compressed_size
#define extra_font_ar_size    30.f // 21.f

#include "3rd/fonts/ntkr.c"   // noto sans cjk (kr only), regular
#define extra_font_kr_buf     ntkr_compressed_data
#define extra_font_kr_len     ntkr_compressed_size
#define extra_font_kr_size    15.f // 21.f

#include "3rd/fonts/mtir.c" // material icons, regular
#define extra_font_icon_buf   mtir_compressed_data
#define extra_font_icon_len   mtir_compressed_size
#define extra_font_icon_size  19.f

void imgui_fonts() {
    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig *cfg = &ImFontConfig();

    cfg->FontDataOwnedByAtlas = false;             // avoid crashes when deinitializing
    //cfg->OversampleH = 2; // 3// Rasterize at higher quality for sub-pixel positioning. We don't use sub-pixel positions on the Y axis.
    //cfg->OversampleV = 1; // 1

    // int texmax;
    // glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texmax);
//    io.Fonts->TexDesiredWidth = texmax; // 4096 .. 16384
//    printf("%d\n", texmax);
    // float limit_atlas_dim = texmax <= 4096 ? 0.9f : 1.f;

    static const ImWchar default_ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0080, 0x00FF, // Latin_Supplement
        0x0100, 0x017F, // Latin_Extended-A
        0x0180, 0x024F, // Latin_Extended-B
//      0x2C60, 0x2C7F, // Latin Extended-C (Uighur New Script, the Uralic Phonetic Alphabet, Shona, and Claudian Latin)
        0x1E00, 0x1EFF, // Latin_Extended_Additional
        0x0250, 0x02AF, // International Phonetic Alphabet (IPA)
        0x02B0, 0x02FF, // Spacing modifier letters (and quotes)
        0xFB00, 0xFB4F, // Ligatures for the Latin, Armenian, and Hebrew scripts
        0x2000, 0x206F, // General punctuaction

        0x16A0, 0x16F0, // Old Norse / Runic

        0x01FF, 0x03FF, // fix IPA missing glyphs (?)
        0x1F00, 0x1FFF, // fix ancient greek glyphs (?)

        0,
    };

cfg->RasterizerMultiply = 1.1f; // increase brightness

    io.Fonts->AddFontFromMemoryCompressedTTF( font_buf, font_len, font_size, cfg, default_ranges ); // io.Fonts->GetGlyphRangesDefault() );
    cfg->MergeMode = true;

    if( 1 ) {
        static const ImWchar ranges[] = {  
        0x0370, 0x03FF, // Greek and Coptic
        0x10A0, 0x10FF, // Modern Georgian, Svan, and Mingrelian
        0 };
        io.Fonts->AddFontFromMemoryCompressedTTF( font_buf, font_len, font_size + 1, cfg, ranges ); // io.Fonts->GetGlyphRangesJapanese() );
    }

    if( 1 ) {
        static const ImWchar ranges[] = {  
        0x0590, 0x05FF, // Hebrew, Yiddish, Ladino, and other Jewish diaspora languages.
        0 };
        auto *f = io.Fonts->AddFontFromMemoryCompressedTTF( font_buf, font_len, font_size + 2, cfg, ranges ); // io.Fonts->GetGlyphRangesJapanese() );
        // f->Scale = 100.f;
    }

    if( font_cyr ) {
        static const ImWchar ranges[] = {  
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
            0 };
        io.Fonts->AddFontFromMemoryCompressedTTF( font_buf, font_len, font_size, cfg, ranges ); //io.Fonts->GetGlyphRangesCyrillic() );
    }

    cfg->OversampleH = 2;
    cfg->OversampleV = 2; // 1

    if( 1 ) {
        static const ImWchar ranges[] = {  
        0x0600, 0x06FF, // Arabic script, and the Arabic-Indic digits
        0 };
        auto *f = io.Fonts->AddFontFromMemoryCompressedTTF( extra_font_ar_buf, extra_font_ar_len, extra_font_ar_size, cfg, ranges ); // io.Fonts->GetGlyphRangesJapanese() );
        // f->Scale = 100.f;
    }

    if( font_jap ) {
#if 1
        static const ImWchar ranges[] = {  
        0x3000, 0x30FF, // Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0 };
#endif
        io.Fonts->AddFontFromMemoryCompressedTTF( font_buf, font_len, font_size + 1.5, cfg, ranges ); // io.Fonts->GetGlyphRangesJapanese() );
    }

    if( font_chn ) {
#if 0
        io.Fonts->AddFontFromMemoryCompressedTTF( font_buf, font_len, font_size, cfg, io.Fonts->GetGlyphRangesChinese() );
#else
        static const ImWchar ranges[] = {  
            //0x3000, 0x303F, // CJK punctuation
            0x4e00, 0x9FAF, // CJK Ideograms
            0 };
        /*
        CJK Unified Ideographs                  4E00-9FFF   Common
        CJK Unified Ideographs Extension A      3400-4DBF   Rare
        CJK Unified Ideographs Extension B      20000-2A6DF Rare, historic
        CJK Unified Ideographs Extension C      2A700–2B73F Rare, historic
        CJK Unified Ideographs Extension D      2B740–2B81F Uncommon, some in current use
        CJK Unified Ideographs Extension E      2B820–2CEAF Rare, historic
        CJK Compatibility Ideographs            F900-FAFF   Duplicates, unifiable variants, corporate characters
        CJK Compatibility Ideographs Supplement 2F800-2FA1F Unifiable variants
        */
        io.Fonts->AddFontFromMemoryCompressedTTF( font_buf, font_len, font_size + 2, cfg, ranges );
#endif
    }
    if( 1 ) { // font_kor ) {
#if 0
        io.Fonts->AddFontFromMemoryCompressedTTF( font_buf, font_len, font_size, cfg, io.Fonts->GetGlyphRangesKorean() );
#else
        static const ImWchar ranges[] = {
            //0x3000, 0x303F, // CJK punctuation
            0x3131, 0x3163, // Korean alphabets,
            0xAC00, 0xD7A4, // Hangul syllabes (0xD7AF?)
              // 0x1100, 0x11FF,
            0 };
        io.Fonts->AddFontFromMemoryCompressedTTF( extra_font_kr_buf, extra_font_kr_len, extra_font_kr_size, cfg, ranges );
#endif
    }
    if( font_tha ) {
        static const ImWchar ranges[] = {  
        0x2010, 0x205E, // Punctuations
        0x0E00, 0x0E7F, // Thai
            0 };
        io.Fonts->AddFontFromMemoryCompressedTTF( font_buf, font_len, font_size, cfg, ranges ); //io.Fonts->GetGlyphRangesThai() );
    }

    cfg->OversampleH = 1;
    cfg->OversampleV = 1; // 1

    if( 1 ) {
        /*
        static const ImWchar ranges[] = { 
            ICON_MD_MIN, ICON_MD_MAX
            0
        }; */
        cfg->MergeMode = true; cfg->PixelSnapH = true; cfg->GlyphOffset.y += 4;
        auto *icon = io.Fonts->AddFontFromMemoryCompressedTTF( extra_font_icon_buf, extra_font_icon_len, extra_font_icon_size - 3, cfg, IconsMaterialDesignRanges );
    }


    io.Fonts->AddFontDefault(); // just in case none of the above is available at all...
    io.Fonts->Build();

    io.Fonts->Fonts[0]->Scale = 1.0f; // 0.9f;

    //applyFrameworkStyle();
    //applyCinderStyle();
    //applyA5Style();

    //applyFrameworkTheme();  // blue-ish
    //applyCinderTheme(); // red
    //applyBlackTheme(); // black-ish
    //applyWhiteTheme(); // white
    //applyA5Theme();
}



