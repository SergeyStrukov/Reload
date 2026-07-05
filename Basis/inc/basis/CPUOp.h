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

/* classes */    

struct UInt64;
struct UInt32;
struct UInt16;
struct UInt8;

struct SInt64;
struct SInt32;
struct SInt16;
struct SInt8;

struct OpCast;
struct OpNeg;
struct OpNot;

struct OpAdd;
struct OpSub;
struct OpMul;
struct Div;
struct Rem;

/* struct UInt64 */

struct UInt64
 {
  uint64 val;  
 };

/* struct UInt32 */

struct UInt32
 {
  uint32 val;  
 };

/* struct UInt16 */

struct UInt16
 {
  uint16 val;  
 };

/* struct UInt8 */

struct UInt8
 {
  uint8 val;  
 };

/* struct SInt64 */

struct SInt64
 {
  uint64 val;  
 };

/* struct SInt32 */

struct SInt32
 {
  uint32 val;  
 };

/* struct SInt16 */

struct SInt16
 {
  uint16 val;  
 };

/* struct SInt8 */

struct SInt8
 {
  uint8 val;  
 };

/* struct OpCast */ 

struct OpCast
 {
  template <class Dst,class Src>  
  void operator () (Dst &dst,Src src);  
 };

/* struct OpAdd */

struct OpAdd
 {
  template <class Dst,class Src1,class Src2>  
  void operator () (Dst &dst,Src1 src1,Src2 src2);  
 };

} // namespace Basis::Op    

#endif
