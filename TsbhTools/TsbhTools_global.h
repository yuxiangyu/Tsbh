#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(TsbhTools_LIB)
#  define TsbhTools_EXPORT Q_DECL_EXPORT
# else
#  define TsbhTools_EXPORT Q_DECL_IMPORT
# endif
#else
# define TsbhTools_EXPORT
#endif
