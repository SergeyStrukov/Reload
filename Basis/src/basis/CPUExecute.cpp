/* CPUExecute.cpp */
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

/* functions */  

template <unsigned W,UIntType UInt>
uint64 ExtTo64(uint8 sign,UInt val)
 {
  if( sign && (val>>(W-1)) ) return uint64(val)-OneBit(W);

  return val;
 }

uint64 Ext32to64(uint8 sign,uint32 val) { return ExtTo64<32,uint32>(sign,val); }

uint64 Ext16to64(uint8 sign,uint16 val) { return ExtTo64<16,uint16>(sign,val); }

uint64 Ext8to64(uint8 sign,uint8 val) { return ExtTo64<8,uint8>(sign,val); }

template <unsigned W,UIntType UInt>
uint64 ExtTo64(uint8 sign,uint64 val,uint8 part)
 {
  return ExtTo64<W,UInt>(sign,UInt( val>>(part*W) ));
 }

uint64 Ext32to64(uint8 sign,uint64 val,uint8 part) { return ExtTo64<32,uint32>(sign,val,part); }

uint64 Ext16to64(uint8 sign,uint64 val,uint8 part) { return ExtTo64<16,uint16>(sign,val,part); }

uint64 Ext8to64(uint8 sign,uint64 val,uint8 part) { return ExtTo64<8,uint8>(sign,val,part); }

template <unsigned W>
void SetPart(uint64 &reg,uint8 part,uint64 val)
 {
  unsigned shift=part*W;

  uint64 mask=BitMask(W);

  reg &= ~(mask<<shift);

  reg |= (val<<shift) ;
 }

void SetPart32(uint64 &reg,uint8 part,uint32 val) { SetPart<32>(reg,part,val); }

void SetPart16(uint64 &reg,uint8 part,uint16 val) { SetPart<16>(reg,part,val); }

void SetPart8(uint64 &reg,uint8 part,uint8 val) { SetPart<8>(reg,part,val); }

/* class CPUCore */

bool CPUCore::testCond() const
 {
  switch( command.cond )
    {
     case CmdAlways : return true;

     case CmdIfZ : return condFlags().bitZ();
     case CmdNotZ : return !condFlags().bitZ();

     case CmdIfC : return condFlags().bitC();
     case CmdNotC : return !condFlags().bitC();

     case CmdIfN : return condFlags().bitN();
     case CmdNotN : return !condFlags().bitN();

     case CmdIfO : return condFlags().bitO();
     case CmdNotO : return !condFlags().bitO();

     case CmdIfCnZ : return condFlags().isCnZ();
     case CmdNotCnZ : return !condFlags().isCnZ();

     case CmdIfNisO : return condFlags().isNisO();
     case CmdNotNisO : return !condFlags().isNisO();

     case CmdIfZNisO : return condFlags().isZNisO();
     case CmdNotZNisO : return !condFlags().isZNisO();

     default: 
      {
       Printf(Exception,"Basis::CPUCore::testCond() : unknown condition code #;",command.cond); 

       return false;
      }
    }
 }

uint64 CPUCore::get64(const RegArg &reg) const
 {
  switch( reg.width )
    {
     case 0 : // 64
       return regs[reg.num];
     break;

     case 1 : // 32
       return Ext32to64(reg.sign,regs[reg.num/2],reg.num%2);
     break;

     case 2 : // 16
       return Ext16to64(reg.sign,regs[reg.num/4],reg.num%4);
     break;

     case 3 : // 8
       return Ext8to64(reg.sign,regs[reg.num/8],reg.num%8);
     break;

     default: 
      {
       Printf(Exception,"Basis::CPUCore::get64(...) : unknown register width #;",reg.width); 

       return 0;
      }
    }
 }

uint64 CPUCore::get64(const ConstArg &cnst) const
 {
  if( cnst.ext )
    return cmd[1];
  else
    return cnst.val;
 }

uint64 CPUCore::get64(const ConstRegArg &reg) const
 {
  if( reg.isReg ) 
    return get64(reg.reg);
  else  
    return get64(reg.cnst);
 }

