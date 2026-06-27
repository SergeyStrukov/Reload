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

     // control op

     // addr op

     // sys op
    }
 }

} // namespace Basis    

