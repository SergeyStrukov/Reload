/* SysMem.cpp */
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

#include "basis/SysMem.h"

#include <CCore/inc/Exception.h>

namespace Basis {

/* class SysMem */

static_assert( (BootROMSize%CacheLineSize)==0 );

static_assert( CacheLineLen>0 );

SysMem::SysMem()
 {
 }

SysMem::~SysMem()
 {
 }

void SysMem::init(uint64 ramSize)
 {
  if( ramSize%CacheLineSize )
    {
     Printf(Exception,"Basis::SysMem::init(#;) : unaligned",ramSize);
    }

  if( ramSize>MaxRAMSize )
    {
     Printf(Exception,"Basis::SysMem::init(#;) : too big",ramSize);
    }

  rom=SimpleArray<uint64>(BootROMLen);  
  ram=SimpleArray<uint64>(ramSize/sizeof (uint64));
 }

Status SysMem::readData(uint64 pa,uint64 line[CacheLineLen])
 {
  if( pa%sizeof (uint64) ) return StatusErrorAlign;

  if( pa>=BootROMAddress && pa<BootROMAddress+BootROMSize )
    {
     RangeCopy(line,rom.getPtr()+(pa-BootROMAddress)/sizeof (uint64),CacheLineLen);

     return StatusDone;
    }

  if( pa>=RAMAddress && pa<RAMAddress+ram.getLen()*sizeof (uint64) )
    {
     RangeCopy(line,ram.getPtr()+(pa-RAMAddress)/sizeof (uint64),CacheLineLen);

     return StatusDone;
    }

  return StatusErrorVoid;  
 }

Status SysMem::writeData(uint64 pa,const uint64 line[CacheLineLen])
 {
  if( pa%sizeof (uint64) ) return StatusErrorAlign;

  if( pa>=RAMAddress && pa<RAMAddress+ram.getLen()*sizeof (uint64) )
    {
     RangeCopy(ram.getPtr()+(pa-RAMAddress)/sizeof (uint64),line,CacheLineLen);

     return StatusDone;
    }

  return StatusErrorVoid;  
 }

/* class SysMemPort */ 

Status SysMemPort::Port::operator () (SysMemPort &obj) const
 {
  if( obj.useBank(pa) ) 
    {
     if( ret ) 
       {
        return obj.mem->readData(pa,ret); 
       }
     else
       {
        return obj.mem->writeData(pa,data); 
       }
    }
  else
    {
     return StatusPending; 
    }
 }

bool SysMemPort::useBank(uint64 pa)
 {
  uint64 index=pa/MemBankSize;

  for(ulen i=0; i<nbanks ;i++)
    if( banks[i]==index )
      return false; 

  banks[nbanks++]=index;    

  return true;    
 }

SysMemPort::SysMemPort()
 {
 }

SysMemPort::~SysMemPort()
 {
 }

void SysMemPort::init(uint32 count,SysMem &mem_)
 {
  ports=SimpleArray<Port>(count);
  banks=SimpleArray<uint64>(count);
  nbanks=0;

  mem=&mem_;
 }

Status SysMemPort::readData(uint32 port,uint64 pa,uint64 line[CacheLineLen])
 {
  if( useBank(pa) ) 
    {
     return mem->readData(pa,line); 
    }
  else
    {
     ports[port].read(pa,line);

     return StatusPending; 
    }
 }

Status SysMemPort::writeData(uint32 port,uint64 pa,const uint64 line[CacheLineLen])
 {
  if( useBank(pa) ) 
    {
     return mem->writeData(pa,line); 
    }
  else
    {
     ports[port].write(pa,line);

     return StatusPending; 
    }
 }

Status SysMemPort::pending(uint32 port)
 {
  return ports[port](*this);
 }

} // namespace Basis    

