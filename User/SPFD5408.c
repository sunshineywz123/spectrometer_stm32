//-----------------------------------------------------------------
// ��������:
//      ����FSMC����SPFD5408�ں˵�2.8��Һ���������ӳ���
// ��    ��: ���ǵ���
// ��ʼ����: 2014-01-24
// �������: 2014-01-26
// �޸�����: 2014-02-15
// ��ǰ�汾: V1.0.1
// ��ʷ�汾: 
// - V1.0.0: (2014-2-7)����Һ����ʾ����Ӣ�ģ������棬ͼƬ��ʾ
// - V1.0.1: (2014-2-15)ģ��IO�ͼ���FSMC�����ֿ�
// ���Թ���: ����STM32+FPGA����ϵͳ��ƿ����塢2.8��Һ������LZE_ST_LINK2
// ˵    ��:
//       (1) ����ʹ�õ�ϵͳʱ��Ƶ��Fsysclk=72MHz
//		 	 (2) ģ�������������ʽ:FSMC����ͨIO���ƣ�ͨ�����������л� 2014-1-24
//		   (3) ֧����Ӣ����ʾ�ַ��������� 2014-1-26
//       (4) ��ʹ�ù����п��������л���������ʾ  2014-1-27
//			 (5) ����֧��uCGUI�ĵ���,�ܹ���д���ص���ɫֵ 2014-1-27
//			 (6) ����败�������ʵ�ֲ˵�������Ϳѻ��
//			 (7) ֧����ĻУ׼
//			 (8) ***֧����ʾBMP��JPG�ȸ�ʽͼƬ
//			 (9) ***֧�ִӴ洢��(SD����FLASH)�϶�ȡ������������ʾ
//			 (10) ֧�ֿ��ر���� 2014-1-27
//			 (11) ֧�ֵ�����ʾ,���ַ�����ͼƬ����ʾ��������ʵ�ַ��ı���ɫ
//			 (12) ֧��ͼƬ���ַ��Ĺ�����ʾ��û����Ӱ,֧��ͼƬ������ʾ
//			 (13) ֧����ʾ��Ƶ�ļ�
//			 (14) ֧��ʵʱ��ʾͼ����Ϣ��������
//       (15) ֧�ֶ��ο�����Ϊ�ͻ�Ӧ�������ӿ�
//       (16)
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include "SPFD5408.h"	
#include "16x8.h"	
#include "GB1616.h"		 
#include "GB2424.h"

//-----------------------------------------------------------------
// ��ʼ��������
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// void GPIO_SPFD5408_Configuration(void)
//-----------------------------------------------------------------
//
// ��������: Һ���������ú���
// ��ڲ���: ��
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: RCC_APB2PeriphClockCmd(); GPIO_Init();
// ע������: ��GPIO������ʽ��FSMC������ʽ�µ��������ò�һ��
//			
//-----------------------------------------------------------------
void GPIO_SPFD5408_Configuration(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;

	// ʹ��IO��ʱ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | 
													RCC_APB2Periph_GPIOE, ENABLE);  
	// ���������ߡ����ֿ���������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 	| GPIO_Pin_1 	|	// D2,D3 
																GPIO_Pin_4 	| GPIO_Pin_5 	| // RD,WR
																GPIO_Pin_7 	| GPIO_Pin_8 	| // CS,D13
																GPIO_Pin_9 	| GPIO_Pin_10 | // D14,D15
																GPIO_Pin_11 | GPIO_Pin_14 | // RS,D0
																GPIO_Pin_15;								// D1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// ���츴�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;						 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	// ��������������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 	| GPIO_Pin_8 	|	// D4,D5
																GPIO_Pin_9 	| GPIO_Pin_10 | // D6,D7
																GPIO_Pin_11 | GPIO_Pin_12 | // D8,D9
																GPIO_Pin_13 | GPIO_Pin_14 | // D10,D11
																GPIO_Pin_15;								// D12
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;						// ���츴�����
	GPIO_Init(GPIOE, &GPIO_InitStructure); 
  // Һ����λ���������������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;	// RST,LED
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;					// �������
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	SPFD5408_LED_OFF;            															// LED��
}  
//-----------------------------------------------------------------
// void STM32_FSMC_Init(void)
//-----------------------------------------------------------------
//
// ��������: ����FSMC
// ��ڲ���: ��
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: ��
// ע������: ��
//
//-----------------------------------------------------------------
void STM32_FSMC_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
	// ��FSMCʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);	
	// �򿪸��ù���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	

  // FSMC_Bank1_NORSRAM1 ʱ������
  // ��ַ����ʱ������   
	p.FSMC_AddressSetupTime = 0x02;											                                        
	// ��ַ�ĳ���ʱ�� 
	p.FSMC_AddressHoldTime = 0x00;											
	// �趨����ʱ������
	p.FSMC_DataSetupTime = 0x05;												
 	// ����ת��ʱ��
	p.FSMC_BusTurnAroundDuration = 0x00;							
	// CLKʱ������źŵ�HCLK��������ʾʱ��
  p.FSMC_CLKDivision = 0x00;			
	// ָ���ڻ�õ�һ������ǰ��ʱ������
  p.FSMC_DataLatency = 0x00;			
  p.FSMC_AccessMode = FSMC_AccessMode_B;
	// ָ����FSMC��
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	// ��ַ������ֵ�����õ��������� 
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = 
									FSMC_DataAddressMux_Disable; 
	// �ⲿ�洢��������
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = 
														FSMC_MemoryType_NOR;
	// ���ݿ��
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = 
														FSMC_MemoryDataWidth_16b;
	// ����ͻ������ģʽ
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = 
														FSMC_BurstAccessMode_Disable;
	// ָ���ȴ��źŵļ���
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = 
														FSMC_WaitSignalPolarity_Low;
	// disables the Wrapped burst access mode for Flash
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = 
														FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = 
														FSMC_WaitSignalActive_BeforeWaitState;
	// ����ָ����FSMC���д����
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation =
														FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = 
														FSMC_WaitSignal_Disable;
	// ��չģʽ
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = 
														FSMC_ExtendedMode_Disable;
	// ����дͻ������
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = 
														FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;      
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
  // ʹ�� FSMC Bank1_SRAM 
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

