//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2022, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| VPS group configuration                                          |
//+------------------------------------------------------------------+
class IMTConVPSGroup
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(IMTConVPSGroup* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- group list allowed
   virtual LPCWSTR   Group(void) const=0;
   virtual MTAPIRES  Group(LPCWSTR group)=0;
   //--- minimal balance for sponsored VPS
   virtual double    MinBalance(void) const=0;
   virtual MTAPIRES  MinBalance(const double balance)=0;
   //--- inactivity days to switch off sponsored VPS
   virtual UINT      InactiveDays(void) const=0;
   virtual MTAPIRES  InactiveDays(const UINT days)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConVPSGroup(void) {}
  };
//+------------------------------------------------------------------+
//| VPS configuration                                                |
//+------------------------------------------------------------------+
class IMTConVPS
  {
public:
   //--- flags
   enum EnFlags
     {
      VPS_NONE          =0x00000000,         // none
      VPS_SPONSOR_ACTIVE=0x00000001,         // activate VPS
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(IMTConVPS* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- flags
   virtual UINT64    Flags(void) const=0;
   virtual MTAPIRES  Flags(const UINT64 flags)=0;
   //--- MQL5 login
   virtual LPCWSTR   MQL5Login(void) const=0;
   virtual MTAPIRES  MQL5Login(LPCWSTR login)=0;
   //--- MQL5 password
   virtual LPCWSTR   MQL5Password(void) const=0;
   virtual MTAPIRES  MQL5Password(LPCWSTR password)=0;
   //--- groups settings
   virtual MTAPIRES  GroupAdd(IMTConVPSGroup* group)=0;
   virtual MTAPIRES  GroupUpdate(const UINT pos,const IMTConVPSGroup* group)=0;
   virtual MTAPIRES  GroupDelete(const UINT pos)=0;
   virtual MTAPIRES  GroupClear(void)=0;
   virtual MTAPIRES  GroupShift(const UINT pos,const int shift)=0;
   virtual UINT      GroupTotal(void) const=0;
   virtual MTAPIRES  GroupNext(const UINT pos,IMTConVPSGroup* group) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConVPS(void) {}
  };
//+------------------------------------------------------------------+
//| Time config events notification interface                        |
//+------------------------------------------------------------------+
class IMTConVPSSink
  {
public:
   virtual void      OnVPSUpdate(const IMTConVPS* /*config*/) {  }
   virtual void      OnVPSSync(void)                          {  }
  };
//+------------------------------------------------------------------+
