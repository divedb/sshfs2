#pragma once

#define DISABLE_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;     \
  TypeName& operator=(const TypeName&) = delete;

#define UNUSED(x) (void)(x)