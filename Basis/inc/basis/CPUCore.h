/* CPUCore.h */
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

#ifndef Basis_inc_CPUCore_h
#define Basis_inc_CPUCore_h

#include "basis/CPUMem.h"
#include "basis/CmdDecode.h"

namespace Basis {

/* functions */  

template <unsigned W,UIntType UInt>
uint64 ExtTo64(uint8 sign,UInt val);

uint64 Ext32to64(uint8 sign,uint32 val);

uint64 Ext16to64(uint8 sign,uint16 val);

uint64 Ext8to64(uint8 sign,uint8 val);

template <unsigned W,UIntType UInt>
uint64 ExtTo64(uint8 sign,uint64 val,uint8 part);

uint64 Ext32to64(uint8 sign,uint64 val,uint8 part);

uint64 Ext16to64(uint8 sign,uint64 val,uint8 part);

uint64 Ext8to64(uint8 sign,uint64 val,uint8 part);

/* classes */

struct FlagBits;

class CPUCore;

class CPUCoreBlock;

/* struct FlagBits */

struct FlagBits
 {
  uint8 bits : 4 ;

  uint8 bitZ() const { return bits&1u; }
  uint8 bitC() const { return bits&2u; }
  uint8 bitN() const { return bits&4u; }
  uint8 bitO() const { return bits&8u; }

  bool isCnZ() const { return (bits&3u)==2 ; }

  bool isNisO() const { uint8 m=bits&12u; return m==0 || m==12u ; }

  bool isZNisO() const { uint8 m=bits&13u; return m==1u || m==13u ; }
 };

/* class CPUCore */

class CPUCore : NoCopy
 {
   uint64 cmd[3]{};
   uint32 cmdInd = 0 ;
   uint32 cmdLen = 0 ;
   Cmd command;
   
   union 
    {
     uint64 temp64;
     uint32 temp32;
     uint16 temp16;
     uint8 temp8;
    };

   uint64 ioAddr = 0 ; 
   bool ioTemp = false ;

   bool ioPending = false ;
   bool memPending = false ;

   uint64 regs[RegCount]{}; 

   uint32 index = 0 ;

   uint64 sysPC = 0 ;
   uint64 sysSP = 0 ;

   uint64 intPC = 0 ;
   uint64 intSP = 0 ;

   bool modeS = false ;
   bool modeI = false ;
   bool modeL = false ;

   CPUMem mem;

   CPUCoreBlock *block = 0 ;

  private: 

   bool userMode() const { return modeS==0 && modeI==0 ; }

   FlagBits condFlags() const { return {uint8( regs[RegFlags]>>(command.flag*4) )}; }

   bool testCond() const;

   bool isROReg();

   uint64 get64(const RegArg &reg) const;
   uint64 get64(const ConstArg &cnst) const;
   uint64 get64(const ConstArg &cnst,bool cmd2) const;
   uint64 get64(const ConstRegArg &arg) const;

   uint64 get64reg(const RegArg &reg) const;
   uint32 get32reg(const RegArg &reg) const;
   uint16 get16reg(const RegArg &reg) const;
   uint8 get8reg(const RegArg &reg) const;

   uint64 getAddr() const;

   void set64(const RegArg &reg,uint64 val);

   void set64reg(const RegArg &reg,uint64 val);
   void set32reg(const RegArg &reg,uint32 val);
   void set16reg(const RegArg &reg,uint16 val);
   void set8reg(const RegArg &reg,uint8 val);

   uint64 get64arg(const ConstRegArg &arg) const;
   uint32 get32arg(const ConstRegArg &arg) const;
   uint16 get16arg(const ConstRegArg &arg) const;
   uint8 get8arg(const ConstRegArg &arg) const;

   void setFlags(uint8 flags);

   void completeIO(Status status);

   template <class F,class Dst,class Src>
   void executeUn(Dst &dst,Src src);
   template <class F,class Dst>
   void executeUn(Dst &dst);
   template <class F>
   void executeUn();

   void executeCast();
   void executeNeg();
   void executeNot();

   template <class F,class Dst,class Src1,class Src2>
   void executeBin(Dst &dst,Src1 src1,Src2 src2);
   template <class F,class Dst,class Src1>
   void executeBin(Dst &dst,Src1 src1);
   template <class F,class Dst>
   void executeBin(Dst &dst);
   template <class F>
   void executeBin();

   void executeAdd();
   void executeSub();
   void executeMul();
   void executeDiv();
   void executeRem();
   void executeAnd();
   void executeOr();
   void executeXor();
   void executeAndNot();

   void executeLoadAddr();
   void executeLoad();
   void executeStore();
   void executeRegLoadAddr();
   void executeRegLoad();
   void executeRegStore();

   void executeLock();
   void executeUnlock();

   void executeJmp();
   void executeJmpPC();
   void executeCall();
   void executeCallPC();
   void executeRet();

   void executeCoreIndex();
   void executeDebug();
   void executeSetReg();
   void executeGetReg();

   void executeSetupCoreVMT();
   void executeSetupSysVMT();
   void executeSetupSysPC();
   void executeSetupSysSP();
   void executeSetupIntPC();
   void executeSetupIntSP();
   void executeSysEntry();
   void executeSysExit();
   void executeMemEnable();
   void executeMemDisable();
   void executeCacheCoreClear();
   void executeCacheSysClear();
   void executeCoreEnable();
   void executeCoreDisable();
   void executeCoreStop();

   uint64 cmdVA() const { return regs[RegPC]+cmdInd*sizeof (uint64); }

   void updatePC() { regs[RegPC] += cmdLen*sizeof (uint64) ; }

   void finError(Status status,uint64 va);
   void finError(Status status);
   void executeOp();
   void execute();
   void step1();

  public: 

   CPUCore() noexcept;

   ~CPUCore();

   void init(uint32 index,uint64 cmdCacheSize,uint64 dataCacheSize,CPUCoreBlock &block);

   void clearCache();

   void enable();

   void setupCoreVMT(uint64 pa);

   void setupSysPC(uint64 PC);

   void setupSysSP(uint64 SP);

   void setupIntPC(uint64 PC);

   void setupIntSP(uint64 SP);

   void step();
 };

/* class CPUCoreBlock */ 

class CPUCoreBlock : NoCopy
 {
   bool modeCore = false ;
   bool modeStop = false ;

   ulen shift = 0 ;

   SysMemPort mpx;
   L1Mem cache;
   AddressMap sysmap;

   SimpleArray<CPUCore> cores;

   PrintFile log;

   friend class CPUCore;

  public:

   explicit CPUCoreBlock(const char *fileName);

   ~CPUCoreBlock();

   void init(uint32 count,uint64 cmdCacheSize,uint64 dataCacheSize,SysMem &mem);

   void setModeM(bool modeM);

   void clearCache();

   void enableCores();

   void disableCores() { modeCore=false; }

   void setupCoreVMT(uint64 index,uint64 pa,uint32 exeIndex,uint64 exePC);

   void setupSysPC(uint64 index,uint64 PC,uint32 exeIndex,uint64 exePC);

   void setupSysSP(uint64 index,uint64 SP,uint32 exeIndex,uint64 exePC);

   void setupIntPC(uint64 index,uint64 PC,uint32 exeIndex,uint64 exePC);

   void setupIntSP(uint64 index,uint64 SP,uint32 exeIndex,uint64 exePC);

   void setupSysVMT(uint64 pa);

   void fatal(FatalCode code,uint32 index,uint64 PC);

   void stop() { modeStop=true; }

   bool step();
 };

} // namespace Basis    

#endif

