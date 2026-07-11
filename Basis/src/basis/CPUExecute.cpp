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
#include "basis/CPUOp.h"

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

bool CPUCore::isROReg()
 {
  if( userMode() )
    {
     if( command.ereg.num==RegPC || command.ereg.num==RegCLK || command.ereg.num==RegTBP ) 
       {
        finError(StatusErrorROReg);

        return true;
       }
    }
  else
    {
     if( command.ereg.num==RegPC ) 
       {
        block->fatal(FatalROReg,index,regs[RegPC]);

        return true;
       }
    }  

  return false;  
 }

uint64 CPUCore::get64(const RegArg &reg) const
 {
  switch( reg.width )
    {
     case Reg64bit :
       return regs[reg.num];
     break;

     case Reg32bit :
       return Ext32to64(reg.sign,regs[reg.num/2],reg.num%2);
     break;

     case Reg16bit :
       return Ext16to64(reg.sign,regs[reg.num/4],reg.num%4);
     break;

     case Reg8bit :
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

uint64 CPUCore::get64(const ConstArg &cnst,bool cmd2) const
 {
  if( cnst.ext )
    return cmd2? cmd[2] : cmd[1] ;
  else
    return cnst.val;
 }

uint64 CPUCore::get64(const ConstRegArg &arg) const
 {
  if( arg.isReg ) 
    return get64(arg.reg);
  else  
    return get64(arg.cnst);
 }

uint64 CPUCore::get64reg(const RegArg &reg) const
 {
  return regs[reg.num];
 }

uint32 CPUCore::get32reg(const RegArg &reg) const
 {
  return GetPart32(regs[reg.num/2],reg.num%2);
 }

uint16 CPUCore::get16reg(const RegArg &reg) const
 {
  return GetPart16(regs[reg.num/4],reg.num%4);
 }

uint8 CPUCore::get8reg(const RegArg &reg) const
 {
  return GetPart8(regs[reg.num/8],reg.num%8);
 }

uint64 CPUCore::getAddr() const
 {
  uint64 addr=regs[command.address.base.num];

  switch( command.address.type )
    {
     case CmdAddressBaseReg :
      {
       addr+=get64(command.address.src); 
      }
     break;

     case CmdAddressBaseRegConst :
      {
       addr+=get64(command.address.src.reg)*get64(command.address.cnst1); 
      }
     break;

     case CmdAddressBaseRegConstConst : 
      {
       addr+=get64(command.address.src.reg)*get64(command.address.cnst1)+get64(command.address.cnst2,command.address.cnst1.ext); 
      }
     break;
    }

  return addr;
 }

void CPUCore::set64(const RegArg &reg,uint64 val)
 {
  switch( reg.width )
    {
     case Reg64bit :
       regs[reg.num]=val;
     break;

     case Reg32bit :
       SetPart32(regs[reg.num/2],reg.num%2,uint32(val));
     break;

     case Reg16bit :
       SetPart16(regs[reg.num/4],reg.num%4,uint16(val));
     break;

     case Reg8bit :
       SetPart8(regs[reg.num/8],reg.num%8,uint8(val));
     break;

     default: 
      {
       Printf(Exception,"Basis::CPUCore::set64(...) : unknown register width #;",reg.width); 
      }
    }
 }

void CPUCore::set64reg(const RegArg &reg,uint64 val)
 {
  regs[reg.num]=val;
 }

void CPUCore::set32reg(const RegArg &reg,uint32 val)
 {
  SetPart32(regs[reg.num/2],reg.num%2,val);
 }

void CPUCore::set16reg(const RegArg &reg,uint16 val)
 {
  SetPart16(regs[reg.num/4],reg.num%4,val);
 }

void CPUCore::set8reg(const RegArg &reg,uint8 val)
 {
  SetPart8(regs[reg.num/8],reg.num%8,val);
 }

uint64 CPUCore::get64arg(const ConstRegArg &arg) const
 {
  if( arg.isReg )
    return get64reg(arg.reg);
  else
    return get64(arg.cnst);
 }

uint32 CPUCore::get32arg(const ConstRegArg &arg) const
 {
  return get32reg(arg.reg);
 }

uint16 CPUCore::get16arg(const ConstRegArg &arg) const
 {
  return get16reg(arg.reg);
 }

uint8 CPUCore::get8arg(const ConstRegArg &arg) const
 {
  return get8reg(arg.reg);
 }

void CPUCore::setFlags(uint8 flags)
 {
  unsigned part=command.flagOut;

  if( part<8 )
    {
     SetPart<4>(regs[RegFlags],part,flags);
    }
  else
    {
     OrPart<4>(regs[RegFlags],part,flags);
    }
 }

void CPUCore::completeIO(Status status)
 {
  if( status==StatusDone )  
    {
     if( ioTemp ) 
       {
        switch( command.dst.width )
          {
           case Reg64bit : set64reg(command.dst,temp64); break;
           case Reg32bit : set32reg(command.dst,temp32); break;
           case Reg16bit : set16reg(command.dst,temp16); break;
           case Reg8bit : set8reg(command.dst,temp8); break;
          }
       }

     updatePC();  
    }
  else
    {
     finError(status,ioAddr); 
    }
 }

template <class F,class Dst,class Src>
void CPUCore::executeUn(Dst &dst,Src src)
 {
  setFlags(F()(dst,src));

  updatePC();
 }

template <class F,class Dst>
void CPUCore::executeUn(Dst &dst)
 {
  if( command.src1.getSign() )
    {
     switch( command.src1.getWidth() )
       {
        case Reg64bit : { Op::SInt64 src1{get64arg(command.src1)}; executeUn<F>(dst,src1); } break;
        case Reg32bit : { Op::SInt32 src1{get32arg(command.src1)}; executeUn<F>(dst,src1); } break;
        case Reg16bit : { Op::SInt16 src1{get16arg(command.src1)}; executeUn<F>(dst,src1); } break;
        case Reg8bit :  { Op::SInt8 src1{get8arg(command.src1)};  executeUn<F>(dst,src1); } break;
       }
    }
  else
    {
     switch( command.src1.getWidth() )
       {
        case Reg64bit : { Op::UInt64 src1{get64arg(command.src1)}; executeUn<F>(dst,src1); } break;
        case Reg32bit : { Op::UInt32 src1{get32arg(command.src1)}; executeUn<F>(dst,src1); } break;
        case Reg16bit : { Op::UInt16 src1{get16arg(command.src1)}; executeUn<F>(dst,src1); } break;
        case Reg8bit :  { Op::UInt8 src1{get8arg(command.src1)};  executeUn<F>(dst,src1); } break;
       }
    }
 }

template <class F>
void CPUCore::executeUn()
 {
  if( command.dst.sign )
    {
     switch( command.dst.width )
       {
        case Reg64bit : { Op::SInt64 dst; executeUn<F>(dst); set64reg(command.dst,dst.val); } break;
        case Reg32bit : { Op::SInt32 dst; executeUn<F>(dst); set32reg(command.dst,dst.val); } break;
        case Reg16bit : { Op::SInt16 dst; executeUn<F>(dst); set16reg(command.dst,dst.val); } break;
        case Reg8bit :  { Op::SInt8 dst;  executeUn<F>(dst); set8reg(command.dst,dst.val); } break;
       }
    }
  else
    {
     switch( command.dst.width )
       {
        case Reg64bit : { Op::UInt64 dst; executeUn<F>(dst); set64reg(command.dst,dst.val); } break;
        case Reg32bit : { Op::UInt32 dst; executeUn<F>(dst); set32reg(command.dst,dst.val); } break;
        case Reg16bit : { Op::UInt16 dst; executeUn<F>(dst); set16reg(command.dst,dst.val); } break;
        case Reg8bit :  { Op::UInt8 dst;  executeUn<F>(dst); set8reg(command.dst,dst.val); } break;
       }
    }
 }

void CPUCore::executeCast()
 {
  executeUn<Op::OpCast>();
 }

void CPUCore::executeNeg()
 {
  executeUn<Op::OpNeg>();
 }

void CPUCore::executeNot()
 {
  executeUn<Op::OpNot>();
 }

template <class F,class Dst,class Src1,class Src2>
void CPUCore::executeBin(Dst &dst,Src1 src1,Src2 src2)
 {
  setFlags(F()(dst,src1,src2));

  updatePC();
 }

template <class F,class Dst,class Src1>
void CPUCore::executeBin(Dst &dst,Src1 src1)
 {
  if( command.src2.getSign() )
    {
     switch( command.src2.getWidth() )
       {
        case Reg64bit : { Op::SInt64 src2{get64arg(command.src2)}; executeBin<F>(dst,src1,src2); } break;
        case Reg32bit : { Op::SInt32 src2{get32arg(command.src2)}; executeBin<F>(dst,src1,src2); } break;
        case Reg16bit : { Op::SInt16 src2{get16arg(command.src2)}; executeBin<F>(dst,src1,src2); } break;
        case Reg8bit :  { Op::SInt8 src2{get8arg(command.src2)};  executeBin<F>(dst,src1,src2); } break;
       }
    }
  else
    {
     switch( command.src2.getWidth() )
       {
        case Reg64bit : { Op::UInt64 src2{get64arg(command.src2)}; executeBin<F>(dst,src1,src2); } break;
        case Reg32bit : { Op::UInt32 src2{get32arg(command.src2)}; executeBin<F>(dst,src1,src2); } break;
        case Reg16bit : { Op::UInt16 src2{get16arg(command.src2)}; executeBin<F>(dst,src1,src2); } break;
        case Reg8bit :  { Op::UInt8 src2{get8arg(command.src2)};  executeBin<F>(dst,src1,src2); } break;
       }
    }
 }

template <class F,class Dst>
void CPUCore::executeBin(Dst &dst)
 {
  if( command.src1.getSign() )
    {
     switch( command.src1.getWidth() )
       {
        case Reg64bit : { Op::SInt64 src1{get64arg(command.src1)}; executeBin<F>(dst,src1); } break;
        case Reg32bit : { Op::SInt32 src1{get32arg(command.src1)}; executeBin<F>(dst,src1); } break;
        case Reg16bit : { Op::SInt16 src1{get16arg(command.src1)}; executeBin<F>(dst,src1); } break;
        case Reg8bit :  { Op::SInt8 src1{get8arg(command.src1)};  executeBin<F>(dst,src1); } break;
       }
    }
  else
    {
     switch( command.src1.getWidth() )
       {
        case Reg64bit : { Op::UInt64 src1{get64arg(command.src1)}; executeBin<F>(dst,src1); } break;
        case Reg32bit : { Op::UInt32 src1{get32arg(command.src1)}; executeBin<F>(dst,src1); } break;
        case Reg16bit : { Op::UInt16 src1{get16arg(command.src1)}; executeBin<F>(dst,src1); } break;
        case Reg8bit :  { Op::UInt8 src1{get8arg(command.src1)};  executeBin<F>(dst,src1); } break;
       }
    }
 }

template <class F>
void CPUCore::executeBin()
 {
  if( command.dst.sign )
    {
     switch( command.dst.width )
       {
        case Reg64bit : { Op::SInt64 dst; executeBin<F>(dst); set64reg(command.dst,dst.val); } break;
        case Reg32bit : { Op::SInt32 dst; executeBin<F>(dst); set32reg(command.dst,dst.val); } break;
        case Reg16bit : { Op::SInt16 dst; executeBin<F>(dst); set16reg(command.dst,dst.val); } break;
        case Reg8bit :  { Op::SInt8 dst;  executeBin<F>(dst); set8reg(command.dst,dst.val); } break;
       }
    }
  else
    {
     switch( command.dst.width )
       {
        case Reg64bit : { Op::UInt64 dst; executeBin<F>(dst); set64reg(command.dst,dst.val); } break;
        case Reg32bit : { Op::UInt32 dst; executeBin<F>(dst); set32reg(command.dst,dst.val); } break;
        case Reg16bit : { Op::UInt16 dst; executeBin<F>(dst); set16reg(command.dst,dst.val); } break;
        case Reg8bit :  { Op::UInt8 dst;  executeBin<F>(dst); set8reg(command.dst,dst.val); } break;
       }
    }
 }

void CPUCore::executeAdd()
 {
  executeBin<Op::OpAdd>();
 }

void CPUCore::executeSub()
 {
  executeBin<Op::OpSub>();
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
  uint64 addr=getAddr();

  set64(command.dst,addr);

  updatePC();  
 }

void CPUCore::executeLoad()
 {
  uint64 addr=getAddr();

  Status status=StatusError;

  switch( command.dst.width )
    {
     case Reg64bit : status=mem.readData(addr,temp64); break;
     case Reg32bit : status=mem.readData(addr,temp32); break;
     case Reg16bit : status=mem.readData(addr,temp16); break;
     case Reg8bit : status=mem.readData(addr,temp8); break;
    }

  if( status==StatusDone )  
    {
     switch( command.dst.width )
       {
        case Reg64bit : set64reg(command.dst,temp64); break;
        case Reg32bit : set32reg(command.dst,temp32); break;
        case Reg16bit : set16reg(command.dst,temp16); break;
        case Reg8bit : set8reg(command.dst,temp8); break;
       }

     updatePC();  
    }
  else if( status==StatusError )
    {
     finError(status,addr); 
    }
  else
    {
     ioAddr=addr;
     ioTemp=true;
     ioPending=true; 
    }  
 }

void CPUCore::executeStore()
 {
  uint64 addr=getAddr();

  Status status=StatusError;

  switch( command.dst.width )
    {
     case Reg64bit : status=mem.writeData(addr,get64reg(command.dst)); break;
     case Reg32bit : status=mem.writeData(addr,get32reg(command.dst)); break;
     case Reg16bit : status=mem.writeData(addr,get16reg(command.dst)); break;
     case Reg8bit : status=mem.writeData(addr,get8reg(command.dst)); break;
    }

  if( status==StatusDone )  
    {
     updatePC();  
    }
  else if( status==StatusError )
    {
     finError(status,addr); 
    }
  else
    {
     ioAddr=addr;
     ioTemp=false;
     ioPending=true; 
    }  
 }

void CPUCore::executeRegLoadAddr()
 {
  if( isROReg() ) return;

  uint64 addr=getAddr();

  regs[command.ereg.num]=addr;

  updatePC();  
 }

void CPUCore::executeRegLoad()
 {
  if( isROReg() ) return;

  uint64 addr=getAddr();

  Status status=mem.readData(addr,regs[command.ereg.num]);

  if( status==StatusDone )  
    {
     updatePC();  
    }
  else if( status==StatusError )
    {
     finError(status,addr); 
    }
  else
    {
     ioAddr=addr;
     ioTemp=false;
     ioPending=true; 
    }  
 }

void CPUCore::executeRegStore()
 {
  uint64 addr=getAddr();

  Status status=mem.writeData(addr,regs[command.ereg.num]);

  if( status==StatusDone )  
    {
     updatePC();  
    }
  else if( status==StatusError )
    {
     finError(status,addr); 
    }
  else
    {
     ioAddr=addr;
     ioTemp=false;
     ioPending=true; 
    }  
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

  Printf(block->log,"DebugCore#; PC = #; Val = #;\n",index,regs[RegPC],val);

  updatePC();
 }

void CPUCore::executeSetReg()
 {
  if( isROReg() ) return;

  uint64 val=get64(command.src1);

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
     finError(StatusErrorCmd);
     return;
    }

  updatePC();

  regs[RegLR]=regs[RegPC];
  regs[RegR0]=regs[RegSP];

  regs[RegPC]=sysPC;
  regs[RegSP]=sysSP;

  modeS=true;
  mem.setModeDual(true);
 }

void CPUCore::executeSysExit()
 {
  if( !modeS || modeI )
    {
     finError(StatusErrorCmd);
     return; 
    }

  regs[RegPC]=regs[RegLR];
  regs[RegSP]=regs[RegR0];

  modeS=false;
  mem.setModeDual(false);
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

