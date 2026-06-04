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

class L1Mem;

class AddressMap;

class CPUMem;

/* class L1Mem */

class L1Mem : NoCopy
 {
   uint32 port;

   bool modeM;

   SysMemPort *mpx;

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
   uint64 *ret = 0 ;

   uint64 len = 0 ;
   bool lenDone = false ;

   uint64 hugeValue = 0 ;
   HEntrySplit hugeEntry;
   bool hugeDone = false ;

   uint64 pageValue = 0 ;
   PEntrySplit pageEntry;

   uint64 vmt = 0 ;

   L1Mem *cache;

  private: 

   Status completePage();
   Status completeHuge();

  public:

   AddressMap();

   ~AddressMap();

   void init(L1Mem &cache);

   void setup(uint64 pa);

   Status map(uint64 va,VASplit split,uint64 &pa);

   Status pending();
 };

/* class CPUMem */

class CPUMem : NoCopy
 {
   uint64 pa = 0 ;
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

