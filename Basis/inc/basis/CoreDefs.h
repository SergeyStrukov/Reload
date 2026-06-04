/* CoreDefs.h */
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

#ifndef Basis_inc_CoreDefs_h
#define Basis_inc_CoreDefs_h

#include "CCore/inc/Array.h"

namespace Basis {

/* using */

using namespace CCore;

/* consts */

enum Status
 {
  StatusDone = 0,  
  StatusPending,
  StatusError,

  StatusErrorAlign,  
  StatusErrorCmd,
  StatusErrorNoCmd,
  StatusErrorRO,

  StatusErrorMap,
  StatusErrorAbsent,
  StatusErrorVoid
 };

/* unit functions */

inline constexpr ulen operator "" _GByte (unsigned long long len) { return len*1024u*1024u*1024u; }

inline constexpr ulen operator "" _TByte (unsigned long long len) { return len*1024u*1024u*1024u*1024u; }

/* classes */

struct VASplit
 {
  bool S : 1 ;
  uint64 hpage : 39 ;
  uint32 page : 12 ;
  uint32 offset : 12 ;

  VASplit(uint64 va)
   {
    S=va>>63;
    hpage=va>>24;
    page=va>>12;
    offset=va;
   }
 };

} // namespace Basis    

#endif

