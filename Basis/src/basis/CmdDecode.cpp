/* CmdDecode.cpp */
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

#include "basis/CmdDecode.h"

namespace Basis {

/* consts */    

static_assert( CmdBinBase-CmdUnBase >= CmdUnopCount );
static_assert( CmdOtherBase-CmdBinBase >= CmdBinopCount );

/* struct Cmd */ 
    
uint32 Cmd::decode(uint64 cmd)
 {
  opcode=CmdUndef;  

  // TODO

  return 1;
 }

} // namespace Basis    