//-----------------------------------------------------------------
// void SPFD5408_Init(void)
//-----------------------------------------------------------------
//
// ��������: Һ����ʼ������
// ��ڲ���: ��
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: SPFD5408_Delay(), SPFD5408_Write_Data(), 
// 					 SPFD5408_Write_Reg (). SPFD5408_WR_Start().   
// ע������: ��
//-----------------------------------------------------------------
void SPFD5408_Init(void)
{	 
	GPIO_SPFD5408_Configuration();						// IO����
	STM32_FSMC_Init();												// FSMC��ʼ��
	SPFD5408_RST_Set;													// �½��ظ�λ	
	SPFD5408_Delay(10);
	SPFD5408_RST_Clr;								 	   	
	SPFD5408_Delay(10);
	SPFD5408_RST_Set;								 	 
	SPFD5408_Delay(10); 
	
	SPFD5408_Write_Data(0xffff);							// ������ȫ��
	SPFD5408_Write_Reg(0x0000,0x0001);   			// ֹͣ�ڲ��������� 
	SPFD5408_Write_Reg(0x0001,0x0100);   			// �����������

#if ID_AM==000    
	SPFD5408_Write_Reg(0x0003,0x1000);				// ������ʾ��ˮƽ����ֱ�ݼ� 
#elif ID_AM==001
	SPFD5408_Write_Reg(0x0003,0x1008);				// ������ʾ��ˮƽ����ֱ�ݼ�  
#elif ID_AM==010
	SPFD5408_Write_Reg(0x0003,0x1010);				// ������ʾ��ˮƽ��������ֱ�ݼ�
#elif ID_AM==011 
	SPFD5408_Write_Reg(0x0003,0x1018);				// ������ʾ��ˮƽ��������ֱ�ݼ�
#elif ID_AM==100
	SPFD5408_Write_Reg(0x0003,0x1020);				// ������ʾ��ˮƽ�ݼ�����ֱ����
#elif ID_AM==101
	SPFD5408_Write_Reg(0x0003,0x1028);				// ������ʾ��ˮƽ�ݼ�����ֱ���� 
#elif ID_AM==110
	SPFD5408_Write_Reg(0x0003,0x1030);				// ������ʾ��ˮƽ����ֱ����
#elif ID_AM==111
	SPFD5408_Write_Reg(0x0003,0x1038);				// ������ʾ��ˮƽ����ֱ����
#endif
//																					// D3.AMѡ��ֱ(1)/ˮƽɨ��(0)   
//												    							// D5(ID1):D4(ID0):���Ƽ�������������
	SPFD5408_Write_Reg(0x0004,0x0000);   			// ���¶�λ��ʾ�ߴ���ƼĴ���
	SPFD5408_Write_Reg(0x0008,0x0207);   			// ��ʾ����2 	
	SPFD5408_Write_Reg(0x0009,0x0000);   
	SPFD5408_Write_Reg(0x000a,0x0000);   			 	
	SPFD5408_Write_Reg(0x000C,0x0000);   			// RGB��ʾ�ӿڿ���
	SPFD5408_Write_Reg(0x000D,0x0000);   
	SPFD5408_Write_Reg(0x000F,0x0000);   			 	
	SPFD5408_Write_Reg(0x0007,0x0101);
	// ��Դ����   
	SPFD5408_Write_Reg(0x0010,0x12b0);   
	SPFD5408_Write_Reg(0x0011,0x0007);   
	SPFD5408_Write_Reg(0x0017,0x0001);   
   					 	
	SPFD5408_Write_Reg(0x0012,0x01bb);   
	SPFD5408_Write_Reg(0x0013,0x1c00);   
	SPFD5408_Delay(50);   						 	
	SPFD5408_Write_Reg(0x0029,0x0019);				     
	SPFD5408_Delay(50);   						 	
	SPFD5408_Write_Reg(0x0030,0x0102);   
	SPFD5408_Delay(50);
	   
	// GAMMA����						 	
	SPFD5408_Write_Reg(0x0031,0x0c21);				     
	SPFD5408_Write_Reg(0x0032,0x0b22);			   	
	SPFD5408_Write_Reg(0x0033,0x2610);				  	
	SPFD5408_Write_Reg(0x0034,0x1e0b);				    
	SPFD5408_Write_Reg(0x0035,0x0a04);   
	SPFD5408_Write_Reg(0x0036,0x1701);   
	SPFD5408_Write_Reg(0x0037,0x0617);   
	SPFD5408_Write_Reg(0x0038,0x0305);   
	SPFD5408_Write_Reg(0x0039,0x0a05);   			 	
	SPFD5408_Write_Reg(0x003a,0x0c04);			
	SPFD5408_Write_Reg(0x003b,0x0c00);			    
	SPFD5408_Write_Reg(0x003c,0x000c); 			  
	SPFD5408_Write_Reg(0x003d,0x050c);				
	SPFD5408_Write_Reg(0x003e,0x0204);    
	SPFD5408_Write_Reg(0x003f,0x0404); 
	
	// ��ʾ�ߴ�����  
	SPFD5408_Write_Reg(0x0050,0x0000); 				// HSA7~HSA0  
	SPFD5408_Write_Reg(0x0051,239);   	    	// HEA7~HEA0
	SPFD5408_Write_Reg(0x0052,0x0000);     		// VSA8~VSA0
	SPFD5408_Write_Reg(0x0053,319); 					// VEA8~VEA0
	 
	// ��ɨ����� 			 	
	SPFD5408_Write_Reg(0x0060,0x2700);		 		  
	SPFD5408_Write_Reg(0x0061,0x0001);     		 
	SPFD5408_Write_Reg(0x006A,0x0000);   			 	
	
	SPFD5408_Write_Reg(0x0080,0x0000);   			// ����ͼ��1��ʾλ��
	SPFD5408_Write_Reg(0x0081,0x0000);   			// ����ͼ��1RAM��ʼ��ַ
	SPFD5408_Write_Reg(0x0082,0x0000);   			// ����ͼ��1RAM��ֹ��ַ
	SPFD5408_Write_Reg(0x0083,0x0000);   			// ����ͼ��2��ʾλ��
	SPFD5408_Write_Reg(0x0084,0x0000);   			// ����ͼ��2RAM��ʼ��ַ
	SPFD5408_Write_Reg(0x0085,0x0000);   			// ����ͼ��2RAM��ֹ��ַ
												
	// ��Ļ�ӿڿ���1					
	SPFD5408_Write_Reg(0x0090,0x0010);   
	SPFD5408_Write_Reg(0x0092,0x0000);   
	SPFD5408_Write_Reg(0x0093,0x0103);   
	SPFD5408_Write_Reg(0x0095,0x0210);   
	SPFD5408_Write_Reg(0x0097,0x0000);   
	SPFD5408_Write_Reg(0x0098,0x0000);   
	SPFD5408_Write_Reg(0x00f0,0x5408);   
	SPFD5408_Write_Reg(0x00f3,0x0005);   
	SPFD5408_Write_Reg(0x00f4,0x001f);   
	SPFD5408_Write_Reg(0x00f0,0x0000);   
	SPFD5408_Write_Reg(0x0007,0x0173);  
	SPFD5408_LED_ON;  												//  �򿪱����
	SPFD5408_WR_Start();
}
//-----------------------------------------------------------------
// SPFD5408������
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// void SPFD5408_Write_Data(u16 Digital)
//-----------------------------------------------------------------
//
// ��������: SPFD5408д����
// ��ڲ���: Ҫд��Ĳ���
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: ��   
// ע������: ��
//					 
//-----------------------------------------------------------------
void SPFD5408_Write_Data(u16 Digital)
{
	*(volatile uint16_t *) (LCD_Data_Addr)= Digital; 
}
//-----------------------------------------------------------------
// u16 SPFD5408_Read_Data(void)
//-----------------------------------------------------------------
//
// ��������: SPFD5408������
// ��ڲ���: Ҫ�����Ĳ���
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: ��   
// ע������: Ϊ�˼���FSMC��GPIO��������Һ�����������ͨIO��ΪFSMC
//					 ��ռ�õ����ţ���GPIO�����ݴ���ʱ��Ҫ��IO״̬������Ϻͷ���
//-----------------------------------------------------------------
u16 SPFD5408_Read_Data(void)
{
	u16 ReadValue=0;
#ifdef FSMC
	ReadValue = (*(volatile uint16_t *) (LCD_Data_Addr)); 
#else	
	u16 Data[2],tmp[3];
	
	GPIOD->ODR = (GPIOD->ODR |0xc703);				// ��PD0,1,8,9,10,14,15��1 
	GPIOE->ODR = (GPIOE->ODR |0XFF80);				// ��PE7~PE15��1  
	SPFD5408_RS_Set;	
	SPFD5408_CS_Clr;
	SPFD5408_RD_Clr;  
	SPFD5408_RD_Set;
	// ��ȡ����(��GRAMʱ,��Ҫ��2��)
	SPFD5408_RD_Clr;  
	SPFD5408_RD_Set;
	// �ӷֲ��ڲ�ͬIO�ڵ���ֵ��֯��16λ����
	Data[0] = GPIO_ReadInputData(GPIOD);    	// �������� 
	Data[1] = GPIO_ReadInputData(GPIOE);    	// �������� 

	tmp[0]  = (Data[0]&0x0700)<<5;            // ȡPD8-10�ŵ�D13-D15
	tmp[1]  =	(Data[0]&0x0003)<<2;			  		// ��PD1-PD0�ŵ�D3-D2
	tmp[2]  = (Data[0]&0xc000)>>14;						// ��PD15-PD14�ŵ�D1-D0
	// ȡPE15-7λ�ŵ����ݵ�D12-D4λ
	ReadValue = ((Data[1]&0xff80)>>3)+tmp[0]+tmp[1]+tmp[2];
	SPFD5408_CS_Set;
#endif	
	return (ReadValue);
}

