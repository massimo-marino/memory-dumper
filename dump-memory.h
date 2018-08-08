//
// dump-memory.h
//
// Created by massimo on 7/17/18.
//
#pragma once

#include <iostream>
#include <string>
#include <type_traits>
#include <cxxabi.h>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////
// useful macros
// in case we want to dump a variable
#define DMV(var) utilities::dumpMemory(&(var), sizeof(decltype(var)))
//#define DMV(var) utilities::dumpMemory(&(var), sizeof(decltype(var)), getDemangledTypeName<decltype(var)>())
// in case we want to dump a memory of type type, pointed to by a pointer ptr
#define DMP(ptr, type) utilities::dumpMemory(ptr, sizeof(type))
//#define DMP(ptr, type) utilities::dumpMemory(ptr, sizeof(type), getDemangledTypeName<type>())

namespace demangle
{
template<typename T>
static
std::string
getDemangledTypeName() noexcept;

template<typename T>
static
std::string
getDemangledTypeName() noexcept
{
  int status{};
  char *demangledName{abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status)};
  std::string demangledNameString{static_cast<std::string>(demangledName)};

  free(demangledName);

  return demangledNameString;
}
}  // namespace demangle

namespace utilities
{
void
dumpMemory(const void* ptr,
           std::size_t size,
           std::string&& demangledTypeName = "",
           std::ostream& os = std::cout) noexcept;

template <typename T>
void
dumpMemory(T ptr,
           std::size_t size,
           std::ostream& os = std::cout) noexcept;

template <typename T>
void
dumpMemory(const T ptr,
           size_t size,
           std::ostream& os) noexcept
{
  static_assert(std::is_pointer<T>::value, "pointer needed as arg 1 for dumpMemory()");

  dumpMemory(reinterpret_cast<const void*>(ptr),
             size,
             demangle::getDemangledTypeName<decltype(typename std::remove_pointer<T>::type())>(),
             os);
}

template <typename T>
void
dumpMemory(T&& var, std::ostream& os = std::cout) noexcept;

template <typename T>
void
dumpMemory(T&& var, std::ostream& os) noexcept
{
  static_assert(std::is_reference<decltype(var)>::value != 0,
                "reference, or either lvalue or rvalue reference needed as arg 1 for dumpMemory()");

  dumpMemory(&var, sizeof(var), os);
}

void dumpMemory(const char a[], std::ostream& os = std::cout);
}  // namespace utilities
