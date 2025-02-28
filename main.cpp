//
// main.cpp
//
// Examples of memory dumps
//
#include "memDump.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <cstdint>

using namespace std::string_literals;
////////////////////////////////////////////////////////////////////////////////
void logFuncName(const char fn[]) {
  std::cout << memDump::FGGREEN << "\n>>> " << fn << " <<<\n" << memDump::RESET_COLOR;
}
#define LOGFNAME logFuncName(__func__);
////////////////////////////////////////////////////////////////////////////////
class test_t {
private:
  // no padding with this layout
  char   a;  // 1 byte
  int8_t i8; // 1 byte
  short  c;  // 2 bytes
  int    b;  // 4 bytes
  long   d;  // 8 bytes

public:
  test_t() :
  a(0x11),
  i8(0x55),
  c(0x3031),
  b(0x20222221),
  d(0x4044444444444441)
  {
    std::cout << "test_t::test_t(): default CTOR called for object at " << this << "\n";
  }

  ~test_t() {
    std::cout << "test_t::~test_t(): DTOR called for object at " << this << "\n";
  }

  void logSizeInfo() {
    std::cout << std::dec
              << "test_t::logSizeInfo(): sizeof(*this):     " << sizeof(*this) << "\n"
              << "test_t::logSizeInfo(): sizeof(test_t):    " << sizeof(test_t) << "\n"
              << "test_t::logSizeInfo(): sizeof(char a):    " << sizeof(a) << "\n"
              << "test_t::logSizeInfo(): sizeof(int8_t i8): " << sizeof(i8) << "\n"
              << "test_t::logSizeInfo(): sizeof(int b):     " << sizeof(b) << "\n"
              << "test_t::logSizeInfo(): sizeof(short c):   " << sizeof(c) << "\n"
              << "test_t::logSizeInfo(): sizeof(long d):    " << sizeof(d) << "\n";
  }
};

class C {
public:
  // We don't want these objects allocated on the heap: cannot use new() and
  // std::make_unique(); std::make_shared() can be used as it calls default
  // ctor, or any other ctor's by means of the global allocator, aka the
  // placement new, ::new
  // See: https://en.cppreference.com/w/cpp/memory/shared_ptr/make_shared:
  // 'The object is constructed as if by the expression
  //    ::new (pv) T(std::forward<Args>(args)...),
  // where pv is an internal void* pointer to storage suitable to hold an object
  // of type T.'
  // Placement new is faster than operator new() since it creates objects at an
  // already allocated and known memory address
  // See: https://en.cppreference.com/w/cpp/language/new#Placement_new
  void* operator new(std::size_t) = delete;
  void* operator new[](std::size_t) = delete;
  void operator delete(void*) = delete;
  void operator delete[](void*) = delete;

  // default ctor
  explicit C() :
  m_x(999)
  {
    std::cout << "C(): default CTOR called for object at " << this << "\n";
  }

  // another ctor
  explicit C(int x) :
  m_x(x)
  {
    std::cout << "C(int x): CTOR called for object at " << this << "\n";
  }

  // dtor
  ~C() {
    std::cout << "~C(): DTOR called for object at " << this << "\n";
  }

  int m_x {};
};

template <typename T>
class D {
  // this calls the default ctor and allocates on stack
  T t;
};

// we'll dump this memory in the main(); this is an array that would be declared as:
// unsigned char _arr[10];
// or
// std::byte _arr[10];
// and filled with 0xFF:
// std::fill_n(_arr, 10, 0xFF);
__asm__
(
".global _arr;\n"
".section .data;\n"
"_arr: .fill 10, 1, 0xFF;\n"
);

void dumpMemoryCase_1() {
  LOGFNAME
  // iptr is allocated on the stack, while the int pointed to is in the heap
  int* iptr = new int(0x01234567);
  // dump stack memory
  std::cout << "dumping stack memory at " << &iptr << "\n";
  memDump::dumpMemory(&iptr, sizeof(iptr));
  // dump heap memory
  std::cout << "\ndumping heap memory at " << &(*iptr) << "\n";
  memDump::dumpMemory(&(*iptr), sizeof(*iptr));
  delete iptr;
}

void dumpMemoryCase_2() {
  LOGFNAME
  // allocated on stack
  C cObj;
  std::cout << "dumping stack memory at " << &cObj << "\n";
  memDump::dumpMemory(&cObj, sizeof(cObj));
}

void dumpMemoryCase_3() {
  LOGFNAME
  // allocated on stack
  D<C> dObj;
  std::cout << "dumping stack memory at " << &dObj << "\n";
  memDump::dumpMemory(&dObj, sizeof(dObj));
}