//-----------------------------------------------------------------
// void SPFD5408_Write_Reg(u16 RegIndex, u16 RegValue)
//-----------------------------------------------------------------
//
// ��������: ����SPFD5408�Ĵ����ĺ���
// ��ڲ���: RegIndex:�Ĵ�����ַ
//				   RegValue:�Ĵ�������
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: SPFD5408_Write_Data().   
// ע������:
//-----------------------------------------------------------------
void SPFD5408_Write_Reg(u16 RegIndex, u16 RegValue)
{
	*(volatile uint16_t *) (LCD_Reg_Addr) = RegIndex;
	*(volatile uint16_t *) (LCD_Data_Addr) = RegValue;
}

//-----------------------------------------------------------------
// void SPFD5408_Set_Cursor(u16 x,u16 y)
//-----------------------------------------------------------------
//
// ��������: ���õ�ǰ����
// ��ڲ���: X:X����
//				   Y:Y����
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: SPFD5408_Write_Reg ().   
// ע������:
//-----------------------------------------------------------------
void SPFD5408_Set_Cursor(u16 x,u16 y)
{
	SPFD5408_Write_Reg(GRAM_X,x);
	SPFD5408_Write_Reg(GRAM_Y,y);
}

//-----------------------------------------------------------------
// void SPFD5408_WR_Start(void)
//-----------------------------------------------------------------
//
// ��������: дGRAM
// ��ڲ���: ��
// ���ز���: ��
// ȫ�ֱ���: GRAM_WR.
// ����ģ��: SPFD5408_Write_Data ().   
// ע������:
//-----------------------------------------------------------------
void SPFD5408_WR_Start(void)
{
	*(volatile uint16_t *) (LCD_Reg_Addr)= GRAM_WR;
}

//-----------------------------------------------------------------
// void SPFD5408_Write_Pixel(u16 x,u16 y, u16 Color)
//-----------------------------------------------------------------
//
// ��������: ����Һ����һ�������ɫ
// ��ڲ���: X:X����, Y:Y����, Color:��ɫֵ
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: SPFD5408_Set_Cursor(), SPFD5408_WR_Start(),
//					 SPFD5408_Write_Data().
// ע������: ��
//-----------------------------------------------------------------
void SPFD5408_Write_Pixel(u16 x,u16 y, u16 Color)
{
	/*SPFD5408_Set_Cursor(x,y);
	SPFD5408_WR_Start();
//	SPFD5408_RS_Set;
	SPFD5408_Write_Data(Color);	 */
}

//-----------------------------------------------------------------
// u16 SPFD5408_Read_Pixel(u16 x,u16 y)
//-----------------------------------------------------------------
//
// ��������: ��ȡҺ����һ�������ɫ
// ��ڲ���: X:X����, Y:Y���� 
// ���ز���: ԭɫֵ
// ȫ�ֱ���: ��
// ����ģ��: SPFD5408_Set_Cursor(),  SPFD5408_BGR2RGB().  
// ע������: ��
//-----------------------------------------------------------------
u16 SPFD5408_Read_Pixel(u16 x,u16 y)
{
	SPFD5408_Set_Cursor(x,y);									// ��������
	SPFD5408_WR_Start();       								// ѡ��GRAM��ַ 		
	
	return (SPFD5408_BGR2RGB(SPFD5408_Read_Data()));
}

//-----------------------------------------------------------------
// u16 SPFD5408_BGR2RGB(u16 c)
//-----------------------------------------------------------------
//
// ��������: RRRRRGGGGGGBBBBB ��Ϊ BBBBBGGGGGGRRRRR ��ʽ
// ��ڲ���: c:GBR��ʽ����ɫֵ
// ���ز���: RGB��ʽ����ɫֵ
// ȫ�ֱ���: ��
// ����ģ��: ��   
// ע������: ��SPFD����������ΪGBR��ʽ, ������д���ʱ��ΪRGB��ʽ 
// 					 ͨ���ú���ת��.
//-----------------------------------------------------------------
u16 SPFD5408_BGR2RGB(u16 c)
{
	u16  r, g, b, rgb; 
	
	b = (c >> 0) & 0x1f;
	g = (c >> 5) & 0x3f;
	r = (c >> 11) & 0x1f;	 
	rgb = (b << 11) + (g << 5) + (r << 0);
		 
	return(rgb);
} 

//-----------------------------------------------------------------
// void SPFD5408_Set_Window(u16 X_Size,u16 Y_Size)
//-----------------------------------------------------------------
//
// ��������: ���ô��ڴ�С
// ��ڲ���: X_Size: x�᳤��,  Y_Size: y�᳤��.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: SPFD5408_Write_Reg (), SPFD5408_WR_Start ().   
// ע������:
//-----------------------------------------------------------------
void SPFD5408_Set_Window(u16 X_Size,u16 Y_Size)
{
	SPFD5408_Write_Reg (WINDOW_XADDR_START, XSTART);
	SPFD5408_Write_Reg (WINDOW_XADDR_END, X_Size-1);	
	SPFD5408_Write_Reg (WINDOW_YADDR_START, YSTART);	
	SPFD5408_Write_Reg (WINDOW_YADDR_END, Y_Size-1);	
	SPFD5408_WR_Start ();
} 

//-----------------------------------------------------------------
// void SPFD5408_Clear_Window(u16 Color)
//-----------------------------------------------------------------
//
// ��������: ��������
// ��ڲ���: Color:��ɫֵ
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: SPFD5408_Set_Cursor (), SPFD5408_Set_Window(), 
// 					 SPFD5408_Write_Data().   
// ע������:
//-----------------------------------------------------------------
void SPFD5408_Clear_Window(u16 Color)
{
	u32 Point = 0;

	SPFD5408_Set_Cursor (XSTART, YSTART);			// ������ʼ��
	SPFD5408_Set_Window (XSIZE, YSIZE);  		 
	SPFD5408_RS_Set;
	for (Point = XSTART*YSTART; Point < XSIZE*YSIZE; Point ++)	
		SPFD5408_Write_Data (Color);						// ����
	SPFD5408_CS_Set; 
}

//-----------------------------------------------------------------
// void SPFD5408_Fill(u16 x0,u16 y0,u16 x1,u16 y1,u16 Color)
//-----------------------------------------------------------------
//
// ��������: ��ĳһ�������ĳһ����ɫ
// ��ڲ���: x0, y0: ��ʼ����. x1, y1: �յ�����. Color:��ɫֵ
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: SPFD5408_Set_Cursor(), SPFD5408_Write_Reg(), 
// 					 SPFD5408_Write_Data().   
// ע������:
//-----------------------------------------------------------------
void SPFD5408_Fill(u16 x0,u16 y0,u16 x1,u16 y1,u16 Color)
{
	u16 x,y;
	
// ������ʼ��
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
	SPFD5408_Set_Cursor (x0, y0);
#else
	SPFD5408_Set_Cursor (x1, y0);
#endif 
	SPFD5408_Write_Reg(WINDOW_XADDR_START,x0);
	SPFD5408_Write_Reg(WINDOW_XADDR_END,x1);	
	SPFD5408_Write_Reg(WINDOW_YADDR_START,y0);	
	SPFD5408_Write_Reg(WINDOW_YADDR_END,y1);	
	SPFD5408_WR_Start();
	SPFD5408_RS_Set;
	for (x = x0; x <= x1; x ++)	  				
		for (y = y0; y <= y1; y ++)
			SPFD5408_Write_Data (Color);	
	SPFD5408_CS_Set; 
}

//-----------------------------------------------------------------
//�ַ�(��)��ʾ����
//-----------------------------------------------------------------
//-----------------------------------------------------------------
//	 void GetASCIICode(
//		u8* pBuffer,													// ��ʾ����CharBuffer[] 
//		u8 ASCII															// �����ASCII��, ��'A'
//	)
//-----------------------------------------------------------------
//
// ��������: ȡASCII���ӳ���
// ��ڲ���: * pBuffer:��ʾ����CharBuffer[], 
//               ASCII: �����ASCII��, ��'A'.
// ���ز���: �� 
// ȫ�ֱ���: ��
// ����ģ��: ��   
// ע������: ����һ��ASCII�룬ȡ������font16x8�е�16Byte��ʾ����
//				   �������ŵ�һ��16byte��ASCII��ʾ����CharBuffer[]��.
//-----------------------------------------------------------------
void GetASCIICode(
	u8* pBuffer,															// ��ʾ����CharBuffer[] 
	u8 ASCII																	// �����ASCII��, ��'A'
)
{  
	u8 i;
	
	for (i = 0; i < 16; i ++)
	{	
		*(pBuffer + i) = font16x8[(ASCII - 32) * 16 + i];
	}
}

