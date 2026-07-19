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

#include "basis/ExtInt.h"

namespace Basis::Op {

/* classes */    

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
struct OpMac;
struct OpAddCarry;
struct OpSubCarry;
struct OpShiftLeft;
struct OpShiftRight;
struct OpRotLeft;
struct OpRotRight;

/* struct Arg<Body,Width,Sign> */

template <class Body,unsigned Width_,bool Sign_>
struct Arg
 {
  static constexpr unsigned Width = Width_ ;
  static constexpr bool Sign = Sign_ ;

  static constexpr unsigned ExtWidth = Sign? Width : (Width+1) ;

  // body

  Body val;

  // prop

  bool operator ! () const { return !val; }

  uint8 getNZ() const requires( Sign )
   {
    return GetZ(val)|GetN(val);
   }

  uint8 getNZ() const requires( !Sign )
   {
    return GetZ(val);
   }

  uint8 bitC() const requires( Sign )
   {
    return GetN(val)? BitC : 0 ;
   }

  uint8 bitC() const requires( !Sign )
   {
    return 0;
   }

  // ext
  
  template <class Src>
  static Arg Ext(Src src) { Arg ret; ret.ext(src); return ret; }

  template <class Src> requires ( Src::Sign && Sign && Src::Width<Width )
  uint8 ext(Src src) { auto neg=ExtSigned(val,src.val); return neg? BitC : 0 ; }

  template <class Src> requires ( Src::Sign && !Sign && Src::Width<Width )
  uint8 ext(Src src) { auto neg=ExtSigned(val,src.val); return neg? (BitO|BitC) : 0 ; }

  template <class Src> requires ( !Src::Sign && Src::Width<Width )
  uint8 ext(Src src) { ExtUnsigned(val,src.val); return 0; }

  // cut

  template <class Src> requires ( Src::Width>Width )
  uint8 cut(Src src)
   {
    uint8 c=Cut(val,src.val);

    Src temp;

    uint8 o=temp.ext(*this)&BitO;

    return c | ( (temp.val!=src.val)? uint8(BitO) : o ) ;
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

  // cast

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

  // operators

  Arg operator - () const { Arg ret; ret.val=-val; return ret; }
  Arg operator ~ () const { Arg ret; ret.val=~val; return ret; }

  Arg operator + (Arg obj) const { Arg ret=*this; ret.val+=obj.val; return ret; }
  Arg operator - (Arg obj) const { Arg ret=*this; ret.val-=obj.val; return ret; }

  Arg operator / (Arg obj) const { Arg ret=*this; ret.val/=obj.val; return ret; }
  Arg operator % (Arg obj) const { Arg ret=*this; ret.val%=obj.val; return ret; }

  Arg operator & (Arg obj) const { Arg ret=*this; ret.val&=obj.val; return ret; }
  Arg operator | (Arg obj) const { Arg ret=*this; ret.val|=obj.val; return ret; }
  Arg operator ^ (Arg obj) const { Arg ret=*this; ret.val^=obj.val; return ret; }

  Arg swapBit() const { return Arg{ SwapBit(val) }; }
  Arg swapByte() const { return Arg{ SwapByte(val) }; }

  using ScanType = Arg<uint8,8,false> ;

  ScanType scanMSBit() const { return ScanType{ (uint8)UIntFunc<Body>::CountZeroMSB(val) }; }
  ScanType scanLSBit() const { return ScanType{ (uint8)UIntFunc<Body>::CountZeroLSB(val) }; }
  ScanType scanBitCount() const { return ScanType{ BitCount(val) }; }

  Arg addCarry(Arg obj,bool bitC) const { if( bitC ) obj.val+=1u; return (*this)+obj; }
  Arg subCarry(Arg obj,bool bitC) const { if( bitC ) obj.val+=1u; return (*this)-obj; }
 };

/* arg types */

using UInt64 = Arg<uint64,64,false> ; 
using UInt32 = Arg<uint32,32,false> ; 
using UInt16 = Arg<uint16,16,false> ; 
using UInt8  = Arg<uint8,8,false> ; 

using SInt64 = Arg<uint64,64,true> ; 
using SInt32 = Arg<uint32,32,true> ; 
using SInt16 = Arg<uint16,16,true> ; 
using SInt8  = Arg<uint8,8,true> ; 

using SInt72 = Arg<uint72,72,true> ; 

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

/* struct OpMul */

struct OpMul // TODO
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2);
 };

/* struct OpDiv */

struct OpDiv
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2)
   {
    using Type = MaxArg<Src1,Src2> ;

    Type div=Type::Ext(src2);

    if( !div ) return dst.cast( Type::Ext(Src1{0}) )|BitO;

    return dst.cast( Type::Ext(src1) / div );
   }
 };

/* struct OpRem */

struct OpRem
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2)
   {
    using Type = MaxArg<Src1,Src2> ;

    Type div=Type::Ext(src2);

    if( !div ) return dst.cast( Type::Ext(src1) )|BitO;

    return dst.cast( Type::Ext(src1) % div );
   }
 };

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

/* struct OpMac */

struct OpMac // TODO
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2);
 };

/* struct OpAddCarry */ 

struct OpAddCarry
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2,uint8 flags)
   {
    using Type = MaxArg<Src1,Src2> ;

    return dst.cast( Type::Ext(src1).addCarry(Type::Ext(src2),flags&BitC) );
   }
 };

/* struct OpSubCarry */

struct OpSubCarry
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2,uint8 flags)
   {
    using Type = MaxArg<Src1,Src2> ;

    return dst.cast( Type::Ext(src1).subCarry(Type::Ext(src2),flags&BitC) );
   }
 };

/* struct OpShiftLeft */

struct OpShiftLeft // TODO
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2);
 };

/* struct OpShiftRight */

struct OpShiftRight // TODO
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2);
 };

/* struct OpRotLeft */ 

struct OpRotLeft // TODO
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2);
 };

/* struct OpRotRight */

struct OpRotRight // TODO
 {
  template <class Dst,class Src1,class Src2>  
  uint8 operator () (Dst &dst,Src1 src1,Src2 src2);
 };

} // namespace Basis::Op    

#endif
