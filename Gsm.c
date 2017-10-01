#include "Sim80x.h"




//######################################################################################################
bool  Gsm_CallAnswer(void)
{
  uint8_t answer = Sim80x_SendAtCommand("ATA\r\n",20000,2,"ATA\r\r\nOK\r\n","ATA\r\r\nERROR\r\n");
  if(answer == 1)
    return true;
  else
    return false;
}
//######################################################################################################
bool  Gsm_CallDisconnect(void)
{
  uint8_t answer = Sim80x_SendAtCommand("AT+HVOIC\r\n",20000,2,"AT+HVOIC\r\r\nOK\r\n","AT+HVOIC\r\r\nERROR\r\n");
  if(answer == 1)
    return true;
  else
    return false;
}
//######################################################################################################
GsmVoiceCallReturn_t     Gsm_Dial(char *Number,uint8_t WaitForAnswer_second)
{
  char str[24];
  Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_Idle;
  snprintf(Sim80x.Gsm.DiallingNumber,sizeof(Sim80x.Gsm.DiallingNumber),"%s",Number);
  snprintf(str,sizeof(str),"ATD%s;\r\n",Number);
  uint8_t answer=Sim80x_SendAtCommand(str,WaitForAnswer_second*1000,5,"\r\n+COLP:","\r\nBUSY\r\n","\r\nNO DIALTONE\r\n","\r\nNO CARRIER\r\n","\r\nNO ANSWER\r\n");
  switch(answer)
  {
    case 0:
      Gsm_CallDisconnect();
      Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_Error;    
      return Sim80x.Gsm.GsmVoiceCallReturn;    
    case 1:
      Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_OK;
      return Sim80x.Gsm.GsmVoiceCallReturn;
    case 2:
      Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_Busy;
      return Sim80x.Gsm.GsmVoiceCallReturn;
    case 3:
      Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_NoDialTone;
      return Sim80x.Gsm.GsmVoiceCallReturn;
    case 4:
      Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_NoCarrier;
      return Sim80x.Gsm.GsmVoiceCallReturn;
    case 5:
      Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_NoAnswer;
      return Sim80x.Gsm.GsmVoiceCallReturn;
  }      
  Gsm_CallDisconnect();
  Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_Error;
  return Sim80x.Gsm.GsmVoiceCallReturn;    
}
//######################################################################################################


//######################################################################################################


//######################################################################################################