//-----------------------------------------------------------------
//	void GetChineseCode(
//		u8* pBuffer,													// ������ģ���� 
//		u8 Chi[2]															// Ҫ���ҵĺ���
//	)
//-----------------------------------------------------------------
//
// ��������: ȡ16x16������ģ�ӳ���
// ��ڲ���: * pBuffer: ������ģָ��. Chi[2]: Ҫ������ģ�ĺ���.
// ���ز���: ��
// ȫ�ֱ���: codeGB_16: ��ģ����.
// ����ģ��: ��   
// ע������: ����һ�����ģ�ȡ������codeGB_16�����е���ģ,
//					 �������ŵ�һ��32byte����ʾ����ChineseBuffer[].
//-----------------------------------------------------------------
void GetChineseCode(
	u8* pBuffer,															// ������ģ����
	u8 Chi[2]																	// Ҫ���ҵĺ���
)
{	
	u8 k, i;
	// ������100 ������, ����100 ���޷�����. 
	// Ҳ�����޸�k �����ֵ�������Ӳ��ҷ�Χ.
	for (k = 0; k < 100; k ++)								
	{																																			
		if ((codeGB_16[k].Index[0] == Chi[0]) && (codeGB_16[k].Index[1] == Chi[1]))
		{																				// �Ա�����ĺ��ֺ������еĺ����Ƿ�һ��,  					
			for(i=0;i<32;i++)											// ��һ����ȡ����ģ����.
			{
				*(pBuffer+i) = codeGB_16[k].Msk[i] ;// ��ȡ��ģ����
			}
			return ;													
		}
}
	for(i = 0; i < 32; i ++)
	{	// ���ֿ���û��Ҫ���ҵĺ���, ȫ������. ����ʾ����ɫ,
		// ��Ч���൱��û��ʾ, �����˺���.
		*(pBuffer + i) = 0x00 ;									
	}
}

//-----------------------------------------------------------------
//	void GetChineseCode24x24(
//		u8* pBuffer,													// ��ʾ����
//		u8 Chi[2]															// Ҫ���ҵ�����
//	)
//-----------------------------------------------------------------
//
// ��������: ȡ24x24������ģ�ӳ���
// ��ڲ���: * pBuffer:  ��ȡ����ģ����, Chi[2]: Ҫ���ҵĺ���.
// ���ز���: ��
// ȫ�ֱ���: codeGB_24x24: ��ģ����
// ����ģ��: ��   
// ע������: ����һ�����ģ�ȡ������codeGB_24x24�����е���ģ����
//				 �������ŵ�һ��32byte����ʾ����ChineseBuffer[]
//-----------------------------------------------------------------
void GetChineseCode24x24(
	u8* pBuffer,															// ��ʾ���� 
	u8 Chi[2]																	// Ҫ���ҵ�����
)
{	
	u8 k, i;
	// ������100 ������, ����100 ���޷�����. 
	// Ҳ�����޸�k �����ֵ�������Ӳ��ҷ�Χ.
	for (k = 0; k < 100; k ++)									
	{	// �Ա�����ĺ��ֺ������еĺ����Ƿ�һ��, ��һ����ȡ����ģ����.   
		if ((codeGB_24x24[k].Index[0] == Chi[0]) && 
				(codeGB_24x24[k].Index[1] == Chi[1]))	
		{
			for(i = 0; i < 72; i ++)
			{	// ��ȡ��ģ����
				*(pBuffer+i) = codeGB_24x24[k].Msk[i] ;
			}
			return ;																
		}
	}
	for(i = 0; i < 72; i ++)
	{	// ���ֿ���û��Ҫ���ҵĺ���, ȫ������. ����ʾ����ɫ,
		// ��Ч���൱��û��ʾ, �����˺���.
		*(pBuffer + i) = 0x00 ;										
	}
}

//-----------------------------------------------------------------
//	void LCD_WriteASCII(
//		u8 x,
//		u16  y,
//		u16 CharColor,
//		u16 CharBackColor,
//		u8 ASCIICode
//	)
//-----------------------------------------------------------------
//
// ��������: LCDд1��ASCII�ַ�����
// ��ڲ���: x: ��������, �����ҷֱ���0~239.  
//					 y: ��������, ���ϵ��·ֱ�Ϊ0~319.
//           color: ָ��LCDȫ����ɫ RGB(5-6-5).
//					 CharColaor: �ַ�����ɫ. 
//				 	 CharBackColor: �ַ�������ɫ. 		
//					 ASCIICode: ��Ӧ�ַ���ASCII��.
// ���ز���: ��
// ȫ�ֱ���: �� 
// ����ģ��: LCD_SetBox(), SPFD5408_WR_Start(), GetASCIICode().    
// ע������: �������ʹ�ô˺�����Ӧ�ü���LCD_Rs_H()	
//					 ��SPFD5408_CS_Set;Ϊ���Ż�ϵͳʡȥ�����ָ��
//				   ����˺���ִ�е���һ�������д�����RS_L�����
//				   ��д�뽫������ΪILI9320��Ϊ��RS_Lʱд���������
//-----------------------------------------------------------------
void LCD_WriteASCII(
	u8  x,																		// X������	
	u16 y,																		// y������
	u16 CharColor,														// ������ɫ
	u16 CharBackColor,												// ������ɫ
	u8 ASCIICode															// ADCII��
)
{
	u8 RowCounter,BitCounter;
	u8 *ASCIIPointer;
	u8 ASCIIBuffer[16];
  // �жϺ�����������,
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox (x, y, x + 8, y + 16);    			// ���ô��ڴ�С
#else
	LCD_SetBox(x , y, x + 16, y + 8);    
#endif 
	// ����д���ݺ���, ÿ��д����ǰ�ȵ��ô˺���       
	SPFD5408_WR_Start();	
	// ȡ����ַ�����ʾ����										
	GetASCIICode(ASCIIBuffer,ASCIICode);			
	ASCIIPointer=ASCIIBuffer;
  // һ��ASCII��ģ��16��8Bit�������
	for (RowCounter=0; RowCounter<16; RowCounter++)	
	{ // ɨ��һ��8bit��ģ���ݵ�ÿһλ
		for (BitCounter=0; BitCounter<8; BitCounter++)
		{ // �ж���ģ����Ϊ'0'����Ϊ'1'.Ϊ'0'д�뱳����ɫ,Ϊ'1'д��������ɫ
			if ((*ASCIIPointer&(0x80 >> BitCounter)) == 0x00) 
			{
				SPFD5408_Write_Data(CharBackColor);	// д�뱳����ɫ����
			}
			else
			{
				SPFD5408_Write_Data(CharColor);			// д��������ɫ����
			}
		}
		ASCIIPointer++;													// ָ����һ����ģ����
	}
  SPFD5408_CS_Set;	  											
} 

//-----------------------------------------------------------------
//	void LCD_WriteASCIIString(
//	   u8  x,
//	   u16 y,
//	   u16 CharColor,
//	   u16 CharBackColor,
//	   u8 *s
//	)
//-----------------------------------------------------------------
//
// ��������: ��ָ��λ��д��һ�������ַ������������Զ����й���
// ��ڲ���: x: ��������, �����ҷֱ���0~239. 
//					 y: ��������, ���ϵ��·ֱ�Ϊ0~319.
//	         CharColaor: �ַ�����ɫ. CharBackColor: �ַ�������ɫ.
//	         *s: ָ��Ҫд���ַ���.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_WriteASCII().   
// ע������:
//-----------------------------------------------------------------
void LCD_WriteASCIIString(
   u8 x,
   u16  y,
   u16 CharColor,
   u16 CharBackColor,
   u8 *s
)
{
  u8 databuff;
	
  SPFD5408_RS_Set;
  do
  {
    databuff = *s ++;  
    LCD_WriteASCII (x, y, CharColor, CharBackColor, databuff);    
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x < 232)
      x += 8;
    else if (y < 304)
    {
      x = 0;
      y += 16;
    }   
    else
    {
      x = 0;
      y = 0;
    }
       
#else
    if (y<312)
      y += 8;
    else if (x < 224)
    {
      y = 0;
      x += 16;
    }
    else
    {
      x = 0;
      y = 0;
    }
#endif 
  }
  while (*s!=0);
  SPFD5408_CS_Set;
}

