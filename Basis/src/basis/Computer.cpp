/* Computer.cpp */
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

#include "basis/Computer.h"

#include <CCore/inc/Exception.h>

namespace Basis {

/* class Computer */

Computer::Computer(const ComputerCfg &cfg)
 {
  if( cfg.coreCount==0 )
    {
     Printf(Exception,"Basis::Computer::init(...) : no cores");
    }

  if( cfg.coreCount>MaxCores )
    {
     Printf(Exception,"Basis::Computer::init( cfg.coreCount = #; ) : too many cores",cfg.coreCount);
    }

  if( cfg.cmdCacheSize<MinCacheSize )  
    {
     Printf(Exception,"Basis::Computer::init( cfg.cmdCacheSize = #; ) : too small",cfg.cmdCacheSize);
    }

  if( cfg.cmdCacheSize>MaxCacheSize )  
    {
     Printf(Exception,"Basis::Computer::init( cfg.cmdCacheSize = #; ) : too big",cfg.cmdCacheSize);
    }

  if( cfg.dataCacheSize<MinCacheSize )  
    {
     Printf(Exception,"Basis::Computer::init( cfg.dataCacheSize = #; ) : too small",cfg.dataCacheSize);
    }

  if( cfg.dataCacheSize>MaxCacheSize )  
    {
     Printf(Exception,"Basis::Computer::init( cfg.dataCacheSize = #; ) : too big",cfg.dataCacheSize);
    }

  cpu.init(cfg.coreCount,cfg.cmdCacheSize,cfg.dataCacheSize,sysmem);  
  sysmem.init(cfg.ramSize);  
 }

Computer::~Computer()
 {
 }

void Computer::run()
 {
  while( cpu.step() );  
 }

} // namespace Basis    

