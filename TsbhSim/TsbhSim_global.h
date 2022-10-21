#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(TsbhSim_LIB)
#  define RTSSCORE_EXPORT Q_DECL_EXPORT
# else
#  define RTSSCORE_EXPORT Q_DECL_IMPORT
# endif
#else
# define RTSSCORE_EXPORT
#endif
