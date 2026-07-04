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
static_assert( CmdSysLim<=CmdExtBase );

/* struct RegArg */

bool RegArg::decode(uint64 cmd)
 {
  sign=BitField(cmd,8,1);

  uint8 ind=BitField(cmd,0,8);

  constexpr unsigned Cnt = CommonRegCount ;

  if( ind<Cnt )
    {
     width=Reg64bit;
     num=ind;  

     return true;
    }

  ind-=Cnt;
  
  if( ind<2*Cnt )
    {
     width=Reg32bit;
     num=ind;  

     return true;
    }

  ind-=2*Cnt;  

  if( ind<4*Cnt )
    {
     width=Reg16bit;
     num=ind;  

     return true;
    }

  ind-=4*Cnt;  

  if( ind<8*Cnt )
    {
     width=Reg8bit;
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

/* struct ConstArg */ 

void ConstArg::decode(uint64 cmd,unsigned width)
 {
  ext=BitField(cmd,width-1,1);

  if( ext )
    {
     sign=BitField(cmd,width-2,1);
    }
  else
    {
     sign=1; 
     val=BitSField(cmd,0,width-1);
    }  
 }

/* struct ConstRegArg */ 

bool ConstRegArg::decode(uint64 cmd,unsigned width)
 {
  isReg=BitField(cmd,width-1,1);

  if( isReg )
    {
     ext=0; 

     return reg.decode(cmd>>(width-10));
    }
  else
    {
     cnst.decode(cmd,width-1);

     ext=cnst.ext;

     return true;
    }
 }

/* struct CmdAddress */ 

bool CmdAddress::decode(uint64 cmd)
 {
  if( !base.decode(cmd>>38) ) return false;

  type=BitField(cmd,36,2);  

  if( type==0 ) 
    {
     ext=0;

     return true;
    }

  if( type==1 )
    {
     if( !src.decode(cmd,36) ) return false;

     ext=src.ext;

     return true;
    }

  src.isReg=1; 

  if( !src.reg.decode(cmd>>27) ) return false;

  if( type==2 ) 
    {
     cnst1.decode(cmd,27);

     ext=cnst1.ext;

     return true;
    }

  if( type==3 )
    {
     cnst1.decode(cmd>>16,11);
     cnst2.decode(cmd,16);

     ext=cnst1.ext+cnst2.ext;

     return true;  
    }

  return false;  
 }

bool CmdAddress::decode39(uint64 cmd)
 {
  if( !base.decode(cmd>>34) ) return false;

  type=BitField(cmd,32,2);  

  if( type==0 ) 
    {
     ext=0;

     return true;
    }

  if( type==1 )
    {
     if( !src.decode(cmd,32) ) return false;

     ext=src.ext;

     return true;
    }

  src.isReg=1; 

  if( !src.reg.decode(cmd>>23) ) return false;

  if( type==2 ) 
    {
     cnst1.decode(cmd,23);

     ext=cnst1.ext;

     return true;
    }

  if( type==3 )
    {
     cnst1.decode(cmd>>12,11);
     cnst2.decode(cmd,12);

     ext=cnst1.ext+cnst2.ext;

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
  flag=BitField(48,4);

  if( opcode<CmdUnLim )
    {
     flagOut=BitField(44,4);

     if( !dst.decode(cmd>>35) )
       {
        opcode=CmdUndef;   

        return 1;
       }

     if( !src1.decode(cmd,35) )
       {
        opcode=CmdUndef;   

        return 1;
       }

     return 1+src1.ext;
    }

  if( opcode>=CmdBinBase && opcode<CmdBinLim )
    {
     flagOut=BitField(44,4);

     if( !dst.decode(cmd>>35) )
       {
        opcode=CmdUndef;   

        return 1;
       }

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
              src2.isReg=1;

              if( !src2.reg.decode(cmd>>15) )
                {
                 opcode=CmdUndef;   

                 return 1;
                }

              return 1;
             } 
           else
             {
              src2.isReg=0;   

              src2.cnst.decode(cmd,24);

              return 1+src2.cnst.ext;
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

           src1.cnst.decode(cmd,24);

           return 1+src1.cnst.ext;
          }

        opcode=CmdUndef;   

        return 1;
       }
    }

  if( opcode>=CmdOtherBase && opcode<CmdOtherLim )
    {
     if( opcode<=CmdStore )
       {
        if( !dst.decode(cmd>>39) )
          {
           opcode=CmdUndef;   

           return 1;
          }

        if( !address.decode39(cmd) )
          {
           opcode=CmdUndef;   

           return 1;
          }

        return 1+address.ext;
       }

     if( opcode<=CmdRegStore )
       {
        if( !ereg.decode(cmd>>43) )
          {
           opcode=CmdUndef;   

           return 1;
          }

        if( !address.decode(cmd) )
          {
           opcode=CmdUndef;   

           return 1;
          }

        return 1+address.ext;
       }

     if( opcode<=CmdUnlock )
       {
        if( !address.decode(cmd) )
          {
           opcode=CmdUndef;   

           return 1;
          }

        return 1+address.ext;
       }

     if( opcode<=CmdCallPC ) 
       {
        if( !src1.decode(cmd,48) ) 
          {
           opcode=CmdUndef;

           return 1;
          }

        return 1+src1.ext;
       }

     if( opcode==CmdCoreIndex )
       {
        if( !dst.decode(cmd>>39) )
          {
           opcode=CmdUndef;   
          }

        return 1;   
       }

     if( opcode==CmdDebug )
       {
        if( !src1.decode(cmd,48) ) 
          {
           opcode=CmdUndef;

           return 1;
          }

        return 1+src1.ext;
       }

     if( opcode==CmdSetReg )
       {
        if( !ereg.decode(cmd>>43) )
          {
           opcode=CmdUndef;   

           return 1;
          }  

        if( !src1.decode(cmd,43) )
          {
           opcode=CmdUndef;   

           return 1;
          }

        return 1+src1.ext;
       }

     if( opcode==CmdGetReg )
       {
        if( !dst.decode(cmd>>39) )
          {
           opcode=CmdUndef;   

           return 1;
          }

        if( !ereg.decode(cmd>>34) )
          {
           opcode=CmdUndef;   

           return 1;
          }  

        return 1;   
       }

     return 1;   
    }

  if( opcode>=CmdSysBase && opcode<CmdSysLim )
    {
     if( opcode<=CmdSetupIntSP )
       {
        if( !dst.decode(cmd>>39) )
          {
           opcode=CmdUndef;   

           return 1;
          }

        if( !src1.decode(cmd,39) ) 
          {
           opcode=CmdUndef;

           return 1;
          }

        return 1+src1.ext;
       }

     if( opcode==CmdSetupSysVMT )
       {
        if( !src1.decode(cmd,48) ) 
          {
           opcode=CmdUndef;

           return 1;
          }

        return 1+src1.ext;
       }

     return 1;   
    }

  opcode=CmdUndef;   

  return 1;
 }

} // namespace Basis    

