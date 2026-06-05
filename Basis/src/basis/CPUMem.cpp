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

