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
  StatusErrorNoX,
  StatusErrorNoR,
  StatusErrorNoW,

  StatusErrorMap,
  StatusErrorAbsent,
  StatusErrorVoid,
  
  StatusErrorCmd,
  StatusErrorROReg,
  StatusErrorSysExit,
 };

const char * GetTextDesc(Status status); 

enum FatalCode
 {
  FatalCoreIndex = 0,
  FatalROReg,
  FatalSysEntry,
  FatalSysExit,
 };

const char * GetTextDesc(FatalCode code); 

/* unit functions */

inline constexpr ulen operator "" _GByte (unsigned long long len) { return len*1024u*1024u*1024u; }

inline constexpr ulen operator "" _TByte (unsigned long long len) { return len*1024u*1024u*1024u*1024u; }

/* functions */

inline constexpr uint64 OneBit(unsigned ind)
 {
  return (uint64(1)<<ind);
 }

inline constexpr uint64 BitMask(unsigned width)
 {
  return (uint64(1)<<width)-1;
 }

inline constexpr uint64 BitField(uint64 val,unsigned shift)
 {
  return (val>>shift);
 }

inline constexpr uint64 BitField(uint64 val,unsigned shift,unsigned width)
 {
  return (val>>shift)&BitMask(width);
 }

inline constexpr uint64 BitSField(uint64 val,unsigned shift,unsigned width)
 {
  uint64 ret=(val>>shift)&BitMask(width);

  if( ret&OneBit(width-1) ) ret-=OneBit(width);

  return ret;
 }

/* classes */

struct VASplit;

struct AddressFlags;

struct Address;

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
    S=BitField(va,63);
    hpage=BitField(va,24,39);
    page=BitField(va,12,12);
    offset=BitField(va,0,12);
   }
 };

/* struct AddressFlags */ 

struct AddressFlags
 {
  uint8 R : 1 ;
  uint8 W : 1 ;
  uint8 X : 1 ;
  uint8 P : 1 ;

  AddressFlags(uint64 value=0)
   {
    R=BitField(value,11,1);
    W=BitField(value,10,1);
    X=BitField(value,9,1);
    P=BitField(value,8,1);
   }
 };

/* struct Address */

struct Address
 {
  uint64 pa;
  AddressFlags flags;
 };

/* struct HEntrySplit */ 

struct HEntrySplit
 {
  uint64 base : 40 ;
  uint32 len : 12 ;
  AddressFlags flags;
  uint8 H : 1 ;

  HEntrySplit(uint64 value=0)
   {
    base=BitField(value,24);
    len=BitField(value,12,12);
    flags=value;
    H=BitField(value,7,1);
   }
 };

/* struct PEntrySplit */

struct PEntrySplit
 {
  uint64 base : 52 ;
  AddressFlags flags;

  PEntrySplit(uint64 value=0)
   {
    base=BitField(value,12);
    flags=value;
   }
 };

} // namespace Basis    

#endif

