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

struct uint128;

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

  uint72 & operator /= (uint72 obj) // 64 bit operation, msb and obj.msb == 0
   {
    val/=obj.val;

    return *this;
   } 

  uint72 & operator %= (uint72 obj) // 64 bit operation, msb and obj.msb == 0
   {
    val%=obj.val;

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

/* struct uint128 */ 

struct uint128
 {
  uint64 hi;
  uint64 lo;

  bool operator ! () const { return !(hi|lo); }

  bool operator == (uint128 obj) const { return hi==obj.hi && lo==obj.lo ; }

  uint128 operator - () const
   {
    uint128 ret{0,0};

    ret-=(*this);

    return ret;
   }

  uint128 & operator -= (uint128 obj)
   {
    UIntFunc<uint64>::Sub sub(lo,obj.lo); 

    lo=sub.result;
    hi-=obj.hi+sub.borrow;    

    return *this;
   }
 };

/* functions */ 

template <UIntType UInt>
bool IsNeg(UInt src)
 {
  return UIntFunc<UInt>::IsNegative(src);
 }

inline bool IsNeg(uint72 src)
 {
  return IsNeg(src.msb);
 }

inline bool IsNeg(uint128 src)
 {
  return IsNeg(src.hi);
 }

template <class UInt>
uint8 GetZ(UInt src)
 {
  return (!src)? BitZ : 0 ;
 }

template <class UInt>
uint8 GetN(UInt src)
 {
  return IsNeg(src)? BitN : 0 ;
 }

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

template <UIntType UInt1>
void ExtUnsigned(uint128 &dst,UInt1 src)
 {
  dst.lo=src;
  dst.hi=0;
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

template <UIntType UInt1>
auto ExtSigned(uint128 &dst,UInt1 src)
 {
  auto bit=ExtSigned(dst.lo,src);

  dst.hi=0;
  dst.hi-=(dst.lo>>63);

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

template <class UInt>
uint8 Cut(UInt &dst,uint128 src)
 {
  return Cut(dst,src.lo);
 }

inline uint8 Cut(uint64 &dst,uint128 src)
 {
  dst=src.lo;

  return (src.hi&1u)? BitC : 0 ;
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

/* functions */ 

inline uint32 UnsignedMul(uint16 a,uint16 b)
 {
  return (uint32)a*b;
 }

inline uint64 UnsignedMul(uint32 a,uint32 b)
 {
  return (uint64)a*b;
 }

inline uint128 UnsignedMul(uint64 a,uint64 b)
 {
  UIntFunc<uint64>::Mul mul(a,b);

  return uint128{mul.hi,mul.lo};
 }

inline uint128 UnsignedMul(uint72 a,uint72 b)
 {
  return UnsignedMul(a.val,b.val);
 }

template <class UInt>
auto SignedMul(UInt a,UInt b)
 {
  if( IsNeg(a) )
    {
     a=-a;

     if( IsNeg(b) )
       {
        b=-b;

        return UnsignedMul(a,b);
       }
     else 
       {
        return -UnsignedMul(a,b);
       }
    }
  else
    {
     if( IsNeg(b) )
       {
        b=-b;

        return -UnsignedMul(a,b);
       }
     else 
       {
        return UnsignedMul(a,b);
       }
    }
 }

template <class UInt>
UInt SignedDiv(UInt a,UInt b)
 {
  if( IsNeg(a) )
    {
     a=-a;

     if( IsNeg(b) )
       {
        b=-b;
        a/=b;

        return a;
       }
     else 
       {
        a/=b;

        return -a;
       }
    }
  else
    {
     if( IsNeg(b) )
       {
        b=-b;
        a/=b;

        return -a;
       }
     else 
       {
        a/=b;

        return a;
       }
    }
 }

template <class UInt>
UInt SignedRem(UInt a,UInt b)
 {
  if( IsNeg(b) ) b=-b;

  if( IsNeg(a) )
    {
     a=-a;

     a%=b;

     return -a;
    }
  else
    {
     a%=b;

     return a;
    }
 }

} // namespace Basis::Op    

#endif
