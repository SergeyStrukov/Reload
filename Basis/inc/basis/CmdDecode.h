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

enum CmdOpcode // 8 bit
 {
  CmdUndef = 0,
  CmdNop, 

  CmdUnBase = 2,
  CmdCast = CmdUnBase,
  CmdNeg,
  CmdNot,  
  CmdUnLim,

  // hole
  
  CmdBinBase = 64,
  CmdAdd = CmdBinBase,  
  CmdSub,  
  CmdMul,  
  CmdDiv,  
  CmdRem,  
  CmdBinLim,

  // hole

  CmdOtherBase = 128,
  CmdJmp = CmdOtherBase,
  CmdJmpPC,
  CmdCall,
  CmdCallPC,
  CmdCallRet,
  CmdOtherLim,

  // hole

  CmdSysBase = 128+64,
  CmdCoreStop = CmdSysBase,
  CmdSysLim,

  // hole
 };

enum CmdCond // 4 bit
 {
  CmdAlways = 0,  

  CmdIfZ,
  CmdNotZ,

  CmdIfC,
  CmdNotC,

  CmdIfN,
  CmdNotN,

  CmdIfO,
  CmdNotO,

  CmdIfCZ,
  CmdNotCZ,

  CmdIfNisO,
  CmdNotNisO,

  CmdIfZNisO,
  CmdNotZNisO,
 }; 

/* classes */

struct RegArg;

struct ConstArg;

struct ConstRegArg;

struct Cmd;

/* struct RegArg */

struct RegArg
 {
  uint8 sign : 1 ;  
  uint8 width : 2 ; // 64,32,16,8
  uint8 num : 7 ;

  bool decode(uint64 cmd);
 };

/* struct ConstArg */ 

struct ConstArg
 {
  uint8 ext : 1 ;
  uint8 big : 2 ; // 23,33,41
  uint64 val;
 };

/* struct ConstRegArg */ 

struct ConstRegArg
 {
  uint8 isReg : 1 ;  
  union
   {
    RegArg reg;
    ConstArg cnst;
   }; 
 };

/* struct Cmd */ 

struct Cmd
 {
  uint8 opcode = CmdUndef ;
  uint8 cond = CmdAlways ;
  uint8 flag : 4 = 0 ;
  uint8 flagOut : 4 = 0 ;

  RegArg dst{};
  ConstRegArg src1{};
  ConstRegArg src2{};
  
  uint32 decode(uint64 cmd);
 };

} // namespace Basis    

#endif
