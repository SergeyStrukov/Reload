/* Computer.h */
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

#ifndef Basis_inc_Computer_h
#define Basis_inc_Computer_h

#include "basis/CPUCore.h"

namespace Basis {

/* classes */

struct ComputerCfg;

class Computer;

/* struct ComputerCfg */

struct ComputerCfg
 {
 };

/* class Computer */

class Computer : NoCopy
 {
  public:

   explicit Computer(const ComputerCfg &cfg);

   ~Computer();

   void run();
 };

} // namespace Basis    

#endif