//-----------------------------------------------------------------
// void LCD_WriteChinese(
//		u8 x,
//		u16  y,
//		u16 CharColor,
//		u16 CharBackColor,
//		u8 *sp
//	)
//-----------------------------------------------------------------
//
// ��������: LCDд1�����ĺ���
// ��ڲ���: x: ��������, �����ҷֱ���0~239.
//					 y: ��������, ���ϵ��·ֱ�Ϊ0~319.
//				  CharColor: �ַ�����ɫ. 		
//					CharBackColor: �ַ�������ɫ.
//          *sp: ��Ӧ���ĵı���.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��:  LCD_SetBox(), SPFD5408_WR_Start(), GetChineseCode(), 
//						SPFD5408_Write_Data().   
// ע������: 320*240�ֱ��ʵ���ʾ������������ʾ15�����ַ�����������ʾ20��.
//				   �������ʹ�ô˺�����Ӧ�ü���LCD_Rs_H()��SPFD5408_CS_Set;
// 					 Ϊ���Ż�ϵͳʡȥ�����ָ��,
//				  ����˺���ִ�е���һ�������д����, (RS_L���)��д�뽫����, 
//				  ��ΪSPFD5408��Ϊ��RS_Lʱд���������
//-----------------------------------------------------------------
void LCD_WriteChinese(
		u8 x,																		// x����
		u16  y,																	// y����
		u16 CharColor,													// ������ɫ
		u16 CharBackColor,											// ������ɫ
		u8 *sp																	// ָ�����ַ���ָ��
	)
{
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];											// ������ģ����
	// �ж���Ļ����	  
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox (x, y, x + 16, y + 16);    		// ���ô��ڴ�С
#else
	LCD_SetBox (x, y, x + 16, y + 16);    
#endif 
  SPFD5408_WR_Start();											// ����д����
  GetChineseCode(ChineseBuffer, sp);				// �õ������ַ�����
  ChinesePointer=ChineseBuffer;
  // 16x16�������32��8bit����      
  for(ByteCounter=0; ByteCounter<32; ByteCounter++)	
  { // ɨ��8bit��ģ���ݵ�ÿһbit																
    for(BitCounter=0;BitCounter<8;BitCounter++)	
    { // ĳһλΪ��0��
      if((*ChinesePointer & (0x80 >> BitCounter)) == 0x00)
      {
        SPFD5408_Write_Data(CharBackColor);	// д�뱳����ɫ
      }
      else																	// ĳһλΪ��1��
      {
        SPFD5408_Write_Data(CharColor); 		// д��������ɫ
      }
    }
    ChinesePointer++;												// ָ����һ��ģ����
  }
  SPFD5408_CS_Set;	  											
}	

//-----------------------------------------------------------------
//	void LCD_WriteChineseString(
//		u8 x, 
//		u16 y, 
//		u16 CharColor, 
//		u16 CharBackColor, 
//		u8 *sp 
//	)
//-----------------------------------------------------------------
//
// ��������: LCDд�����ַ�������
// ��ڲ���: x: ��������, �����ҷֱ���0~239. 
//					 y: ��������, ���ϵ��·ֱ�Ϊ0~319.
//				 	 CharColor: �ַ�����ɫ. CharBackColor: �ַ�������ɫ .
//         	 *sp: ��Ӧ���ĵı���.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_WriteChinese().   
// ע������: ��
//-----------------------------------------------------------------
void LCD_WriteChineseString(
		u8 x, 
		u16 y, 
		u16 CharColor, 
		u16 CharBackColor, 
		u8 *sp 
	)
{
	u8 * pt;

	pt = sp;
	while (*pt)
	{
	 	LCD_WriteChinese(x, y, CharColor, CharBackColor, pt);
		if (*pt == ' ')
		{
			pt += 1 ;
		}
		else
		{
			pt += 2 ;
		}
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<224)
    {
      x += 16;
    } 
    else if (y < 304)
    {
      x = 0;
      y += 16;
    }   
    else
    {
      x = 0;
      y = 0;
    }     
#else
    if (y < 304)
    {
      y += 16;
    }
    else if (x < 224)
    {
      y = 0;
      x += 16;
    }
    else
    {
      x = 0;
      y = 0;
    }
#endif
	}
}

//-----------------------------------------------------------------
//	void LCD_WriteString(
//		u8 x, 
//		u16  y, 
//		u16 CharColor, 
//		u16 CharBackColor, 
//		u8 *s 
//	) 
//-----------------------------------------------------------------
//
// ��������: LCDд�ַ�������, ����Ӣ���ַ��ͺ����ַ����д.
// ��ڲ���: x: ��������, �����ҷֱ���0~239.  
//					 y: ��������, ���ϵ��·ֱ�Ϊ0~319
//				 CharColor: �ַ�����ɫ. 
//				CharBackColor: �ַ�������ɫ.
//         *s: ��Ӧ�ַ��ı���.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_WriteASCII(),  LCD_WriteChinese().  
// ע������:
//-----------------------------------------------------------------
void LCD_WriteString(
		u8 x, 																	// X������
		u16  y, 																// y������
		u16 CharColor, 													// ������ɫ
		u16 CharBackColor, 											// ������ɫ
		u8 *s 																	// ָ���ַ������ַ�ָ��
	)
{
	u8 * pt;
	u8 tem;

	pt = s;
	while (*pt)
	{	// Ҫ��ʾ���ַ�ΪӢ���ַ�16X8
		if (*pt < 128)													
		{
			tem = *pt;
			// ������ʾASCII�뺯��
			LCD_WriteASCII (x, y, CharColor, CharBackColor, tem);	
			pt += 1 ;
			// �ж���Ļ����, ˮƽ����ΪX�᷽��
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  		
			// ��ʾ����С��231, �����8 �����ص�, ��ʾ��һ���ַ�
			if (x < 231)													
			{
				x += 8;
			} 
			else if (y < 303)		// һ��д��,ˮƽ�����0 ��ʼ,��ֱ��������16�����ص�									
			{
				x = 0;
				y += 16;
			}   
			else								// ˮƽ�������ֱ����д��, ������(0, 0)��ʼ��ʾ																
			{
				x = 0;
				y = 0;
			}
			
#else											// ˮƽ����Ϊy�᷽��
			if (y<311)					// ˮƽ��������С��311, ���ص��8, ������ʾ��һ���ַ�
			{
				y += 8;
			}
			else if (x < 223)		// �����ֱ����ûд��, ˮƽ�����0��ʼ��ʾ, ��ֱ��������16�����ص�
			{
				y = 0;
				x += 16;
			}
			else								// ˮƽ�������ֱ����д��, ��(0, 0)��ʼ��ʾ
			{
				x = 0;
				y = 0;
			}
#endif 
		}
		else									// ��ʾ���������ַ�16X16
		{	// ������ʾ�����ַ�����
		 	LCD_WriteChinese(x, y, CharColor, CharBackColor, pt);	
			pt += 2 ;						// �����ַ�ռ�������ֽ�, ����ָ��ƫ�Ƽӡ�2��
			// ˮƽ����ΪX�᷽��
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  		
			if (x<223)					// X����С��223, X��������16�����ص�, ��ʾ��һ���ַ�
			{
				x += 16;
			} 
			else if (y < 303)		// yС��303, x�����0��ʼ, y����16�����ص�, ������ʾ
			{
				x = 0;
				y += 16;
			}   
			else								// x�����y����д��, ��(0, 0)��ʼ��ʾ
			{
				x = 0;
				y = 0;
			}     
#else											// ��ֱ����Ϊy�᷽��
			if (y < 303)				// X����С��303, y��������16�����ص�, ��ʾ��һ���ַ�
			{
				y += 16;
			}
			else if (x < 223)		// yС��223, y�����0��ʼ, x����16�����ص�, ������ʾ
			{
				y = 0;
				x += 16;
			}
			else								// x�����y����д��, ��(0, 0)��ʼ��ʾ
			{
				x = 0;
				y = 0;
			}
#endif
		}
	}
}

