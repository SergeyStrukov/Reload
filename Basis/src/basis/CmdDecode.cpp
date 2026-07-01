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

static_assert( RegCount<=32 );

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

/* struct ExtRegArg */ 

bool ExtRegArg::decode(uint64 cmd)
 {
  num=BitField(cmd,0,5);

  if( num>=RegCount ) return false;

  return true;
 }

/* struct CmdAddress */ 

uint32 CmdAddress::decode(uint64 cmd)
 {
  if( !base.decode(cmd>>38) ) return 1;

  type=BitField(cmd,36,2);  

  if( type==0 ) return 1;

  if( type==1 )
    {
     uint8 bitRC=BitField(cmd,35,1);

     if( bitRC )
       {
        src.isReg=1; 

        if( !src.reg.decode(cmd>>26) ) return 0;

        return 1;
       }
     else
       {
        src.isReg=0; 

        uint8 bitExt=BitField(cmd,34,1);

        if( bitExt )
          {
           src.cnst.ext=1;

           return 2;
          }
        else
          {
           src.cnst.ext=0;
           src.cnst.big=2;
           src.cnst.val=BitField(cmd,0,34);

           return 1;
          }  
       }
    }

  if( type==2 ) 
    {
     src.isReg=1; 

     if( !src.reg.decode(cmd>>27) ) return 0;

     uint8 bitExt=BitField(cmd,26,1);

     if( bitExt )
       {
        cnst1.ext=1;

        return 2;
       }
     else
       {
        cnst1.ext=0;
        cnst1.big=2;
        cnst1.val=BitField(cmd,0,26);

        return 1;
       }  
    }

  if( type==3 )
    {
     src.isReg=1; 

     if( !src.reg.decode(cmd>>27) ) return 0;

     uint8 bitExt1=BitField(cmd,26,1);
     uint8 bitExt2=BitField(cmd,16,1);

     if( bitExt1 )
       {
        cnst1.ext=1;
       }
     else
       {
        cnst1.ext=0;
        cnst1.big=2;
        cnst1.val=BitField(cmd,16,10);
       }  

     if( bitExt2 )
       {
        cnst2.ext=1;
       }
     else
       {
        cnst2.ext=0;
        cnst2.big=2;
        cnst2.val=BitField(cmd,0,15);
       }  

     return bitExt1+bitExt2+1;  
    }

  return 1;  
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
             }

           return 1;
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

     if( dst.decode( cmd>>35 ) )
       {
        uint8 bitRC1=BitField(cmd,34,1);
        uint8 bitRC2=BitField(cmd,33,1);

        if( bitRC1 )
          {
           src1.isReg=1;

           if( !src1.reg.decode(cmd>>24) )
             {
              opcode=CmdUndef;   

              return 1;
             }

           if( bitRC2 )
             {
              if( !src2.reg.decode(cmd>>15) )
                {
                 opcode=CmdUndef;   
                }

              return 1;
             } 
           else
             {
              src2.isReg=0;   

              uint8 bitExt=BitField(cmd,23,1);

              if( bitExt )
                {
                 src2.cnst.ext=1;

                 return 2;
                }
              else
                {
                 src2.cnst.ext=0;
                 src2.cnst.big=0;
                 src2.cnst.val=BitField(cmd,0,23);

                 return 1;
                }  
             } 
          }
        else if( bitRC2 )
          {
           src2.isReg=1;

           if( !src2.reg.decode(cmd>>24) )
             {
              opcode=CmdUndef;   

              return 1;
             }

           src1.isReg=0;   

           uint8 bitExt=BitField(cmd,23,1);

           if( bitExt )
             {
              src1.cnst.ext=1;

              return 2;
             }
           else
             {
              src1.cnst.ext=0;
              src1.cnst.big=0;
              src1.cnst.val=BitField(cmd,0,23);

              return 1;
             }  
          }

        opcode=CmdUndef;   

        return 1;
       }
     else
       {
        opcode=CmdUndef;   

        return 1;
       }  

     return 1;   
    }

  if( opcode>=CmdOtherBase && opcode<CmdOtherLim )
    {
     if( opcode<=CmdCallPC ) 
       {
        uint8 bitRC=BitField(cmd,51,1);

        if( bitRC )
          {
           src1.isReg=1; 

           if( !src1.reg.decode(cmd>>42) )
             {
              opcode=CmdUndef;   
             }
             
           return 1;
          }
        else
          {
           src1.isReg=0; 

           uint8 bitExt=BitField(cmd,50,1);

           if( bitExt )
             {
              src1.cnst.ext=1;

              return 2;
             }
           else
             {
              src1.cnst.ext=0;
              src1.cnst.big=2;
              src1.cnst.val=BitField(cmd,0,50);

              return 1;
             }  
          }
       }

     if( opcode==CmdCoreIndex )
       {
        if( !dst.decode(cmd>>43) )
          {
           opcode=CmdUndef;   
          }

        return 1;   
       }

     if( opcode==CmdDebug )
       {
        if( !src.decode(cmd>>43) )
          {
           opcode=CmdUndef;   
          }

        return 1;   
       }

     if( opcode==CmdSetReg )
       {
        if( !ereg.decode(cmd>>47) )
          {
           opcode=CmdUndef;   

           return 1;
          }  

        uint8 bitRC=BitField(cmd,46,1);

        if( bitRC )
          {
           src1.isReg=1; 

           if( !src1.reg.decode(cmd>>37) )
             {
              opcode=CmdUndef;   
             }
             
           return 1;
          }
        else
          {
           src1.isReg=0; 

           uint8 bitExt=BitField(cmd,45,1);

           if( bitExt )
             {
              src1.cnst.ext=1;

              return 2;
             }
           else
             {
              src1.cnst.ext=0;
              src1.cnst.big=2;
              src1.cnst.val=BitField(cmd,0,45);

              return 1;
             }  
          }

        return 1;   
       }

     if( opcode==CmdGetReg )
       {
        if( !dst.decode(cmd>>43) )
          {
           opcode=CmdUndef;   

           return 1;
          }

        if( !ereg.decode(cmd>>38) )
          {
           opcode=CmdUndef;   

           return 1;
          }  

        return 1;   
       }

     if( opcode==CmdLoadAddr && opcode<=CmdStore )
       {
        if( !dst.decode( cmd>>43 ) )
          {
           opcode=CmdUndef;   

           return 1;
          }

        uint32 ret=address.decode(cmd); 

        if( !ret )
          {
           opcode=CmdUndef;   

           return 1;
          }

        return ret;
       }

     if( opcode==CmdRegLoadAddr && opcode<=CmdRegStore )
       {
        if( !ereg.decode( cmd>>47 ) )
          {
           opcode=CmdUndef;   

           return 1;
          }

        uint32 ret=address.decode(cmd); 

        if( !ret )
          {
           opcode=CmdUndef;   

           return 1;
          }

        return ret;
       }

     if( opcode>=CmdLock && opcode<=CmdUnlock )
       {
        uint32 ret=address.decode(cmd); 

        if( !ret )
          {
           opcode=CmdUndef;   

           return 1;
          }

        return ret;
       }

     return 1;   
    }

  if( opcode>=CmdSysBase && opcode<CmdSysLim )
    {
     if( opcode>=CmdSetupCoreVMT && opcode<=CmdSetupIntSP )
       {
        if( !dst.decode(cmd>>43) ) opcode=CmdUndef;
       }

     return 1;   
    }

  opcode=CmdUndef;   

  return 1;
 }

} // namespace Basis    

