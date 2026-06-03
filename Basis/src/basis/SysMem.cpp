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

SysMem::SysMem()
 {
 }

SysMem::~SysMem()
 {
 }

void SysMem::init(uint64 ramSize)
 {
  if( ramSize%sizeof (uint64) )
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

Status SysMem::readData(uint64 pa,uint64 &data)
 {
  if( pa%sizeof (uint64) ) return StatusErrorAlign;

  if( pa>=BootROMAddress && pa<BootROMAddress+BootROMSize )
    {
     data=rom[(pa-BootROMAddress)/sizeof (uint64)]; 

     return StatusDone;
    }

  if( pa>=RAMAddress && pa<RAMAddress+ram.getLen()*sizeof (uint64) )
    {
     data=ram[(pa-RAMAddress)/sizeof (uint64)]; 

     return StatusDone;
    }

  return StatusErrorVoid;  
 }

Status SysMem::writeData(uint64 pa,uint64 data)
 {
  if( pa%sizeof (uint64) ) return StatusErrorAlign;

  if( pa>=RAMAddress && pa<RAMAddress+ram.getLen()*sizeof (uint64) )
    {
     ram[(pa-RAMAddress)/sizeof (uint64)]=data; 

     return StatusDone;
    }

  return StatusErrorVoid;  
 }

/* class SysMemPort */ 

SysMemPort::SysMemPort()
 {
 }

SysMemPort::~SysMemPort()
 {
 }

void SysMemPort::init(uint32 count,SysMem &mem_)
 {
  ports=SimpleArray<Port>(count);

  mem=&mem_;
 }

Status SysMemPort::readData(uint32 port,uint64 pa,uint64 &data)
 {
 }

Status SysMemPort::writeData(uint32 port,uint64 pa,uint64 data)
 {
 }

Status SysMemPort::pending(uint32 port)
 {
 }

void SysMemPort::step()
 {
 }

} // namespace Basis    

