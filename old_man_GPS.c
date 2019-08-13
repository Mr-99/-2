#include <HT66F70A.H>
#include "uart.h"
#include "oled.h"
void delay(unsigned char n);
#define Speak_Bus _pg0
#define Speak_Data _pg1
#define Speak_Rest _pg2
#define Key_1 _pd4//发消息
#define Key_2 _pd5//接电话
#define Key_3 _pd6//挂断
#define Key_4 _pd7//打电话
#define Pluse_key _pc2
unsigned char flag=0;
unsigned int p=0;
float p_temp=0;
char Ip_port[38]={"AT+CIPSTART=\"TCP\",\"123.207.179.29\",80"};
//char host_head[]={"Host:15p8s71074.imwork.net"};
char number_1[12]={"15328274799"};  //15223534036
struct SIM_dat
  {
  volatile unsigned char call;
  volatile unsigned char gprs;
  volatile unsigned char signar;
  volatile  unsigned char battery[4];
  volatile unsigned char pulse[5];
  volatile unsigned char home;
  volatile unsigned char userStatus;
  volatile unsigned char latitude[10]; 
  volatile unsigned char longitude[11];
  }SIM;
//按键初始化
void key_init(void)
     {
     _pdc4=in;
     _pdc5=in;
     _pdc6=in;
     _pdc7=in;
     _pcc2=in;
     _pdpu4=1;
     _pdpu5=1;   
     _pdpu6=1;
     _pdpu7=1;
     _pcpu2=1;	
     }
  //处理逗号
volatile unsigned char* Seek_comma(volatile unsigned char *prt,unsigned char n)//把接受数据逗号间数据分离出来
  {
    unsigned char i=0;
    for( ;*prt!='\0'; prt++)
         {
        if(*prt==',')
           {
            ++i;
            if(i==n)
             {
            return prt;
             }
           }
         }
    return 0;
  }
void clear_buff(void)
    {
    unsigned char i=0;
	accept_fi=0;
	for(i=0;i<100;i++)
	    {
	    uart_buff[i]=0;	
	    }
	}
//语音芯片
void speak(unsigned char z)
    {
	Speak_Rest=1;
	delay_us(200);
	Speak_Rest=0;
    delay_us(200);
    while(z--)
     {
     Speak_Data=1;
     delay_us(100);	
     Speak_Data=0;
     delay_us(100);	
     }
	}
void MCU_init(void)
{
    //看门狗除能
    _wdtc = 0b10101011;
      	_pfs0=0b01110111;
  	_pas2=0b00100000;
    _cp1c= 0b01000001;
}
//语音
void Speak_play_init(void)
   {
		   	speak(2);
	   		 delay_us(1000);
	   	while(!Speak_Bus);
	   	speak(5);
	   		 delay_us(1000);
	   	while(!Speak_Bus);
	   	speak(24);
	   		 delay_us(1000);
	   	while(!Speak_Bus);
	   		   	speak(23);
	   		 delay_us(1000);
	   	while(!Speak_Bus);
	}
