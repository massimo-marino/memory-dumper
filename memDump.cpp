//
// memDump.cpp
//
#include "memDump.h"
#include <iomanip>
////////////////////////////////////////////////////////////////////////////////
namespace memDump {
const std::string FGRED       {"\033[1;31m"};  // foreground red
const std::string FGGREEN     {"\033[1;32m"};  // foreground green
const std::string RESET_COLOR {"\033[0m"};

void dumpMemory(const char a[], std::ostream& os) {
  std::cout << "memDump::dumpMemory(char [],...) called before ...\n";
  dumpMemory(a, std::strlen(a), os);
}

// See: https://jrruethe.github.io/blog/2015/08/23/placement-new/ for original code;
// page not found on Feb 2025, it's been archived here last time:
// https://web.archive.org/web/20210728162751/https://jrruethe.github.io/blog/2015/08/23/placement-new/
void dumpMemory(const void* ptr,
                const std::size_t size,
                const std::string&& demangledTypeName,
                std::ostream& os) noexcept {
  using byte_t = unsigned char;
  using uptr_t = unsigned long;

  // dump from ptr-16 bytes through ptr+16 bytes
  const uptr_t memBuffer {16};

  // Allow direct arithmetic on the pointer
  uptr_t iptr = reinterpret_cast<uptr_t>(ptr) - memBuffer;

  os << "[memDump:dumpMemory]---------------------------------------------------\n";
  if ("" != demangledTypeName) {
    os << "Type "
       << demangledTypeName
       << " of "
       << std::dec
       << size
       << " bytes - Memory to dump starts at: "
       << std::hex << std::uppercase
       << ptr
       << "\n\n";
  } else {
    os << size
       << " bytes - Memory to dump starts at: "
       << std::hex << std::uppercase
       << ptr
       << "\n\n";
  }

  // Print the address offsets along the top row
  os << std::string(19, ' ');
  for (std::size_t i {0}; i < 16; ++i) {
    // Spaces between every 4 bytes
    if (i % 4 == 0) {
      os << " ";
    }
    os << std::hex
       << std::uppercase
       << " "
       << std::setfill(' ')
       << std::setw(2)
       << i;
  }

  // If the object is not aligned
  if (iptr % 16 != 0) {
    // Print the first address
    os << "\n0x"
       << std::setfill('0')
       << std::setw(16)
       << (iptr & ~15)
       << std::setw(0)
       << ":";

    // Indent to the offset
    for (std::size_t i {0}; i < iptr % 16; ++i) {
      os << "   ";
      if (i % 4 == 0) {
        os << " ";
      }
    }
  }

  bool closed {false};
  bool marking {false};
  const auto endByteToDump {(size + memBuffer * 2)};
  const auto endByteToMark {(size + memBuffer - 1)};

  // Dump the memory
  for (std::size_t i {0}; i < endByteToDump; ++i, ++iptr) {
    // New line and address every 16 bytes, spaces every 4 bytes
    if (iptr % 16 == 0) {
      os << "\n0x"
         << std::setfill('0')
         << std::setw(16)
         << iptr
         << std::setw(0)
         << ":";
    }
    if (iptr % 4 == 0) {
      os << " ";
    }

    // Print the address contents
    if (16 == i) {
      os << FGRED << "<";  // start marker
      marking = true;
    } else {
      if (closed) {
        closed = false;
        if (iptr % 16 == 0) {
          os << " ";
        }
      } else {
        os << " ";
      }
    }
    os << ((marking) ? FGRED : "")
       << std::hex
       << std::setw(2)
       << std::setfill('0')
       << static_cast<uptr_t>(*reinterpret_cast<byte_t *>(iptr))
       << RESET_COLOR;
    if (i == endByteToMark) {
      closed = true;
      marking = false;
      os << FGRED << ">" << RESET_COLOR;  // end marker
    }
  }
  os << "\n-----------------------------------------------------------------------"
     << std::endl;
}  // dumpMemory
}  // namespace memDump
