//-----------------------------------------------------------------
// SPFD5408内核2.8寸液晶屏驱动头文件
// 头文件名: SPFD5408.h
// 作    者: 凌智电子
// 编写时间: 2014-01-24
// 完成日期: 2014-01-26
// 修改日期: 2014-02-07
// 当前版本: V1.0.1
// 历史版本: 
// - V1.0.0: (2014-2-7)基本液晶显示：中英文，点线面，图片显示
// - V1.0.1: (2014-2-15)模拟IO和集成FSMC驱动分开
//
//-----------------------------------------------------------------

#ifndef _SPFD5408_H
#define _SPFD5408_H

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include <stm32f10x.h>
#include "delay.h"

//-----------------------------------------------------------------
// 宏定义
//-----------------------------------------------------------------
// 定义常见颜色
#define WHITE       	  	0xFFFF						// 白色
#define BLACK         		0x0000	   				// 黑色
#define BLUE         			0x001F  					// 蓝色
#define BRED          		0XF81F						// 紫色
#define GBLUE							0X07FF						// 青色
#define RED           		0xF800						// 红色
#define GREEN         		0x07E0						// 绿色
#define CYAN          		0x7FFF						// 蓝绿色
#define YELLOW        		0xFFE0						// 黄色
#define BROWN 			 			0XBC40 						// 棕色
#define BRRED 						0XFC07 						// 棕红色
#define GRAY  					 	0X8430 						// 灰色

//液晶尺寸定义
#define XSIZE								240							// 横轴尺寸
#define YSIZE								320							// 纵轴尺寸
#define XSTART								0							// 横轴起始点
#define YSTART   							0							// 纵轴起始点
#define XEND								319							// 横轴终点
#define YEND								239							// 纵轴终点

// SPFD5408重要寄存器定义
#define GRAM_X							0x20
#define GRAM_Y							0x21
#define GRAM_WR							0x22
#define WINDOW_XADDR_START 	0x50
#define WINDOW_XADDR_END 		0x51
#define WINDOW_YADDR_START	0x52
#define WINDOW_YADDR_END 		0x53


// 画虚线时间隔点个数
#define DOT		3

// 定义FSMC数据和寄存器地址
#define LCD_Data_Addr    ((uint32_t)0x60020000)    
#define LCD_Reg_Addr     ((uint32_t)0x60000000)	


//屏幕旋转定义 数字按照 ID[1:0]AM 按照PDF中的配置定义
#define ID_AM  110		    								// 竖屏
// #define ID_AM  101			 										// 横屏
 
//-----------------------------------------------------------------
// 位操作
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
// 与下级函数的接口	
//-----------------------------------------------------------------
#define SPFD5408_Delay(n)	Delay_1ms(n)			// 液晶延时函数

//-----------------------------------------------------------------
// 与上级函数的接口:外部函数声明
//-----------------------------------------------------------------
// SPFD5408写数据
extern void SPFD5408_Write_Data(u16 Digital);
extern u16 SPFD5408_Read_Data(void);
// 配置SPFD5408寄存器的函数 
extern void SPFD5408_Write_Reg(u16 RegIndex, u16 RegValue);	
// 设置当前坐标
extern void SPFD5408_Set_Cursor(u16 x,u16 y);
extern void SPFD5408_WR_Start(void);	 			// 写GRAM
// 设置液晶上一个点的颜色
extern void SPFD5408_Write_Pixel(u16 x,u16 y, u16 Color);	
extern u16 SPFD5408_Read_Pixel(u16 x,u16 y);// 读取液晶上一个点的颜色
extern u16 SPFD5408_BGR2RGB(u16 c);					// RRRRRGGGGGGBBBBB 改为
																						// BBBBBGGGGGGRRRRR 格式
extern void STM32_FSMC_Init(void);
// 液晶IO口初始化
extern void GPIO_SPFD5408_Configuration(void);
extern void SPFD5408_Init(void);						// SPFD5408初始化
// 设置窗口大小
extern void SPFD5408_Set_Window(u16 X_Size,u16 Y_Size);
// 清屏函数
extern void SPFD5408_Clear_Window(u16 Color);
extern void SPFD5408_Fill(									// 给某一区域填充某一种颜色
													u16 x0,						// 左上角x起始坐标
													u16 y0,						// 左上角y起始坐标
													u16 x1,						// 右下角x终止坐标
													u16 y1,						// 右下角y终止坐标
													u16 Color					// 填充颜色码
													);

//-----------------------------------------------------------------
// 显示中英文字符函数
//-----------------------------------------------------------------
extern void GetASCIICode(										// 取ASCII码子程序
		u8* pBuffer,														// ASCII码字模缓存
		u8 ASCII																// ASCII码
	);
extern void GetChineseCode(									// 读取中文显示取模子程序
		u8* pBuffer,														// 存放字模指针 
		u8 Chi[2]																// 汉字编码
	);
