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

/* struct ComputerCfg */

void ComputerCfg::validate() const
 {
  if( coreCount==0 )
    {
     Printf(Exception,"Basis::ComputerCfg::validate(...) : no cores");
    }

  if( coreCount>MaxCores )
    {
     Printf(Exception,"Basis::ComputerCfg::validate( coreCount = #; ) : too many cores",coreCount);
    }

  if( cmdCacheSize<MinCacheSize )  
    {
     Printf(Exception,"Basis::ComputerCfg::validate( cmdCacheSize = #; ) : too small",cmdCacheSize);
    }

  if( cmdCacheSize>MaxCacheSize )  
    {
     Printf(Exception,"Basis::ComputerCfg::validate( cmdCacheSize = #; ) : too big",cmdCacheSize);
    }

  if( dataCacheSize<MinCacheSize )  
    {
     Printf(Exception,"Basis::ComputerCfg::validate( dataCacheSize = #; ) : too small",dataCacheSize);
    }

  if( dataCacheSize>MaxCacheSize )  
    {
     Printf(Exception,"Basis::ComputerCfg::validate( dataCacheSize = #; ) : too big",dataCacheSize);
    }
 }

/* class Computer */

Computer::Computer(const ComputerCfg &cfg)
 {
  cfg.validate();

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

