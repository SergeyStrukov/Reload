/* test1.cpp */
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

namespace Test {

namespace {    

void test(int a,int b)
 {
  Printf(Con,"#; / #; = #;\n",a,b,a/b);  
  Printf(Con,"#; % #; = #;\n",a,b,a%b);  
 }    

} // local namespace   

/* test1() */    

void test1()
 {
  test(3,10);  
  test(-3,10);  
  test(3,-10);  
  test(-3,-10);  
 }

} // namespace Test
