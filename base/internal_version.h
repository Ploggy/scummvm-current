#ifndef INCLUDED_FROM_BASE_VERSION_CPP
#error This file may only be included by base/version.cpp
#endif

#ifdef RELEASE_BUILD
#undef RESIDUAL_REVISION
#endif

#ifndef RESIDUAL_REVISION
#define RESIDUAL_REVISION
#endif

#define RESIDUAL_VERSION "0.0.6git" RESIDUAL_REVISION