void dumpMemoryCase_4() {
  LOGFNAME
  //auto up = std::make_unique<C>();  // WRONG: does not compile because new() is deleted: fatal error: call to deleted function 'operator new'
  std::shared_ptr<C> sp = std::make_shared<C>();  // OK: it calls ::new

  std::cout << "dumping heap memory at " << sp << "\n";
  memDump::dumpMemory(sp.get(), sizeof(sp.get()));

  int v = sp->m_x;

  std::cout << "\nv: " << std::dec << v << std::hex << " - 0x" << v << std::endl;
  std::cout << "dumping stack memory at " << &v << "\n";
  memDump::dumpMemory(&v, sizeof(v));
}

void dumpMemoryCase_5() {
  LOGFNAME
  std::shared_ptr<C> sp2 = std::make_shared<C>(2);  // OK: it calls ::new

  std::cout << "dumping heap memory at " << sp2 << "\n";
  memDump::dumpMemory(sp2.get(), sizeof(sp2.get()));

  int v2 = sp2->m_x;

  std::cout << "\nv2: " << v2 << std::endl;
  std::cout << "dumping stack memory at " << &v2 << "\n";
  memDump::dumpMemory(&v2, sizeof(v2));
  // the same with the macro
  std::cout << "\ndumping stack memory at " << &v2 << " with macro DMV()\n";
  DMV(v2);
}

void dumpMemoryCase_6() {
  LOGFNAME
  unsigned char memory[sizeof(test_t)];
  // fill all the array with the same value 0xFF
  std::fill_n(memory, sizeof(test_t), 0xFF);

  std::cout << "dumping stack memory at " << &memory << "\n";
  memDump::dumpMemory(memory, sizeof(test_t));
  // the same with the macro
  std::cout << "\ndumping stack memory at " << &memory << " with macro DMP()\n";
  DMP(memory, test_t);
}

void dumpMemoryCase_7() {
  LOGFNAME
  // Statically allocate the storage with automatic storage duration
  // which is large enough for any object of type test_t
  alignas(test_t) unsigned char memory[sizeof(test_t)];
  // fill all the array with the same value 0xFF
  std::fill_n(memory, sizeof(test_t), 0xFF);

  std::cout << "dumping stack memory at " << &memory << "\n";
  memDump::dumpMemory(memory, sizeof(test_t));

  // Placement new: construct a test_t object, placing it directly into the
  // pre-allocated storage at memory address memory
  test_t* ptr = ::new (memory) test_t();

  ptr->logSizeInfo();
  std::cout << "\ndumping placed new stack memory at " << ptr << "\n";
  memDump::dumpMemory(ptr, sizeof(test_t));
  // the same with the macro
  std::cout << "\ndumping new-placed stack memory at " << ptr << " with macro DMP()\n";
  DMP(ptr, test_t);

  // We must **manually** call the object's destructor if its side effects are
  // depended by the program. Leaving this block scope automatically deallocates memory
  ptr->~test_t();
}

void dumpMemoryCase_8() {
  LOGFNAME
  extern std::byte _arr[];

  for (std::size_t i {0}; i < 10; ++i) {
      std::cout << "_arr["
                << i
                << "] = "
                << static_cast<unsigned short>(_arr[i])
                << "\n";
  }
  std::cout << "dumping global/static memory at " << _arr << "\n";
  memDump::dumpMemory(_arr, 10);
}

void dumpMemoryCase_9() {
  LOGFNAME
  extern std::byte _arr[];

  std::fill_n(_arr, 10, static_cast<std::byte>(0xAA));
  for (std::size_t i {0}; i < 10; ++i) {
      std::cout << "_arr["
                << i
                << "] = "
                << static_cast<unsigned short>(_arr[i])
                << "\n";
  }
  std::cout << "dumping global/static memory at " << _arr << "\n";
  memDump::dumpMemory(_arr, 10);
  std::fill_n(_arr, 10, static_cast<std::byte>(0xFF));
}

void dumpMemoryCase_10() {
  LOGFNAME
  int v3 {0x00000004};
  std::cout << "dumping stack memory at " << &v3 << "\n";
  memDump::dumpMemory(&v3, sizeof(v3));
}

void dumpMemoryCase_11() {
  LOGFNAME
  int v4 {0x0BDF1177};
  std::cout << "dumping stack memory at " << &v4 << "\n";
  memDump::dumpMemory(v4);
}

void dumpMemoryCase_12() {
  LOGFNAME
  std::cout << "dumping stack memory\n";
  memDump::dumpMemory(0x00123456);
}