unsigned char Check_Sim_data(char *p1,char *p2)
         {
         unsigned char flag=0,i=4,n_flag=0,h=10;
             clear_buff();
           while((!n_flag)&&(i--))
             {
             Uart_Send_str(p1,1);	
             delay_us(100000);
             delay_us(100000);	
             n_flag=Hand(p2);
             //心跳监测
             if(0==Pluse_key)
                {
                delay_us(100000);
      _int0e = 0;
       _t0on = 0;	 	    	 	
		_cp1f=0;
	 	    p=0;
        _cp1e=1; 
         speak(5);
	 	delay(15);
	 	_cp1e=0;            
       p_temp=p/2;
      p_temp= p_temp*4;
      if(p_temp<60)
           {
           p_temp=60+p_temp*(0.3);
           }
      if(p_temp>100)
           {
           p_temp=50+p_temp*(0.4);
           }
      SIM.pulse[0]=((unsigned char)p_temp/100+0x30);
      SIM.pulse[1]=((unsigned char)p_temp/10%10+0x30);
      SIM.pulse[2]=((unsigned char)p_temp%10+0x30);
      SIM.pulse[3]=0;
      speak(5);	
      while(!Pluse_key);
      _int0e = 1;
       _t0on = 1;
                }
             //拨打电话
             if(0==Key_4)
                {
                		_emi = 0;
             	Uart_Send_str("AT",0);
             	delay_us(100000);
             	 Uart_Send_str("AT",0);
             	delay_us(100000);
	   		 clear_buff();
	   		              		_emi = 1;
	   		 flag=0;
             h=10;
	   		 while(!flag&&(h--))
	   		  {
             Uart_Send_str("ATD",0);
	 		 Uart_Send_str(number_1,0);
	 		 Uart_Send_str(";",1);//分号
	         delay_us(100000);
	         flag=Hand("OK");
	         if(flag)
	            {
	          speak(52);
	   		 delay_us(1000);
	   		 while(!Speak_Bus);
	            }
	   		  }
	   		 clear_buff();
	  		 while(1)
	  		 {
	  		 if(Hand("NO CARRIER"))
                      {
                    speak(46);	
                    delay_us(1000);
    		        while(!Speak_Bus); 
    		    _emi = 0;        
               clear_buff();
               	_emi = 1;
    		        break;	
                      }	
                if(0==Key_3)
                    {
                flag=0;
                h=10;
                _emi = 0;        
               clear_buff();
               	_emi = 1;
	   			while(!flag&&(h--))
	   			{
                Uart_Send_str("ATH",1);
                delay_us(100000);
                delay_us(100000);
                flag=Hand("OK");
	   			}              
	   				speak(46);	
                    delay_us(1000);
    		        while(!Speak_Bus);
    		    _emi = 0;        
               clear_buff();
               	_emi = 1;
    		        break;
                    }
	  		 }	_emi = 0;
              clear_buff(); 
              	_emi = 1;
                }
             //END	
             //发送消息处理
             if(0==Key_1)
                 {
                Uart_Send_str("AT+CMGD=1,4",1);
                speak(7);
	   		 	delay_us(1000);
	   			while(!Speak_Bus);
                delay_us(100000);
                delay_us(100000);
                while(!Key_1);
                speak(7);
	   		 	delay_us(1000);
	   			while(!Speak_Bus);
	   			flag=0;
	   			while(!flag)
	   			{
	   			clear_buff();
                Uart_Send_str("AT+CMGF=0",1);
                delay_us(100000);
                flag=Hand("OK");
	   			}
	   			flag=0;
	   			while(!flag)
	   			{
	   			clear_buff();
                Uart_Send_str("AT+CMGS=61",1);
                delay_us(100000);
                flag=Hand(">");
	   			}
                Uart_Send_str("0891683108100005F011000D91685122534330F60008B02E5C0A656C76847528623760A8597DFF0180014EBA53EF80FD9700898160A876845E2E52A9FF0C8BF751736CE8FF01",0);
	  	   		delay_us(100000);
	  	   		  urat_send(0x1a);//16进制
	  	   		  urat_send(0x0d);
	  	          urat_send(0x0a);
	  	          		delay_us(100000);
	  	          		delay_us(100000);
	  	          		delay_us(100000);
	  		      _emi = 0;
               clear_buff(); 
                	_emi = 1;
                 }
             //END
             //通话处理
             if( Hand("RING"))
                  {
                  		_emi = 0;
             	delay_us(100000);
                Uart_Send_str("AT",0);
             	delay_us(100000);
             	                Uart_Send_str("AT",0);
             	delay_us(100000);
             	                Uart_Send_str("AT",0);
             	delay_us(100000);
              speak(28);
              delay_us(1000);	
    		  while(!Speak_Bus);
    		  clear_buff();
              while(1)
                {
                if(0==Key_2)
                    {  
                h=10;
	   			while(h--)
	   			{
                Uart_Send_str("ATA",1);
                delay_us(100000);
	   			}
                    }
                if(0==Key_3)
                    {
                h=10;
	   			while(h--)
	   			{
                Uart_Send_str("ATH",1);
                delay_us(100000);
	   			}
				    speak(46);	
                    delay_us(1000);
    		        while(!Speak_Bus);
    		    _emi = 0;
              clear_buff(); 
              	_emi = 1;
    		        break;
                    } 
                     _emi = 1;
                if(Hand("NO CARRIER"))
                      {
                    speak(46);	
                    delay_us(1000);
    		        while(!Speak_Bus);
    		     _emi = 0;
              clear_buff(); 
              	_emi = 1;
    		        break;	
                      } 
                      _emi = 0;
                }
                _emi = 0;
              clear_buff(); 
              	_emi = 1; }
             }  
         return n_flag;
         }
