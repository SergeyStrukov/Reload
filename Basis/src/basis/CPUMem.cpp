/* CPUMem.cpp */
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

#include "basis/CPUMem.h"

#include <CCore/inc/Exception.h>

namespace Basis {

/* struct MemOp */

void MemOp::fetchCommand(uint64 &cmd)
 {
  op=OpFetch;
  arg.ret64=&cmd;
 }

void MemOp::readData(uint8 &data)
 {
  op=OpRead8;
  arg.ret8=&data;
 }

void MemOp::readData(uint16 &data)
 {
  op=OpRead16;
  arg.ret16=&data;
 }

void MemOp::readData(uint32 &data)
 {
  op=OpRead32;
  arg.ret32=&data;
 }

void MemOp::readData(uint64 &data)
 {
  op=OpRead64;
  arg.ret64=&data;
 }

void MemOp::writeData(uint8 data)
 {
  op=OpWrite8;
  arg.val8=data;
 }

void MemOp::writeData(uint16 data)
 {
  op=OpWrite16;
  arg.val16=data;
 }

void MemOp::writeData(uint32 data)
 {
  op=OpWrite32;
  arg.val32=data;
 }

void MemOp::writeData(uint64 data)
 {
  op=OpWrite64;
  arg.val64=data;
 }

template <class T,class ... SS>
Status MemOp::operator () (T &obj,SS && ... ss) const
 {
  switch( op )
    {
     case OpFetch : return obj.fetchCommand(*arg.ret64, std::forward<SS>(ss)... );

     case OpRead8 : return obj.readData(*arg.ret8, std::forward<SS>(ss)... );
     case OpRead16 : return obj.readData(*arg.ret16, std::forward<SS>(ss)... );
     case OpRead32 : return obj.readData(*arg.ret32, std::forward<SS>(ss)... );
     case OpRead64 : return obj.readData(*arg.ret64, std::forward<SS>(ss)... );

     case OpWrite8 : return obj.writeData(arg.val8, std::forward<SS>(ss)... );
     case OpWrite16 : return obj.writeData(arg.val16, std::forward<SS>(ss)... );
     case OpWrite32 : return obj.writeData(arg.val32, std::forward<SS>(ss)... );
     case OpWrite64 : return obj.writeData(arg.val64, std::forward<SS>(ss)... );
    }

  return StatusError;  
 }

/* class LRUTable<unsigned Count> */

template <unsigned Count>
void LRUTable<Count>::shift(unsigned pos)
 {
  if( pos==0 ) return;

  unsigned ind=table[pos];

  for(unsigned i=pos; i>0 ;i--) table[i]=table[i-1];

  table[0]=ind;
 }

template <unsigned Count>
LRUTable<Count>::LRUTable() noexcept
 {
  for(unsigned pos=0; pos<Count ;pos++) table[pos]=pos;
 }

template <unsigned Count>
void LRUTable<Count>::use(unsigned ind)
 {
  for(unsigned pos=0; pos<Count ;pos++)
    if( table[pos]==ind )
      {
       shift(pos); 

       return;
      }
 }

template <unsigned Count>
unsigned LRUTable<Count>::pick()
 {
  unsigned ret=table[Count-1];

  shift(Count-1);

  return ret;
 }

/* class Cache */

template <class Func1,class Func2,class Func3>
void Cache::Block::find(uint64 tag,Func1 match,Func2 fresh,Func3 taken)
 {
  for(unsigned i=0; i<NWay ;i++)
    {
     CacheLine &line=lines[i];

     if( line.used && line.tag==tag )
       {
        match(line);

        table.use(i);

        return; 
       }
    }

  for(unsigned i=0; i<NWay ;i++)
    {
     CacheLine &line=lines[i];
     
     if( !line.used )
       {
        fresh(line);

        table.use(i);

        return; 
       }
    }

  taken(lines[table.pick()]);
 }

void Cache::Block::clear()
 {
  for(CacheLine &line : lines ) line.used=0;
 }

Cache::Cache()
 {
 }

Cache::~Cache()
 {
 }

void Cache::init(uint64 size)
 {
  if( !size )
    {
     Printf(Exception,"Basis::Cache::init(#;) : null size",size);
    }

  unsigned nbit=UIntBitFunc<uint64>::BitsOf(size)-1;

  if( nbit<=CacheLineBits+NWayBits )
    {
     Printf(Exception,"Basis::Cache::init(#;) : too small size",size);
    }

  shift=(64+CacheLineBits+NWayBits)-nbit;

  mem=SimpleArray<Block>( uint64(1)<<(64-shift) );
 }

void Cache::clear()
 {
  for(Block &obj : mem ) obj.clear();
 }

template <class Func1,class Func2,class Func3>
void Cache::find(uint64 pa,Func1 match,Func2 fresh,Func3 taken)
 {
  uint64 index=pa>>shift;
  uint64 tag=CacheLine::Tag(pa);

  mem[index].find(tag,match,fresh,taken);
 }

/* class L1Mem */

Status L1Mem::fetchCommand(uint64 &cmd,CacheLine &line)
 {
  cmd=line[pa]; 
  
  return StatusDone;
 }

Status L1Mem::readData(uint8 &data,CacheLine &line)
 {
  data=GetPart8(line[pa],pa&7u); 
  
  return StatusDone;
 }

Status L1Mem::readData(uint16 &data,CacheLine &line)
 {
  data=GetPart16(line[pa],pa&3u); 
  
  return StatusDone;
 }

Status L1Mem::readData(uint32 &data,CacheLine &line)
 {
  data=GetPart32(line[pa],pa&1u); 
  
  return StatusDone;
 }

Status L1Mem::readData(uint64 &data,CacheLine &line)
 {
  data=line[pa]; 
  
  return StatusDone;
 }

Status L1Mem::writeData(uint8 data,CacheLine &line)
 {
  SetPart8(line[pa],pa&7u,data); 

  line.dirty=1;
  
  return StatusDone;
 }

Status L1Mem::writeData(uint16 data,CacheLine &line)
 {
  SetPart16(line[pa],pa&3u,data); 

  line.dirty=1;
  
  return StatusDone;
 }

Status L1Mem::writeData(uint32 data,CacheLine &line)
 {
  SetPart32(line[pa],pa&1u,data); 

  line.dirty=1;
  
  return StatusDone;
 }

Status L1Mem::writeData(uint64 data,CacheLine &line)
 {
  line[pa]=data; 

  line.dirty=1;
  
  return StatusDone;
 }

Status L1Mem::match(CacheLine &line)
 {
  return memop(*this,line);
 }

Status L1Mem::fresh(CacheLine &line)
 {
  Status status;

  status=mpx->readData(port,pa&~CacheLineMask,line.line);

  if( status==StatusPending ) 
    {
     nextOp=NextFinish;
     nextLine=&line;

     return status;
    } 

  if( status ) return status; 

  line.setTag(pa);

  return match(line);
 }

Status L1Mem::taken(CacheLine &line)
 {
  if( line.dirty )
    {
     Status status=mpx->writeData(port,line.pa(),line.line);

     if( status==StatusPending ) 
       {
        nextOp=NextRead;
        nextLine=&line;

        return status;
       } 

     if( status ) return status; 
    }

  line.used=0;  

  return fresh(line);
 }

L1Mem::L1Mem()
 {
 }

L1Mem::~L1Mem()
 {
 }

void L1Mem::init(uint32 port_,uint64 cmdSize,uint64 dataSize,SysMemPort &mpx_)
 {
  pa=0;
  memop={};
  nextOp=NextFinish;
  nextLine=0;
  port=port_;

  cmdCache.init(cmdSize);
  dataCache.init(dataSize);

  mpx=&mpx_;
 }

void L1Mem::clearCache()
 {
  cmdCache.clear();
  dataCache.clear();
 } 

Status L1Mem::fetchCommand(uint64 pa_,uint64 &cmd)
 {
  if( pa_%8 ) return StatusErrorAlign;

  pa=pa_;
  memop.fetchCommand(cmd);

  Status status;

  cmdCache.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                     [&] (CacheLine &line) { status=fresh(line); } , 
                     [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::readData(uint64 pa_,uint8 &data)
 {
  pa=pa_;
  memop.readData(data);

  Status status;

  dataCache.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                      [&] (CacheLine &line) { status=fresh(line); } , 
                      [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::readData(uint64 pa_,uint16 &data)
 {
  if( pa_%2 ) return StatusErrorAlign;

  pa=pa_;
  memop.readData(data);

  Status status;

  dataCache.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                      [&] (CacheLine &line) { status=fresh(line); } , 
                      [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::readData(uint64 pa_,uint32 &data)
 {
  if( pa_%4 ) return StatusErrorAlign;

  pa=pa_;
  memop.readData(data);

  Status status;

  dataCache.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                      [&] (CacheLine &line) { status=fresh(line); } , 
                      [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::readData(uint64 pa_,uint64 &data)
 {
  if( pa_%8 ) return StatusErrorAlign;

  pa=pa_;
  memop.readData(data);

  Status status;

  dataCache.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                      [&] (CacheLine &line) { status=fresh(line); } , 
                      [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::writeData(uint64 pa_,uint8 data)
 {
  pa=pa_;
  memop.writeData(data);

  Status status;

  dataCache.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                      [&] (CacheLine &line) { status=fresh(line); } , 
                      [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::writeData(uint64 pa_,uint16 data)
 {
  if( pa_%2 ) return StatusErrorAlign;

  pa=pa_;
  memop.writeData(data);

  Status status;

  dataCache.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                      [&] (CacheLine &line) { status=fresh(line); } , 
                      [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::writeData(uint64 pa_,uint32 data)
 {
  if( pa_%4 ) return StatusErrorAlign;

  pa=pa_;
  memop.writeData(data);

  Status status;

  dataCache.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                      [&] (CacheLine &line) { status=fresh(line); } , 
                      [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::writeData(uint64 pa_,uint64 data)
 {
  if( pa_%8 ) return StatusErrorAlign;

  pa=pa_;
  memop.writeData(data);

  Status status;

  dataCache.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                      [&] (CacheLine &line) { status=fresh(line); } , 
                      [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::pending()
 {
  Status status=mpx->pending(port); 

  if( status ) return status; 

  switch( nextOp )
    {
     case NextFinish :
      {
       nextLine->setTag(pa);

       return match(*nextLine);
      }

     case NextRead : 
      {
       nextLine->used=0;  

       return fresh(*nextLine);
      }

     default: return StatusError; 
    }
 }

/* class AddressMap */

Status AddressMap::completePage()
 {
  pageEntry=pageValue;

  ret->pa=(pageEntry.base<<12)|split.offset; 
  ret->flags=pageEntry.flags;

  return StatusDone;
 }

Status AddressMap::completeHuge()
 {
  if( hugeEntry.H )
    {
     ret->pa=(hugeEntry.base<<24)|(split.page<<12)|split.offset; 
     ret->flags=hugeEntry.flags;

     return StatusDone;
    }
  else  
    {
     if( split.page>=hugeEntry.len ) return StatusErrorMap;

     uint64 addr=vmt+(hugeEntry.base+split.page)*sizeof (uint64);

     Status status=cache->fetchCommand(addr,pageValue); 

     if( status ) return status;

     return completePage();
    }
 }

Status AddressMap::begHuge()
 {
  if( split.hpage>=len ) return StatusErrorMap; 

  uint64 addr=vmt+(split.hpage+1)*sizeof (uint64);

  hugeDone=false;

  Status status=cache->fetchCommand(addr,hugeValue); 

  if( status ) return status;

  hugeDone=true;
  hugeEntry=hugeValue;

  return completeHuge();
 }

AddressMap::AddressMap()
 {
 }

AddressMap::~AddressMap()
 {
 }

void AddressMap::init(L1Mem &cache_)
 {
  split=0;
  ret=0;
  len=0;
  lenDone=false;
  hugeValue=0;
  hugeEntry=0;
  hugeDone=false;
  pageValue=0;
  pageEntry=0;

  vmt=0;
  
  cache=&cache_;
 }

void AddressMap::setup(uint64 pa)
 {
  vmt=pa;
  lenDone=false;  
 }

Status AddressMap::map(uint64 va,VASplit split_,Address &address)
 {
  if( !vmt ) 
    {
     address.pa=va;
     address.flags.R=1;
     address.flags.W=1;
     address.flags.X=1;
     address.flags.P=1;

     return StatusDone; 
    }

  split=split_;  
  ret=&address;

  if( !lenDone )
    {
     Status status=cache->fetchCommand(vmt,len); 

     if( status ) return status;

     lenDone=true;
    }  

  return begHuge();
 }

Status AddressMap::pending()
 {
  Status status=cache->pending(); 

  if( status ) return status;

  if( !lenDone )
    {
     lenDone=true;

     return begHuge();     
    }

  if( !hugeDone )
    {
     hugeDone=true;
     hugeEntry=hugeValue;

     return completeHuge();
    }  

  return completePage();
 }

/* class CPUMem */

Status CPUMem::mapAddress(uint64 va)
 {
  VASplit split(va);

  if( modeDual )  
    {
     if( split.S )
       {
        useSysMap=true;
        
        return sysmap->map(va,split,address);
       }
     else
       {
        useSysMap=false;

        return map.map(va,split,address);
       }
    }
  else
    {
     if( split.S ) return StatusErrorMap;  

     useSysMap=false;

     return map.map(va,split,address);
    }
 }

Status CPUMem::fetchCommand(uint64 &cmd)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.X ) return StatusErrorNoX;

  return cache.fetchCommand(address.pa,cmd);
 }

Status CPUMem::readData(uint8 &data)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.R ) return StatusErrorNoR;

  return cache.readData(address.pa,data);
 }

Status CPUMem::readData(uint16 &data)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.R ) return StatusErrorNoR;

  return cache.readData(address.pa,data);
 }

Status CPUMem::readData(uint32 &data)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.R ) return StatusErrorNoR;

  return cache.readData(address.pa,data);
 }

Status CPUMem::readData(uint64 &data)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.R ) return StatusErrorNoR;

  return cache.readData(address.pa,data);
 }

Status CPUMem::writeData(uint8 data)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.W ) return StatusErrorNoW;

  return cache.writeData(address.pa,data);
 }

Status CPUMem::writeData(uint16 data)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.W ) return StatusErrorNoW;

  return cache.writeData(address.pa,data);
 }

Status CPUMem::writeData(uint32 data)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.W ) return StatusErrorNoW;

  return cache.writeData(address.pa,data);
 }

Status CPUMem::writeData(uint64 data)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.W ) return StatusErrorNoW;

  return cache.writeData(address.pa,data);
 }

CPUMem::CPUMem()
 {
 }

CPUMem::~CPUMem()
 {
 }

void CPUMem::init(uint32 port,uint64 cmdCacheSize,uint64 dataCacheSize,SysMemPort &mpx,AddressMap &sysmap_)
 {
  address={};  
  paDone=false;
  useSysMap=false;
  memop={};

  modeDual=false;

  cache.init(port,cmdCacheSize,dataCacheSize,mpx);
  map.init(cache);

  sysmap=&sysmap_;
 }

Status CPUMem::fetchCommand(uint64 va,uint64 &cmd)
 {
  paDone=false;

  Status status=mapAddress(va);  

  if( status ) 
    {
     if( status==StatusPending ) memop.fetchCommand(cmd);

     return status;
    }    

  paDone=true;

  return fetchCommand(cmd);
 }

Status CPUMem::readData(uint64 va,uint8 &data)
 {
  paDone=false;

  Status status=mapAddress(va);  

  if( status ) 
    {
     if( status==StatusPending ) memop.readData(data);

     return status;
    }    

  paDone=true;

  return readData(data);
 }

Status CPUMem::readData(uint64 va,uint16 &data)
 {
  paDone=false;

  Status status=mapAddress(va);  

  if( status ) 
    {
     if( status==StatusPending ) memop.readData(data);

     return status;
    }    

  paDone=true;

  return readData(data);
 }

Status CPUMem::readData(uint64 va,uint32 &data)
 {
  paDone=false;

  Status status=mapAddress(va);  

  if( status ) 
    {
     if( status==StatusPending ) memop.readData(data);

     return status;
    }    

  paDone=true;

  return readData(data);
 }

Status CPUMem::readData(uint64 va,uint64 &data)
 {
  paDone=false;

  Status status=mapAddress(va);  

  if( status ) 
    {
     if( status==StatusPending ) memop.readData(data);

     return status;
    }    

  paDone=true;

  return readData(data);
 }

Status CPUMem::writeData(uint64 va,uint8 data)
 {
  paDone=false;

  Status status=mapAddress(va);  

  if( status ) 
    {
     if( status==StatusPending ) memop.writeData(data);

     return status;
    }    

  paDone=true;

  return writeData(data);
 }

Status CPUMem::writeData(uint64 va,uint16 data)
 {
  paDone=false;

  Status status=mapAddress(va);  

  if( status ) 
    {
     if( status==StatusPending ) memop.writeData(data);

     return status;
    }    

  paDone=true;

  return writeData(data);
 }

Status CPUMem::writeData(uint64 va,uint32 data)
 {
  paDone=false;

  Status status=mapAddress(va);  

  if( status ) 
    {
     if( status==StatusPending ) memop.writeData(data);

     return status;
    }    

  paDone=true;

  return writeData(data);
 }

Status CPUMem::writeData(uint64 va,uint64 data)
 {
  paDone=false;

  Status status=mapAddress(va);  

  if( status ) 
    {
     if( status==StatusPending ) memop.writeData(data);

     return status;
    }    

  paDone=true;

  return writeData(data);
 }

Status CPUMem::pending()
 {
  if( paDone )  
    {
     return cache.pending();   
    }
  else
    {
     Status status;

     if( useSysMap )   
       {
        status=sysmap->pending();
       }
     else
       {
        status=map.pending();
       }  

     if( status ) return status;        

     paDone=true;

     return memop(*this);
    }
 }

} // namespace Basis    

