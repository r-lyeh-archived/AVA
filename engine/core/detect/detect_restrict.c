// # restrict

#pragma once

#ifdef __GNUC__
#define RESTRICT restrict
#else
#define RESTRICT
#endif
