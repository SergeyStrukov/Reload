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
#include <CCore/inc/Print.h>

namespace Basis {

/* class CPUCore */

void CPUCore::finError(Status status)
 {
  finError(status,regs[RegPC]);
 }

void CPUCore::finError(Status status,uint64 va)
 {
  Printf(Exception,"Basis::CPUCore::finError(#;,#.h;)",status,va);

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

     uint64 va=cmdVA();

     Status status=mem.fetchCommand(va,cmd[cmdInd]);

     if( status==StatusPending ) 
       {
        memPending=true; 
        return;
       } 

     if( status!=StatusDone )
       {
        finError(status,va);  
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

  sysPC=0;
  sysSP=0;

  intPC=0;
  intSP=0;

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

void CPUCore::enable()
 {
  regs[RegPC]=sysPC;
  regs[RegSP]=sysSP;
  modeS=true;

  clearCache();
 }

void CPUCore::setupCoreVMT(uint64 pa)
 {
  mem.setupVMT(pa);
 }

void CPUCore::setupSysPC(uint64 PC)
 {
  sysPC=PC;
 }

void CPUCore::setupSysSP(uint64 SP)
 {
  sysSP=SP;
 }

void CPUCore::setupIntPC(uint64 PC)
 {
  intPC=PC;
 }

void CPUCore::setupIntSP(uint64 SP)
 {
  intSP=SP;
 }

void CPUCore::step()
 {
  if( userMode() ) regs[RegCLK]++;

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
        finError(status,cmdVA());  
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
        finError(status,PC);  
       } 
    }
 }

/* class CPUCoreBlock */ 

void CPUCoreBlock::init(uint32 count,uint64 cmdCacheSize,uint64 dataCacheSize,SysMem &mem)
 {
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

void CPUCoreBlock::setModeM(bool modeM)
 {
  mpx.setModeM(modeM);

  for(CPUCore &core : cores ) core.clearCache();

  cache.clearCache();
 }

void CPUCoreBlock::clearCache()
 {
  cache.clearCache();
 }

void CPUCoreBlock::enableCores()
 {
  for(ulen ind=1,len=cores.getLen(); ind<len ;ind++) cores[ind].enable();

  modeCore=true;
 }

void CPUCoreBlock::setupCoreVMT(uint64 index,uint64 pa,uint32 exeIndex,uint64 exePC)
 {
  if( index<cores.getLen() )
    {
     cores[index].setupCoreVMT(pa); 
    }
  else
    {
     fatal(FatalCoreIndex,exeIndex,exePC); 
    }  
 }

void CPUCoreBlock::setupSysPC(uint64 index,uint64 PC,uint32 exeIndex,uint64 exePC)
 {
  if( index<cores.getLen() )
    {
     cores[index].setupSysPC(PC); 
    }
  else
    {
     fatal(FatalCoreIndex,exeIndex,exePC); 
    }  
 }

void CPUCoreBlock::setupSysSP(uint64 index,uint64 SP,uint32 exeIndex,uint64 exePC)
 {
  if( index<cores.getLen() )
    {
     cores[index].setupSysSP(SP); 
    }
  else
    {
     fatal(FatalCoreIndex,exeIndex,exePC); 
    }  
 }

void CPUCoreBlock::setupIntPC(uint64 index,uint64 PC,uint32 exeIndex,uint64 exePC)
 {
  if( index<cores.getLen() )
    {
     cores[index].setupIntPC(PC); 
    }
  else
    {
     fatal(FatalCoreIndex,exeIndex,exePC); 
    }  
 }

void CPUCoreBlock::setupIntSP(uint64 index,uint64 SP,uint32 exeIndex,uint64 exePC)
 {
  if( index<cores.getLen() )
    {
     cores[index].setupIntSP(SP); 
    }
  else
    {
     fatal(FatalCoreIndex,exeIndex,exePC); 
    }  
 }

void CPUCoreBlock::setupSysVMT(uint64 pa)
 {
  sysmap.setup(pa);
 }

CPUCoreBlock::CPUCoreBlock()
 {
 }

CPUCoreBlock::~CPUCoreBlock()
 {
 }

void CPUCoreBlock::fatal(FatalCode code,uint32 index,uint64 PC)
 {
  Printf(Con,"Fatal CPU stop: core #; PC = #.h; #;\n",index,PC,code);

  stop();
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