//获取电量
void V_get(void)//百分比
    {
      const volatile unsigned char *prt; 
      unsigned char i=0;
     if(Check_Sim_data("AT+CBC","OK"))
     {
     	SIM.battery[0] = 0;
        SIM.battery[1] = 0;
        SIM.battery[2] = 0;
        SIM.battery[3] = 0;
     prt=Seek_comma(uart_buff,1);
    for(++prt;*prt!=',';prt++)
         {
      if((0x30<=(*prt))&&((*prt)<=0x39))
           {
         SIM.battery[i]=(*prt);
            }
      else
           {
         SIM.battery[i]=0;
           }
          i++;
         }
     }
        
    }
//////////////GPRS发送数据///////////////////////
void GPRS_send(void)
     {
    if(Check_Sim_data("AT+CREG?","+CREG: 0,1"))//网络注册情况
    {
     SIM.signar=1;
    }
    else
    {
     SIM.signar=0;	
    }
    if(Check_Sim_data("AT+CIICR","OK"))//激活移动场景
        {
        }
  Uart_Send_str("AT+CIFSR",1);
  delay_us(100000);	
  if(Check_Sim_data(Ip_port,"CONNECT OK"))//启动方式，冷启动
   {
     //出错
   } 
///////////////////////////
  if(Check_Sim_data("AT+CIPSEND",">"))//启动方式，冷启动
   {   
   //发送数据标志
   SIM.gprs=1;
   //END
	//构建GET数据格式
	Uart_Send_str("GET",0);
	urat_send(0x20);
	Uart_Send_str("http://123.207.179.29/app/suitcase/saveSuitcaseInfo?data=%7B",0);//%7B({)
	Uart_Send_str("%22longitude%22:",0);
	Uart_Send_str(SIM.longitude,0); 
	urat_send(0x2c);
	Uart_Send_str("%22latitude%22:",0);
	Uart_Send_str(SIM.latitude,0);
	urat_send(0x2c);
	Uart_Send_str("%22pulse%22:",0);
	Uart_Send_str(SIM.pulse,0);
	urat_send(0x2c);
	Uart_Send_str("%22battery%22:",0);
	Uart_Send_str(SIM.battery,0);
	urat_send(0x2c);
	Uart_Send_str("%22home%22:",0);
	urat_send(SIM.home+0x30);
	urat_send(0x2c);
	Uart_Send_str("%22userStatus%22:",0);
	urat_send(SIM.userStatus+0x30);
	urat_send(0x2c);
	Uart_Send_str("%22altitude%22:",0);
	Uart_Send_str("123",0);
	Uart_Send_str("%7D",0);
	urat_send(0x20);
	Uart_Send_str("HTTP/1.1",1);
	  	    urat_send(0x0d);//回车
	  	urat_send(0x0a);//换行
	  	  	    urat_send(0x0d);//回车
	  	urat_send(0x0a);//换行
   //END
////////构建GET结束   
   urat_send(0x1a);
   	  	 urat_send(0x0d);//回车
	  	urat_send(0x0a);//换行
   delay_us(100000);
   }
  else
  {
  SIM.gprs=0;
   if(Check_Sim_data("AT+CGATT=1","OK"))//指示GPRS附着状态
          {}
   if(Check_Sim_data("AT+CGACT=1,1","OK"))//激活PDP
          {}
   if(Check_Sim_data("AT+CIPSHUT","SHUT OK"))//关闭移动场景
          {}
     if(Check_Sim_data("AT+CIICR","OK"))//激活移动场景
         {}
	
  }
	 }
