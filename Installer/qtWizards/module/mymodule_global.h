#ifndef %{JS: Cpp.headerGuard('%{GlbHdrFileName}')}
#define %{JS: Cpp.headerGuard('%{GlbHdrFileName}')}

#include <QtCore/qglobal.h>

#if defined(%{ProjectName}_LIBRARY)
#  define %{ProjectName}SHARED_EXPORT Q_DECL_EXPORT
#else
#  define %{ProjectName}HARED_EXPORT Q_DECL_IMPORT
#endif


#endif // %{JS: Cpp.headerGuard('%{GlbHdrFileName}')}