//-----------------------------------------------------------------
//	void LCD_WriteChinese24x24(
//		u8 x,
//		u16  y,
//		u16 CharColor,
//		u16 CharBackColor,
//		u8 *sp
//	)
//-----------------------------------------------------------------
//
// ��������: LCDд1�����ĺ���(24X24)
// ��ڲ���: x: ��������, �����ҷֱ���0~239. 
// 					 y: ��������, ���ϵ��·ֱ�Ϊ0~319.
//				 CharColor: �ַ�����ɫ, 
//				CharBackColor: �ַ�������ɫ.
//         	 *sp: ��Ӧ���ĵı���.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_SetBox(), SPFD5408_WR_Start(), 
//					 GetChineseCode24x24(), SPFD5408_Write_Data().  
// ע������:
//-----------------------------------------------------------------
void LCD_WriteChinese24x24(
		u8 x,																		// x������
		u16  y,																	// y������
		u16 CharColor,													// ������ɫ
		u16 CharBackColor,											// ������ɫ
		u8 *sp																	// �����ַ�ָ��
	)
{

  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer24x24[72];								// 24x24���建��
	// ���ƵĻ��ʾ����	  
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox (x, y, x + 24, y + 24);    		// ���ô���
#else
	LCD_SetBox (x, y, x + 24, y + 24);    
#endif 
  SPFD5408_WR_Start();											//	����д����
  GetChineseCode24x24(ChineseBuffer24x24, sp);// �õ������ַ�����
  ChinesePointer=ChineseBuffer24x24;
  // 24x24�������72��8bit���� 
  for(ByteCounter=0; ByteCounter<72; ByteCounter++)
  { //	��ģ���ݵ�ÿһbit																		
    for(BitCounter=0;BitCounter<8;BitCounter++)				
    {																				// ��Ϊ��0��																		
      if((*ChinesePointer & (0x80 >> BitCounter)) == 0x00)	
      {
        SPFD5408_Write_Data(CharBackColor);	// ��ʾ������ɫ
      }
      else																	// ��Ϊ��1��
      {
        SPFD5408_Write_Data(CharColor); 		// ��ʾ������ɫ
      }
    }
    ChinesePointer++;												// ָ����һ��ģ����
  }
  SPFD5408_CS_Set;	  											// ȡ��Ƭѡ
}	

//-----------------------------------------------------------------
//	void LCD_WriteChinese24x24string(
//		u8 x,
//		u16  y,
//		u16 CharColor,
//		u16 CharBackColor,
//		u8 *sp
//	)
//-----------------------------------------------------------------
//
// ��������: LCDд���ַ�������(24X24)
// ��ڲ���: x: ��������, �����ҷֱ���0~239. 
//					 y: ��������, ���ϵ��·ֱ�Ϊ0~319.
//				 CharColor: �ַ�����ɫ, 
//				CharBackColor: �ַ�������ɫ.
//         	 *sp: ��Ӧ���ĵı���.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_WriteChinese24x24().   
// ע������:
//-----------------------------------------------------------------
void LCD_WriteChinese24x24string(
		u8 x,
		u16  y,
		u16 CharColor,
		u16 CharBackColor,
		u8 *sp
	)
{
	u8 * pt;

	pt = sp;
	while (*pt)
	{
		if ((x < 215) & (y < 295))
		{
	 	LCD_WriteChinese24x24(x, y, CharColor, CharBackColor, pt);
		if (*pt == ' ')
		{
			pt += 1 ;
		}
		else
		{
			pt += 2 ;
		}
		}
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<215)
    {
      x += 24;
    } 
    else if (y < 295)
    {
      x = 0;
      y += 24;
    }   
    else
    {
      x = 0;
      y = 0;
    }     
#else
    if (y < 295)
    {
      y += 24;
    }
    else if (x < 215)
    {
      y = 0;
      x += 24;
    }
    else
    {
      x = 0;
      y = 0;
    }
#endif

	}
}

//-----------------------------------------------------------------
//	void LCD_WriteNumChar (
//		u8 x,
//		u16  y,
//		u16 CharColor,
//		u16 CharBackColor,
//		u8 udata
//	)
//-----------------------------------------------------------------
//
// ��������: LCD��ʾ�ַ���ֵ
// ��ڲ���: x: ��������, �����ҷֱ���0~239. 
//					 y: ��������, ���ϵ��·ֱ�Ϊ0~319.
//				 CharColor: �ַ�����ɫ,
//				CharBackColor: �ַ�������ɫ.
//         	 udata: �ַ���ֵ.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_WriteString().   
// ע������: (1) ��ʾ��ֵΪ0~255֮��
//				 	 (2) ������ʾ��ʽ: �Ҷ���
//-----------------------------------------------------------------
void LCD_WriteNumChar (
		u8 x,
		u16  y,
		u16 CharColor,
		u16 CharBackColor,
		u8 udata
	)
{
	u8 temp[4];
	u8 i;
	for (i = 3; i > 0; i --)
	{
		if (udata != 0)
		{
			temp[i - 1] = (udata % 10) + 48;
			udata /= 10;
		}
		else
		{
			if(i!=3)
				temp[i - 1] = ' ';									// ��λ��ʾ�ո�
			else
				temp[2] = '0';											// ��ʾ0
		}
	}
	temp[3] = '\0';
	LCD_WriteString (x, y, CharColor, CharBackColor, temp);
}

//-----------------------------------------------------------------
//	void LCD_WriteNumInt (
//		u8 x,
//		u16  y,
//		u16 CharColor,
//		u16 CharBackColor,
//		u16 udata
//	)
//-----------------------------------------------------------------
//
// ��������: LCD��ʾ������ֵ
// ��ڲ���: x: ��������, �����ҷֱ���0~239. 
//					 y: ��������, ���ϵ��·ֱ�Ϊ0~319.
//				 CharColor: �ַ�����ɫ, 
// 			CharBackColor: �ַ�������ɫ.
//         	 udata: ����������ֵ.
// ���ز���: �� 
// ȫ�ֱ���: ��
// ����ģ��: LCD_WriteString().   
// ע������: (1) ��ʾ��ֵΪ0~65535֮��
//				 	 (2) ������ʾ��ʽ: �Ҷ���
//-----------------------------------------------------------------
void LCD_WriteNumInt (
		u8 x,
		u16  y,
		u16 CharColor,
		u16 CharBackColor,
		u16 udata
	)
{
	u8 temp[6];
	u8 i;
	for (i = 5; i > 0; i --)
	{
		if (udata != 0)
		{
			temp[i - 1] = (udata % 10) + 48;
			udata /= 10;
		}
		else
		{
			if(i!=5)
				temp[i - 1] = ' ';									// ��λ��ʾ�ո�
			else
				temp[4] = '0';											// ��ʾ0
		}
	}
	temp[5] = '\0';
	LCD_WriteString (x, y, CharColor, CharBackColor, temp);
}

//-----------------------------------------------------------------
//	void LCD_WriteNumInt (
//		u8 x,
//		u16  y,
//		u16 CharColor,
//		u16 CharBackColor,
//		unsigned long udata
//	)
//-----------------------------------------------------------------
//
// ��������: LCD��ʾ������ֵ
// ��ڲ���: x: ��������, �����ҷֱ���0~239. 
//					 y: ��������, ���ϵ��·ֱ�Ϊ0~319.
//				 CharColor: �ַ�����ɫ, 
//				CharBackColor: �ַ�������ɫ.
//         	 udata: ������������ֵ.
// ���ز���: �� 
// ȫ�ֱ���: ��
// ����ģ��: LCD_WriteString().   
// ע������: (1) ��ʾ��ֵΪ0~4294967296֮��
//				   (2) ������ʾ��ʽ: �Ҷ���
//-----------------------------------------------------------------
void LCD_WriteNumLong (
		u8 x,
		u16  y,
		u16 CharColor,
		u16 CharBackColor,
		unsigned long udata
	)
{
	u8 temp[11];
	u8 i;
	for (i = 10; i > 0; i --)
	{
		if (udata != 0)
		{
			temp[i - 1] = (udata % 10) + 48;
			udata /= 10;
		}
		else
		{
			if(i!=10)
				temp[i - 1] = ' ';									// ��λ��ʾ�ո�
			else
				temp[9] = '0';											// ��ʾ0
		}
	}
	temp[10] = '\0';
	LCD_WriteString (x, y, CharColor, CharBackColor, temp);
}

