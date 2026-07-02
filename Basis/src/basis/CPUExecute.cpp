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
  return true;
  
  // TODO
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
     case CmdSetupSysVMT : executeSetupSysVMT(); break;
     case CmdSetupSysPC : executeSetupSysPC(); break;
     case CmdSetupSysSP : executeSetupSysSP(); break;
     case CmdSetupIntPC : executeSetupIntPC(); break;
     case CmdSetupIntSP : executeSetupIntSP(); break;
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