extern void LCD_SetBox(						 
   u8 xStart,              									// x方向起始点
   u16 yStart,															// y方向起始点
   u8 xStop,																// x方向终止点
   u16  yStop																// y方向终止点
);
extern void LCD_ColorBox(
	u8 xStart,              									// x方向的起始点
	u16  yStart,															// y方向的起始点
	u8 xLong,																	// x方向长度
	u16  yLong,																// y方向长度
	u16 Color						    									// 填充颜色
);
extern void LCD_ClearCharBox(
	u8 x,																			// 横坐标
	u16  y,							 											// 纵坐标
	u16 Color																	// 写入的颜色
);
extern void LCD_WriteASCII(									// 写入一个字符
	u8 x,																			// x 起始坐标
	u16  y,																		// y 起始坐标
	u16 CharColor,														// 字符颜色
	u16 CharBackColor,												// 背景颜色
	u8 ASCIICode															// 带写入的字符
);
extern void LCD_WriteASCIIString(						// 写入字符串
   u8 x,																		// x 起始坐标
   u16  y,																	// y 起始坐标
   u16 CharColor,														// 字体颜色
   u16 CharBackColor,												// 背景颜色
   u8 *s																		// 待写入字符串指针
);
extern void LCD_WriteChinese(								// 写入一个中文字符
		u8 x,																		// x 起始坐标
		u16  y,																	// y 起始坐标
		u16 CharColor,													// 字体颜色
		u16 CharBackColor,											// 背景颜色
		u8 *sp																	// 待写入汉字指针
	);
extern void LCD_WriteChineseString(					// 写入中文字符串
		u8 x, 																	// x 起始坐标
		u16  y, 																// y 起始坐标
		u16 CharColor, 													// 字体颜色
		u16 CharBackColor, 											// 背景颜色
		u8 *sp 																	// 待写入中文字符串指针
	);
extern void LCD_WriteString(								// 写入字符串(中英文混写)
		u8 x, 																	// x 起始坐标
		u16  y, 																// y 起始坐标
		u16 CharColor, 													// 字体颜色
		u16 CharBackColor, 											// 背景颜色
		u8 *s 																	// 待写入字符串指针
	);
extern void LCD_WriteChinese24x24(					// 显示24x24中文汉字
		u8 x,																		// x起始坐标
		u16  y,																	// y起始坐标
		u16 CharColor,													// 字体颜色
		u16 CharBackColor,											// 背景颜色
		u8 *sp																	// 显示汉字字符指针
	);
extern void LCD_WriteChinese24x24string(		// 显示24x24汉字字符串
		u8 x,																		// x起始地址
		u16  y,																	// y起始地址
		u16 CharColor,													// 字体颜色
		u16 CharBackColor,											// 背景颜色
		u8 *sp																	// 显示汉字字符串指针
	);
//-----------------------------------------------------------------
// 显示数值函数
//-----------------------------------------------------------------
extern void LCD_WriteNumChar (							// 显示字符数值,右对齐
		u8 x,																		// x起始坐标
		u16  y,																	// y起始坐标
		u16 CharColor,													// 字体颜色
		u16 CharBackColor,											// 背景颜色
		u8 udata																// 字符数值
	);
extern void LCD_WriteNumInt (								// 显示整形数值,右对齐
		u8 x,																		// x起始坐标
		u16  y,																	// y起始坐标
		u16 CharColor,													// 字体颜色
		u16 CharBackColor,											// 背景颜色
		u16 udata																// 整形数值
	);
extern void LCD_WriteNumLong (							// 显示长整形数值,右对齐
		u8 x,																		// x起始坐标
		u16  y,																	// y起始坐标
		u16 CharColor,													// 字体颜色
		u16 CharBackColor,											// 背景颜色
		unsigned long udata											// 长整形数值
	);
//-----------------------------------------------------------------
// 显示图形函数
//-----------------------------------------------------------------
extern void LCD_SetPoint(										// 画点函数
		u8 x, 																	// x 坐标
		u16 y, 																	// y 坐标
		u16 Color																// 点的颜色
	);
extern void LCD_HLine(											// 画横线
		u8 x, 																	// x起始坐标
		u16 y, 																	// y起始坐标
		u16 Length, 														// 线长度
		u16 Color																// 线颜色码
	); 
extern void LCD_HDotLine(										// 画横虚线
		u8 x, 																	// x 起始坐标
		u16 y, 																	// y 起始坐标
		u16 Length, 														// 长度
		u16 Color,															// 线颜色
		u16 CharBackColor												// 背景颜色
	);
extern void LCD_RLine(											// 画竖线
		u8 x, 																	// x起始坐标
		u16 y, 																	// y起始坐标
		u16 High, 															// 高度
		u16 Color																// 颜色码
	) ;
extern void LCD_RDotLine(										// 画竖虚线
		u8 x, 																	// x 起始坐标
		u16 y, 																	// y 起始坐标
		u16 High, 															// 线高度
		u16 Color,															// 线颜色
		u16 CharBackColor												// 背景颜色
	);
extern void LCD_Print_Box(									// 画矩形
		u16 x0,																	// 左上角x起始坐标
		u16 y0,																	// 左上角y起始坐标
		u16 x1,																	// 右下角x终止坐标
		u16 y1,																	// 右下角y终止坐标
		u16 Color																// 矩形框颜色码
	);

//-----------------------------------------------------------------
// 显示图片函数
//-----------------------------------------------------------------
extern void LCD_DisPicture(									// 显示图片函数
		u8 x,																		// 显示起始X坐标 
		u16 y, 																	// 显示起始y坐标
		u8 length, 															// 图片长度
		u16 high, 															// 图片高度
		u8 *s																		// 指向图片数据指针
	);
#endif

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