//-----------------------------------------------------------------
// ͼ����ʾ����
//-----------------------------------------------------------------

//-----------------------------------------------------------------
//	void LCD_SetBox(
//	   u8 xStart,														// x������ʼ��
//	   u16  yStart,													// y������ʼ��
//	   u8 xStop,						  						  // x������ֹ��
//	   u16  yStop						   							// y������ֹ��
//	)
//-----------------------------------------------------------------
//
// ��������: LCD��ѡ����, ѡ��LCD��ָ���ľ�������.
// ��ڲ���: xStart: x�������ʼ��, ySrart: y�������ֹ��
//         	 xStop:  x������ֹ��,   yStop:  y������ֹ��
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: SPFD5408_Set_Cursor(), SPFD5408_Write_Reg().   
// ע������: xStart�� yStart������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���
//-----------------------------------------------------------------
void LCD_SetBox(
   u8 xStart,                								// x������ʼ��
   u16  yStart,															// y������ʼ��
   u8 xStop,							   								// x������ֹ��
   u16  yStop							   								// y������ֹ��
)
{
	u8 xLong;
	u16  yLong;

	xLong = xStop - xStart ;
	yLong = yStop - yStart ;
  
#if ID_AM==000    
	SPFD5408_Set_Cursor (xStart + xLong - 1, yStart + yLong - 1);
#elif ID_AM==001
	SPFD5408_Set_Cursor (xStart + xLong - 1, yStart + yLong - 1);
#elif ID_AM==010
	SPFD5408_Set_Cursor (xStart, yStart + yLong - 1);
#elif ID_AM==011 
	SPFD5408_Set_Cursor (xStart, yStart + yLong - 1);
#elif ID_AM==100
	SPFD5408_Set_Cursor (xStart + xLong - 1, yStart);     
#elif ID_AM==101
	SPFD5408_Set_Cursor (xStart + xLong - 1, yStart);     
#elif ID_AM==110
	SPFD5408_Set_Cursor (xStart, yStart); 
#elif ID_AM==111
	SPFD5408_Set_Cursor (xStart, yStart);  
#endif     
	SPFD5408_Write_Reg (0x0050, xStart);						// ˮƽ GRAM��ʼλ��
	SPFD5408_Write_Reg (0x0051, xStart + xLong - 1);// ˮƽGRAM��ֹλ��
	SPFD5408_Write_Reg (0x0052, yStart);						// ��ֱGRAM��ʼλ��
	SPFD5408_Write_Reg (0x0053, yStart + yLong - 1);// ��ֱGRAM��ֹλ�� 
}
 
//-----------------------------------------------------------------
//	void LCD_ColorBox(
//		u8 xStart,                       			// x�������ʼ��
//		u16  yStart,													// y�������ʼ��
//		u8 xStop,															// x�������ֹ��
//		u16  yStop,														// y�������ֹ��
//		u16 Color						    							// �����ɫ
//	)
//-----------------------------------------------------------------
//
// ��������: ��������������ɫ
// ��ڲ���: xStart: x�������ʼ��, ySrart: y�������ֹ��
//         	 xStop:  x������ֹ��,   yStop:  y������ֹ��
//				 Color: �����ɫ
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_SetBox(), SPFD5408_WR_Start(), SPFD5408_Write_Data().   
// ע������:
//-----------------------------------------------------------------
void LCD_ColorBox(
	u8 xStart,                 								// x�������ʼ��
	u16  yStart,															// y�������ʼ��
	u8 xStop,																	// x�������ֹ��
	u16  yStop,																// y�������ֹ��
	u16 Color						    									// �����ɫ
)
{
	u8 xLong;																	// ����x���򳤶�
	u16  yLong;						    								// ����y���򳤶�
	unsigned long temp;

	xLong = xStop - xStart;
	yLong = yStop - yStart;
  
	LCD_SetBox (xStart, yStart, xStop + 1, yStop + 1);
	SPFD5408_WR_Start ();
	SPFD5408_RS_Set;
  
	for (temp = 0; temp < (xLong + 1) * (yLong + 1); temp ++)
		SPFD5408_Write_Data (Color);						

	SPFD5408_CS_Set;
}

//-----------------------------------------------------------------
//	void LCD_ClearCharBox(
//		u8 x,																	// ������
//		u16  y,							 									// ������
//		u16 Color															// д�����ɫ
//	)
//-----------------------------------------------------------------
//
// ��������: ��ָ�������Ϊָ����ɫ
// ��ڲ���: x: x�������ʼ��, 
//					 y: y�������ʼ��,
//           color: ָ��LCDȫ����ɫ RGB(5-6-5).
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_SetBox(), SPFD5408_WR_Start(), SPFD5408_Write_Data().   
// ע������:
//-----------------------------------------------------------------
void LCD_ClearCharBox(
	u8 x,																			// ������
	u16  y,							 											// ������
	u16 Color																	// д�����ɫ
)
{
	unsigned long temp;
  
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
	LCD_SetBox (x, y, x + 8, y + 16);    
#else
	LCD_SetBox(x,y,x + 16, y + 8);    
#endif 
	SPFD5408_WR_Start();
	SPFD5408_RS_Set;
	for (temp=0; temp < 128; temp++)
	{
		SPFD5408_Write_Data(Color);  
	}	
	SPFD5408_CS_Set;
}

//-----------------------------------------------------------------
// void LCD_SetPoint(u8 x, u16 y, u16 Color)
//-----------------------------------------------------------------
//
// ��������: ��ָ������ɫ��ָ��������λ���ϻ���һ����
// ��ڲ���: x: ��������, �����ҷֱ���0~239. 
//					 y: ��������, ���ϵ��·ֱ�Ϊ0~319.
//	         Color: ������ɫ.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_ColorBox().   
// ע������:
//-----------------------------------------------------------------
void LCD_SetPoint(u8 x, u16 y, u16 Color)
{  
	LCD_ColorBox (x, y, x, y, Color);					// �����趨һ����Ĵ�С
	//SPFD5408_Read_Pixel(X,Y,Color);
}

//-----------------------------------------------------------------
// void LCD_HLine(u8 x, u16 y, u16 Length, u16 Color)
//-----------------------------------------------------------------
//
// ��������: ��ָ������ɫ��ָ��λ���ϻ���ָ�����ȵ�һ������ 
// ��ڲ���: x: �����������, �����ҷֱ���0~239. 
//					 y: �����������, ���ϵ��·ֱ�Ϊ0~319.
//				 	Length: ���߳���. Color: ������ɫ.
// ���ز���: �� 
// ȫ�ֱ���: ��
// ����ģ��: LCD_SetBox(),SPFD5408_WR_Start(),SPFD5408_Write_Data(). 
// ע������:
//-----------------------------------------------------------------
void LCD_HLine(u8 x, u16 y, u16 Length, u16 Color) 
{  																					// �ж���Ļ����
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox(x, y, x + Length + 1, y + 1);	// ���ô���
#else
	LCD_SetBox(x, y, x + 1, y + Length + 1);
#endif 
	SPFD5408_WR_Start();											// ����д����
 	do
	{  
		SPFD5408_Write_Data(Color);							// д����ɫ����						
		Length--;
	}while(Length);
	SPFD5408_CS_Set;													// ȡ��Ƭѡ
}

