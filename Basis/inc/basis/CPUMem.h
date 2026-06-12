/* CPUMem.h */
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

#ifndef Basis_inc_CPUMem_h
#define Basis_inc_CPUMem_h

#include "basis/SysMem.h"

namespace Basis {

/* classes */

struct CacheLine;

class Cache;

class L1Mem;

class AddressMap;

class CPUMem;

/* struct CacheLine */

struct CacheLine
 {
  uint64 line[CacheLineLen];
  uint64 tag : 63 ; 
  uint8 full : 1 ;

  CacheLine() noexcept { full=0; }

  static uint64 Tag(uint64 pa) { return pa>>CacheLineBits; }

  void setTag(uint64 pa) { tag=Tag(pa); full=1; }

  uint64 pa() const { return tag<<CacheLineBits; }

  uint64 & operator [] (uint64 pa) { return line[(pa>>3)&7u]; } // TODO
 };

/* class Cache */

class Cache : NoCopy
 {
   struct Block
    {
     CacheLine lines[8]; 

     template <class Func1,class Func2,class Func3>
     void find(uint64 tag,Func1 match,Func2 fresh,Func3 taken);
    };

   uint32 shift = 0 ; 
   SimpleArray<Block> mem;

  public:

   Cache();

   ~Cache();

   void init(uint64 size);

   template <class Func1,class Func2,class Func3>
   void find(uint64 pa,Func1 match,Func2 fresh,Func3 taken);
 };

/* class L1Mem */

class L1Mem : NoCopy
 {
   uint64 pa = 0 ;

   enum OpCode
    {
     OpRead64,
     OpRead32,
     OpRead16,
     OpRead8,

     OpWrite64,
     OpWrite32,
     OpWrite16,
     OpWrite8,
    };

   OpCode op = OpRead64 ;

   union Arg
    {
     uint64 *ret64; 
     uint32 *ret32; 
     uint16 *ret16; 
     uint8 *ret8; 

     uint64 data64; 
     uint32 data32; 
     uint16 data16; 
     uint8 data8; 
    };

   Arg arg; 

   enum NextOp
    {
     NextFinish, 
     NextRead 
    };

   NextOp nextOp = NextFinish ; 
   CacheLine *nextLine = 0 ;

   uint32 port = 0 ;

   bool modeM = false ;

   Cache cmd;
   Cache data;

   SysMemPort *mpx = 0 ;

  private:
  
   Status match(CacheLine &line);

   Status fresh(CacheLine &line);

   Status taken(CacheLine &line);

  public:

   L1Mem();

   ~L1Mem();

   void init(uint32 port,uint64 cmdSize,uint64 dataSize,SysMemPort &mpx);

   void extmem(bool enable);

   Status fetchCommand(uint64 pa,uint64 &cmd);

   Status readData(uint64 pa,uint8 &data);
   Status readData(uint64 pa,uint16 &data);
   Status readData(uint64 pa,uint32 &data);
   Status readData(uint64 pa,uint64 &data);

   Status writeData(uint64 pa,uint8 data);
   Status writeData(uint64 pa,uint16 data);
   Status writeData(uint64 pa,uint32 data);
   Status writeData(uint64 pa,uint64 data);

   Status pending();   
 };

/* class AddressMap */

class AddressMap : NoCopy
 {
   VASplit split;
   Address *ret = 0 ;

   uint64 len = 0 ;
   bool lenDone = false ;

   uint64 hugeValue = 0 ;
   HEntrySplit hugeEntry;
   bool hugeDone = false ;

   uint64 pageValue = 0 ;
   PEntrySplit pageEntry;

   uint64 vmt = 0 ;

   L1Mem *cache = 0 ;

  private: 

   Status completePage();
   Status completeHuge();

  public:

   AddressMap();

   ~AddressMap();

   void init(L1Mem &cache);

   void setup(uint64 pa);

   Status map(uint64 va,VASplit split,Address &address);

   Status pending();
 };

/* class CPUMem */

class CPUMem : NoCopy
 {
   Address address;
   bool paDone = false ;
   bool useSysMap = false ;

   enum OpCode
    {
     OpFetch, 

     OpRead64,
     OpRead32,
     OpRead16,
     OpRead8,

     OpWrite64,
     OpWrite32,
     OpWrite16,
     OpWrite8,
    };

   OpCode op = OpFetch ;

   union Arg
    {
     uint64 *ret64; 
     uint32 *ret32; 
     uint16 *ret16; 
     uint8 *ret8; 

     uint64 data64; 
     uint32 data32; 
     uint16 data16; 
     uint8 data8; 
    };

   Arg arg; 

   bool modeDual = false ;

   L1Mem cache;
   AddressMap map;

   AddressMap *sysmap = 0 ;

  private:   

   Status mapAddress(uint64 va);

   Status fetchCommand(uint64 &cmd);

   Status readData(uint8 &data);
   Status readData(uint16 &data);
   Status readData(uint32 &data);
   Status readData(uint64 &data);

   Status writeData(uint8 data);
   Status writeData(uint16 data);
   Status writeData(uint32 data);
   Status writeData(uint64 data);

  public:
  
   CPUMem();

   ~CPUMem();

   void init(uint32 port,uint64 cmdCacheSize,uint64 dataCacheSize,SysMemPort &mpx,AddressMap &sysmap);

   void setupVMT(uint64 pa) { map.setup(pa); }

   void extmem(bool enable);
   void dualVMT(bool enable) { modeDual=enable; }

   Status fetchCommand(uint64 va,uint64 &cmd);

   Status readData(uint64 va,uint8 &data);
   Status readData(uint64 va,uint16 &data);
   Status readData(uint64 va,uint32 &data);
   Status readData(uint64 va,uint64 &data);

   Status writeData(uint64 va,uint8 data);
   Status writeData(uint64 va,uint16 data);
   Status writeData(uint64 va,uint32 data);
   Status writeData(uint64 va,uint64 data);

   Status pending();
 };

} // namespace Basis    

#endif

