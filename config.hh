#pragma once

#if defined(__linux__) || defined(__APPLE__)
#define FUSE_USE_VERSION 31
#include <fuse.h>
#else
#error \
    "Unsupported platform: Only Linux (libfuse) and macOS (macFUSE) are supported"
#endif
