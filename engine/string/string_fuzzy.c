// ## Fuzzy string matching
// Ideas from https://blog.forrestthewoods.com/reverse-engineering-sublime-text-s-fuzzy-match-4cffeed33fdb
// - rlyeh, public domain.

#ifndef FUZZY_H
#define FUZZY_H

API int string_fuzzy_score( const char *str1, const char *str2 );
API const char *string_fuzzy_find( const char *str, int num, const char *words[] );
API int string_fuzzy_find_index( const char *str, int num, const char *words[] );

#endif

#ifdef FUZZY_C
#pragma once

int string_fuzzy_score( const char *str1, const char *str2 ) {
    int score = 0, consecutive = 0, maxerrors = 0;
    while( *str1 && *str2 ) {
        int is_leading = (*str1 & 64) && !(str1[1] & 64);
        if( (*str1 & ~32) == (*str2 & ~32) ) {
            int had_separator = (str1[-1] <= 32);
            int x = had_separator || is_leading ? 10 : consecutive * 5;
            consecutive = 1;
            score += x;
            ++str2;
        } else {
            int x = -1, y = is_leading * -3;
            consecutive = 0;
            score += x;
            maxerrors += y;
        }
        ++str1;
    }
    return score + (maxerrors < -9 ? -9 : maxerrors);
}

const char *string_fuzzy_find( const char *str, int num, const char *words[] ) {
    int scoremax = 0;
    const char *best = 0;
    for( int i = 0; i < num; ++i ) {
        int score = string_fuzzy_score( words[i], str );
        int record = ( score >= scoremax );
        int draw = ( score == scoremax );
        if( record ) {
            scoremax = score;
            if( !draw ) best = words[i];
            else best = best && strlen(best) < strlen(words[i]) ? best : words[i];
        }
    }
    return best ? best : "";
}

int string_fuzzy_find_index( const char *str, int num, const char *words[] ) {
    int scoremax = 0, bestidx = 0;
    const char *best = 0;
    for( int i = 0; i < num; ++i ) {
        int score = string_fuzzy_score( words[i], str );
        int record = ( score >= scoremax );
        int draw = ( score == scoremax );
        if( record ) {
            scoremax = score;
            if( !draw ) {
                best = words[bestidx = i];
            } else {
                best = words[bestidx = best && strlen(best) < strlen(words[i]) ? bestidx : i];
            }
        }
    }
    return best ? bestidx : -1;
}

#endif

#ifdef FUZZY_DEMO
#include <stdio.h>
int main( int argc, char **argv ) {
    if( argc <= 1 ) printf("%s command\n", argv[0]), exit(0);
    const char *texts[] = {
        "The Black Knight",
        "SVisualLoggerLogsList.h",
        "AnimGraphNode_CopyBone",
        "client_unit.cpp",
        "Miskatonic university",
        "MockAI.h",
        "MacroCallback.cpp",
        "MockGameplayTasks.h",
        "MovieSceneColorTrack.cpp",
        "ce skipsplash",
        "ce skipsplashscreen",
        "ce skipscreen",
        "ce skipaa",
        "r.maxfps",
        "r.maxsteadyfps",
        "slomo 10",
        "retrograd",
        "retrocomputer",
        "return",
        "level",
        "leveler",
        "letrograd",
        "restart",
        "resource",
        "reset",
        "reboot",
        "rescale",
    };
    const char *partial_word = argv[1]; // try 'tk', 'reset', 'rest', etc...
    printf("Scoring '%s'\n", partial_word);
    for( int i = 0; i < sizeof(texts) / sizeof(texts[0]); ++i ) {
        printf("Score %d: (%s vs %s\n", string_fuzzy_score(texts[i], partial_word), texts[i], partial_word );
    }
    const char *fuzzy_search = string_fuzzy_find( partial_word, sizeof(texts)/sizeof(texts[0]), texts );
    printf(">> Did you mean '%s' instead of '%s'?\n", fuzzy_search, partial_word );
}
#endif
