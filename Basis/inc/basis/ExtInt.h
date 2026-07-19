/* ExtInt.h */
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

#ifndef Basis_inc_ExtInt_h
#define Basis_inc_ExtInt_h

#include "basis/CoreDefs.h"

namespace Basis::Op {

/* classes */    

struct uint72;

/* struct uint72 */

struct uint72
 {
  uint64 val;  
  uint8 msb; 

  bool operator ! () const { return !(val|msb); }

  bool operator == (uint72 obj) const { return val==obj.val && msb==obj.msb ; }
  
  uint72 operator - () const
   {
    uint72 ret{0,0};

    ret-=(*this);

    return ret;
   }

  uint72 operator ~ () const
   {
    return {~val,(uint8)~msb};
   }

  uint72 & operator += (uint64 obj)
   {
    UIntFunc<uint64>::Add add(val,obj); 

    val=add.result;
    msb+=add.carry;    

    return *this;
   }

  uint72 & operator += (uint72 obj)
   {
    UIntFunc<uint64>::Add add(val,obj.val); 

    val=add.result;
    msb+=obj.msb+add.carry;    

    return *this;
   }
  
  uint72 & operator -= (uint72 obj)
   {
    UIntFunc<uint64>::Sub sub(val,obj.val); 

    val=sub.result;
    msb-=obj.msb+sub.borrow;    

    return *this;
   }

  uint72 & operator /= (uint72 obj) // TODO
   {
    return *this;
   } 

  uint72 & operator %= (uint72 obj) // TODO
   {
    return *this;
   } 

  uint72 & operator &= (uint72 obj)
   {
    val&=obj.val;
    msb&=obj.msb;

    return *this;
   }

  uint72 & operator |= (uint72 obj)
   {
    val|=obj.val;
    msb|=obj.msb;

    return *this;
   }

  uint72 & operator ^= (uint72 obj)
   {
    val^=obj.val;
    msb^=obj.msb;

    return *this;
   }
 };

/* functions */ 

template <class UInt>
uint8 GetZ(UInt src)
 {
  return (!src)? (uint8)BitZ : 0 ;
 }

template <UIntType UInt>
uint8 GetN(UInt src)
 {
  return UIntFunc<UInt>::IsNegative(src)? BitN : 0 ;
 }

inline uint8 GetN(uint72 src) { return GetN(src.msb); }

template <UIntType UInt,UIntType UInt1>
UInt MSBit(UInt1 src)
 {
  UInt dst=src;

  dst&=UIntFunc<UInt1>::MSBit;

  return dst;
 }

/* functions */ 

template <UIntType UInt,UIntType UInt1>
void ExtUnsigned(UInt &dst,UInt1 src)
 {
  dst=src;
 }

template <UIntType UInt1>
void ExtUnsigned(uint72 &dst,UInt1 src)
 {
  dst.val=src;
  dst.msb=0;
 }

template <UIntType UInt,UIntType UInt1>
auto ExtSigned(UInt &dst,UInt1 src)
 {
  dst=src;

  UInt bit=MSBit<UInt>(src);

  dst-=(bit<<1);

  return bit;
 }

template <UIntType UInt1>
auto ExtSigned(uint72 &dst,UInt1 src)
 {
  auto bit=ExtSigned(dst.val,src);

  dst.msb=0;
  dst.msb-=(dst.val>>63);

  return bit;
 }

template <UIntType UInt,UIntType UInt1>
uint8 Cut(UInt &dst,UInt1 src)
 {
  dst=(UInt)src;

  UInt1 bit=UInt1(1)<<UIntFunc<UInt>::Bits;

  return (src&bit)? BitC : 0 ;
 }

template <UIntType UInt>
uint8 Cut(UInt &dst,uint72 src)
 {
  return Cut(dst,src.val);
 }

inline uint8 Cut(uint64 &dst,uint72 src)
 {
  dst=src.val;

  return (src.msb&1u)? BitC : 0 ;
 }

/* functions */ 

template <UIntType UInt>
UInt SwapBit(UInt val)
 {
  UInt ret=0;

  for(UInt bit=1,out=UIntFunc<UInt>::MSBit; out ;bit<<=1,out>>=1) if( val&bit ) ret|=out;

  return ret;
 }

template <UIntType UInt>
UInt SwapByte(UInt val)
 {
  UInt ret=0;
  unsigned shift=UIntFunc<UInt>::Bits-8;

  for(; val ;val>>=8,shift-=8) ret|=((val&0xFFu)<<shift);

  return ret;
 }

template <UIntType UInt>
uint8 BitCount(UInt val)
 {
  uint8 ret=0;

  for(; val ;val>>=1) if( val&1u ) ret++;

  return ret;
 }

} // namespace Basis::Op    

#endif
