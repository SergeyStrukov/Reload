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

namespace Basis {

/* class CPUCore */

bool CPUCore::testCond()
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

     default: return false; 
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
  // TODO
 }

void CPUCore::executeJmpPC()
 {
  // TODO
 }

void CPUCore::executeCall()
 {
  // TODO
 }

void CPUCore::executeCallPC()
 {
  // TODO
 }

void CPUCore::executeRet()
 {
  // TODO
 }

void CPUCore::executeCoreIndex()
 {
  // TODO
 }

void CPUCore::executeDebug()
 {
  // TODO
 }

void CPUCore::executeSetReg()
 {
  // TODO
 }

void CPUCore::executeGetReg()
 {
  // TODO
 }

void CPUCore::executeSetupCoreVMT()
 {
  // TODO
 }

void CPUCore::executeSetupSysPC()
 {
  // TODO
 }

void CPUCore::executeSetupSysSP()
 {
  // TODO
 }

void CPUCore::executeSetupIntPC()
 {
  // TODO
 }

void CPUCore::executeSetupIntSP()
 {
  // TODO
 }

void CPUCore::executeSetupSysVMT()
 {
  // TODO
 }

void CPUCore::executeSysEntry()
 {
  // TODO
 }

void CPUCore::executeSysExit()
 {
  // TODO
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

