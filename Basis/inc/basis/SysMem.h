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

inline constexpr unsigned CacheLineBits = 6 ;

inline constexpr uint64 CacheLineSize = uint64(1)<<CacheLineBits ;

inline constexpr uint64 CacheLineMask = CacheLineSize-1 ;

inline constexpr uint64 CacheLineLen = CacheLineSize/sizeof (uint64) ;

/* classes */

class SysMem;

class SysMemPort;

/* class SysMem */

class SysMem : NoCopy
 {
   bool modeM = false ;

   SimpleArray<uint64> rom;
   SimpleArray<uint64> ram;

  public:

   SysMem();

   ~SysMem();

   void init(uint64 ramSize);

   uint64 * bootROM() { return rom.getPtr(); }

   void setModeM(bool modeM_) { modeM=modeM_; }

   Status readData(uint64 pa,uint64 line[CacheLineLen]);

   Status writeData(uint64 pa,const uint64 line[CacheLineLen]);
 };

/* class SysMemPort */ 

class SysMemPort : NoCopy
 {
   struct Port
    {
     uint64 pa;
     uint64 *ret;
     const uint64 *val;

     void read(uint64 pa_,uint64 *ret_) { pa=pa_; ret=ret_; }

     void write(uint64 pa_,const uint64 *val_) { pa=pa_; ret=0; val=val_; }

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

   Status readData(uint32 port,uint64 pa,uint64 line[CacheLineLen]);

   Status writeData(uint32 port,uint64 pa,const uint64 line[CacheLineLen]);

   Status pending(uint32 port);

   void stepEnd() {}
 };

} // namespace Basis    

#endif
