//-----------------------------------------------------------------
// SPFD5408�ں�2.8��Һ��������ͷ�ļ�
// ͷ�ļ���: SPFD5408.h
// ��    ��: ���ǵ���
// ��дʱ��: 2014-01-24
// �������: 2014-01-26
// �޸�����: 2014-02-07
// ��ǰ�汾: V1.0.1
// ��ʷ�汾: 
// - V1.0.0: (2014-2-7)����Һ����ʾ����Ӣ�ģ������棬ͼƬ��ʾ
// - V1.0.1: (2014-2-15)ģ��IO�ͼ���FSMC�����ֿ�
//
//-----------------------------------------------------------------

#ifndef _SPFD5408_H
#define _SPFD5408_H

//-----------------------------------------------------------------
// ͷ�ļ�����
//-----------------------------------------------------------------
#include <stm32f10x.h>
#include "delay.h"

//-----------------------------------------------------------------
// �궨��
//-----------------------------------------------------------------
// ���峣����ɫ
#define WHITE       	  	0xFFFF						// ��ɫ
#define BLACK         		0x0000	   				// ��ɫ
#define BLUE         			0x001F  					// ��ɫ
#define BRED          		0XF81F						// ��ɫ
#define GBLUE							0X07FF						// ��ɫ
#define RED           		0xF800						// ��ɫ
#define GREEN         		0x07E0						// ��ɫ
#define CYAN          		0x7FFF						// ����ɫ
#define YELLOW        		0xFFE0						// ��ɫ
#define BROWN 			 			0XBC40 						// ��ɫ
#define BRRED 						0XFC07 						// �غ�ɫ
#define GRAY  					 	0X8430 						// ��ɫ

//Һ���ߴ綨��
#define XSIZE								240							// ����ߴ�
#define YSIZE								320							// ����ߴ�
#define XSTART								0							// ������ʼ��
#define YSTART   							0							// ������ʼ��
#define XEND								319							// �����յ�
#define YEND								239							// �����յ�

// SPFD5408��Ҫ�Ĵ�������
#define GRAM_X							0x20
#define GRAM_Y							0x21
#define GRAM_WR							0x22
#define WINDOW_XADDR_START 	0x50
#define WINDOW_XADDR_END 		0x51
#define WINDOW_YADDR_START	0x52
#define WINDOW_YADDR_END 		0x53


// ������ʱ��������
#define DOT		3

// ����FSMC���ݺͼĴ�����ַ
#define LCD_Data_Addr    ((uint32_t)0x60020000)    
#define LCD_Reg_Addr     ((uint32_t)0x60000000)	


//��Ļ��ת���� ���ְ��� ID[1:0]AM ����PDF�е����ö���
#define ID_AM  110		    								// ����
// #define ID_AM  101			 										// ����
 
//-----------------------------------------------------------------
// λ����
//-----------------------------------------------------------------
#define SPFD5408_CS_Set 	(GPIO_SetBits(GPIOD,GPIO_Pin_7)) 
#define SPFD5408_CS_Clr 	(GPIO_ResetBits(GPIOD,GPIO_Pin_7))

#define SPFD5408_RST_Set	(GPIO_SetBits(GPIOD,GPIO_Pin_12)) 
#define SPFD5408_RST_Clr	(GPIO_ResetBits(GPIOD,GPIO_Pin_12))

#define SPFD5408_RS_Set 	(GPIO_SetBits(GPIOD,GPIO_Pin_11)) 
#define SPFD5408_RS_Clr 	(GPIO_ResetBits(GPIOD,GPIO_Pin_11))

#define SPFD5408_WR_Set 	(GPIO_SetBits(GPIOD,GPIO_Pin_5)) 
#define SPFD5408_WR_Clr 	(GPIO_ResetBits(GPIOD,GPIO_Pin_5))

#define SPFD5408_RD_Set 	(GPIO_SetBits(GPIOD,GPIO_Pin_4))
#define SPFD5408_RD_Clr 	(GPIO_ResetBits(GPIOD,GPIO_Pin_4))

#define SPFD5408_LED_OFF 	(GPIO_SetBits(GPIOD,GPIO_Pin_13))
#define SPFD5408_LED_ON	(GPIO_ResetBits(GPIOD,GPIO_Pin_13))


