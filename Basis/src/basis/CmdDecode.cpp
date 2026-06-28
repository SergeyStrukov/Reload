/* CmdDecode.cpp */
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

#include "basis/CmdDecode.h"

namespace Basis {

/* consts */    

static_assert( CmdUnLim<=CmdBinBase );
static_assert( CmdBinLim<=CmdOtherBase );
static_assert( CmdOtherLim<=CmdSysBase );
static_assert( CmdOtherLim<=CmdSysBase );

static_assert( CmdSysLim<=256 );

/* struct RegArg */

bool RegArg::decode(uint64 cmd)
 {
  sign=BitField(cmd,8,1);

  uint8 ind=BitField(cmd,0,8);

  if( ind<16 )
    {
     width=0;
     num=ind;  

     return true;
    }

  ind-=16;
  
  if( ind<2*16 )
    {
     width=1;
     num=ind;  

     return true;
    }

  ind-=2*16;  

  if( ind<4*16 )
    {
     width=2;
     num=ind;  

     return true;
    }

  ind-=4*16;  

  if( ind<8*16 )
    {
     width=3;
     num=ind;  

     return true;
    }

  return false;
 }

/* struct Cmd */ 
    
uint32 Cmd::decode(uint64 cmd)
 {
  opcode=BitField(cmd,56);  

  if( opcode<CmdUnBase ) return 1;

  cond=BitField(cmd,52,4);

  if( opcode<CmdUnLim )
    {
     flag=BitField(48,4);
     flagOut=BitField(44,4);

     if( dst.decode( cmd>>35 ) )
       {
        uint8 bitRC=BitField(cmd,34,1);

        if( bitRC )
          {
           src1.isReg=1; 

           if( !src1.reg.decode(cmd>>25) )
             {
              opcode=CmdUndef;   

              return 1;
             }
          }
        else
          {
           src1.isReg=0; 

           uint8 bitExt=BitField(cmd,33,1);

           if( bitExt )
             {
              src1.cnst.ext=1;

              return 2;
             }
           else
             {
              src1.cnst.ext=0;
              src1.cnst.big=1;
              src1.cnst.val=BitField(cmd,0,33);

              return 1;
             }  
          }
       }
     else
       {
        opcode=CmdUndef;   

        return 1;
       }  
    }

  if( opcode>=CmdBinBase && opcode<CmdBinLim )
    {
     flag=BitField(48,4);
     flagOut=BitField(44,4);

     // TODO

     return 1;   
    }

  if( opcode>=CmdOtherBase && opcode<CmdOtherLim )
    {
     // TODO

     return 1;   
    }

  if( opcode>=CmdSysBase && opcode<CmdSysLim )
    {
     // TODO

     return 1;   
    }

  opcode=CmdUndef;   

  return 1;
 }

} // namespace Basis    

