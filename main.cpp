//
// main.cpp
//
// Examples of memory dumps
//
// Created by massimo on 8/7/18.
//
#include "dump-memory.h"
////////////////////////////////////////////////////////////////////////////////
//// test code from here

class test_t
{
  [[maybe_unused]] char  a; // 1 byte
  [[maybe_unused]] int   b; // 4 bytes
  [[maybe_unused]] short c; // 2 bytes
  [[maybe_unused]] long  d; // 8 bytes

public:
  test_t() :
          a(0x11),
          b(0x22222222),
          c(0x3333),
          d(0x4444444444444444)
  {}
};

class C
{
public:
  // we don't want these objects allocated on the heap
  // cannot use new() and std::make_unique()
  // std::make_shared() can be used as it calls default ctor, or any other ctor's
  // by means of the global allocator, aka the placement new, ::new
  // see: https://en.cppreference.com/w/cpp/memory/shared_ptr/make_shared
  // 'The object is constructed as if by the expression
  //    ::new (pv) T(std::forward<Args>(args)...),
  // where pv is an internal void* pointer to storage suitable to hold an object
  // of type T.'
  // Placement new is faster than operator new() since it allocates objects at an
  // already allocated and known memory address
  void* operator new(std::size_t) = delete;
  void* operator new[](std::size_t) = delete;
  void operator delete(void*) = delete;
  void operator delete[](void*) = delete;

  // default ctor
  explicit
  C () :
          m_x(999)
  {
    std::cout << "C(): def ctor...\n";
  }

  // another ctor
  explicit
  C (int x) :
          m_x(x)
  {
    std::cout << "C(int x): ctor...\n";
  }

  // dtor
  ~C ()
  {
    std::cout << "~C(): bye from dtor!\n";
  }

  int m_x {};
};

template <typename T>
class D
{
  // this calls the default ctor and allocates on stack
  T t;
};

// we'll dump this memory in the main()
// this is an array that would be declared as:
// unsigned char _arr[10];
// or
// std::byte _arr[10];
// and filled with 0xFF:
// std::fill_n(_arr, 10, 0xFF);
__asm__
(
"       .global _arr;\n"
"       .section .data;\n"
"       _arr: .fill 10, 1, 0xFF;\n"
);

////////////////////////////////////////////////////////////////////////////////

int main ()
{
  using namespace std::string_literals;

  // iptr is allocated on the stack, while the int pointed to is in the heap
  int* iptr = new int(0x01234567);

  // allocated on stack
  C cObj;
  // allocated on stack
  D<C> dObj;

  //auto up = std::make_unique<C>();  // WRONG: does not compile because new() is deleted
  auto sp = std::make_shared<C>();    // OK: it calls ::new
  auto sp2 = std::make_shared<C>(2);  // OK: it calls ::new

  auto v = sp->m_x;
  std::cout << v << std::endl;

  auto v2 = sp2->m_x;
  std::cout << v2 << std::endl;

  utilities::dumpMemory(&iptr, sizeof(iptr)); //utilities::dumpMemory(&iptr,    sizeof(iptr),  utilities::getDemangledTypeName<decltype(iptr)>());
  // dump heap memory
  utilities::dumpMemory(&(*iptr), sizeof(*iptr)); //utilities::dumpMemory(&(*iptr), sizeof(*iptr), getDemangledTypeName<decltype(*iptr)>());
  utilities::dumpMemory(&cObj,    sizeof(cObj));  //utilities::dumpMemory(&cObj,    sizeof(cObj),  getDemangledTypeName<decltype(cObj)>());
  utilities::dumpMemory(&dObj,    sizeof(dObj));  //utilities::dumpMemory(&dObj,    sizeof(dObj),  getDemangledTypeName<decltype(dObj)>());
  utilities::dumpMemory(&v,       sizeof(v));     //utilities::dumpMemory(&v,       sizeof(v),     getDemangledTypeName<decltype(v)>());
  utilities::dumpMemory(&v2,      sizeof(v2));    //utilities::dumpMemory(&v2,      sizeof(v2),    getDemangledTypeName<decltype(v2)>());
  // the same with the macro
  DMV(v2);

  {
    unsigned char memory[sizeof(test_t)];
    // fill all the array with the same value 0xFF
    std::fill_n(memory, sizeof(test_t), 0xFF);

    utilities::dumpMemory(memory, sizeof(test_t)); //utilities::dumpMemory(memory, sizeof(test_t), getDemangledTypeName<test_t>());
    // the same with the macro
    DMP(memory, test_t);

    test_t* ptr = ::new (memory) test_t();
    utilities::dumpMemory(ptr, sizeof(test_t)); //utilities::dumpMemory(ptr, sizeof(test_t), getDemangledTypeName<test_t>());
    // the same with the macro
    DMP(ptr, test_t);

    ptr->~test_t();
  }

  {
    extern std::byte _arr[];

    for (int i {0}; i < 10; ++i)
    {
      std::cout << "_arr["
                << i
                << "] = "
                << static_cast<unsigned short>(_arr[i])
                << "\n";
    }
    utilities::dumpMemory(_arr, 10); // utilities::dumpMemory(_arr, 10, getDemangledTypeName<decltype(_arr)>());

    std::fill_n(_arr, 10, static_cast<std::byte>(0xAA));
    for (int i {0}; i < 10; ++i)
    {
      std::cout << "_arr["
                << i
                << "] = "
                << static_cast<unsigned short>(_arr[i])
                << "\n";
    }
    utilities::dumpMemory(_arr, 10); // utilities::dumpMemory(_arr, 10, getDemangledTypeName<decltype(_arr)>());
  }

  {
    int v3 {0x00000004};
    utilities::dumpMemory(&v3, sizeof(v3));
  }

  {
    int v4 {0x0BDF1177};
    utilities::dumpMemory(v4);
  }

  utilities::dumpMemory(0x00123456);
  utilities::dumpMemory(1.0);
  utilities::dumpMemory(static_cast<std::string>("Hello World!"));
  utilities::dumpMemory("Hello World!"s);
  utilities::dumpMemory("Hello World!", 13);
  utilities::dumpMemory("Hello World!");

  return 0;
}