void CPUCore::set64(const RegArg &reg,uint64 val)
 {
  switch( reg.width )
    {
     case 0 : // 64
       regs[reg.num]=val;
     break;

     case 1 : // 32
       SetPart32(regs[reg.num/2],reg.num%2,uint32(val));
     break;

     case 2 : // 16
       SetPart16(regs[reg.num/4],reg.num%4,uint16(val));
     break;

     case 3 : // 8
       SetPart8(regs[reg.num/8],reg.num%8,uint8(val));
     break;

     default: 
      {
       Printf(Exception,"Basis::CPUCore::set64(...) : unknown register width #;",reg.width); 
      }
    }
 }

void CPUCore::executeCast()
 {
  // TODO
 }

void CPUCore::executeNeg()
 {
  // TODO
 }

void CPUCore::executeNot()
 {
  // TODO
 }

void CPUCore::executeAdd()
 {
  // TODO
 }

void CPUCore::executeSub()
 {
  // TODO
 }

void CPUCore::executeMul()
 {
  // TODO
 }

void CPUCore::executeDiv()
 {
  // TODO
 }

void CPUCore::executeRem()
 {
  // TODO
 }

void CPUCore::executeLoadAddr()
 {
  // TODO
 }

void CPUCore::executeLoad()
 {
  // TODO
 }

void CPUCore::executeStore()
 {
  // TODO
 }

void CPUCore::executeRegLoadAddr()
 {
  // TODO
 }

void CPUCore::executeRegLoad()
 {
  // TODO
 }

void CPUCore::executeRegStore()
 {
  // TODO
 }

void CPUCore::executeLock()
 {
  // TODO
 }

void CPUCore::executeUnlock()
 {
  // TODO
 }

void CPUCore::executeJmp()
 {
  uint64 PC=get64(command.src1);

  regs[RegPC]=PC;
 }

void CPUCore::executeJmpPC()
 {
  updatePC();

  uint64 dPC=get64(command.src1);

  regs[RegPC]+=dPC;
 }

void CPUCore::executeCall()
 {
  uint64 PC=get64(command.src1);

  regs[RegLR]=regs[RegPC];
  regs[RegPC]=PC;
 }

void CPUCore::executeCallPC()
 {
  updatePC();

  uint64 dPC=get64(command.src1);

  regs[RegLR]=regs[RegPC];
  regs[RegPC]+=dPC;
 }

void CPUCore::executeRet()
 {
  regs[RegPC]=regs[RegLR];
 }

void CPUCore::executeCoreIndex()
 {
  set64(command.dst,index);

  updatePC();
 }

void CPUCore::executeDebug()
 {
  uint64 val=get64(command.src1);

  Printf(Con,"DebugCore#; PC = #; Val = #;",index,regs[RegPC],val);

  updatePC();
 }

void CPUCore::executeSetReg()
 {
  uint64 val=get64(command.src1);

  if( userMode() )
    {
     if( command.ereg.num==RegPC || command.ereg.num==RegCLK || command.ereg.num==RegTBP ) 
       {
        finError(StatusErrorROReg);
        return;
       }
    }
  else
    {
     if( command.ereg.num==RegPC ) 
       {
        block->fatal(FatalROReg,index,regs[RegPC]);
        return;
       }
    }  

  regs[command.ereg.num]=val;

  updatePC();
 }

void CPUCore::executeGetReg()
 {
  uint64 val=regs[command.ereg.num];

  set64(command.dst,val);

  updatePC();
 }

void CPUCore::executeSetupCoreVMT()
 {
  block->setupCoreVMT(get64(command.dst),get64(command.src1),index,regs[RegPC]);

  updatePC();
 }

void CPUCore::executeSetupSysPC()
 {
  block->setupSysPC(get64(command.dst),get64(command.src1),index,regs[RegPC]);

  updatePC();
 }

void CPUCore::executeSetupSysSP()
 {
  block->setupSysSP(get64(command.dst),get64(command.src1),index,regs[RegPC]);

  updatePC();
 }

void CPUCore::executeSetupIntPC()
 {
  block->setupIntPC(get64(command.dst),get64(command.src1),index,regs[RegPC]);

  updatePC();
 }

void CPUCore::executeSetupIntSP()
 {
  block->setupIntSP(get64(command.dst),get64(command.src1),index,regs[RegPC]);

  updatePC();
 }

