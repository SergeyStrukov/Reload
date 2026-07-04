/* Computer.h */
//----------------------------------------------------------------------------------------
//
//  Target: Basis 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2026 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef Basis_inc_Computer_h
#define Basis_inc_Computer_h

#include "basis/CPUCore.h"

namespace Basis {

/* consts */    

inline constexpr uint32 MaxCores = 64 ;

inline constexpr uint32 MinCacheSize = 64_KByte ;
inline constexpr uint32 MaxCacheSize = 64_MByte ;

/* classes */

struct ComputerCfg;

class Computer;

/* struct ComputerCfg */

struct ComputerCfg
 {
  uint64 ramSize = 1_GByte ;
  uint64 cmdCacheSize = 64_KByte ;
  uint64 dataCacheSize = 64_KByte ;
  uint32 coreCount = 4 ;

  void validate() const;
 };

/* class Computer */

class Computer : NoCopy
 {
   SysMem sysmem;
   CPUCoreBlock cpu;

  public:

   explicit Computer(const ComputerCfg &cfg);

   ~Computer();

   uint64 * bootROM() { return sysmem.bootROM(); }

   void run();
 };

} // namespace Basis    

#endif
