/* CoreDefs.cpp */
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

#include "basis/CoreDefs.h"

namespace Basis {

/* consts */

const char * GetTextDesc(Status status)
 {
  static const char *const Table[]=
   {
    "StatusDone",
    "StatusPending",
    "StatusError",

    "StatusErrorAlign",
    "StatusErrorNoX",
    "StatusErrorNoR",
    "StatusErrorNoW",

    "StatusErrorMap",
    "StatusErrorAbsent",
    "StatusErrorVoid",
    
    "StatusErrorCmd",
   };

  return Table[status];  
 }

} // namespace Basis    

