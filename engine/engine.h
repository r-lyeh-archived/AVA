//                     /\\ \// /\\
//                  small game engine
//
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.
//
// Dear contributor, check following directives before contributing:
//   - APIs follow an imperative, immediate mode design.
//   - Functions use the least possible arguments.
//   - Implementations use the simplest possible code.
//   - Style is readable and compact enough. Indent is 4-spaces.
//   - Your contributions are Zero-BSD, CC0 or Unlicensed.
//
// People who worked on this file: @r-lyeh,
//

#ifndef AVA_VERSION
#define AVA_VERSION "2018.3" // 'YOB-Marrow' release

#ifdef __cplusplus
extern "C" {
#endif

#include "engine_config.c"
#include "engine_core.c"
#include "engine_framework.c"
#include "engine_server.c"

API int init();
API int loop();
API int quit();

#ifdef __cplusplus
}
#endif

#endif /* AVA_VERSION */
