//
// memDump.h
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
#define DMV(var) memDump::dumpMemory(&(var), sizeof(decltype(var)))
//#define DMV(var) memDump::dumpMemory(&(var), sizeof(decltype(var)), getDemangledTypeName<decltype(var)>())
// in case we want to dump a memory of type type, pointed to by a pointer ptr
#define DMP(ptr, type) memDump::dumpMemory(ptr, sizeof(type))
//#define DMP(ptr, type) memDump::dumpMemory(ptr, sizeof(type), getDemangledTypeName<type>())

namespace demangle {
template<typename T>
static std::string getDemangledTypeName() noexcept;

template<typename T>
static std::string getDemangledTypeName() noexcept {
  int status {};
  char *demangledName {abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status)};
  const std::string demangledNameString {static_cast<std::string>(demangledName)};

  free(demangledName);

  return demangledNameString;
}
}  // namespace demangle

namespace memDump
{
using byte_t = unsigned char;
using uptr_t = unsigned long;

enum CONTEXT_OPTIONS {
  Fixed = 1,
  Dynamic = 2
};

extern const std::string FGRED;    // foreground red
extern const std::string FGGREEN;  // foreground green
extern const std::string RESET_COLOR;

int setFixedContextOption();
int setDynamicContextOption();

uptr_t setFixedPreBufferSize();
uptr_t setFixedPostBufferSize();

void dumpMemory(const void* ptr,
                const std::size_t size,
                const std::string&& demangledTypeName = "",
                std::ostream& os = std::cout) noexcept;

template <typename T>
void dumpMemory(const T ptr,
                const std::size_t size,
                std::ostream& os = std::cout) noexcept;

template <typename T>
void dumpMemory(const T ptr,
                const std::size_t size,
                std::ostream& os) noexcept {
  std::cout << "memDump::dumpMemory(T ptr,...) called before ...\n";
  static_assert(std::is_pointer<T>::value, "pointer needed as arg 1 for dumpMemory()");

  dumpMemory(reinterpret_cast<const void*>(ptr),
             size,
             demangle::getDemangledTypeName<decltype(typename std::remove_pointer<T>::type())>(),
             os);
}

template <typename T>
void dumpMemory(T&& var, std::ostream& os = std::cout) noexcept;

template <typename T>
void dumpMemory(T&& var, std::ostream& os) noexcept {
  std::cout << "memDump::dumpMemory(T&& var,...) called before ...\n";
  static_assert(std::is_reference<decltype(var)>::value,
                "reference, or either lvalue or rvalue reference needed as arg 1 for dumpMemory()");

  dumpMemory(&var, sizeof(var), os);
}

template <typename T>
void dumpMemory(T& var, std::ostream& os = std::cout) noexcept;

template <typename T>
void dumpMemory(T& var, std::ostream& os) noexcept {
  std::cout << "memDump::dumpMemory(T& var,...) called before ...\n";
  static_assert(std::is_reference<decltype(var)>::value,
                "reference needed as arg 1 for dumpMemory()");

  dumpMemory(&var, sizeof(var), os);
}

void dumpMemory(const char a[], std::ostream& os = std::cout);

template <class T>
T* addressof(T& v) noexcept {
  return reinterpret_cast<T*>(& const_cast<char&>(reinterpret_cast<const volatile char&>(v)));
}
}  // namespace memDump
