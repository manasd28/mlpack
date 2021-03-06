/**
 * @file core/util/arma_config_check.hpp
 * @author Ryan Curtin
 *
 * Using the contents of arma_config.hpp, try to catch the condition where the
 * user has included mlpack with ARMA_64BIT_WORD enabled but mlpack was compiled
 * without ARMA_64BIT_WORD enabled.  This should help prevent a long, drawn-out
 * debugging process where nobody can figure out why the stack is getting
 * mangled.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_UTIL_ARMA_CONFIG_CHECK_HPP
#define MLPACK_CORE_UTIL_ARMA_CONFIG_CHECK_HPP

#include "arma_config.hpp"

#ifdef ARMA_64BIT_WORD
  #ifdef MLPACK_ARMA_NO_64BIT_WORD
    #pragma message "mlpack was compiled without ARMA_64BIT_WORD, but you are \
compiling with ARMA_64BIT_WORD.  This will almost certainly cause irreparable \
disaster.  Either disable ARMA_64BIT_WORD in your application which is using \
mlpack, or, recompile mlpack against a version of Armadillo which has \
ARMA_64BIT_WORD enabled."
  #endif
#else
  #ifdef MLPACK_ARMA_64BIT_WORD
    #pragma message "mlpack was compiled with ARMA_64BIT_WORD, but you are \
compiling without ARMA_64BIT_WORD.  This will almost certainly cause \
irreparable disaster.  Either enable ARMA_64BIT_WORD in your application which \
is using mlpack, or, recompile mlpack against a version of Armadillo which has \
ARMA_64BIT_WORD disabled."
  #endif
#endif

// Check if OpenMP was enabled when mlpack was built.
#ifdef ARMA_USE_OPENMP
  #ifdef MLPACK_ARMA_DONT_USE_OPENMP
    #pragma message "mlpack was compiled without OpenMP support, but you are \
compiling with OpenMP support (either -fopenmp or another option).  This will \
almost certainly cause irreparable disaster.  Either compile your application \
*without* OpenMP support (i.e. remove -fopenmp or another flag), or, recompile \
mlpack with OpenMP support."
  #endif
#else
  #ifdef MLPACK_ARMA_USE_OPENMP
    #pragma message "mlpack was compiled with OpenMP support, but you are \
compiling without OpenMP support.  This will almost certainly cause \
irreparable disaster.  Either enable OpenMP support in your application (e.g., \
add -fopenmp to your compiler command line), or, recompile mlpack *without* \
OpenMP support."
  #endif
#endif

#endif
