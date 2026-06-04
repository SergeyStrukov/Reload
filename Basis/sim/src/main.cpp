/* main.cpp */
//----------------------------------------------------------------------------------------
//
//  Target: Basis Sim 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2026 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include "basis/CoreDefs.h"

#include <CCore/inc/Print.h>
#include <CCore/inc/Printf.h>

/* main() */

using namespace Basis;

int main(int argc,const char *const* argv)
 {
  HEntrySplit split(0xFFFF'FFFF'FFFF'FFFFu);

  Printf(Con,"#.h; #.h; #.h; #.h; #.h; #.h; #.h;\n",split.base,split.len,split.R,split.W,split.X,split.P,split.H);

  return 0;   
 }


