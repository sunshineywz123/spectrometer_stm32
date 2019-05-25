//-----------------------------------------------------------------
// ��������:
//    �ɼ�FPGA��ɲ�Ƶ�����ݣ��������ݲ���ʾ
// ��    ��: ���ǵ���
// ��ʼ����: 2014-02-18
// �������: 2014-02-18
// �޸�����:
// ��    ��: V1.0
//   - V1.0:
// ���Թ���: ����STM32+FPGA����ϵͳ��ƿ����塢LZE_ST LINK2
//           2.8��Һ�����ź�Դ��ʾ����
// ˵    ��:
//    - ����ʹ�õ�ϵͳʱ��Ƶ��Fsysclk=72MHz��
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include <stm32f10x.h>
#include <stdio.h>
#include "Freq_Test.h"
#include "SPI.h"
#include "SPFD5408.h"
//-----------------------------------------------------------------
// ȫ�ֱ���
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// ���ܳ�����
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// void Frequency_Calculations_Show (void)
//-----------------------------------------------------------------
//
// ��������: ʹ��spiͨ�ţ���FPGA��ȡ���ݣ�������ʾ��׼����������
//           ���������Ƶ�ʡ����ڡ��ߵ͵�ƽʱ�䡢ռ�ձ�
// ��ڲ���: 
// �� �� ֵ:
// ע������: ��FPGA��ȡ����ǰ��Ҫ����һ�������أ�FPGAʹ��100M��׼����ʱ��
//-----------------------------------------------------------------
void Frequency_Calculations_Show (void)
{
	u8 buf[16]; 
	float freq;
	float zq,zkb;
	u32  Flv_cnt_b;														// ��׼��������
	u32	 Flv_cnt_d;														// �����������
	u32  Flv_cnt_H;														// �ߵ�ƽ
//	u32  Flv_cnt_L;													// �͵�ƽ
	// ����һ�������ظ�FPGA���ٽ��ж�ȡ����
	SPI2_Send_Cmd_Data(128,0);	   							
	SPI2_Send_Cmd_Data(128,1);

	Flv_cnt_b = SPI2_Rece_Cmd_Data(2);	   			// 100Mhz��׼Ƶ�ʼ���ֵ
	Flv_cnt_d = SPI2_Rece_Cmd_Data(3);					// �ⲿ�ź��������ֵ
	Flv_cnt_H = SPI2_Rece_Cmd_Data(0);					// �ⲿ�ź���������ֵ
//	Flv_cnt_L = Recive_ldate (1);		
	// ��ʾ��׼����ֵ											     
	SPFD5408_Fill(86,74,214,90,BLACK);										  
	sprintf((char*)buf,"%10.0f",(float)Flv_cnt_b);
	LCD_WriteString(94,74,YELLOW,BLACK,buf);
	// ��ʾ�������
	SPFD5408_Fill(86,101,214,117,BLACK);										  
	sprintf((char*)buf,"%10.0f",(float)Flv_cnt_d);
	LCD_WriteString(94,101,YELLOW,BLACK,buf);
	// ����Ƶ��											  
	freq = (float)(100000000lu *( Flv_cnt_d * 1.0 / Flv_cnt_b));  
	//Ϊ������0.01%���ȣ�Ƶ��С��1Hz����ʾС���ĺ�5λ
	if(freq<1)														
	{ 
		// ��ʾƵ��
		SPFD5408_Fill(86,128,214,144,BLACK);								  
		sprintf((char*)buf,"%8.5f",(float)freq);		 
		LCD_WriteString(94,128,YELLOW,BLACK,buf);
		// ��ʾ����
		zq = (float)(Flv_cnt_b*1.0 / ( Flv_cnt_d*100));						 
		SPFD5408_Fill(86,155,214,171,BLACK);
		sprintf((char*)buf,"%8.3f",(float)zq);	 
		LCD_WriteString(94,155,YELLOW,BLACK,buf);
		// ��ʾ�ߵ�ƽ
		SPFD5408_Fill(86,182,214,198,BLACK);							
		sprintf((char*)buf,"%8.3f",(float)Flv_cnt_H/100);	
		LCD_WriteString(94,182,YELLOW,BLACK,buf);
		// �͵�ƽ
		SPFD5408_Fill(86,209,214,225,BLACK);							
		sprintf((char*)buf,"%6.3f",(float)zq-(float)Flv_cnt_H/100);
		LCD_WriteString(94,209,YELLOW,BLACK,buf);
	}
	// Ƶ����1Hz-10Hz����ʾС�����4λ
	else if(freq<10)									   
	{
		// ��ʾƵ��
		SPFD5408_Fill(86,128,214,144,BLACK);								  
		sprintf((char*)buf,"%8.4f",(float)freq);		 
		LCD_WriteString(94,128,YELLOW,BLACK,buf);
		// ����
		zq = (float)(Flv_cnt_b*1.0 / ( Flv_cnt_d*100));						 
		SPFD5408_Fill(86,155,214,171,BLACK);
		sprintf((char*)buf,"%8.4f",(float)zq);	 
		LCD_WriteString(94,155,YELLOW,BLACK,buf);
		// �ߵ�ƽ
		SPFD5408_Fill(86,182,214,198,BLACK);							
		sprintf((char*)buf,"%8.4f",(float)Flv_cnt_H/100);	
		LCD_WriteString(94,182,YELLOW,BLACK,buf);
		// �͵�ƽ
		SPFD5408_Fill(86,209,214,225,BLACK);							
		sprintf((char*)buf,"%6.4f",(float)zq-(float)Flv_cnt_H/100);
		LCD_WriteString(94,209,YELLOW,BLACK,buf);
	}
	// Ƶ�ʴ���10Hz����ʾС�����3λ
	else													  
	{
		// ��ʾƵ��
		SPFD5408_Fill(86,128,214,144,BLACK);								  
		sprintf((char*)buf,"%8.3f",(float)freq);		 
		LCD_WriteString(94,128,YELLOW,BLACK,buf);
		// ����
		zq = (float)(Flv_cnt_b*1.0 / ( Flv_cnt_d*100));						 
		SPFD5408_Fill(86,155,214,171,BLACK);
		sprintf((char*)buf,"%8.5f",(float)zq);	 
		LCD_WriteString(94,155,YELLOW,BLACK,buf);
		// �ߵ�ƽ
		SPFD5408_Fill(86,182,214,198,BLACK);							
		sprintf((char*)buf,"%8.5f",(float)Flv_cnt_H/100);	
		LCD_WriteString(94,182,YELLOW,BLACK,buf);
		// �͵�ƽ
		SPFD5408_Fill(86,209,214,225,BLACK);							
		sprintf((char*)buf,"%6.5f",(float)zq-(float)Flv_cnt_H/100);
		LCD_WriteString(94,209,YELLOW,BLACK,buf);
	}
	// ����ռ�ձ�
	zkb=(float)(Flv_cnt_H * 1.0 / zq);
	// ����ռ�ձ�
	zkb=0.02+zkb;
	// ��ʾռ�ձ�ֵС�����2λ
	SPFD5408_Fill(86,235,214,251,BLACK);						   
	sprintf((char*)buf,"%3.2f",(float)zkb);					  
	LCD_WriteString(94,235,YELLOW,BLACK,buf);
}

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
