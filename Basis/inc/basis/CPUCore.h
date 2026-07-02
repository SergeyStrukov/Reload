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

/* classes */

struct FlagBits;

class CPUCore;

class CPUCoreBlock;

/* struct FlagBits */

struct FlagBits
 {
  uint64 bits;

  uint8 bitZ() const { return bits&1u; }
  uint8 bitC() const { return bits&2u; }
  uint8 bitN() const { return bits&4u; }
  uint8 bitO() const { return bits&8u; }

  bool isCnZ() const { return (bits&3u)==2 ; }

  bool isNisO() const { uint8 m=bits&12u; return m==0 || m==12 ; }

  bool isZNisO() const { uint8 m=bits&13u; return m==1 || m==13 ; }
 };

/* class CPUCore */

class CPUCore : NoCopy
 {
   uint64 cmd[3]{};
   uint32 cmdInd = 0 ;
   uint32 cmdLen = 0 ;
   Cmd command;

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

   FlagBits condFlags() const { return {regs[RegFlags]>>(command.flag*4)}; }

   bool testCond();

   void executeCast();
   void executeNeg();
   void executeNot();

   void executeAdd();
   void executeSub();
   void executeMul();
   void executeDiv();
   void executeRem();

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

   void updatePC() { regs[RegPC] += cmdLen*sizeof (uint64) ; }

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

   friend class CPUCore;

  public:

   CPUCoreBlock();

   ~CPUCoreBlock();

   void init(uint32 count,uint64 cmdCacheSize,uint64 dataCacheSize,SysMem &mem);

   void setModeM(bool modeM);

   void clearCache();

   void enableCores();

   void disableCores() { modeCore=false; }

   void setupCoreVMT(uint64 index,uint64 pa);

   void setupSysPC(uint64 index,uint64 PC);

   void setupSysSP(uint64 index,uint64 SP);

   void setupIntPC(uint64 index,uint64 PC);

   void setupIntSP(uint64 index,uint64 SP);

   void setupSysVMT(uint64 pa);

   void stop() { modeStop=true; }

   bool step();
 };

} // namespace Basis    

#endif

