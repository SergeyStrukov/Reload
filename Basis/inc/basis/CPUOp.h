/* CPUOp.h */
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

#ifndef Basis_inc_CPUOp_h
#define Basis_inc_CPUOp_h

#include "basis/CoreDefs.h"

namespace Basis::Op {

/* consts */    

enum OpBit : uint8
 {
  BitZ = 1u,  
  BitC = 2u,  
  BitN = 4u,  
  BitO = 8u,  
 };

/* functions */ 

template <UIntType UInt,UIntType UInt1>
void ExtUnsigned(UInt &dst,UInt1 src)
 {
  dst=src;
 }

template <UIntType UInt,UIntType UInt1>
UInt MSBit(UInt1 src)
 {
  UInt dst=src;

  dst&=UIntFunc<UInt1>::MSBit;

  return dst;
 }

template <UIntType UInt,UIntType UInt1>
auto ExtSigned(UInt &dst,UInt1 src)
 {
  dst=src;

  auto bit=MSBit<UInt>(src);

  dst-=(bit<<1);

  return bit;
 }

template <UIntType UInt>
uint8 GetZ(UInt src)
 {
  return src? 0 : BitZ ;
 }

template <UIntType UInt>
uint8 GetN(UInt src)
 {
  return UIntFunc<UInt>::IsNegative(src)? BitZ : 0 ;
 }

template <UIntType UInt,UIntType UInt1>
uint8 Cut(UInt &dst,UInt1 src)
 {
  dst=(UInt)src;

  UInt1 bit=UInt1(1)<<UIntFunc<UInt>::Bits;

  return (src&bit)? BitC : 0 ;
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

/* classes */    

struct uint72;

template <class Body,unsigned Width_,bool Sign_> struct Arg;

struct OpCast;
struct OpNeg;
struct OpNot;
struct OpSwapBit;
struct OpSwapByte;
struct OpScanMSBit;
struct OpScanLSBit;
struct OpScanBitCount;

struct OpAdd;
struct OpSub;
struct OpMul;
struct OpDiv;
struct OpRem;
struct OpAnd;
struct OpOr;
struct OpXor;
struct OpAndNot;

/* struct uint72 */

struct uint72
 {
  uint64 val;  
  uint8 msb; 

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

template <UIntType UInt1>
void ExtUnsigned(uint72 &dst,UInt1 src)
 {
  dst.val=src;
  dst.msb=0;
 }

template <UIntType UInt1>
auto ExtSigned(uint72 &dst,UInt1 src)
 {
  auto bit=ExtSigned(dst.val,src);

  dst.msb=0;
  dst.msb-=(dst.val>>63);

  return bit;
 }

inline uint8 GetZ(uint72 src)
 {
  return (src.val|src.msb)? 0 : BitZ ;
 }

inline uint8 GetN(uint72 src)
 {
  return UIntFunc<uint8>::IsNegative(src.msb)? BitN : 0 ;
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

/* struct Arg<Body,Width,Sign> */

template <class Body,unsigned Width_,bool Sign_>
struct Arg
 {
  static constexpr unsigned Width = Width_ ;
  static constexpr bool Sign = Sign_ ;

  static constexpr unsigned ExtWidth = Sign? Width : (Width+1) ;

  Body val;
  
  template <class Src>
  static Arg Ext(Src src) { Arg ret; ret.ext(src); return ret; }

  template <class Src> requires ( Src::Sign && Sign && Src::Width<Width )
  uint8 ext(Src src) { auto neg=ExtSigned(val,src.val); return neg? BitC : 0 ; }

  template <class Src> requires ( Src::Sign && !Sign && Src::Width<Width )
  uint8 ext(Src src) { auto neg=ExtSigned(val,src.val); return neg? (BitO|BitC) : 0 ; }

  template <class Src> requires ( !Src::Sign && Src::Width<Width )
  uint8 ext(Src src) { ExtUnsigned(val,src.val); return 0; }

  template <class Src> requires ( Src::Width>Width )
  uint8 cut(Src src)
   {
    uint8 c=Cut(val,src.val);

    Src temp;

    uint8 o=temp.ext(*this);

    return c | ( (temp.val!=src.val)? uint8(BitO) : o ) ;
   }

  uint8 bitC() const requires( Sign )
   {
    return GetN(val)? BitC : 0 ;
   }

  uint8 bitC() const requires( !Sign )
   {
    return 0;
   }

  template <class Src> requires ( Src::Width==Width && Src::Sign==Sign )
  uint8 cut(Src src)
   {
    val=src.val;

    return src.bitC();
   }

  template <class Src> requires ( Src::Width==Width && Src::Sign!=Sign )
  uint8 cut(Src src)
   {
    val=src.val;

    return src.bitC() | ( GetN(val)? BitO : 0 ) ;
   }

  uint8 getNZ() const requires( Sign )
   {
    return GetZ(val)|GetN(val);
   }

  uint8 getNZ() const requires( !Sign )
   {
    return GetZ(val);
   }

  template <class Src> requires ( Src::Width<Width )
  uint8 cast(Src src)
   {
    uint8 oc=ext(src);

    return src.getNZ()|oc;
   }

  template <class Src> requires ( Src::Width>=Width )
  uint8 cast(Src src)
   {
    uint8 oc=cut(src);

    return src.getNZ()|oc;
   }

  Arg operator - () const { Arg ret; ret.val=-val; return ret; }
  Arg operator ~ () const { Arg ret; ret.val=~val; return ret; }

  Arg operator + (Arg obj) const { Arg ret=*this; ret.val+=obj.val; return ret; }
  Arg operator - (Arg obj) const { Arg ret=*this; ret.val-=obj.val; return ret; }

  Arg operator & (Arg obj) const { Arg ret=*this; ret.val&=obj.val; return ret; }
  Arg operator | (Arg obj) const { Arg ret=*this; ret.val|=obj.val; return ret; }
  Arg operator ^ (Arg obj) const { Arg ret=*this; ret.val^=obj.val; return ret; }

  Arg swapBit() const { Arg ret{ SwapBit(val) }; return ret; }
  Arg swapByte() const { Arg ret{ SwapByte(val) }; return ret; }

  Arg<uint8,8,false> scanMSBit() const { Arg<uint8,8,false> ret{ (uint8)UIntFunc<Body>::CountZeroMSB(val) }; return ret; }
  Arg<uint8,8,false> scanLSBit() const { Arg<uint8,8,false> ret{ (uint8)UIntFunc<Body>::CountZeroLSB(val) }; return ret; }
  Arg<uint8,8,false> scanBitCount() const { Arg<uint8,8,false> ret{ BitCount(val) }; return ret; }
 };

using UInt64 = Arg<uint64,64,false> ; 
using UInt32 = Arg<uint32,32,false> ; 
using UInt16 = Arg<uint16,16,false> ; 
using UInt8  = Arg<uint8,8,false> ; 

using SInt64 = Arg<uint64,64,true> ; 
using SInt32 = Arg<uint32,32,true> ; 
using SInt16 = Arg<uint16,16,true> ; 
using SInt8  = Arg<uint8,8,true> ; 

using SInt72 = Arg<uint72,65,true> ; 

template <unsigned W>
using SIntArg = Meta::Select<( W<=8 ), SInt8 , Meta::Select<( W<=16 ), SInt16 , Meta::Select<( W<=32 ), SInt32 , Meta::Select<( W<=64 ), SInt64 , SInt72 > > > > ;

template <class Src1,class Src2>
using MaxArg = SIntArg< Max(Src1::ExtWidth,Src2::ExtWidth)+1 > ;

template <class Src>
using UpArg = SIntArg< Src::ExtWidth+1 > ;

/* struct OpCast */ 

struct OpCast
 {
  template <class Dst,class Src>  
  uint8 operator () (Dst &dst,Src src) { return dst.cast(src); }
 };

/* struct OpNeg */

struct OpNeg
 {
  template <class Dst,class Src>  
  uint8 operator () (Dst &dst,Src src) 
   { 
    using Type = UpArg<Src> ;

    return dst.cast( -Type::Ext(src) ); 
   }
 };

/* struct OpNot */

struct OpNot
 {
  template <class Dst,class Src>  
  uint8 operator () (Dst &dst,Src src) 
   { 
    using Type = UpArg<Src> ;

    return dst.cast( ~Type::Ext(src) ); 
   }
 };

/* struct OpSwapBit */

struct OpSwapBit
 {
  template <class Dst,class Src>  
  uint8 operator () (Dst &dst,Src src)
   { 
    return dst.cast( src.swapBit() ); 
   }
 };

/* struct OpSwapByte */

struct OpSwapByte
 {
  template <class Dst,class Src>  
  uint8 operator () (Dst &dst,Src src)
   { 
    return dst.cast( src.swapByte() ); 
   }
 };

/* struct OpScanMSBit */

struct OpScanMSBit
 {
  template <class Dst,class Src>  
  uint8 operator () (Dst &dst,Src src)
   { 
    return dst.cast( src.scanMSBit() );
   }
 };

/* struct OpScanLSBit */

struct OpScanLSBit
 {
  template <class Dst,class Src>  
  uint8 operator () (Dst &dst,Src src)
   { 
    return dst.cast( src.scanLSBit() );
   }
 };

/* struct OpScanBitCount */ 

struct OpScanBitCount
 {
  template <class Dst,class Src>  
  uint8 operator () (Dst &dst,Src src)
   { 
    return dst.cast( src.scanBitCount() );
   }
 };

/* struct OpAdd */

struct OpAdd
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2)
   {
    using Type = MaxArg<Src1,Src2> ;

    return dst.cast( Type::Ext(src1) + Type::Ext(src2) );
   }
 };

/* struct OpSub */ 

struct OpSub
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2)
   {
    using Type = MaxArg<Src1,Src2> ;

    return dst.cast( Type::Ext(src1) - Type::Ext(src2) );
   }
 };

//struct OpMul;
//struct OpDiv;
//struct OpRem;

/* struct OpAnd */

struct OpAnd
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2)
   {
    using Type = MaxArg<Src1,Src2> ;

    return dst.cast( Type::Ext(src1) & Type::Ext(src2) );
   }
 };

/* struct OpOr */

struct OpOr
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2)
   {
    using Type = MaxArg<Src1,Src2> ;

    return dst.cast( Type::Ext(src1) | Type::Ext(src2) );
   }
 };

/* struct OpXor */

struct OpXor
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2)
   {
    using Type = MaxArg<Src1,Src2> ;

    return dst.cast( Type::Ext(src1) ^ Type::Ext(src2) );
   }
 };

/* struct OpAndNot */

struct OpAndNot
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2)
   {
    using Type = MaxArg<Src1,Src2> ;

    return dst.cast( Type::Ext(src1) & ~Type::Ext(src2) );
   }
 };

} // namespace Basis::Op    

#endif
