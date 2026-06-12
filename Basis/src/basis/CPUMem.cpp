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

namespace Basis {

/* class Cache */

template <class Func1,class Func2,class Func3>
void Cache::Block::find(uint64 tag,Func1 match,Func2 fresh,Func3 taken)
 {
  for(CacheLine &line : lines )
    if( line.full && line.tag==tag )
      {
       match(line);

       return; 
      }

  for(CacheLine &line : lines )
    if( !line.full )
      {
       fresh(line);

       return; 
      }

  // TODO LRU    

  taken(lines[0]);
 }

void Cache::Block::clear()
 {
  for(CacheLine &line : lines ) line.full=0;
 }

Cache::Cache()
 {
 }

Cache::~Cache()
 {
 }

void Cache::init(uint64 size) // TODO
 {
  // size>= 1024 

  unsigned nbit=UIntBitFunc<uint64>::BitsOf(size)-1;

  // nbit >= 10

  //
  // (2^(64-shift))*8*64 <= size
  //
  // (64-shift)+3+6 <= nbit
  //
  // shift >= 73-nbit , >= 6 , < 64
  //

  shift=73-nbit; // 73-64 < nbit <= 73-6
                 // 10 <= nbit <= 67

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

uint32 L1Mem::Part32(uint64 data,uint64 pa)
 {
  uint64 shift=(pa&1u)*32;

  return uint32( data>>shift );
 }

uint16 L1Mem::Part16(uint64 data,uint64 pa)
 {
  uint64 shift=(pa&3u)*16;

  return uint16( data>>shift );
 }

uint8 L1Mem::Part8(uint64 data,uint64 pa)
 {
  uint64 shift=(pa&7u)*8;

  return uint8( data>>shift );
 }

void L1Mem::InsField32(uint64 &data,uint64 val,unsigned shift)
 {
  uint64 mask=(uint64(1)<<32)-1;

  data &= ~(mask<<shift) ;
  data |=  (val<<shift) ;
 }

void L1Mem::InsField16(uint64 &data,uint64 val,unsigned shift)
 {
  uint64 mask=(uint64(1)<<16)-1;

  data &= ~(mask<<shift) ;
  data |=  (val<<shift) ;
 }

void L1Mem::InsField8(uint64 &data,uint64 val,unsigned shift)
 {
  uint64 mask=(uint64(1)<<8)-1;

  data &= ~(mask<<shift) ;
  data |=  (val<<shift) ;
 }

void L1Mem::Part32(uint64 &data,uint64 pa,uint32 val)
 {
  uint64 shift=(pa&1u)*32;

  InsField32(data,val,shift); 
 }

void L1Mem::Part16(uint64 &data,uint64 pa,uint16 val)
 {
  uint64 shift=(pa&3u)*16;

  InsField16(data,val,shift); 
 }

void L1Mem::Part8(uint64 &data,uint64 pa,uint8 val)
 {
  uint64 shift=(pa&7u)*8;

  InsField8(data,val,shift); 
 }

Status L1Mem::match(CacheLine &line)
 {
  switch( op )
    {
     case OpRead64 : *arg.ret64=line[pa]; return StatusDone;

     case OpRead32 : *arg.ret32=Part32(line[pa],pa); return StatusDone;
     case OpRead16 : *arg.ret16=Part16(line[pa],pa); return StatusDone;
     case OpRead8 : *arg.ret8=Part8(line[pa],pa); return StatusDone;

     case OpWrite64 : line[pa]=arg.data64; return StatusDone;

     case OpWrite32 : Part32(line[pa],pa,arg.data32); return StatusDone;
     case OpWrite16 : Part16(line[pa],pa,arg.data16); return StatusDone;
     case OpWrite8 : Part8(line[pa],pa,arg.data8); return StatusDone;

     default: return StatusError;
    }
 }

Status L1Mem::fresh(CacheLine &line)
 {
  Status status;

  if( modeM )
    {
     status=mpx->readData(port,pa&CacheLineMask,line.line);
    }
  else
    {
     Range(line.line).set_null(); 

     status=StatusDone;
    }  

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
  if( !modeM ) return StatusErrorVoid;

  Status status=mpx->writeData(port,line.pa(),line.line);

  if( status==StatusPending ) 
    {
     nextOp=NextRead;
     nextLine=&line;

     return status;
    } 

  if( status ) return status; 

  line.full=0;  

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
  op=OpRead64;
  nextOp=NextFinish;
  nextLine=0;
  port=port_;
  modeM=false;

  cmd.init(cmdSize);
  data.init(dataSize);

  mpx=&mpx_;
 }

void L1Mem::extmem(bool enable)
 {
  modeM=enable;

  cmd.clear();
  data.clear();
 }

Status L1Mem::fetchCommand(uint64 pa_,uint64 &ret)
 {
  if( pa_%8 ) return StatusErrorAlign;

  pa=pa_;
  op=OpRead64;
  arg.ret64=&ret;

  Status status;

  cmd.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                [&] (CacheLine &line) { status=fresh(line); } , 
                [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::readData(uint64 pa_,uint8 &ret)
 {
  pa=pa_;
  op=OpRead8;
  arg.ret8=&ret;

  Status status;

  data.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                 [&] (CacheLine &line) { status=fresh(line); } , 
                 [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::readData(uint64 pa_,uint16 &ret)
 {
  if( pa_%2 ) return StatusErrorAlign;

  pa=pa_;
  op=OpRead16;
  arg.ret16=&ret;

  Status status;

  data.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                 [&] (CacheLine &line) { status=fresh(line); } , 
                 [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::readData(uint64 pa_,uint32 &ret)
 {
  if( pa_%4 ) return StatusErrorAlign;

  pa=pa_;
  op=OpRead32;
  arg.ret32=&ret;

  Status status;

  data.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                 [&] (CacheLine &line) { status=fresh(line); } , 
                 [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::readData(uint64 pa_,uint64 &ret)
 {
  if( pa_%8 ) return StatusErrorAlign;

  pa=pa_;
  op=OpRead64;
  arg.ret64=&ret;

  Status status;

  data.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                 [&] (CacheLine &line) { status=fresh(line); } , 
                 [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::writeData(uint64 pa_,uint8 val)
 {
  pa=pa_;
  op=OpWrite8;
  arg.data8=val;

  Status status;

  data.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                 [&] (CacheLine &line) { status=fresh(line); } , 
                 [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::writeData(uint64 pa_,uint16 val)
 {
  if( pa_%2 ) return StatusErrorAlign;

  pa=pa_;
  op=OpWrite16;
  arg.data16=val;

  Status status;

  data.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                 [&] (CacheLine &line) { status=fresh(line); } , 
                 [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::writeData(uint64 pa_,uint32 val)
 {
  if( pa_%4 ) return StatusErrorAlign;

  pa=pa_;
  op=OpWrite32;
  arg.data32=val;

  Status status;

  data.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                 [&] (CacheLine &line) { status=fresh(line); } , 
                 [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::writeData(uint64 pa_,uint64 val)
 {
  if( pa_%8 ) return StatusErrorAlign;

  pa=pa_;
  op=OpWrite64;
  arg.data64=val;

  Status status;

  data.find(pa_, [&] (CacheLine &line) { status=match(line); } , 
                 [&] (CacheLine &line) { status=fresh(line); } , 
                 [&] (CacheLine &line) { status=taken(line); } );

  return status;             
 }

Status L1Mem::pending()
 {
  switch( nextOp )
    {
     case NextFinish :
      {
       Status status=mpx->pending(port); 

       if( status ) return status; 

       nextLine->setTag(pa);

       return match(*nextLine);
      }

     case NextRead : 
      {
       Status status=mpx->pending(port); 

       if( status ) return status; 

       nextLine->full=0;  

       return fresh(*nextLine);
      }

     default: return StatusError; 
    }
 }

/* class AddressMap */

Status AddressMap::completePage()
 {
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

     pageEntry=pageValue;

     return completePage();
    }
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

  if( split.hpage>=len ) return StatusErrorMap; 

  uint64 addr=vmt+split.hpage*sizeof (uint64);

  hugeDone=false;

  {
   Status status=cache->fetchCommand(addr,hugeValue); 

   if( status ) return status;

   hugeDone=true;
   hugeEntry=hugeValue;
  }  

  return completeHuge();
 }

Status AddressMap::pending()
 {
  if( !lenDone )
    {
     Status status=cache->pending(); 

     if( status ) return status;

     lenDone=true;
    }

  if( !hugeDone )
    {
     Status status=cache->pending(); 

     if( status ) return status;

     hugeDone=true;
     hugeEntry=hugeValue;

     return completeHuge();
    }  

  pageEntry=pageValue;

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

  if( !address.flags.R ) return StatusErrorNoW;

  return cache.writeData(address.pa,data);
 }

Status CPUMem::writeData(uint16 data)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.R ) return StatusErrorNoW;

  return cache.writeData(address.pa,data);
 }

Status CPUMem::writeData(uint32 data)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.R ) return StatusErrorNoW;

  return cache.writeData(address.pa,data);
 }

Status CPUMem::writeData(uint64 data)
 {
  if( !address.flags.P ) return StatusErrorAbsent;

  if( !address.flags.R ) return StatusErrorNoW;

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
  address={0};  
  paDone=false;
  useSysMap=false;
  op=OpFetch;

  modeDual=false;

  cache.init(port,cmdCacheSize,dataCacheSize,mpx);
  map.init(cache);

  sysmap=&sysmap_;
 }

void CPUMem::extmem(bool enable)
 {
  cache.extmem(enable);  
 }

Status CPUMem::fetchCommand(uint64 va,uint64 &cmd)
 {
  paDone=false;

  Status status=mapAddress(va);  

  if( status ) 
    {
     if( status==StatusPending )
       {
        op=OpFetch;
        arg.ret64=&cmd;
       }   

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
     if( status==StatusPending )
       {
        op=OpRead8;
        arg.ret8=&data;
       }   

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
     if( status==StatusPending )
       {
        op=OpRead16;
        arg.ret16=&data;
       }   

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
     if( status==StatusPending )
       {
        op=OpRead32;
        arg.ret32=&data;
       }   

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
     if( status==StatusPending )
       {
        op=OpRead64;
        arg.ret64=&data;
       }   

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
     if( status==StatusPending )
       {
        op=OpWrite8;
        arg.data8=data;
       }   

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
     if( status==StatusPending )
       {
        op=OpWrite16;
        arg.data16=data;
       }   

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
     if( status==StatusPending )
       {
        op=OpWrite32;
        arg.data32=data;
       }   

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
     if( status==StatusPending )
       {
        op=OpWrite64;
        arg.data64=data;
       }   

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

     switch( op )
       {
        case OpFetch : return fetchCommand(*arg.ret64);

        case OpRead8 : return readData(*arg.ret8);
        case OpRead16 : return readData(*arg.ret16);
        case OpRead32 : return readData(*arg.ret32);
        case OpRead64 : return readData(*arg.ret64);

        case OpWrite8 : return writeData(arg.data8);
        case OpWrite16 : return writeData(arg.data16);
        case OpWrite32 : return writeData(arg.data32);
        case OpWrite64 : return writeData(arg.data64);
       }

     return StatusError;  
    }
 }

} // namespace Basis    

