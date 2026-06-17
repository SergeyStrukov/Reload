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

/* class CPUCore */

CPUCore::CPUCore() noexcept
 {
 }

CPUCore::~CPUCore()
 {
 }

void CPUCore::init(uint32 index_,uint64 cmdCacheSize,uint64 dataCacheSize,CPUCoreBlock &block_)
 {
  Range(regs).set_null();

  flags=0;

  if( index_==0 ) regs[RegPC]=BootROMAddress;

  index=index_;

  modeS=false;
  modeI=false;
  modeL=false;

  mem.init(index_,cmdCacheSize,dataCacheSize,block_.mpx,block_.sysmap);

  block=&block_;
 }

void CPUCore::clearCache()
 {
  mem.clearCache();
 }

void CPUCore::step()
 {
  // TODO
 }

/* class CPUCoreBlock */ 

void CPUCoreBlock::setModeM(bool modeM)
 {
  mpx.setModeM(modeM);

  for(CPUCore &core : cores ) core.clearCache();
 }

CPUCoreBlock::CPUCoreBlock()
 {
 }

CPUCoreBlock::~CPUCoreBlock()
 {
 }

void CPUCoreBlock::init(uint32 count,uint64 cmdCacheSize,uint64 dataCacheSize,SysMem &mem)
 {
  sysPC=0;
  sysSP=0;

  intPC=0;
  intSP=0;

  modeCore=false;
  modeStop=false;

  shift=0;

  mpx.init(count+1,mem); 
  cache.init(count,cmdCacheSize,0,mpx);
  sysmap.init(cache);

  cores=SimpleArray<CPUCore>(count);

  for(uint32 i=0; i<count ;i++)
    {
     cores[i].init(i,cmdCacheSize,dataCacheSize,*this);   
    }
 }

bool CPUCoreBlock::step()
 {
  if( modeStop ) return false;

  mpx.stepBeg();

  if( modeCore )
    {
     ulen i=shift,len=cores.getLen();

     for(; i<len ;i++) cores[i].step();

     for(i=0; i<shift ;i++) cores[i].step();

     shift++;

     if( shift==len ) shift=0;
    }
  else
    {
     cores[0].step(); 
    }

  mpx.stepEnd();

  return !modeStop;  
 }

} // namespace Basis    

