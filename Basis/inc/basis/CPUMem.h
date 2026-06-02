/* CPUMem.h */
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

#ifndef Basis_inc_CPUMem_h
#define Basis_inc_CPUMem_h

#include "basis/CoreDefs.h"

namespace Basis {

/* classes */

class CPUMem;

/* class CPUMem */

class CPUMem : NoCopy
 {
   uint64 userVMT;
   uint64 sysVMT;

   bool modeM;
   bool modeDual;

  public:
  
   CPUMem();

   ~CPUMem();

   Status fetchCommand(uint64 va,uint64 &cmd);

   Status readData(uint64 va,uint8 &data);
   Status readData(uint64 va,uint16 &data);
   Status readData(uint64 va,uint32 &data);
   Status readData(uint64 va,uint64 &data);

   Status writeData(uint64 va,uint8 data);
   Status writeData(uint64 va,uint16 data);
   Status writeData(uint64 va,uint32 data);
   Status writeData(uint64 va,uint64 data);

   void setUserVMT(uint64 pa);
   void setSysVMT(uint64 pa);

   void extmem(bool enable);
   void dualVMT(bool enable);
 };

} // namespace Basis    

#endif

