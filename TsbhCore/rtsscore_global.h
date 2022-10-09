#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(RTSSCORE_LIB)
#  define RTSSCORE_EXPORT Q_DECL_EXPORT
# else
#  define RTSSCORE_EXPORT Q_DECL_EXPORT
# endif
#else
# define RTSSCORE_EXPORT
#endif