//////////////END
////////////GPS操作函数/////////////
//GPS初始化
void GPS_Init(void)
  { 
    if(Check_Sim_data("AT+CGPSPWR=1","OK"))//指示GPRS附着状态
    {
   //出错
    }
  }
//把GPS数据保存到结构体
void GPS_Get(void)
 {
    volatile unsigned char *prt;
    unsigned char i=0;
   if(Check_Sim_data("AT+CGPSINF=0","OK"))//
    { 
  prt=Seek_comma(uart_buff,1);
    for(++prt;*prt!=',';prt++)
      {
        SIM.latitude[i++]=*prt;
      }
      i=0;
    prt=Seek_comma(uart_buff,2);
    for(++prt;*prt!=',';prt++)
      {
        SIM.longitude[i++]=*prt;
      }
      i=0;
    //prt=Seek_comma(SIM_buff,3);
    //for(++prt;*prt!=',';prt++)
     // {
     //   SIM.altitude[i++]=*prt;
      //}
        //SIM.altitude[10]='\0';
    if((SIM.latitude[0]==0x30)||(SIM.latitude[0]==0))
     {
      SIM.latitude[2]='1';
      SIM.latitude[3]=0;
//
      SIM.longitude[2]='1';
      SIM.longitude[3]=0;
     }
        SIM.latitude[9]=0;
        SIM.longitude[10]=0;
   }
 }
void GPS(void)
     {
    GPS_Init();
	GPS_Get();
	 }
//进度条
void jingdutiao(void)
  {
  	unsigned char i=0,flag=0;		  		
OLED_Set_Pos(i,7);
for(i=0;i<128;i++)
   {
	OLED_WR_Byte(0x81,OLED_DATA);
   }
i=0;
OLED_Set_Pos(i,7);
for(i=0;i<128;i++)
   {
	OLED_WR_Byte(0xff,OLED_DATA);
if(1==Speak_Bus)
{
	switch(flag)
	   {
	   	 		case 0:
	   	speak(2);
		flag=1;
		break;
			   	case 1:
	   	speak(5);
		flag=2;
		break;
			   	case 2:
	   	speak(24);
		flag=3;
		break;
			   	case 3:
	   	speak(23);
		flag=66;
		break;	   	
	   	}
}
	if((1==Speak_Bus)&&(flag==0))
	      {

		     if((1==Speak_Bus)&&(flag==1))
	      {
		 speak(5);
		 flag=2;
		 	if((1==Speak_Bus)&&(flag==2))
	      {
		 speak(24);
		 flag=3;
		 
	if((1==Speak_Bus)&&(flag==3))
	      {
		 speak(23);
		 flag=66;
		  }
		  }
		  }
		  }


		  
	          if((i>=50)&&(i<=68))
              {
      Check_Sim_data("AT","OK");//适应波特率


              }
                            else
              {
              delay_ms(25);	
              }
          if((i>=70)&&(i<=75))
              {
            Uart_Send_str("AT+IPR=9600",1);
                  if(Check_Sim_data("AT+CGPSPWR=1","OK"))//gps开启
             {
   			//出错
             }
              }
                            else
              {
              delay_ms(25);		
              }
      if((i>=90)&&(i<=95))
              {
      Uart_Send_str("AT+CIPQSEND=1",1);
      Uart_Send_str("ATE0",1);//关闭回显
              	    if(Check_Sim_data("AT+CREG?","+CREG: 0,1"))//网络注册情况
    {
     SIM.signar=1;
    }
    else
    {
    SIM.signar=0;	
    }	
              }
              else
              {
              delay_ms(25);	
              }
	 	       
   }
  }
