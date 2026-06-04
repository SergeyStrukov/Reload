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

struct VASplit;
struct HEntrySplit;
struct PEntrySplit;

/* struct VASplit */

struct VASplit
 {
  uint8 S : 1 ;
  uint64 hpage : 39 ;
  uint32 page : 12 ;
  uint32 offset : 12 ;

  VASplit(uint64 va=0)
   {
    S=va>>63;
    hpage=va>>24;
    page=va>>12;
    offset=va;
   }
 };

/* struct HEntrySplit */ 

struct HEntrySplit
 {
  uint64 base : 40 ;
  uint32 len : 12 ;
  uint8 R : 1 ;
  uint8 W : 1 ;
  uint8 X : 1 ;
  uint8 P : 1 ;
  uint8 H : 1 ;

  HEntrySplit(uint64 value=0)
   {
    base=value>>24;
    len=value>>12;
    R=value>>11;
    W=value>>10;
    X=value>>9;
    P=value>>8;
    H=value>>7;
   }
 };

/* struct PEntrySplit */

struct PEntrySplit
 {
  uint64 base : 52 ;
  bool R : 1 ;
  bool W : 1 ;
  bool X : 1 ;
  bool P : 1 ;

  PEntrySplit(uint64 value=0)
   {
    base=value>>12;
    R=value>>11;
    W=value>>10;
    X=value>>9;
    P=value>>8;
   }
 };

} // namespace Basis    

#endif