void dumpMemoryCase_13() {
  LOGFNAME
  std::cout << "dumping stack memory\n";
  memDump::dumpMemory(1.0);
}

void dumpMemoryCase_14() {
  LOGFNAME
  std::cout << "dumping stack memory\n";
  memDump::dumpMemory(static_cast<std::string>("Hello World!"));
}

void dumpMemoryCase_15() {
  LOGFNAME
  std::cout << "dumping stack memory\n";
  memDump::dumpMemory("Hello World!"s);
}

void dumpMemoryCase_16() {
  LOGFNAME
  std::cout << "dumping global/static memory\n";
  memDump::dumpMemory("Hello World!", 13);
}

void dumpMemoryCase_17() {
  LOGFNAME
  std::cout << "dumping global/static memory\n";
  memDump::dumpMemory("Hello World!");
}

void dumpMemoryCase_18() {
  LOGFNAME
  std::string s {"AAAAA"};  // A is 0x41
  std::cout << "main: dumping stack memory for std::string s of sizeof(s): " << std::dec << sizeof(s) << " at " << &s << "\n";
  memDump::dumpMemory(s);

  std::cout << "\nmain: dumping stack memory for s.c_str() of s.length(): " << std::dec << s.length() << " at " << static_cast<const void*>(s.c_str()) << "\n";
  memDump::dumpMemory(s.c_str());
}

class nonaddressable {
  public:
  typedef double useless_type;

  useless_type operator&() const;
};

void dumpMemoryCase_19() {
  LOGFNAME
  nonaddressable* p = new nonaddressable;

  std::cout << "nonaddressable pointer at " << p << "\n";
  memDump::dumpMemory(p);
  delete p;
}

void dumpMemoryCase_20() {
  LOGFNAME
  nonaddressable na;
  //nonaddressable *naptr1 = &na; // Compiler error here
  nonaddressable* naptr2 = std::addressof(na); // No compiler error with std::addressof()
  nonaddressable* naptr3 = memDump::addressof(na); // No compiler error with our own memDump::addressof()

  std::cout << "sizeof(na): " << sizeof(na)
            << "\nsizeof(nonaddressable): " << sizeof(nonaddressable)
            << "\n\ndumping heap memory at " << naptr2 << "\n";
  memDump::dumpMemory(naptr2);

  std::cout << "\ndumping heap memory at " << naptr3 << "\n";
  memDump::dumpMemory(naptr3);
}

void dumpMemoryCase_21() {
  LOGFNAME
  std::cout << "sizeof(nonaddressable): " << sizeof(nonaddressable) << "\n";
  // Statically allocate the storage with automatic storage duration
  // which is large enough for any object of type nonaddressable
  alignas(nonaddressable) unsigned char memory[sizeof(nonaddressable)];
  // fill all the array with the same value 0xEE
  std::fill_n(memory, sizeof(nonaddressable), 0xEE);

  std::cout << "dumping stack memory at " << &memory << "\n";
  memDump::dumpMemory(memory, sizeof(nonaddressable));

  // Placement new: construct a nonaddressable object, placing it directly into the
  // pre-allocated storage at memory address memory
  nonaddressable* ptr = ::new (memory) nonaddressable();

  std::cout << "\ndumping new-placed stack memory at " << ptr << "\n";
  memDump::dumpMemory(ptr, sizeof(nonaddressable));

  // We must **manually** call the object's destructor if its side effects are
  // depended by the program. Leaving this block scope automatically deallocates memory
  ptr->~nonaddressable();
}

void runExamples() {
  dumpMemoryCase_1();
  dumpMemoryCase_2();
  dumpMemoryCase_3();
  dumpMemoryCase_4();
  dumpMemoryCase_5();
  dumpMemoryCase_6();
  dumpMemoryCase_7();
  dumpMemoryCase_8();
  dumpMemoryCase_9();
  dumpMemoryCase_10();
  dumpMemoryCase_11();
  dumpMemoryCase_12();
  dumpMemoryCase_13();
  dumpMemoryCase_14();
  dumpMemoryCase_15();
  dumpMemoryCase_16();
  dumpMemoryCase_17();
  dumpMemoryCase_18();
  dumpMemoryCase_19();
  dumpMemoryCase_20();
  dumpMemoryCase_21();
}
////////////////////////////////////////////////////////////////////////////////
int main () {
  memDump::checkEndianness();
  memDump::setFixedContextOption();
  runExamples();
  std::cout << "\n\n================================================================================\n\n";
  memDump::setDynamicContextOption();
  runExamples();
  std::cout << "\n";
  return 0;
}
