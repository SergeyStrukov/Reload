/* CPUCore.h */
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

#ifndef Basis_inc_CPUCore_h
#define Basis_inc_CPUCore_h

#include "basis/CPUMem.h"

namespace Basis {

/* consts */

enum RegIndex : uint8
 {
  RegR0 = 0,
  RegR1,
  RegR2,
  RegR3,
  
  RegR4,
  RegR5,
  RegR6,
  RegR7,
  
  RegR8,
  RegR9,
  RegR10,
  RegR11,

  RegR12,
  RegR13,
  RegR14,
  RegR15,

  RegPC,
  RegLR,
  RegSP,
  RegBP,
  RegOP,

  RegCTX,
  RegCLK, // RO
  RegTBP, // RO

  RegCount
 };

/* classes */

class CPUCore;
class CPUCoreBlock;

/* class CPUCore */

class CPUCore : NoCopy
 {
   uint64 regs[RegCount]{}; 
   uint64 flags = 0 ;

   uint32 index = 0 ;

   bool modeS = false ;
   bool modeI = false ;
   bool modeL = false ;

   CPUMem mem;

   CPUCoreBlock *block = 0 ;

  public: 

   CPUCore() noexcept;

   ~CPUCore();

   void init(uint32 index,uint64 cmdCacheSize,uint64 dataCacheSize,CPUCoreBlock &block);

   void clearCache();

   void step();
 };

/* class CPUCoreBlock */ 

class CPUCoreBlock : NoCopy
 {
   uint64 sysPC = 0 ;
   uint64 sysSP = 0 ;

   uint64 intPC = 0 ;
   uint64 intSP = 0 ;

   bool modeCore = false ;
   bool modeStop = false ;

   ulen shift = 0 ;

   SysMemPort mpx;
   L1Mem cache;
   AddressMap sysmap;

   SimpleArray<CPUCore> cores;

   friend class CPUCore;

  private: 

   void setModeM(bool modeM);

  public:

   CPUCoreBlock();

   ~CPUCoreBlock();

   void init(uint32 count,uint64 cmdCacheSize,uint64 dataCacheSize,SysMem &mem);

   bool step();
 };

} // namespace Basis    

#endif