void CPUCore::executeSetupSysVMT()
 {
  block->setupSysVMT(get64(command.src1));

  updatePC();
 }

void CPUCore::executeSysEntry()
 {
  if( !userMode() )
    {
     block->fatal(FatalSysEntry,index,regs[RegPC]); 
     return;
    }

  updatePC();

  regs[RegR1]=regs[RegPC];
  regs[RegR2]=regs[RegSP];

  regs[RegPC]=sysPC;
  regs[RegSP]=sysSP;
 }

void CPUCore::executeSysExit()
 {
  if( userMode() )
    {
     finError(StatusErrorSysExit);
     return; 
    }
  else if( modeI )
    {
     block->fatal(FatalSysExit,index,regs[RegPC]); 
     return;
    }

  regs[RegPC]=regs[RegR1];
  regs[RegSP]=regs[RegR2];
 }

void CPUCore::executeMemEnable()
 {
  block->setModeM(true);

  updatePC();
 }

void CPUCore::executeMemDisable()
 {
  block->setModeM(false);

  updatePC();
 }

void CPUCore::executeCacheCoreClear()
 {
  clearCache();

  updatePC();
 }

void CPUCore::executeCacheSysClear()
 {
  block->clearCache();

  updatePC();
 }

void CPUCore::executeCoreEnable()
 {
  block->enableCores();

  updatePC();
 }

void CPUCore::executeCoreDisable()
 {
  block->disableCores();

  updatePC();
 }

void CPUCore::executeCoreStop()
 {
  block->stop();

  updatePC();
 }

void CPUCore::executeOp()
 {
  if( !testCond() )
    {
     updatePC();
     return;   
    }

  switch( command.opcode )  
    {
     case CmdNop : updatePC(); break;   

     // un op

     case CmdCast : executeCast(); break;
     case CmdNeg : executeNeg(); break;
     case CmdNot : executeNot(); break;

     // bin op

     case CmdAdd : executeAdd(); break;
     case CmdSub : executeSub(); break;
     case CmdMul : executeMul(); break;
     case CmdDiv : executeDiv(); break;
     case CmdRem : executeRem(); break;

     // mem op

     case CmdLoadAddr : executeLoadAddr(); break;
     case CmdLoad : executeLoad(); break;
     case CmdStore : executeStore(); break;
     case CmdRegLoadAddr : executeRegLoadAddr(); break;
     case CmdRegLoad : executeRegLoad(); break;
     case CmdRegStore : executeRegStore(); break;

     // lock op

     case CmdLock : executeLock(); break;
     case CmdUnlock : executeUnlock(); break;

     // control op

     case CmdJmp : executeJmp(); break;
     case CmdJmpPC : executeJmpPC(); break;
     case CmdCall : executeCall(); break;
     case CmdCallPC : executeCallPC(); break;
     case CmdRet : executeRet(); break;

     // misc op

     case CmdCoreIndex : executeCoreIndex(); break;
     case CmdDebug : executeDebug(); break;
     case CmdSetReg : executeSetReg(); break;
     case CmdGetReg : executeGetReg(); break;

     // sys op

     case CmdSetupCoreVMT : executeSetupCoreVMT(); break;
     case CmdSetupSysPC : executeSetupSysPC(); break;
     case CmdSetupSysSP : executeSetupSysSP(); break;
     case CmdSetupIntPC : executeSetupIntPC(); break;
     case CmdSetupIntSP : executeSetupIntSP(); break;
     case CmdSetupSysVMT : executeSetupSysVMT(); break;
     case CmdSysEntry : executeSysEntry(); break;
     case CmdSysExit : executeSysExit(); break;
     case CmdMemEnable : executeMemEnable(); break;
     case CmdMemDisable : executeMemDisable(); break;
     case CmdCacheCoreClear : executeCacheCoreClear(); break;
     case CmdCacheSysClear : executeCacheSysClear(); break;
     case CmdCoreEnable : executeCoreEnable(); break;
     case CmdCoreDisable : executeCoreDisable(); break;
     case CmdCoreStop : executeCoreStop(); break;

     default:
      {
       Printf(Exception,"Basis::CPUCore::executeOp() : unknown opcode #;",command.opcode);
      }
    }
 }

} // namespace Basis    