//-----------------------------------------------------------------
// ���¼������Ľӿ�	
//-----------------------------------------------------------------
#define SPFD5408_Delay(n)	Delay_1ms(n)			// Һ����ʱ����

//-----------------------------------------------------------------
// ���ϼ������Ľӿ�:�ⲿ��������
//-----------------------------------------------------------------
// SPFD5408д����
extern void SPFD5408_Write_Data(u16 Digital);
extern u16 SPFD5408_Read_Data(void);
// ����SPFD5408�Ĵ����ĺ��� 
extern void SPFD5408_Write_Reg(u16 RegIndex, u16 RegValue);	
// ���õ�ǰ����
extern void SPFD5408_Set_Cursor(u16 x,u16 y);
extern void SPFD5408_WR_Start(void);	 			// дGRAM
// ����Һ����һ�������ɫ
extern void SPFD5408_Write_Pixel(u16 x,u16 y, u16 Color);	
extern u16 SPFD5408_Read_Pixel(u16 x,u16 y);// ��ȡҺ����һ�������ɫ
extern u16 SPFD5408_BGR2RGB(u16 c);					// RRRRRGGGGGGBBBBB ��Ϊ
																						// BBBBBGGGGGGRRRRR ��ʽ
extern void STM32_FSMC_Init(void);
// Һ��IO�ڳ�ʼ��
extern void GPIO_SPFD5408_Configuration(void);
extern void SPFD5408_Init(void);						// SPFD5408��ʼ��
// ���ô��ڴ�С
extern void SPFD5408_Set_Window(u16 X_Size,u16 Y_Size);
// ��������
extern void SPFD5408_Clear_Window(u16 Color);
extern void SPFD5408_Fill(									// ��ĳһ�������ĳһ����ɫ
													u16 x0,						// ���Ͻ�x��ʼ����
													u16 y0,						// ���Ͻ�y��ʼ����
													u16 x1,						// ���½�x��ֹ����
													u16 y1,						// ���½�y��ֹ����
													u16 Color					// �����ɫ��
													);

//-----------------------------------------------------------------
// ��ʾ��Ӣ���ַ�����
//-----------------------------------------------------------------
extern void GetASCIICode(										// ȡASCII���ӳ���
		u8* pBuffer,														// ASCII����ģ����
		u8 ASCII																// ASCII��
	);
extern void GetChineseCode(									// ��ȡ������ʾȡģ�ӳ���
		u8* pBuffer,														// �����ģָ�� 
		u8 Chi[2]																// ���ֱ���
	);
extern void LCD_SetBox(						 
   u8 xStart,              									// x������ʼ��
   u16 yStart,															// y������ʼ��
   u8 xStop,																// x������ֹ��
   u16  yStop																// y������ֹ��
);
extern void LCD_ColorBox(
	u8 xStart,              									// x�������ʼ��
	u16  yStart,															// y�������ʼ��
	u8 xLong,																	// x���򳤶�
	u16  yLong,																// y���򳤶�
	u16 Color						    									// �����ɫ
);
extern void LCD_ClearCharBox(
	u8 x,																			// ������
	u16  y,							 											// ������
	u16 Color																	// д�����ɫ
);
extern void LCD_WriteASCII(									// д��һ���ַ�
	u8 x,																			// x ��ʼ����
	u16  y,																		// y ��ʼ����
	u16 CharColor,														// �ַ���ɫ
	u16 CharBackColor,												// ������ɫ
	u8 ASCIICode															// ��д����ַ�
);
extern void LCD_WriteASCIIString(						// д���ַ���
   u8 x,																		// x ��ʼ����
   u16  y,																	// y ��ʼ����
   u16 CharColor,														// ������ɫ
   u16 CharBackColor,												// ������ɫ
   u8 *s																		// ��д���ַ���ָ��
);
extern void LCD_WriteChinese(								// д��һ�������ַ�
		u8 x,																		// x ��ʼ����
		u16  y,																	// y ��ʼ����
		u16 CharColor,													// ������ɫ
		u16 CharBackColor,											// ������ɫ
		u8 *sp																	// ��д�뺺��ָ��
	);
extern void LCD_WriteChineseString(					// д�������ַ���
		u8 x, 																	// x ��ʼ����
		u16  y, 																// y ��ʼ����
		u16 CharColor, 													// ������ɫ
		u16 CharBackColor, 											// ������ɫ
		u8 *sp 																	// ��д�������ַ���ָ��
	);
