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

AddressMap::AddressMap()
 {
 }

AddressMap::~AddressMap()
 {
 }

void AddressMap::init(L1Mem &cache_)
 {
  vmt=0;
  
  cache=&cache_;
 }

void AddressMap::setup(uint64 pa)
 {
  vmt=pa;  
 }

Status AddressMap::map(uint64 va,VASplit split,uint64 &pa)
 {
  if( !vmt ) 
    {
     pa=va;

     return StatusDone; 
    }

  // TODO
 }

Status AddressMap::pending()
 {
  return StatusDone;
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
        return sysmap->map(va,split,pa);
       }
     else
       {
        useSysMap=false;
        return map.map(va,split,pa);
       }
    }
  else
    {
     if( split.S ) return StatusErrorVoid;  

     useSysMap=false;
     return map.map(va,split,pa);
    }
 }

CPUMem::CPUMem()
 {
 }

CPUMem::~CPUMem()
 {
 }

void CPUMem::init(uint32 port,uint64 cmdCacheSize,uint64 dataCacheSize,SysMemPort &mpx,AddressMap &sysmap_)
 {
  pa=0;  
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

  return cache.fetchCommand(pa,cmd);
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

  return cache.readData(pa,data);
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

  return cache.readData(pa,data);
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

  return cache.readData(pa,data);
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

  return cache.readData(pa,data);
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

  return cache.writeData(pa,data);
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

  return cache.writeData(pa,data);
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

  return cache.writeData(pa,data);
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

  return cache.writeData(pa,data);
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
        case OpFetch : return cache.fetchCommand(pa,*arg.ret64);

        case OpRead8 : return cache.readData(pa,*arg.ret8);
        case OpRead16 : return cache.readData(pa,*arg.ret16);
        case OpRead32 : return cache.readData(pa,*arg.ret32);
        case OpRead64 : return cache.readData(pa,*arg.ret64);

        case OpWrite8 : return cache.writeData(pa,arg.data8);
        case OpWrite16 : return cache.writeData(pa,arg.data16);
        case OpWrite32 : return cache.writeData(pa,arg.data32);
        case OpWrite64 : return cache.writeData(pa,arg.data64);
       }

     return StatusError;  
    }
 }

} // namespace Basis    