//-----------------------------------------------------------------
//	void LCD_HDotLine(
//		u8 x, 																// x ��ʼ����
//		u16 y, 																// y ��ʼ����
//		u16 Length, 													// ����
//		u16 Color,														// ����ɫ
//		u16 CharBackColor											// ������ɫ
//	) 
//-----------------------------------------------------------------
//
// ��������: ��ָ������ɫ��ָ��λ���ϻ���ָ�����ȵ�һ��������
// ��ڲ���: x: �����������, �����ҷֱ���0~239. 
//					 y: �����������, ���ϵ��·ֱ�Ϊ0~319.
//				 Length: ���߳���.
//					Color: ������ɫ.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_SetBox(), SPFD5408_WR_Start(), SPFD5408_Write_Data().   
// ע������:
//-----------------------------------------------------------------
void LCD_HDotLine(
		u8 x, 																	// x ��ʼ����
		u16 y, 																	// y ��ʼ����
		u16 Length, 														// ����
		u16 Color,															// ����ɫ
		u16 CharBackColor												// ������ɫ
	) 
{  																					// �ж���Ļ����
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox(x, y, x + Length + 1, y + 1);	// ���ô���
#else
	LCD_SetBox(x, y, x + 1, y + Length + 1);
#endif 
	SPFD5408_WR_Start();											// ����д����
	do
	{ 
		if ((Length % DOT) == 0)								// ���DOT�㻭����
		{
			SPFD5408_Write_Data(Color);						// д������ɫ
		} 
		else																		// ������
		{
			SPFD5408_Write_Data(CharBackColor);		// д�뱳����ɫ	
		}
		Length--;
	}while(Length);
	SPFD5408_CS_Set;												 	// ȡ��Ƭѡ
}

//-----------------------------------------------------------------
//	void LCD_RLine(
//		u8 x, 
//		u16  y, 
//		u16  High, 
//		u16 Color
//	) 
//-----------------------------------------------------------------
//
// ��������: ��ָ������ɫ��ָ��λ���ϻ���ָ�����ȵ�һ������
// ��ڲ���: x: �����������, �����ҷֱ���0~239.
//					 y: �����������, ���ϵ��·ֱ�Ϊ0~319.
//			    High: ���߳���. 
//				 Color: ������ɫ.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_SetBox(), SPFD5408_WR_Start(),SPFD5408_Write_Data().   
// ע������:
//-----------------------------------------------------------------
void LCD_RLine(
		u8 x,																		// x������ 
		u16  y, 																// y������
		u16  High, 															// �߸߶�
		u16 Color																// ����ɫ
	) 
{  																					// �ж���Ļ��ʾ����
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox(x, y, x + 1, y + High + 1);		// ���ô���
#else
	LCD_SetBox(x, y, x + High + 1, y + 1);
#endif 
	SPFD5408_WR_Start();											// ����д����
	do
	{  
		SPFD5408_Write_Data(Color);							// д����ɫ����
		High--;
	}while(High);
	SPFD5408_CS_Set;													// ȡ��Ƭѡ
}

//-----------------------------------------------------------------
//	void LCD_RDotLine(
//		u8 x, 																// x ��ʼ����
//		u16 y, 																// y ��ʼ����
//		u16 High, 														// �߸߶�
//		u16 Color,														// ����ɫ
//		u16 CharBackColor											// ������ɫ
//	)
//-----------------------------------------------------------------
//
// ��������: ��ָ������ɫ��ָ��λ���ϻ���ָ�����ȵ�һ��������
// ��ڲ���: x�� �����������, �����ҷֱ���0~239. 
//					 y: �����������, ���ϵ��·ֱ�Ϊ0~319.
//			    High: ���߳���. 
//				 Color: ������ɫ. 
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_SetBox(),SPFD5408_WR_Start(),SPFD5408_Write_Data().   
// ע������:
//-----------------------------------------------------------------
void LCD_RDotLine(
		u8 x, 																	// x ��ʼ����
		u16 y, 																	// y ��ʼ����
		u16 High, 															// �߸߶�
		u16 Color,															// ����ɫ
		u16 CharBackColor												// ������ɫ
	) 
{  																					// �ж���Ļ��ʾ����
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox(x, y, x + 1, y + High + 1);		// ���ô���
#else
	LCD_SetBox(x, y, x + High + 1, y + 1);
#endif 
	SPFD5408_WR_Start();											// ����д����
	do
	{  
		if ((High % DOT) == 0)									// ���DOT�㻭����
		{
			SPFD5408_Write_Data(Color);						// д������ɫ
		}
		else 																		// ������
		{
			SPFD5408_Write_Data(CharBackColor);		// д�뱳����ɫ	
		}
		High--;
	}while(High);
	SPFD5408_CS_Set;													// ȡ��Ƭѡ
}

//-----------------------------------------------------------------
//	void LCD_Print_Box(
//		u16 x0,																// ���Ͻ���ʼx������
//		u16 y0,																// ���Ͻ���ʼy������
//		u16 x1,																// ���½���ֹx������
//		u16 y1,																// ���½���ֹy������
//		u16 Color															// ���ο���ɫ
//	)
//-----------------------------------------------------------------
//
// ��������: ��������
// ��ڲ���: x0: ���Ͻ�x��ʼ����. y0: ���Ͻ�y��ʼ����.
//				 x1: ���½�x��ֹ����. y1: ���½�y��ֹ����.
//				 Color: �����ԭɫֵ.
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: LCD_RLine(), LCD_HLine().   
// ע������:
//-----------------------------------------------------------------
void LCD_Print_Box(
		u16 x0,
		u16 y0,
		u16 x1,
		u16 y1,
		u16 Color
	)
{
	u16 High;
	u16 Length;
																						// ����
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	  
	High   = y1 - y0;
	Length = x1 - x0;
	LCD_RLine (x0, y0, High + 1, Color) ; 		// ����
	LCD_RLine (x1, y0, High + 1, Color) ;
	LCD_HLine (x0, y0, Length + 1, Color) ;		// ����
	LCD_HLine (x0, y1, Length + 1, Color) ;
#else																				// ����
	High   = x1 - x0;
	Length = y1 - y0;
	LCD_RLine (x0, y0, High, Color); 					// ����
	LCD_RLine (x0, y1, High, Color);
	LCD_HLine (x0, y0, Length + 1, Color);		// ����
	LCD_HLine (x1, y0, Length + 1, Color);
#endif 
}


//-----------------------------------------------------------------
// ͼƬ��ʾ����
//-----------------------------------------------------------------

//-----------------------------------------------------------------
//	void LCD_DisPicture(
//		u8 x,																	// ��ʾ��ʼX���� 
//		u16 y, 																// ��ʾ��ʼy����
//		u8 length, 														// ͼƬ����
//		u16 high, 														// ͼƬ�߶�
//		u8 *s																	// ָ��ͼƬ����ָ��
//	)
//-----------------------------------------------------------------
//
// ��������: ��ָ��λ������ʾ���ȶ����ͼƬ
// ��ڲ���: (x, y): �ǿ�ʼ�������. 
//					 length: ͼƬ����. 
//						 high: ͼƬ�߶�. 
//							 *S: ͼƬ�����ָ�� 
// ���ز���: �� 
// ȫ�ֱ���: ��
// ����ģ��: LCD_SetBox(), SPFD5408_WR_Start(), SPFD5408_Write_Data().    
// ע������: ��Ҫ��ʾ��ͼƬ���ȶ�����bmp.h�е�pic[]������,
//				 ������޸�ͼƬ��С�����ݣ����޸�bmp.h�е�pic[]����,
//				 ������Image2Lcd�������Ҫ��ʾ��ͼ���Զ�ת��Ϊ��������.
//-----------------------------------------------------------------
void LCD_DisPicture(
		u8 x,																		// ��ʾ��ʼX���� 
		u16 y, 																	// ��ʾ��ʼy����
		u8 length, 															// ͼƬ����
		u16 high, 															// ͼƬ�߶�
		u8 *s																		// ָ��ͼƬ����ָ��
	)
{
   u16 temp = 0 ;
	u16 tmp  = 0 ;
	u16 num  = 0 ;	
																						// �ж���Ļ��ʾ����
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox (x, y, x + length, y + high) ; // ���ô���
#else
	LCD_SetBox(x, y, x + high, y + length);    
#endif 
	SPFD5408_WR_Start();											// ����д����
	num = length * high * 2 ;									// ����д�����ݸ���
	do
	{  
	   temp = s[tmp + 1] ;										// ��ȡ��8λ����
	   temp = temp << 8 ;											// ��ȡ��8λ����
	   temp = temp | s[tmp] ;									// ��������ϳ�һ��16λ������
	   SPFD5408_Write_Data (temp) ;						// �����ʾ
	   tmp += 2 ;															// ������ȡ����������
	}while (tmp < num) ;
	SPFD5408_CS_Set;													// ȡ��Ƭѡ
}

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
