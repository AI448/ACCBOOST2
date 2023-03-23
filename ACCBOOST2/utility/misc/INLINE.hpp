#ifndef ACCBOOST2_UTILITY_MISC_INLINE_HPP_
#define ACCBOOST2_UTILITY_MISC_INLINE_HPP_


#if defined(__GNUC__)

/// インライン展開しない．
#define ACCBOOST2_NOINLINE __attribute__((noinline))

/// 強制的にインライン展開する．
#define ACCBOOST2_INLINE __attribute__((always_inline))

#else

#define ACCBOOST2_NOINLINE

#define ACCBOOST2_INLINE

#endif


#endif
