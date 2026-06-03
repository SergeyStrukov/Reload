/* SysMem.h */
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

#ifndef Basis_inc_SysMem_h
#define Basis_inc_SysMem_h

#include "basis/CoreDefs.h"

namespace Basis {

/* consts */    

inline constexpr uint64 BootROMSize = 4_MByte ;

inline constexpr uint64 BootROMLen = BootROMSize/sizeof (uint64) ;

inline constexpr uint64 MaxRAMSize = 4_TByte ;

/* classes */

class SysMem;

class SysMemPort;

/* class SysMem */

class SysMem : NoCopy
 {
  public:

   SysMem();

   ~SysMem();

   void init(uint64 ramSize);

   uint64 * bootROM();

   Status readData(uint64 pa,uint64 &data);

   Status writeData(uint64 pa,uint64 data);
 };

/* class SysMemPort */ 

class SysMemPort : NoCopy
 {
   SysMem *mem;

  public:  

   SysMemPort();

   ~SysMemPort();

   void init(uint32 count,SysMem &mem);

   Status readData(uint32 port,uint64 pa,uint64 &data);

   Status writeData(uint32 port,uint64 pa,uint64 data);
 };

} // namespace Basis    

#endif
