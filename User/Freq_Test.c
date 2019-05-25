//-----------------------------------------------------------------
// 程序描述:
//    采集FPGA完成测频的数据，计算数据并显示
// 作    者: 凌智电子
// 开始日期: 2014-02-18
// 完成日期: 2014-02-18
// 修改日期:
// 版    本: V1.0
//   - V1.0:
// 调试工具: 凌智STM32+FPGA电子系统设计开发板、LZE_ST LINK2
//           2.8寸液晶、信号源、示波器
// 说    明:
//    - 调试使用的系统时钟频率Fsysclk=72MHz；
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include <stm32f10x.h>
#include <stdio.h>
#include "Freq_Test.h"
#include "SPI.h"
#include "SPFD5408.h"
//-----------------------------------------------------------------
// 全局变量
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// 功能程序区
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// void Frequency_Calculations_Show (void)
//-----------------------------------------------------------------
//
// 函数功能: 使用spi通信，从FPGA读取数据，计算显示标准计数个数、
//           待测计数、频率、周期、高低电平时间、占空比
// 入口参数: 
// 返 回 值:
// 注意事项: 从FPGA读取数据前先要发送一个上升沿，FPGA使用100M标准计数时钟
//-----------------------------------------------------------------
void Frequency_Calculations_Show (void)
{
	u8 buf[16]; 
	float freq;
	float zq,zkb;
	u32  Flv_cnt_b;														// 标准计数个数
	u32	 Flv_cnt_d;														// 待测计数个数
	u32  Flv_cnt_H;														// 高电平
//	u32  Flv_cnt_L;													// 低电平
	// 发送一个上升沿给FPGA，再进行读取数据
	SPI2_Send_Cmd_Data(128,0);	   							
	SPI2_Send_Cmd_Data(128,1);

	Flv_cnt_b = SPI2_Rece_Cmd_Data(2);	   			// 100Mhz标准频率计数值
	Flv_cnt_d = SPI2_Rece_Cmd_Data(3);					// 外部信号输入计数值
	Flv_cnt_H = SPI2_Rece_Cmd_Data(0);					// 外部信号输入脉宽值
//	Flv_cnt_L = Recive_ldate (1);		
	// 显示标准计数值											     
	SPFD5408_Fill(86,74,214,90,BLACK);										  
	sprintf((char*)buf,"%10.0f",(float)Flv_cnt_b);
	LCD_WriteString(94,74,YELLOW,BLACK,buf);
	// 显示待测计数
	SPFD5408_Fill(86,101,214,117,BLACK);										  
	sprintf((char*)buf,"%10.0f",(float)Flv_cnt_d);
	LCD_WriteString(94,101,YELLOW,BLACK,buf);
	// 计算频率											  
	freq = (float)(100000000lu *( Flv_cnt_d * 1.0 / Flv_cnt_b));  
	//为了满足0.01%精度，频率小于1Hz，显示小数的后5位
	if(freq<1)														
	{ 
		// 显示频率
		SPFD5408_Fill(86,128,214,144,BLACK);								  
		sprintf((char*)buf,"%8.5f",(float)freq);		 
		LCD_WriteString(94,128,YELLOW,BLACK,buf);
		// 显示周期
		zq = (float)(Flv_cnt_b*1.0 / ( Flv_cnt_d*100));						 
		SPFD5408_Fill(86,155,214,171,BLACK);
		sprintf((char*)buf,"%8.3f",(float)zq);	 
		LCD_WriteString(94,155,YELLOW,BLACK,buf);
		// 显示高电平
		SPFD5408_Fill(86,182,214,198,BLACK);							
		sprintf((char*)buf,"%8.3f",(float)Flv_cnt_H/100);	
		LCD_WriteString(94,182,YELLOW,BLACK,buf);
		// 低电平
		SPFD5408_Fill(86,209,214,225,BLACK);							
		sprintf((char*)buf,"%6.3f",(float)zq-(float)Flv_cnt_H/100);
		LCD_WriteString(94,209,YELLOW,BLACK,buf);
	}
	// 频率在1Hz-10Hz，显示小数点后4位
	else if(freq<10)									   
	{
		// 显示频率
		SPFD5408_Fill(86,128,214,144,BLACK);								  
		sprintf((char*)buf,"%8.4f",(float)freq);		 
		LCD_WriteString(94,128,YELLOW,BLACK,buf);
		// 周期
		zq = (float)(Flv_cnt_b*1.0 / ( Flv_cnt_d*100));						 
		SPFD5408_Fill(86,155,214,171,BLACK);
		sprintf((char*)buf,"%8.4f",(float)zq);	 
		LCD_WriteString(94,155,YELLOW,BLACK,buf);
		// 高电平
		SPFD5408_Fill(86,182,214,198,BLACK);							
		sprintf((char*)buf,"%8.4f",(float)Flv_cnt_H/100);	
		LCD_WriteString(94,182,YELLOW,BLACK,buf);
		// 低电平
		SPFD5408_Fill(86,209,214,225,BLACK);							
		sprintf((char*)buf,"%6.4f",(float)zq-(float)Flv_cnt_H/100);
		LCD_WriteString(94,209,YELLOW,BLACK,buf);
	}
	// 频率大于10Hz，显示小数点后3位
	else													  
	{
		// 显示频率
		SPFD5408_Fill(86,128,214,144,BLACK);								  
		sprintf((char*)buf,"%8.3f",(float)freq);		 
		LCD_WriteString(94,128,YELLOW,BLACK,buf);
		// 周期
		zq = (float)(Flv_cnt_b*1.0 / ( Flv_cnt_d*100));						 
		SPFD5408_Fill(86,155,214,171,BLACK);
		sprintf((char*)buf,"%8.5f",(float)zq);	 
		LCD_WriteString(94,155,YELLOW,BLACK,buf);
		// 高电平
		SPFD5408_Fill(86,182,214,198,BLACK);							
		sprintf((char*)buf,"%8.5f",(float)Flv_cnt_H/100);	
		LCD_WriteString(94,182,YELLOW,BLACK,buf);
		// 低电平
		SPFD5408_Fill(86,209,214,225,BLACK);							
		sprintf((char*)buf,"%6.5f",(float)zq-(float)Flv_cnt_H/100);
		LCD_WriteString(94,209,YELLOW,BLACK,buf);
	}
	// 计算占空比
	zkb=(float)(Flv_cnt_H * 1.0 / zq);
	// 修正占空比
	zkb=0.02+zkb;
	// 显示占空比值小数点后2位
	SPFD5408_Fill(86,235,214,251,BLACK);						   
	sprintf((char*)buf,"%3.2f",(float)zkb);					  
	LCD_WriteString(94,235,YELLOW,BLACK,buf);
}

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
