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

struct MemOp;

struct CacheLine;

template <unsigned Count> class LRUTable;

class Cache;

class L1Mem;

class AddressMap;

class CPUMem;

/* struct MemOp */

struct MemOp
 {
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

    uint64 val64; 
    uint32 val32; 
    uint16 val16; 
    uint8 val8; 
   };

  Arg arg; 

  void fetchCommand(uint64 &cmd);

  void readData(uint8 &data);
  void readData(uint16 &data);
  void readData(uint32 &data);
  void readData(uint64 &data);

  void writeData(uint8 data);
  void writeData(uint16 data);
  void writeData(uint32 data);
  void writeData(uint64 data);

  template <class T,class ... SS>
  Status operator () (T &obj,SS && ... ss) const;
};

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

  uint64 & operator [] (uint64 pa) { return line[(pa>>3)&(CacheLineLen-1)]; }
 };

/* class LRUTable<unsigned Count> */

template <unsigned Count>
class LRUTable : NoCopy
 {
   unsigned table[Count];

  private: 

   void shift(unsigned pos);

  public:

   LRUTable() noexcept;

   void use(unsigned ind);

   unsigned pick();
 };

/* class Cache */

class Cache : NoCopy
 {
   static constexpr unsigned NWayBits = 3 ;
   static constexpr unsigned NWay = 1u<<NWayBits ;

   struct Block
    {
     CacheLine lines[NWay]; 
     LRUTable<NWay> table;

     template <class Func1,class Func2,class Func3>
     void find(uint64 tag,Func1 match,Func2 fresh,Func3 taken);

     void clear();
    };

   uint32 shift = 0 ; 
   SimpleArray<Block> mem;

  public:

   Cache();

   ~Cache();

   void init(uint64 size);

   void clear();

   template <class Func1,class Func2,class Func3>
   void find(uint64 pa,Func1 match,Func2 fresh,Func3 taken);
 };

/* class L1Mem */

class L1Mem : NoCopy
 {
   uint64 pa = 0 ;

   MemOp memop;

   enum NextOp
    {
     NextFinish, 
     NextRead 
    };

   NextOp nextOp = NextFinish ; 
   CacheLine *nextLine = 0 ;

   uint32 port = 0 ;

   Cache cmdCache;
   Cache dataCache;

   SysMemPort *mpx = 0 ;

  private:

   static uint32 Part32(uint64 data,uint64 pa);

   static uint16 Part16(uint64 data,uint64 pa);

   static uint8 Part8(uint64 data,uint64 pa);

   static void InsField32(uint64 &data,uint64 val,unsigned shift);

   static void InsField16(uint64 &data,uint64 val,unsigned shift);

   static void InsField8(uint64 &data,uint64 val,unsigned shift);

   static void Part32(uint64 &data,uint64 pa,uint32 val);

   static void Part16(uint64 &data,uint64 pa,uint16 val);

   static void Part8(uint64 &data,uint64 pa,uint8 val);
  
   Status fetchCommand(uint64 &cmd,CacheLine &line);

   Status readData(uint8 &data,CacheLine &line);
   Status readData(uint16 &data,CacheLine &line);
   Status readData(uint32 &data,CacheLine &line);
   Status readData(uint64 &data,CacheLine &line);

   Status writeData(uint8 data,CacheLine &line);
   Status writeData(uint16 data,CacheLine &line);
   Status writeData(uint32 data,CacheLine &line);
   Status writeData(uint64 data,CacheLine &line);

   friend struct MemOp;

   Status match(CacheLine &line);

   Status fresh(CacheLine &line);

   Status taken(CacheLine &line);

  public:

   L1Mem();

   ~L1Mem();

   void init(uint32 port,uint64 cmdSize,uint64 dataSize,SysMemPort &mpx);

   void clearCache();

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
   Status begHuge();

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
   MemOp memop;

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

   friend struct MemOp;

  public:
  
   CPUMem();

   ~CPUMem();

   void init(uint32 port,uint64 cmdCacheSize,uint64 dataCacheSize,SysMemPort &mpx,AddressMap &sysmap);

   void clearCache();

   void setupVMT(uint64 pa) { map.setup(pa); }

   void setModeDual(bool enable) { modeDual=enable; }

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

