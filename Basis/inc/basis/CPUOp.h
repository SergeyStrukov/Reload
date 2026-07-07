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
UInt MSBitUp(UInt1 src)
 {
  UInt dst=src;

  dst&=UIntFunc<UInt1>::MSBit;

  return dst<<1;
 }

template <class UInt,class UInt1>
void ExtSigned(UInt &dst,UInt1 src)
 {
  dst=src;

  dst-=MSBitUp<UInt>(src);
 }

/* classes */    

struct uint65;

template <class Body,unsigned Width_,bool Sign_> struct Arg;

struct OpCast;
struct OpNeg;
struct OpNot;

struct OpAdd;
struct OpSub;
struct OpMul;
struct Div;
struct Rem;

/* struct uint65 */

struct uint65
 {
  uint64 val;  
  uint8 msb; 
  
  uint65 & operator += (uint65 obj)
   {
    UIntFunc<uint64>::Add add(val,obj.val); 

    val=add.result;
    msb+=obj.msb+add.carry;    

    return *this;
   }
 };

template <class UInt1>
void ExtUnsigned(uint65 &dst,UInt1 src)
 {
  dst.val=src;
  dst.msb=0;
 }

template <class UInt1>
void ExtSigned(uint65 &dst,UInt1 src)
 {
  ExtSigned(dst.val,src);
  dst.msb=0;
  dst.msb-=(dst.val>>63);
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

  template <class Src> requires ( Src::Sign )
  void ext(Src src) { ExtSigned(val,src.val); }

  template <class Src> requires ( !Src::Sign )
  void ext(Src src) { ExtUnsigned(val,src.val); }

  template <class Src>
  uint8 cast(Src src);

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

using SInt65 = Arg<uint65,65,true> ; 

template <unsigned W>
using SIntArg = Meta::Select<( W<=8 ), SInt8 , Meta::Select<( W<=16 ), SInt16 , Meta::Select<( W<=32 ), SInt32 , Meta::Select<( W<=64 ), SInt64 , SInt65 > > > > ;

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