//界面数据获取
void temp_data(void)
  {
  		            if((SIM.latitude[0]==0x30)||(SIM.latitude[0]==0))
                 {
                 OLED_ShowCHinese(2,0,GPS_clear);//GPS图标	
                 }
                 else
                 {
                 OLED_ShowCHinese(2,0,GPS_tubiao);//GPS图标
                 }
            if(1==SIM.gprs)
                 {
                 OLED_ShowCHinese(5,0,wangluo);//网络	
                 }
               else
                 {
               	OLED_ShowCHinese(5,0,GPS_clear);//网络	
                 }
             if(1==SIM.signar)
                 {
                 signar(15,0,1);//手机信号格子数	
                 }
               else
                 {
                 signar(15,0,0);//手机信号格子数	
                 }	
  	              V_get();
                OLED_num16X32(0,1,SIM.battery[0]-0x30);
    			OLED_num16X32(1,1,SIM.battery[1]-0x30);
  }
/////////END
void delay(unsigned char n)
   {
  while(n--)
  {
   	delay_ms(100);
   	delay_ms(100);
   	delay_ms(100);
   	delay_ms(100);
   	delay_ms(100);
   	delay_ms(100);
   	delay_ms(100);
   	delay_ms(100);
   	delay_ms(100);
   	delay_ms(100);
  }
   	
    }
void main()

 {
	MCU_init();
	key_init();
//GPS数据初始化
SIM.latitude[2]='1';
SIM.latitude[3]=0;
//
SIM.longitude[2]='1';
SIM.longitude[3]=0;
//END
	//OLED
	OLED_Init();
	OLED_Clear();
	//END
	////////////////串口初始化
	time_init();
	interrupt_int0_init();
	//END
	//LED
	_pfc2=out;
	_pfc5=out;
	LED1=0;
	LED2=0;
	//END
	//语音
	 Speak_Rest=0;
	 Speak_Data=0;
	_pgpu0=1;
	_pgc0=in;
	_pgc1=out;
	_pgc2=out;
    //END
	_emi = 1;
	 SIM.pulse[0]=(0+0x30);
     SIM.pulse[1]=(0+0x30);
     SIM.pulse[2]=(0+0x30);
     SIM.pulse[3]=0;
	////////////////////////
	   		 //delay_us(1000);
	   	//while(!Speak_Bus);
	   	//speak(24);
		xitong();
	jingdutiao();
_emi = 0;
////////////////////////////////
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
	OLED_ShowChar(0,0,char_G);
	OLED_ShowChar(1,0,char_P);
	OLED_ShowChar(2,0,char_S);
    //OLED_ShowChar(3,0,char_douhao);//:
    //OLED_ShowCHinese(2,0,GPS_tubiao);//GPS图标
    OLED_ShowChar(14,0,char_xinghao);

    //OLED_ShowCHinese(5,0,wangluo);//网络
    OLED_ShowCHinese(0,1,CHinese_dian);
    OLED_ShowCHinese(2,1,CHinese_liang);
	OLED_baifenhao();
    OLED_ShowCHinese(5,1,CHinese_mai);
    OLED_ShowCHinese(7,1,CHinese_bo);
    OLED_maibo_P();//脉搏P
///////////////////////////////
	   		_emi = 1;
    V_get();
temp_data();
OLED_num16X32(4,1,0);
OLED_num16X32(5,1,0);
OLED_num16X32(6,1,0);
		   	speak(5);
	   		 delay_us(1000);
	   	while(!Speak_Bus);
	   		 speak(4);
	   		 delay_us(1000);
	   	while(!Speak_Bus);
///////////////////////////////////////////////////	

	while(1)
	 {    
	  	 	SIM.home=1;
		    SIM.userStatus=1;
		    //
	 	    GPS();
	 	    GPRS_send();
            	_emi = 0;
            OLED_num16X32(4,1,SIM.pulse[0]-0x30); 
            OLED_num16X32(5,1,SIM.pulse[1]-0x30);
            OLED_num16X32(6,1,SIM.pulse[2]-0x30);
            //
            temp_data();
            	_emi = 1;

              
	 }
 }
void __attribute__((interrupt(0x10))) comparator_1(void)
   {
   delay_us(50000);
   p++; 
   if(p>=1000)
      {
      p=0;	
      	}
   _cp1f=0;	
   }