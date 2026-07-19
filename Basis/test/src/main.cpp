/* main.cpp */
//----------------------------------------------------------------------------------------
//
//  Target: Basis Test 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2026 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include "test.h"

/* main() */

namespace Test {

void test1();

} // namespace Test

using namespace Test;

int main()
 {
  ReportException report;

  try
    {
     test1();

     report.guard();

     return 0;   
    }
  catch(CatchType)
    {
     return 1;   
    }

  return 1;   
 }


