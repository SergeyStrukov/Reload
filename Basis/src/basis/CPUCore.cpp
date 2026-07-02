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

#include <CCore/inc/Exception.h>

namespace Basis {

/* class CPUCore */

void CPUCore::finError(Status status)
 {
  Printf(Exception,"Basis::CPUCore::finError(#;)",status);

  // TODO
 }

void CPUCore::execute()
 {
  if( command.opcode==CmdUndef )
    {
     finError(StatusErrorCmd); 
    }
  else if( command.opcode>=CmdSysBase && userMode() )
    {
     finError(StatusErrorCmd); 
    }
  else
    {
     executeOp(); 
    }
 } 

void CPUCore::step1()
 {
  if( cmdInd==0 )
    {
     cmdLen=command.decode(cmd[0]); 
    }

  while( cmdInd<cmdLen )
    {
     cmdInd++;  

     uint64 PC=regs[RegPC]; 

     Status status=mem.fetchCommand(PC+cmdInd*sizeof (uint64),cmd[cmdInd]);

     if( status==StatusPending ) 
       {
        memPending=true; 
        return;
       } 

     if( status!=StatusDone )
       {
        finError(status);  
        return;
       } 
    }

  execute(); 
 } 

CPUCore::CPUCore() noexcept
 {
 }

CPUCore::~CPUCore()
 {
 }

void CPUCore::init(uint32 index_,uint64 cmdCacheSize,uint64 dataCacheSize,CPUCoreBlock &block_)
 {
  cmdInd=0;
  cmdLen=0;
  command={};
  memPending=false;

  Range(regs).set_null();

  index=index_;

  modeS=false;
  modeI=false;
  modeL=false;

  mem.init(index_,cmdCacheSize,dataCacheSize,block_.mpx,block_.sysmap);

  block=&block_;

  if( index_==0 ) 
    {
     regs[RegPC]=BootROMAddress;
     modeS=true;
    }
 }

void CPUCore::clearCache()
 {
  mem.clearCache();
 }

void CPUCore::step()
 {
  if( memPending )
    {
     Status status=mem.pending(); 

     if( status==StatusPending ) return;

     memPending=false;

     if( status==StatusDone )
       {
        step1();
       }
     else
       {
        finError(status);  
       }
    }
  else
    {
     cmdInd=0;

     uint64 PC=regs[RegPC]; 

     Status status=mem.fetchCommand(PC,cmd[0]);

     if( status==StatusPending ) 
       {
        memPending=true; 
        return;
       } 

     if( status==StatusDone )
       {
        step1(); 
       }
     else
       {
        finError(status);  
       } 
    }
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

