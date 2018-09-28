// # restrict

#pragma once

#if defined __GNUC__
#define __restrict restrict
#else
#define __restrict
#endif
