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
    "Status Done",
    "Status Pending",
    "Status Error",

    "Status Error Align",
    "Status Error NoX",
    "Status Error NoR",
    "Status Error NoW",

    "Status Error Map",
    "Status Error Absent",
    "Status Error Void",
    
    "Status Error unknown command",
    "Status Error read-only register",
   };

  return Table[status];  
 }

const char * GetTextDesc(FatalCode code)
 {
  static const char *const Table[]=
   {
    "Bad core index",
    "Read-only register",
    "Execution command fail",
   };

  return Table[code];  
 }

} // namespace Basis    