extern void LCD_WriteString(								// д���ַ���(��Ӣ�Ļ�д)
		u8 x, 																	// x ��ʼ����
		u16  y, 																// y ��ʼ����
		u16 CharColor, 													// ������ɫ
		u16 CharBackColor, 											// ������ɫ
		u8 *s 																	// ��д���ַ���ָ��
	);
extern void LCD_WriteChinese24x24(					// ��ʾ24x24���ĺ���
		u8 x,																		// x��ʼ����
		u16  y,																	// y��ʼ����
		u16 CharColor,													// ������ɫ
		u16 CharBackColor,											// ������ɫ
		u8 *sp																	// ��ʾ�����ַ�ָ��
	);
extern void LCD_WriteChinese24x24string(		// ��ʾ24x24�����ַ���
		u8 x,																		// x��ʼ��ַ
		u16  y,																	// y��ʼ��ַ
		u16 CharColor,													// ������ɫ
		u16 CharBackColor,											// ������ɫ
		u8 *sp																	// ��ʾ�����ַ���ָ��
	);
//-----------------------------------------------------------------
// ��ʾ��ֵ����
//-----------------------------------------------------------------
extern void LCD_WriteNumChar (							// ��ʾ�ַ���ֵ,�Ҷ���
		u8 x,																		// x��ʼ����
		u16  y,																	// y��ʼ����
		u16 CharColor,													// ������ɫ
		u16 CharBackColor,											// ������ɫ
		u8 udata																// �ַ���ֵ
	);
extern void LCD_WriteNumInt (								// ��ʾ������ֵ,�Ҷ���
		u8 x,																		// x��ʼ����
		u16  y,																	// y��ʼ����
		u16 CharColor,													// ������ɫ
		u16 CharBackColor,											// ������ɫ
		u16 udata																// ������ֵ
	);
extern void LCD_WriteNumLong (							// ��ʾ��������ֵ,�Ҷ���
		u8 x,																		// x��ʼ����
		u16  y,																	// y��ʼ����
		u16 CharColor,													// ������ɫ
		u16 CharBackColor,											// ������ɫ
		unsigned long udata											// ��������ֵ
	);
//-----------------------------------------------------------------
// ��ʾͼ�κ���
//-----------------------------------------------------------------
extern void LCD_SetPoint(										// ���㺯��
		u8 x, 																	// x ����
		u16 y, 																	// y ����
		u16 Color																// �����ɫ
	);
extern void LCD_HLine(											// ������
		u8 x, 																	// x��ʼ����
		u16 y, 																	// y��ʼ����
		u16 Length, 														// �߳���
		u16 Color																// ����ɫ��
	); 
extern void LCD_HDotLine(										// ��������
		u8 x, 																	// x ��ʼ����
		u16 y, 																	// y ��ʼ����
		u16 Length, 														// ����
		u16 Color,															// ����ɫ
		u16 CharBackColor												// ������ɫ
	);
extern void LCD_RLine(											// ������
		u8 x, 																	// x��ʼ����
		u16 y, 																	// y��ʼ����
		u16 High, 															// �߶�
		u16 Color																// ��ɫ��
	) ;
extern void LCD_RDotLine(										// ��������
		u8 x, 																	// x ��ʼ����
		u16 y, 																	// y ��ʼ����
		u16 High, 															// �߸߶�
		u16 Color,															// ����ɫ
		u16 CharBackColor												// ������ɫ
	);
extern void LCD_Print_Box(									// ������
		u16 x0,																	// ���Ͻ�x��ʼ����
		u16 y0,																	// ���Ͻ�y��ʼ����
		u16 x1,																	// ���½�x��ֹ����
		u16 y1,																	// ���½�y��ֹ����
		u16 Color																// ���ο���ɫ��
	);

//-----------------------------------------------------------------
// ��ʾͼƬ����
//-----------------------------------------------------------------
extern void LCD_DisPicture(									// ��ʾͼƬ����
		u8 x,																		// ��ʾ��ʼX���� 
		u16 y, 																	// ��ʾ��ʼy����
		u8 length, 															// ͼƬ����
		u16 high, 															// ͼƬ�߶�
		u8 *s																		// ָ��ͼƬ����ָ��
	);
#endif

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
