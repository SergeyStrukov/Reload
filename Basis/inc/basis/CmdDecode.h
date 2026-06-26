/* CmdDecode.h */
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

#ifndef Basis_inc_CmdDecode_h
#define Basis_inc_CmdDecode_h

#include "basis/CoreDefs.h"

namespace Basis {

/* consts */ 

enum RegIndex
 {
  RegR0 = 0,
  RegR1,
  RegR2,
  RegR3,
  
  RegR4,
  RegR5,
  RegR6,
  RegR7,
  
  RegR8,
  RegR9,
  RegR10,
  RegR11,

  RegR12,
  RegR13,
  RegR14,
  RegR15,

  RegPC,
  RegLR,
  RegSP,
  RegBP,
  RegOP,

  RegCTX,
  RegCLK, // RO
  RegTBP, // RO
  RegFlags,

  RegCount
 };

enum CmdUnop
 {
  CmdNeg,
  CmdNot,  

  // TODO

  CmdUnopCount
 };

enum CmdBinop
 {
  CmdAdd,  
  CmdSub,  
  CmdMul,  
  CmdDiv,  
  CmdRem,  

  // TODO

  CmdBinopCount
 }; 

enum CmdOpcode
 {
  CmdUndef = 0,
  CmdNop, 

  CmdUnBase = 2,
  
  CmdBinBase = 64,

  CmdOtherBase = 128,

  CmdSysBase = 128+64,

  CmdOpcodeCount
 };

/* classes */

struct Cmd;

/* struct Cmd */ 

struct Cmd
 {
  CmdOpcode opcode = CmdUndef ;
  
  uint32 decode(uint64 cmd);
 };

} // namespace Basis    

#endif