//######################################################################################################
//######################################################################################################
//######################################################################################################
GsmMsgFormat_t  Gsm_MsgGetFormat(void)
{
  uint8_t   answer;
  answer = Sim80x_SendAtCommand("AT+CMGF?\r\n",1000,2,"+CMGF: 0\r\n\r\nOK\r\n","+CMGF: 1\r\n\r\nOK\r\n");
  if(answer == 1)
  {
    Sim80x.Gsm.MsgFormat = GsmMsgFormat_PDU;
    return GsmMsgFormat_PDU;
  }
  else if(answer == 2)
  {
    Sim80x.Gsm.MsgFormat = GsmMsgFormat_Text;
    return GsmMsgFormat_Text;
  }
  else
    return GsmMsgFormat_Error;  
}
//######################################################################################################
bool  Gsm_MsgSetFormat(GsmMsgFormat_t GsmMsgFormat)
{
  uint8_t   answer;
  if(GsmMsgFormat == GsmMsgFormat_PDU)
    answer = Sim80x_SendAtCommand("AT+CMGF=0\r\n",1000,2,"AT+CMGF=0\r\r\nOK\r\n","AT+CMGF=0\r\r\nERROR\r\n");
  else  if(GsmMsgFormat == GsmMsgFormat_Text)
    answer = Sim80x_SendAtCommand("AT+CMGF=1\r\n",1000,2,"AT+CMGF=1\r\r\nOK\r\n","AT+CMGF=1\r\r\nERROR\r\n");
  else
    return false;
  if(answer == 1)
  {
    if(GsmMsgFormat == GsmMsgFormat_PDU)
      Sim80x.Gsm.MsgFormat = GsmMsgFormat_PDU;
    if(GsmMsgFormat == GsmMsgFormat_Text)
      Sim80x.Gsm.MsgFormat = GsmMsgFormat_Text;
    return true;    
  }
  else
    return false;  
}
//######################################################################################################
GsmMsgMemory_t  Gsm_MsgGetMemoryStatus(void)
{
  uint8_t   answer;
  answer = Sim80x_SendAtCommand("AT+CPMS?\r\n",1000,2,"AT+CPMS?\r\r\n+CPMS: \"SM\"","AT+CPMS?\r\r\n+CPMS: \"ME\"");
  if(answer == 1)
    Sim80x.Gsm.MsgMemory = GsmMsgMemory_OnSim;
  else if(answer == 2)
    Sim80x.Gsm.MsgMemory = GsmMsgMemory_OnModule;
  else
    Sim80x.Gsm.MsgMemory = GsmMsgMemory_Error;
  return  Sim80x.Gsm.MsgMemory; 
}
//######################################################################################################
bool  Gsm_MsgSetMemoryLocation(GsmMsgMemory_t GsmMsgMemory)
{
  uint8_t   answer;
  if(GsmMsgMemory == GsmMsgMemory_OnSim)
  {
    answer = Sim80x_SendAtCommand("AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n",1000,1,"\r\n+CPMS:");
    if(answer == 1)
    {
      Sim80x.Gsm.MsgMemory = GsmMsgMemory_OnSim;
      return true;
    }
    else
      return false;
  }
  if(GsmMsgMemory == GsmMsgMemory_OnModule)
  {
    answer = Sim80x_SendAtCommand("AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n",1000,1,"\r\n+CPMS:");
    if(answer == 1)
    {
      Sim80x.Gsm.MsgMemory = GsmMsgMemory_OnModule;
      return true;
    }
    else
      return false;
  }
  return false;
}
//######################################################################################################
GsmTECharacterSet_t     Gsm_MsgGetCharacterFormat(void)
{
  Sim80x.Gsm.TeCharacterFormat = (GsmTECharacterSet_t)Sim80x_SendAtCommand("AT+CSCS?\r\n",1000,7,"\r\n+CSCS: \"GSM\"\r\n","\r\n+CSCS: \"UCS2\"\r\n","\r\n+CSCS: \"IRA\"\r\n","\r\n+CSCS: \"HEX\"\r\n","\r\n+CSCS: \"PCCP\"\r\n","\r\n+CSCS: \"PCDN\"\r\n","\r\n+CSCS: \"8859-1\"\r\n");
  return Sim80x.Gsm.TeCharacterFormat;
}
//######################################################################################################
bool  Gsm_MsgSetCharacterFormat(GsmTECharacterSet_t GsmTECharacterSet)
{
  uint8_t answer;
  switch(GsmTECharacterSet)
  {
    case GsmTECharacterSet_Error:
    return false;
    case GsmTECharacterSet_GSM:
      answer = Sim80x_SendAtCommand("AT+CSCS=\"GSM\"\r\n",1000,2,"AT+CSCS=\"GSM\"\r\r\nOK\r\n","AT+CSCS=\"GSM\"\r\r\n+CME ERROR");  
    break;
    case GsmTECharacterSet_UCS2:
      answer = Sim80x_SendAtCommand("AT+CSCS=\"UCS2\"\r\n",1000,2,"AT+CSCS=\"UCS2\"\r\r\nOK\r\n","AT+CSCS=\"UCS2\"\r\r\n+CME ERROR");  
    break;
    case GsmTECharacterSet_IRA:
      answer = Sim80x_SendAtCommand("AT+CSCS=\"IRA\"\r\n",1000,2,"AT+CSCS=\"IRA\"\r\r\nOK\r\n","AT+CSCS=\"IRA\"\r\r\n+CME ERROR");  
    break;
    case GsmTECharacterSet_HEX:
      answer = Sim80x_SendAtCommand("AT+CSCS=\"HEX\"\r\n",1000,2,"AT+CSCS=\"HEX\"\r\r\nOK\r\n","AT+CSCS=\"HEX\"\r\r\n+CME ERROR");  
    break;
    case GsmTECharacterSet_PCCP:
      answer = Sim80x_SendAtCommand("AT+CSCS=\"PCCP\"\r\n",1000,2,"AT+CSCS=\"PCCP\"\r\r\nOK\r\n","AT+CSCS=\"PCCP\"\r\r\n+CME ERROR");  
    break;
    case GsmTECharacterSet_PCDN:
      answer = Sim80x_SendAtCommand("AT+CSCS=\"PCDN\"\r\n",1000,2,"AT+CSCS=\"PCDN\"\r\r\nOK\r\n","AT+CSCS=\"PCDN\"\r\r\n+CME ERROR");  
    break;
    case GsmTECharacterSet_8859_1:
      answer = Sim80x_SendAtCommand("AT+CSCS=\"8859-1\"\r\n",1000,2,"AT+CSCS=\"8859-1\"\r\r\nOK\r\n","AT+CSCS=\"8859-1\"\r\r\n+CME ERROR");  
    break;   
  }  
  Gsm_MsgGetCharacterFormat();
  if(answer == 1)
    return true;
  else
    return false;
}
//######################################################################################################
bool  Gsm_MsgRead(uint8_t index)
{
  uint8_t answer;
  char str[16];
  sprintf(str,"AT+CMGR=%d\r\n",index);
  answer = Sim80x_SendAtCommand(str,5000,1,"\r\nOK\r\n");
  if((answer == 1 ) && (Sim80x.Gsm.MsgReadIsOK==1))
    return true;
  else
    return false;    
}
//######################################################################################################
bool  Gsm_MsgDelete(uint8_t index)
{
  uint8_t answer;
  char str[16];
  char strP1[22];
  char strP2[22];
  sprintf(str,"AT+CMGD=%d\r\n",index);
  sprintf(strP1,"AT+CMGD=%d\r\r\nOK\r\n",index);
  sprintf(strP2,"AT+CMGD=%d\r\r\nERROR\r\n",index);
  answer = Sim80x_SendAtCommand(str,25000,2,strP1,strP2);
  if(answer == 1)
    return true;
  else
    return false;  
}
//######################################################################################################



//######################################################################################################

//######################################################################################################

//######################################################################################################

//######################################################################################################

