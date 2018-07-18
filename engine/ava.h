//                     /\\ \// /\\
//                  small game engine
//
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.
//
// People who worked on this file: @r-lyeh,

// ----------------------------------------------------------------------------

#ifndef AVA_H
#define AVA_H "2018.3"

// api decl

#  if !defined(API) && defined(__cplusplus)
#   define API extern "C" LINKAGE
#elif !defined(API)
#   define API            LINKAGE
#endif

// linkage

#ifndef LINKAGE
#define LINKAGE IMPORT
#endif

#ifndef STATIC
#define STATIC
#endif
#ifndef EXPORT
#define EXPORT __declspec(dllexport)
#endif
#ifndef IMPORT
#define IMPORT __declspec(dllimport)
#endif

#endif // AVA_H

// ----------------------------------------------------------------------------
// compat

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// ----------------------------------------------------------------------------
// modules

#ifdef __cplusplus
extern "C" {
#endif

#include "core/system.c"

#include "async/async.c"
#include "debug/debug.c"
#include "ecs/ecs.c"
#include "serial/json5.c"
#include "framework/framework.c"

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------

