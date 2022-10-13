#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(TsbhPlot_LIB)
#  define TsbhPlot_EXPORT Q_DECL_EXPORT
# else
#  define TsbhPlot_EXPORT Q_DECL_IMPORT
# endif
#else
# define TsbhPlot_EXPORT
#endif
