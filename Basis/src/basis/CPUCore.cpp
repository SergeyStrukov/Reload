/* CPUCore.cpp */
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

#include "basis/CPUCore.h"

namespace Basis {

/* class CPUCoreBlock */ 

CPUCoreBlock::CPUCoreBlock()
 {
 }

CPUCoreBlock::~CPUCoreBlock()
 {
 }

void CPUCoreBlock::init(uint32 count,SysMem &mem)
 {
  cores=SimpleArray<CPUCore>(count);

  for(ulen i=0; i<count ;i++)
    {
     cores[i].init(i,mpx);   
    }

  sysPC=0;
  sysSP=0;

  intPC=0;
  intSP=0;

  modeCore=false;
  modeStop=false;

  mpx.init(count,mem); 
 }

bool CPUCoreBlock::step()
 {
  // TODO

  return !modeStop;  
 }

} // namespace Basis    

