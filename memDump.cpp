//
// memDump.cpp
//
#include "memDump.h"
#include <iomanip>
////////////////////////////////////////////////////////////////////////////////
namespace memDump {
static int contextOption {Dynamic};      // default option
static uptr_t fixedPreBufferSize  {24};  // default size
static uptr_t fixedPostBufferSize {24};  // default size

const std::string FGRED       {"\033[1;31m"};  // foreground red
const std::string FGGREEN     {"\033[1;32m"};  // foreground green
const std::string RESET_COLOR {"\033[0m"};

int setFixedContextOption() {
  contextOption = Fixed;
  return contextOption;
}

int setDynamicContextOption() {
  contextOption = Dynamic;
  return contextOption;
}

uptr_t setFixedPreBufferSize(const uptr_t newSize) {
  fixedPreBufferSize = newSize;
  return fixedPreBufferSize;
}

uptr_t setFixedPostBufferSize(const uptr_t newSize) {
  fixedPostBufferSize = newSize;
  return fixedPostBufferSize;
}

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
  // Allow direct arithmetic on the pointer
  uptr_t sptr {reinterpret_cast<uptr_t>(ptr)}; // Start pointer of data to dump
  uptr_t eptr {sptr + size - 1};               // End pointer of data to dump

  // set-up the context buffers around the data to dump
  uptr_t preBuffer {};
  uptr_t postBuffer {};
  switch (contextOption) {
    case Fixed:
    // Option Fixed: fixed context: dump from (sptr - preBuffer) bytes through (eptr + postBuffer) bytes
    preBuffer = fixedPreBufferSize;
    postBuffer = fixedPostBufferSize;
    break;

    case Dynamic:
    default:
    // Option Dynamic: dynamic context
    uptr_t smemptr {sptr & ~15}; // Round down to the last multiple of 16
    smemptr = smemptr - 16;      // Step back one line for context
    uptr_t ememptr {eptr & ~15}; // Round down to the last multiple of 16
    ememptr = ememptr + 31;      // Step forward one line for context
/*
    os << ">>>>> smemptr (hex): 0x" << std::hex << smemptr << std::dec
       << " ememptr (hex): 0x" << std::hex << ememptr << std::dec
       << " (ememptr - smemptr): " << (ememptr - smemptr)
       << " preBuffer = (sptr - smemptr): " << (sptr - smemptr)
       << " postBuffer = (ememptr - eptr): " << (ememptr - eptr)
       << "\n";
*/
    preBuffer = (sptr - smemptr);
    postBuffer = (ememptr - eptr);
    break;
  }

  sptr = sptr - preBuffer;   // Start pointer - preBuffer
  eptr = eptr + postBuffer;  // End pointer + postBuffer

  [[maybe_unused]] const auto memToDumpSize {eptr - sptr + 1};
  const auto endByteToDump {(preBuffer + size + postBuffer)};
  const auto endByteToMark {(preBuffer + size - 1)};
/*
  os << ">>>>> sptr (hex): 0x" << std::hex << sptr << std::dec
     << " eptr (hex): 0x" << std::hex << eptr << std::dec
     << " preBuffer: " << preBuffer
     << " postBuffer: " << postBuffer
     << " memToDumpSize: " << memToDumpSize
     << " size: " << size
     << " endByteToMark: " << endByteToMark
     << " endByteToDump: " << endByteToDump << "\n";
*/
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
  if (sptr % 16 != 0) {
    // Print the first address
    os << "\n0x"
       << std::setfill('0')
       << std::setw(16)
       << (sptr & ~15)
       << std::setw(0)
       << ":";

    // Indent to the offset
    for (std::size_t i {0}; i < sptr % 16; ++i) {
      os << "   ";
      if (i % 4 == 0) {
        os << " ";
      }
    }
  }

  bool closed {false};
  bool marking {false};

  // Dump the memory
  for (std::size_t i {0}; i < endByteToDump; ++i, ++sptr) {
    // New line and address every 16 bytes, spaces every 4 bytes
    if (sptr % 16 == 0) {
      os << "\n0x"
         << std::setfill('0')
         << std::setw(16)
         << sptr
         << std::setw(0)
         << ":";
    }
    if (sptr % 4 == 0) {
      os << " ";
    }

    // Print the address contents
    if (preBuffer == i) {
      os << FGRED << "<";  // start highlighting marker
      marking = true;
    } else {
      if (closed) {
        closed = false;
        if (sptr % 16 == 0) {
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
       << static_cast<uptr_t>(*reinterpret_cast<byte_t *>(sptr))
       << RESET_COLOR;
    if (endByteToMark == i) {
      closed = true;
      marking = false;
      os << FGRED << ">" << RESET_COLOR;  // end highlighting marker
    }
  }
  os << "\n-----------------------------------------------------------------------"
     << std::endl;
}  // dumpMemory
}  // namespace memDump
