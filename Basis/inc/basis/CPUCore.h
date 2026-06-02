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

/* classes */

enum RegIndex : uint8 ;

class CPUCore;
class CPUCoreBlock;

/* enum RegIndex */

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

/* class CPUCore */

class CPUCore : NoCopy
 {
   uint64 regs[RegCount]; 
   uint64 flags;

   uint32 number;

   bool modeS;
   bool modeI;
   bool modeL;

   CPUMem mem;

  public: 

   CPUCore();

   ~CPUCore();
 };

/* class CPUCoreBlock */ 

class CPUCoreBlock : NoCopy
 {
   SimpleArray<CPUCore> cores;

   uint64 sysPC;
   uint64 sysSP;

   uint64 intPC;
   uint64 intSP;

  public:

   CPUCoreBlock();

   ~CPUCoreBlock();

   void init(uint32 count);
 };

} // namespace Basis    

#endif

