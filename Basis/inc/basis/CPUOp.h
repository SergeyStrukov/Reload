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
 };

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

  template <class Src>
  void ext(Src src);

  template <class Src>
  uint8 cast(Src src);

  Arg operator + (Arg obj) const;
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
using SMaxArg = SIntArg< Max(Src1::ExtWidth,Src2::ExtWidth) > ;

template <class Src1,class Src2>
using UMaxArg = Meta::Select<( Src1::Width>=Src2::Width ), Src1 , Src2 > ;

template <class Src1,class Src2>
using MaxArg = Meta::Select< Src1::Sign || Src2::Sign , SMaxArg<Src1,Src2> , UMaxArg<Src1,Src2> > ;

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
