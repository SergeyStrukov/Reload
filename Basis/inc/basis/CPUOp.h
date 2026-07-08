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

template <class UInt,class UInt1>
void ExtUnsigned(UInt &dst,UInt1 src)
 {
  dst=src;
 }

template <class UInt,class UInt1>
UInt MSBit(UInt1 src)
 {
  UInt dst=src;

  dst&=UIntFunc<UInt1>::MSBit;

  return dst;
 }

template <class UInt,class UInt1>
auto ExtSigned(UInt &dst,UInt1 src)
 {
  dst=src;

  auto bit=MSBit<UInt>(src);

  dst-=(bit<<1);

  return bit;
 }

template <class UInt>
uint8 GetZ(UInt src)
 {
  return src? 0 : BitZ ;
 }

template <class UInt>
uint8 GetN(UInt src)
 {
  return UIntFunc<UInt>::IsNegative(src)? BitZ : 0 ;
 }

template <class UInt,class UInt1>
void Cut(UInt &dst,UInt1 src) 
 {
  dst=(UInt)src;
 }

/* classes */    

struct uint72;

template <class Body,unsigned Width_,bool Sign_> struct Arg;

struct OpCast;
struct OpNeg;
struct OpNot;

struct OpAdd;
struct OpSub;
struct OpMul;
struct Div;
struct Rem;

/* struct uint72 */

struct uint72
 {
  uint64 val;  
  uint8 msb; 

  bool operator == (uint72 obj) const { return val==obj.val && msb==obj.msb ; }
  
  uint72 & operator += (uint72 obj)
   {
    UIntFunc<uint64>::Add add(val,obj.val); 

    val=add.result;
    msb+=obj.msb+add.carry;    

    return *this;
   }
 };

template <class UInt1>
void ExtUnsigned(uint72 &dst,UInt1 src)
 {
  dst.val=src;
  dst.msb=0;
 }

template <class UInt1>
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

template <class UInt>
void Cut(UInt &dst,uint72 src) 
 {
  dst=(UInt)src.val;
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
  uint8 ext(Src src) { ExtSigned(val,src.val); return 0; }

  template <class Src> requires ( Src::Sign && !Sign && Src::Width<Width )
  uint8 ext(Src src) { auto neg=ExtSigned(val,src.val); return neg? BitO : 0 ; }

  template <class Src> requires ( !Src::Sign && Src::Width<Width )
  uint8 ext(Src src) { ExtUnsigned(val,src.val); return 0; }

  template <class Src> requires ( Src::Width>Width )
  uint8 cut(Src src)
   {
    Cut(val,src.val);

    Src temp;

    temp.ext(*this);

    return (temp.val!=src.val)? BitO : 0 ;
   }

  template <class Src> requires ( Src::Width==Width && Src::Sign==Sign )
  uint8 cut(Src src)
   {
    val=src.val;

    return 0;
   }

  template <class Src> requires ( Src::Width==Width && Src::Sign!=Sign )
  uint8 cut(Src src)
   {
    val=src.val;

    return GetN(val)? BitO : 0 ;
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
    uint8 o=ext(src);

    return src.getNZ()|o; // TODO C
   }

  template <class Src> requires ( Src::Width>=Width )
  uint8 cast(Src src)
   {
    uint8 o=cut(src);

    return src.getNZ()|o; // TODO C
   }

  Arg operator + (Arg obj) const { Arg ret=*this; ret.val+=obj.val; return ret; }
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

/* struct OpCast */ 

struct OpCast
 {
  template <class Dst,class Src>  
  uint8 operator () (Dst &dst,Src src) { return dst.cast(src); }
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

} // namespace Basis::Op    

#endif
