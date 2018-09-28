#ifndef BENCHMARK_H
#define BENCHMARK_H

#if _MSC_VER
#include <omp.h>
#define BENCHMARK(x)    for( x = -omp_get_wtime(); x < 0; x += omp_get_wtime() )
#else
#define BENCHMARK(x)    if( !(x *= 0 ) )
#endif

#endif
