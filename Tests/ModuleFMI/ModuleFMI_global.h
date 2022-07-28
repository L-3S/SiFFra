#ifndef ModuleFMI_GLOBAL_H
#define ModuleFMI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ModuleFMI_LIBRARY)
#  define ModuleFMI_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define ModuleFMI_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ModuleFMI_GLOBAL_H
