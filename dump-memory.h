//
// dump-memory.h
//
// Created by massimo on 7/17/18.
//
#pragma once

#include <iostream>
#include <string>
#include <cxxabi.h>
////////////////////////////////////////////////////////////////////////////////
// useful macros
// in case we want to dump a variable
#define DMV(var) utilities::dumpMemory(&(var), sizeof(decltype(var)))
//#define DMV(var) utilities::dumpMemory(&(var), sizeof(decltype(var)), getDemangledTypeName<decltype(var)>())
// in case we want to dump a memory of type type, pointed to by a pointer ptr
#define DMP(ptr, type) utilities::dumpMemory(ptr, sizeof(type))
//#define DMP(ptr, type) utilities::dumpMemory(ptr, sizeof(type), getDemangledTypeName<type>())


template<typename T>
static
std::string
getDemangledTypeName() noexcept;

template<typename T>
static
std::string
getDemangledTypeName() noexcept
{
  int status {};
  char *demangledName {abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status)};
  std::string demangledNameString {static_cast<std::string>(demangledName)};

  free(demangledName);

  return demangledNameString;
}

namespace utilities {
void
dumpMemory(const void *ptr,
           std::size_t size,
           std::string &&demangledTypeName = "",
           std::ostream &os = std::cout);

template <typename T>
void
dumpMemory(const T *ptr,
           std::size_t size,
           std::ostream &os = std::cout);

template <typename T>
void
dumpMemory(const T *ptr,
           size_t size,
           std::ostream &os)
{
  dumpMemory(reinterpret_cast<const void *>(ptr), size, getDemangledTypeName<T>(), os);
}

}  // namespace utilities
