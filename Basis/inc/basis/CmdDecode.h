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

enum RegIndex // 5 bit
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

  RegPC,  // RO
  RegLR,
  RegSP,
  RegBP,
  RegOP,

  RegCTX,
  RegCLK, // RO in user mode
  RegTBP, // RO in user mode
  RegFlags,

  RegCount
 };

inline constexpr unsigned CommonRegCount = 16 ; 

enum CmdOpcode // 8 bit
 {
  CmdUndef = 0,
  CmdNop, 

  CmdUnBase = 2,
  CmdCast = CmdUnBase, // dst src1
  CmdNeg,
  CmdNot, 
  CmdSwapBit,
  CmdSwapByte,
  CmdUnLim,

  // hole
  
  CmdBinBase = 64,
  CmdAdd = CmdBinBase, // dst src1 src2
  CmdSub,  
  CmdMul,  
  CmdDiv,  
  CmdRem,  
  CmdAnd,
  CmdOr,
  CmdXor,
  CmdAndNot,
  CmdSelect,
  CmdBinLim,

  // hole

  CmdOtherBase = 128,
  CmdLoadAddr = CmdOtherBase, // dst address
  CmdLoad,                    // dst address
  CmdStore,                   // address dst
  CmdRegLoadAddr,             // ereg address
  CmdRegLoad,                 // ereg address
  CmdRegStore,                // address ereg 
  CmdLock,                    // address
  CmdUnlock,                  // address
  CmdJmp,                     // src1
  CmdJmpPC,                   // src1 
  CmdCall,                    // src1
  CmdCallPC,                  // src1
  CmdRet,                     //
  CmdCoreIndex,               // dst 
  CmdDebug,                   // src1
  CmdSetReg,                  // ereg src1 
  CmdGetReg,                  // dst ereg
  CmdOtherLim,

  // hole

  CmdSysBase = 128+32,
  CmdSetupCoreVMT = CmdSysBase, // dst src1
  CmdSetupSysPC,                // dst src1
  CmdSetupSysSP,                // dst src1
  CmdSetupIntPC,                // dst src1
  CmdSetupIntSP,                // dst src1
  CmdSetupSysVMT,               // src1
  CmdSysEntry,                  //
  CmdSysExit,                   // 
  CmdMemEnable,                 //
  CmdMemDisable,                // 
  CmdCacheCoreClear,            //
  CmdCacheSysClear,             //
  CmdCoreEnable,                // 
  CmdCoreDisable,               //
  CmdCoreStop,                  //
  CmdSysLim,  

  // hole

  CmdExtBase = 128+64,
 };

enum CmdCond // 4 bit
 {
  CmdAlways = 0,  

  CmdIfZ,      // Z != 0
  CmdNotZ,     // Z == 0

  CmdIfC,      // C != 0
  CmdNotC,     // C == 0

  CmdIfN,      // N != 0               <
  CmdNotN,     // N == 0               >=

  CmdIfO,      // O != 0
  CmdNotO,     // O == 0

  CmdIfNorZ,   // N != 0  or Z != 0     <=
  CmdNotNorZ   // N == 0 and Z == 0     >
 }; 

enum RegWidth
 {
  Reg64bit = 0 ,  
  Reg32bit,
  Reg16bit,
  Reg8bit
 }; 

enum CmdAddressType
 {
  CmdAddressBase = 0,  
  CmdAddressBaseReg,  
  CmdAddressBaseRegConst,  
  CmdAddressBaseRegConstConst  
 }; 

/* classes */

struct RegArg;

struct ExtRegArg;

struct ConstArg;

struct ConstRegArg;

struct CmdAddress;

struct Cmd;

/* struct RegArg */

struct RegArg // 9 bit
 {
  uint8 sign : 1 ;  
  uint8 width : 2 ; // 64,32,16,8
  uint8 num : 7 ;

  bool decode(uint64 cmd);
 };

/* struct ExtRegArg */ 

struct ExtRegArg // 5 bit
 {
  uint8 num : 5 ;

  bool decode(uint64 cmd);
 };

/* struct ConstArg */ 

struct ConstArg // >= 2 bit
 {
  uint8 ext : 1 ;
  uint8 sign : 1 ;
  uint64 val;

  void decode(uint64 cmd,unsigned width);
 };

/* struct ConstRegArg */ 

struct ConstRegArg // >= 10 bit
 {
  uint8 isReg : 1 ;  
  uint8 ext : 1 ;
  union
   {
    RegArg reg;
    ConstArg cnst;
   }; 

  bool decode(uint64 cmd,unsigned width); 

  uint8 getSign() const { return isReg? reg.sign : cnst.sign ; }

  uint8 getWidth() const { return isReg? reg.width : 64 ; }
 };

/* struct CmdAddress */ 

struct CmdAddress // 43 or 39
 {
  ExtRegArg base;
  uint8 type : 2 ;
  ConstRegArg src;
  ConstArg cnst1;
  ConstArg cnst2;
  uint8 ext : 2 ;

  bool decode(uint64 cmd);
  bool decode39(uint64 cmd);
 };

/* struct Cmd */ 

struct Cmd
 {
  uint8 opcode : 8 = CmdUndef ;
  uint8 cond : 4 = CmdAlways ;

  uint8 flag : 4 = 0 ;
  uint8 flagOut : 4 = 0 ;

  RegArg dst{};
  ConstRegArg src1{};
  ConstRegArg src2{};

  ExtRegArg ereg{};
  CmdAddress address{};
  
  uint32 decode(uint64 cmd);
 };

} // namespace Basis    

#endif
