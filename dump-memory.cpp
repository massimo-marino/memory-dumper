//
// dump-memory.cpp
//
// Created by massimo on 7/17/18.
//
#include "dump-memory.h"

#include <iomanip>
//#include <boost/format.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace utilities {
//
// see: https://jrruethe.github.io/blog/2015/08/23/placement-new/
//
void
dumpMemory(const void* ptr,
           const std::size_t size,
           std::string&& demangledTypeName,
           std::ostream& os) noexcept
{
  using byte_t = unsigned char;
  using uptr_t = unsigned long;

  // dump from ptr-16 bytes through ptr+16 bytes
  const uptr_t memBuffer {16};

  // Allow direct arithmetic on the pointer
  uptr_t iptr = reinterpret_cast<uptr_t>(ptr) - memBuffer;

  os << "-----------------------------------------------------------------------\n";
  if ("" != demangledTypeName)
  {
    //os << boost::format("Type %s of %d bytes\n") % demangledTypeName % size;
    os << "Type "
       << demangledTypeName
       << " of "
       << std::dec
       << size
       << " bytes - Memory to dump starts at: "
       << std::hex << std::uppercase
       << ptr
       << "\n\n";
  }
  else
  {
    //os << boost::format("%d bytes\n") % size;
    os << size
       << " bytes - Memory to dump starts at: "
       << std::hex << std::uppercase
       << ptr
       << "\n\n";
  }

  // Write the address offsets along the top row
  os << std::string(19, ' ');
  for (std::size_t i{0}; i < 16; ++i)
  {
    // Spaces between every 4 bytes
    if (i % 4 == 0)
    {
      os << " ";
    }
    //os << boost::format(" %2hhX") % i; // Write the address offset
    os << std::hex
       << std::uppercase
       << " "
       << std::setfill(' ')
       << std::setw(2)
       << i;
  }

  // If the object is not aligned
  if (iptr % 16 != 0)
  {
    // Print the first address
    //os << boost::format("\n0x%016lX:") % (iptr & ~15);
    os << "\n0x"
       << std::setfill('0')
       << std::setw(16)
       << (iptr & ~15)
       << std::setw(0)
       << ":";

    // Indent to the offset
    for (std::size_t i{0}; i < iptr % 16; ++i)
    {
      os << "   ";
      if (i % 4 == 0)
      {
        os << " ";
      }
    }
  }

  bool closed {false};
  const auto endByteToDump {(size + memBuffer * 2)};
  const auto endByteToMark {(size + memBuffer - 1)};

  // Dump the memory
  for (std::size_t i {0}; i < endByteToDump; ++i, ++iptr)
  {
    // New line and address every 16 bytes, spaces every 4 bytes
    if ( iptr % 16 == 0 )
    {
      //os << boost::format("\n0x%016lX:") % iptr;
      os << "\n0x"
         << std::setfill('0')
         << std::setw(16)
         << iptr
         << std::setw(0)
         << ":";
    }
    if ( iptr % 4 == 0 )
    {
      os << " ";
    }

    // Write the address contents
    //os << boost::format(" %02hhX") % static_cast<uptr_t>(*reinterpret_cast<byte_t *>(iptr));
    if ( 16 == i )
    {
      os << "<";
    }
    else
    {
      if ( closed )
      {
        closed = false;
        if ( iptr % 16 == 0 )
        {
          os << " ";
        }
      }
      else
      {
        os << " ";
      }
    }
    os << std::hex
       << std::setw(2)
       << std::setfill('0')
       << static_cast<uptr_t>(*reinterpret_cast<byte_t *>(iptr));
    if ( i == endByteToMark )
    {
      closed = true;
      os << ">";
    }
  }

  os << "\n-----------------------------------------------------------------------"
     << std::endl;
}  // dumpMemory

void dumpMemory(const char a[], std::ostream& os)
{
  dumpMemory(a, std::strlen(a), os);
}
}  // namespace utilities

