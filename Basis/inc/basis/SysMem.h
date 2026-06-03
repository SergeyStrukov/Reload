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

inline constexpr uint64 BootROMAddress = 4_MByte ;

inline constexpr uint64 BootROMSize = 4_MByte ;

inline constexpr uint64 BootROMLen = BootROMSize/sizeof (uint64) ;

inline constexpr uint64 RegSpaceAddress = 8_MByte ;

inline constexpr uint64 RegSpaceSize = 4_MByte ;

inline constexpr uint64 RAMAddress = 12_MByte ;

inline constexpr uint64 MaxRAMSize = 4_TByte ;

inline constexpr uint64 DevAddress = 12_MByte + 4_TByte ;

inline constexpr uint64 MaxDevSize = 4_TByte ;

inline constexpr uint64 MemBankSize = 4_MByte ;

/* classes */

class SysMem;

class SysMemPort;

/* class SysMem */

class SysMem : NoCopy
 {
   SimpleArray<uint64> rom;
   SimpleArray<uint64> ram;

  public:

   SysMem();

   ~SysMem();

   void init(uint64 ramSize);

   uint64 * bootROM() { return rom.getPtr(); }

   Status readData(uint64 pa,uint64 &data);

   Status writeData(uint64 pa,uint64 data);
 };

/* class SysMemPort */ 

class SysMemPort : NoCopy
 {
   struct Port
    {
     uint64 pa;
     uint64 data;
     uint64 *ret;

     void read(uint64 pa_,uint64 &data_) { pa=pa_; ret=&data_; }

     void write(uint64 pa_,uint64 data_) { pa=pa_; data=data_; ret=0; }

     Status operator () (SysMemPort &obj) const;
    };

   SimpleArray<Port> ports;

   SimpleArray<uint64> banks;
   ulen nbanks = 0 ;

   SysMem *mem = 0 ;

  private:

   bool useBank(uint64 pa);
  
  public:  

   SysMemPort();

   ~SysMemPort();

   void init(uint32 count,SysMem &mem);

   void stepBeg() { nbanks=0; }

   Status readData(uint32 port,uint64 pa,uint64 &data);

   Status writeData(uint32 port,uint64 pa,uint64 data);

   Status pending(uint32 port);

   void stepEnd() {}
 };

} // namespace Basis    

#endif
