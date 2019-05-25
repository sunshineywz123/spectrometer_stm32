//-----------------------------------------------------------------
// 程序描述:
//      基于FSMC驱动SPFD5408内核的2.8寸液晶屏驱动子程序
// 作    者: 凌智电子
// 开始日期: 2014-01-24
// 完成日期: 2014-01-26
// 修改日期: 2014-02-15
// 当前版本: V1.0.1
// 历史版本: 
// - V1.0.0: (2014-2-7)基本液晶显示：中英文，点线面，图片显示
// - V1.0.1: (2014-2-15)模拟IO和集成FSMC驱动分开
// 调试工具: 凌智STM32+FPGA电子系统设计开发板、2.8寸液晶屏、LZE_ST_LINK2
// 说    明:
//       (1) 调试使用的系统时钟频率Fsysclk=72MHz
//		 	 (2) 模块分两种驱动方式:FSMC和普通IO控制，通过条件编译切换 2014-1-24
//		   (3) 支持中英文显示字符及标点符号 2014-1-26
//       (4) 在使用过程中可以自由切换横竖屏显示  2014-1-27
//			 (5) 后续支持uCGUI的调用,能够读写像素的颜色值 2014-1-27
//			 (6) 与电阻触摸屏配合实现菜单操作、涂鸦等
//			 (7) 支持屏幕校准
//			 (8) ***支持显示BMP、JPG等格式图片
//			 (9) ***支持从存储器(SD卡或FLASH)上读取到数据流并显示
//			 (10) 支持开关背光灯 2014-1-27
//			 (11) 支持叠加显示,即字符能在图片上显示，而不现实字符的背景色
//			 (12) 支持图片、字符的滚动显示，没有拖影,支持图片缩放显示
//			 (13) 支持显示视频文件
//			 (14) 支持实时显示图像信息，如拍照
//       (15) 支持二次开发，为客户应用留出接口
//       (16)
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include "SPFD5408.h"	
#include "16x8.h"	
#include "GB1616.h"		 
#include "GB2424.h"

//-----------------------------------------------------------------
// 初始化程序区
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// void GPIO_SPFD5408_Configuration(void)
//-----------------------------------------------------------------
//
// 函数功能: 液晶引脚配置函数
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: RCC_APB2PeriphClockCmd(); GPIO_Init();
// 注意事项: 用GPIO驱动方式和FSMC驱动方式下的引脚配置不一样
//			
//-----------------------------------------------------------------
void GPIO_SPFD5408_Configuration(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;

	// 使能IO口时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | 
													RCC_APB2Periph_GPIOE, ENABLE);  
	// 部分数据线、部分控制线配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 	| GPIO_Pin_1 	|	// D2,D3 
																GPIO_Pin_4 	| GPIO_Pin_5 	| // RD,WR
																GPIO_Pin_7 	| GPIO_Pin_8 	| // CS,D13
																GPIO_Pin_9 	| GPIO_Pin_10 | // D14,D15
																GPIO_Pin_11 | GPIO_Pin_14 | // RS,D0
																GPIO_Pin_15;								// D1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	// 推挽复用输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;						 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	// 部分数据线配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 	| GPIO_Pin_8 	|	// D4,D5
																GPIO_Pin_9 	| GPIO_Pin_10 | // D6,D7
																GPIO_Pin_11 | GPIO_Pin_12 | // D8,D9
																GPIO_Pin_13 | GPIO_Pin_14 | // D10,D11
																GPIO_Pin_15;								// D12
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;						// 推挽复用输出
	GPIO_Init(GPIOE, &GPIO_InitStructure); 
  // 液晶复位，背光灯引脚配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;	// RST,LED
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;					// 推挽输出
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	SPFD5408_LED_OFF;            															// LED关
}  
//-----------------------------------------------------------------
// void STM32_FSMC_Init(void)
//-----------------------------------------------------------------
//
// 函数功能: 配置FSMC
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void STM32_FSMC_Init(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;
	// 打开FSMC时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);	
	// 打开复用功能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	

  // FSMC_Bank1_NORSRAM1 时间配置
  // 地址建立时间期限   
	p.FSMC_AddressSetupTime = 0x02;											                                        
	// 地址的持续时间 
	p.FSMC_AddressHoldTime = 0x00;											
	// 设定数据时间期限
	p.FSMC_DataSetupTime = 0x05;												
 	// 总线转向时间
	p.FSMC_BusTurnAroundDuration = 0x00;							
	// CLK时钟输出信号的HCLK周期数表示时间
  p.FSMC_CLKDivision = 0x00;			
	// 指定在获得第一个数据前的时钟周期
  p.FSMC_DataLatency = 0x00;			
  p.FSMC_AccessMode = FSMC_AccessMode_B;
	// 指定的FSMC块
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	// 地址和数据值不复用的数据总线 
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = 
									FSMC_DataAddressMux_Disable; 
	// 外部存储器的类型
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = 
														FSMC_MemoryType_NOR;
	// 数据宽度
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = 
														FSMC_MemoryDataWidth_16b;
	// 禁用突发访问模式
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = 
														FSMC_BurstAccessMode_Disable;
	// 指定等待信号的极性
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = 
														FSMC_WaitSignalPolarity_Low;
	// disables the Wrapped burst access mode for Flash
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = 
														FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = 
														FSMC_WaitSignalActive_BeforeWaitState;
	// 启用指定的FSMC块的写操作
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation =
														FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = 
														FSMC_WaitSignal_Disable;
	// 扩展模式
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = 
														FSMC_ExtendedMode_Disable;
	// 禁用写突发操作
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = 
														FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;      
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
  // 使能 FSMC Bank1_SRAM 
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

//-----------------------------------------------------------------
// void SPFD5408_Init(void)
//-----------------------------------------------------------------
//
// 函数功能: 液晶初始化函数
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPFD5408_Delay(), SPFD5408_Write_Data(), 
// 					 SPFD5408_Write_Reg (). SPFD5408_WR_Start().   
// 注意事项: 无
//-----------------------------------------------------------------
void SPFD5408_Init(void)
{	 
	GPIO_SPFD5408_Configuration();						// IO配置
	STM32_FSMC_Init();												// FSMC初始化
	SPFD5408_RST_Set;													// 下降沿复位	
	SPFD5408_Delay(10);
	SPFD5408_RST_Clr;								 	   	
	SPFD5408_Delay(10);
	SPFD5408_RST_Set;								 	 
	SPFD5408_Delay(10); 
	
	SPFD5408_Write_Data(0xffff);							// 数据线全高
	SPFD5408_Write_Reg(0x0000,0x0001);   			// 停止内部晶体振荡器 
	SPFD5408_Write_Reg(0x0001,0x0100);   			// 输出驱动控制

#if ID_AM==000    
	SPFD5408_Write_Reg(0x0003,0x1000);				// 横屏显示：水平、垂直递减 
#elif ID_AM==001
	SPFD5408_Write_Reg(0x0003,0x1008);				// 竖屏显示：水平、垂直递减  
#elif ID_AM==010
	SPFD5408_Write_Reg(0x0003,0x1010);				// 横屏显示：水平递增、垂直递减
#elif ID_AM==011 
	SPFD5408_Write_Reg(0x0003,0x1018);				// 竖屏显示：水平递增、垂直递减
#elif ID_AM==100
	SPFD5408_Write_Reg(0x0003,0x1020);				// 横屏显示：水平递减、垂直递增
#elif ID_AM==101
	SPFD5408_Write_Reg(0x0003,0x1028);				// 竖屏显示：水平递减、垂直递增 
#elif ID_AM==110
	SPFD5408_Write_Reg(0x0003,0x1030);				// 横屏显示：水平、垂直递增
#elif ID_AM==111
	SPFD5408_Write_Reg(0x0003,0x1038);				// 竖屏显示：水平、垂直递增
#endif
//																					// D3.AM选择垂直(1)/水平扫描(0)   
//												    							// D5(ID1):D4(ID0):控制计数器增减方向
	SPFD5408_Write_Reg(0x0004,0x0000);   			// 重新定位显示尺寸控制寄存器
	SPFD5408_Write_Reg(0x0008,0x0207);   			// 显示控制2 	
	SPFD5408_Write_Reg(0x0009,0x0000);   
	SPFD5408_Write_Reg(0x000a,0x0000);   			 	
	SPFD5408_Write_Reg(0x000C,0x0000);   			// RGB显示接口控制
	SPFD5408_Write_Reg(0x000D,0x0000);   
	SPFD5408_Write_Reg(0x000F,0x0000);   			 	
	SPFD5408_Write_Reg(0x0007,0x0101);
	// 电源控制   
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
	   
	// GAMMA控制						 	
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
	
	// 显示尺寸设置  
	SPFD5408_Write_Reg(0x0050,0x0000); 				// HSA7~HSA0  
	SPFD5408_Write_Reg(0x0051,239);   	    	// HEA7~HEA0
	SPFD5408_Write_Reg(0x0052,0x0000);     		// VSA8~VSA0
	SPFD5408_Write_Reg(0x0053,319); 					// VEA8~VEA0
	 
	// 门扫描控制 			 	
	SPFD5408_Write_Reg(0x0060,0x2700);		 		  
	SPFD5408_Write_Reg(0x0061,0x0001);     		 
	SPFD5408_Write_Reg(0x006A,0x0000);   			 	
	
	SPFD5408_Write_Reg(0x0080,0x0000);   			// 部分图像1显示位置
	SPFD5408_Write_Reg(0x0081,0x0000);   			// 部分图像1RAM起始地址
	SPFD5408_Write_Reg(0x0082,0x0000);   			// 部分图像1RAM终止地址
	SPFD5408_Write_Reg(0x0083,0x0000);   			// 部分图像2显示位置
	SPFD5408_Write_Reg(0x0084,0x0000);   			// 部分图像2RAM起始地址
	SPFD5408_Write_Reg(0x0085,0x0000);   			// 部分图像2RAM终止地址
												
	// 屏幕接口控制1					
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
	SPFD5408_LED_ON;  												//  打开背光灯
	SPFD5408_WR_Start();
}
//-----------------------------------------------------------------
// SPFD5408驱动区
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// void SPFD5408_Write_Data(u16 Digital)
//-----------------------------------------------------------------
//
// 函数功能: SPFD5408写数据
// 入口参数: 要写入的参数
// 返回参数: 无
// 全局变量: 无
// 调用模块: 无   
// 注意事项: 无
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
// 函数功能: SPFD5408读数据
// 入口参数: 要读出的参数
// 返回参数: 无
// 全局变量: 无
// 调用模块: 无   
// 注意事项: 为了兼容FSMC与GPIO两种驱动液晶，故设计普通IO口为FSMC
//					 所占用的引脚，即GPIO在数据传送时须要对IO状态重新组合和分配
//-----------------------------------------------------------------
u16 SPFD5408_Read_Data(void)
{
	u16 ReadValue=0;
#ifdef FSMC
	ReadValue = (*(volatile uint16_t *) (LCD_Data_Addr)); 
#else	
	u16 Data[2],tmp[3];
	
	GPIOD->ODR = (GPIOD->ODR |0xc703);				// 把PD0,1,8,9,10,14,15置1 
	GPIOE->ODR = (GPIOE->ODR |0XFF80);				// 把PE7~PE15置1  
	SPFD5408_RS_Set;	
	SPFD5408_CS_Clr;
	SPFD5408_RD_Clr;  
	SPFD5408_RD_Set;
	// 读取数据(读GRAM时,需要读2次)
	SPFD5408_RD_Clr;  
	SPFD5408_RD_Set;
	// 从分布在不同IO口的数值组织成16位数据
	Data[0] = GPIO_ReadInputData(GPIOD);    	// 数据输入 
	Data[1] = GPIO_ReadInputData(GPIOE);    	// 数据输入 

	tmp[0]  = (Data[0]&0x0700)<<5;            // 取PD8-10放到D13-D15
	tmp[1]  =	(Data[0]&0x0003)<<2;			  		// 把PD1-PD0放到D3-D2
	tmp[2]  = (Data[0]&0xc000)>>14;						// 把PD15-PD14放到D1-D0
	// 取PE15-7位放到数据的D12-D4位
	ReadValue = ((Data[1]&0xff80)>>3)+tmp[0]+tmp[1]+tmp[2];
	SPFD5408_CS_Set;
#endif	
	return (ReadValue);
}

//-----------------------------------------------------------------
// void SPFD5408_Write_Reg(u16 RegIndex, u16 RegValue)
//-----------------------------------------------------------------
//
// 函数功能: 配置SPFD5408寄存器的函数
// 入口参数: RegIndex:寄存器地址
//				   RegValue:寄存器数据
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPFD5408_Write_Data().   
// 注意事项:
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
// 函数功能: 设置当前坐标
// 入口参数: X:X坐标
//				   Y:Y坐标
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPFD5408_Write_Reg ().   
// 注意事项:
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
// 函数功能: 写GRAM
// 入口参数: 无
// 返回参数: 无
// 全局变量: GRAM_WR.
// 调用模块: SPFD5408_Write_Data ().   
// 注意事项:
//-----------------------------------------------------------------
void SPFD5408_WR_Start(void)
{
	*(volatile uint16_t *) (LCD_Reg_Addr)= GRAM_WR;
}

//-----------------------------------------------------------------
// void SPFD5408_Write_Pixel(u16 x,u16 y, u16 Color)
//-----------------------------------------------------------------
//
// 函数功能: 设置液晶上一个点的颜色
// 入口参数: X:X坐标, Y:Y坐标, Color:颜色值
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPFD5408_Set_Cursor(), SPFD5408_WR_Start(),
//					 SPFD5408_Write_Data().
// 注意事项: 无
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
// 函数功能: 读取液晶上一个点的颜色
// 入口参数: X:X坐标, Y:Y坐标 
// 返回参数: 原色值
// 全局变量: 无
// 调用模块: SPFD5408_Set_Cursor(),  SPFD5408_BGR2RGB().  
// 注意事项: 无
//-----------------------------------------------------------------
u16 SPFD5408_Read_Pixel(u16 x,u16 y)
{
	SPFD5408_Set_Cursor(x,y);									// 设置坐标
	SPFD5408_WR_Start();       								// 选择GRAM地址 		
	
	return (SPFD5408_BGR2RGB(SPFD5408_Read_Data()));
}

//-----------------------------------------------------------------
// u16 SPFD5408_BGR2RGB(u16 c)
//-----------------------------------------------------------------
//
// 函数功能: RRRRRGGGGGGBBBBB 改为 BBBBBGGGGGGRRRRR 格式
// 入口参数: c:GBR格式的颜色值
// 返回参数: RGB格式的颜色值
// 全局变量: 无
// 调用模块: 无   
// 注意事项: 从SPFD读出的数据为GBR格式, 而我们写入的时候为RGB格式 
// 					 通过该函数转换.
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
// 函数功能: 设置窗口大小
// 入口参数: X_Size: x轴长度,  Y_Size: y轴长度.
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPFD5408_Write_Reg (), SPFD5408_WR_Start ().   
// 注意事项:
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
// 函数功能: 清屏函数
// 入口参数: Color:颜色值
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPFD5408_Set_Cursor (), SPFD5408_Set_Window(), 
// 					 SPFD5408_Write_Data().   
// 注意事项:
//-----------------------------------------------------------------
void SPFD5408_Clear_Window(u16 Color)
{
	u32 Point = 0;

	SPFD5408_Set_Cursor (XSTART, YSTART);			// 设置起始点
	SPFD5408_Set_Window (XSIZE, YSIZE);  		 
	SPFD5408_RS_Set;
	for (Point = XSTART*YSTART; Point < XSIZE*YSIZE; Point ++)	
		SPFD5408_Write_Data (Color);						// 画点
	SPFD5408_CS_Set; 
}

//-----------------------------------------------------------------
// void SPFD5408_Fill(u16 x0,u16 y0,u16 x1,u16 y1,u16 Color)
//-----------------------------------------------------------------
//
// 函数功能: 给某一区域填充某一种颜色
// 入口参数: x0, y0: 起始坐标. x1, y1: 终点坐标. Color:颜色值
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPFD5408_Set_Cursor(), SPFD5408_Write_Reg(), 
// 					 SPFD5408_Write_Data().   
// 注意事项:
//-----------------------------------------------------------------
void SPFD5408_Fill(u16 x0,u16 y0,u16 x1,u16 y1,u16 Color)
{
	u16 x,y;
	
// 设置起始点
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
//字符(串)显示区域
//-----------------------------------------------------------------
//-----------------------------------------------------------------
//	 void GetASCIICode(
//		u8* pBuffer,													// 显示缓冲CharBuffer[] 
//		u8 ASCII															// 输入的ASCII码, 如'A'
//	)
//-----------------------------------------------------------------
//
// 函数功能: 取ASCII码子程序
// 入口参数: * pBuffer:显示缓冲CharBuffer[], 
//               ASCII: 输入的ASCII码, 如'A'.
// 返回参数: 无 
// 全局变量: 无
// 调用模块: 无   
// 注意事项: 输入一个ASCII码，取得它在font16x8中的16Byte显示代码
//				   并将其存放到一个16byte的ASCII显示缓冲CharBuffer[]中.
//-----------------------------------------------------------------
void GetASCIICode(
	u8* pBuffer,															// 显示缓冲CharBuffer[] 
	u8 ASCII																	// 输入的ASCII码, 如'A'
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
//		u8* pBuffer,													// 汉字字模缓冲 
//		u8 Chi[2]															// 要查找的汉字
//	)
//-----------------------------------------------------------------
//
// 函数功能: 取16x16中文字模子程序
// 入口参数: * pBuffer: 汉字字模指针. Chi[2]: 要查找字模的汉字.
// 返回参数: 无
// 全局变量: codeGB_16: 字模数组.
// 调用模块: 无   
// 注意事项: 输入一个中文，取得它在codeGB_16数组中的字模,
//					 并将其存放到一个32byte的显示缓冲ChineseBuffer[].
//-----------------------------------------------------------------
void GetChineseCode(
	u8* pBuffer,															// 汉字字模缓冲
	u8 Chi[2]																	// 要查找的汉字
)
{	
	u8 k, i;
	// 最多查找100 个汉字, 多于100 个无法查找. 
	// 也可以修改k 的最大值进行增加查找范围.
	for (k = 0; k < 100; k ++)								
	{																																			
		if ((codeGB_16[k].Index[0] == Chi[0]) && (codeGB_16[k].Index[1] == Chi[1]))
		{																				// 对比输入的汉字和数组中的汉字是否一致,  					
			for(i=0;i<32;i++)											// 若一致则取出字模数据.
			{
				*(pBuffer+i) = codeGB_16[k].Msk[i] ;// 读取字模数据
			}
			return ;													
		}
}
	for(i = 0; i < 32; i ++)
	{	// 若字库中没有要查找的汉字, 全部清理. 即显示背景色,
		// 其效果相当于没显示, 跳过此汉字.
		*(pBuffer + i) = 0x00 ;									
	}
}

//-----------------------------------------------------------------
//	void GetChineseCode24x24(
//		u8* pBuffer,													// 显示缓冲
//		u8 Chi[2]															// 要查找的数据
//	)
//-----------------------------------------------------------------
//
// 函数功能: 取24x24中文字模子程序
// 入口参数: * pBuffer:  读取的字模缓冲, Chi[2]: 要查找的汉字.
// 返回参数: 无
// 全局变量: codeGB_24x24: 字模数组
// 调用模块: 无   
// 注意事项: 输入一个中文，取得它在codeGB_24x24数组中的字模数据
//				 并将其存放到一个32byte的显示缓冲ChineseBuffer[]
//-----------------------------------------------------------------
void GetChineseCode24x24(
	u8* pBuffer,															// 显示缓冲 
	u8 Chi[2]																	// 要查找的数据
)
{	
	u8 k, i;
	// 最多查找100 个汉字, 多于100 个无法查找. 
	// 也可以修改k 的最大值进行增加查找范围.
	for (k = 0; k < 100; k ++)									
	{	// 对比输入的汉字和数组中的汉字是否一致, 若一致则取出字模数据.   
		if ((codeGB_24x24[k].Index[0] == Chi[0]) && 
				(codeGB_24x24[k].Index[1] == Chi[1]))	
		{
			for(i = 0; i < 72; i ++)
			{	// 读取字模数据
				*(pBuffer+i) = codeGB_24x24[k].Msk[i] ;
			}
			return ;																
		}
	}
	for(i = 0; i < 72; i ++)
	{	// 若字库中没有要查找的汉字, 全部清理. 即显示背景色,
		// 其效果相当于没显示, 跳过此汉字.
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
// 函数功能: LCD写1个ASCII字符函数
// 入口参数: x: 横向坐标, 由左到右分别是0~239.  
//					 y: 纵向坐标, 由上到下分别为0~319.
//           color: 指定LCD全屏颜色 RGB(5-6-5).
//					 CharColaor: 字符的颜色. 
//				 	 CharBackColor: 字符背景颜色. 		
//					 ASCIICode: 相应字符的ASCII码.
// 返回参数: 无
// 全局变量: 无 
// 调用模块: LCD_SetBox(), SPFD5408_WR_Start(), GetASCIICode().    
// 注意事项: 如果单独使用此函数则应该加上LCD_Rs_H()	
//					 和SPFD5408_CS_Set;为了优化系统省去了这个指令
//				   假设此函数执行的上一条语句是写命令，（RS_L情况）
//				   则写入将出错，因为ILI9320认为当RS_L时写入的是命令
//-----------------------------------------------------------------
void LCD_WriteASCII(
	u8  x,																		// X轴坐标	
	u16 y,																		// y轴坐标
	u16 CharColor,														// 字体颜色
	u16 CharBackColor,												// 背景颜色
	u8 ASCIICode															// ADCII码
)
{
	u8 RowCounter,BitCounter;
	u8 *ASCIIPointer;
	u8 ASCIIBuffer[16];
  // 判断横屏还是竖屏,
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox (x, y, x + 8, y + 16);    			// 设置窗口大小
#else
	LCD_SetBox(x , y, x + 16, y + 8);    
#endif 
	// 启动写数据函数, 每次写数据前先调用此函数       
	SPFD5408_WR_Start();	
	// 取这个字符的显示代码										
	GetASCIICode(ASCIIBuffer,ASCIICode);			
	ASCIIPointer=ASCIIBuffer;
  // 一个ASCII字模有16个8Bit数据组成
	for (RowCounter=0; RowCounter<16; RowCounter++)	
	{ // 扫描一个8bit字模数据的每一位
		for (BitCounter=0; BitCounter<8; BitCounter++)
		{ // 判断字模数据为'0'还是为'1'.为'0'写入背景颜色,为'1'写入字体颜色
			if ((*ASCIIPointer&(0x80 >> BitCounter)) == 0x00) 
			{
				SPFD5408_Write_Data(CharBackColor);	// 写入背景颜色数据
			}
			else
			{
				SPFD5408_Write_Data(CharColor);			// 写入字体颜色数据
			}
		}
		ASCIIPointer++;													// 指向下一个字模数据
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
// 函数功能: 向指定位置写入一个或多个字符，本函数带自动换行功能
// 入口参数: x: 横向坐标, 由左到右分别是0~239. 
//					 y: 纵向坐标, 由上到下分别为0~319.
//	         CharColaor: 字符的颜色. CharBackColor: 字符背景颜色.
//	         *s: 指向要写的字符串.
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_WriteASCII().   
// 注意事项:
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
// 函数功能: LCD写1个中文函数
// 入口参数: x: 横向坐标, 由左到右分别是0~239.
//					 y: 纵向坐标, 由上到下分别为0~319.
//				  CharColor: 字符的颜色. 		
//					CharBackColor: 字符背景颜色.
//          *sp: 相应中文的编码.
// 返回参数: 无
// 全局变量: 无
// 调用模块:  LCD_SetBox(), SPFD5408_WR_Start(), GetChineseCode(), 
//						SPFD5408_Write_Data().   
// 注意事项: 320*240分辨率的显示屏，横向能显示15中文字符，竖向能显示20行.
//				   如果单独使用此函数则应该加上LCD_Rs_H()和SPFD5408_CS_Set;
// 					 为了优化系统省去了这个指令,
//				  假设此函数执行的上一条语句是写命令, (RS_L情况)则写入将出错, 
//				  因为SPFD5408认为当RS_L时写入的是命令
//-----------------------------------------------------------------
void LCD_WriteChinese(
		u8 x,																		// x坐标
		u16  y,																	// y坐标
		u16 CharColor,													// 字体颜色
		u16 CharBackColor,											// 背景颜色
		u8 *sp																	// 指向汉字字符的指针
	)
{
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];											// 汉字字模缓存
	// 判断屏幕方向	  
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox (x, y, x + 16, y + 16);    		// 设置窗口大小
#else
	LCD_SetBox (x, y, x + 16, y + 16);    
#endif 
  SPFD5408_WR_Start();											// 启动写数据
  GetChineseCode(ChineseBuffer, sp);				// 得到中文字符编码
  ChinesePointer=ChineseBuffer;
  // 16x16字体包括32个8bit数据      
  for(ByteCounter=0; ByteCounter<32; ByteCounter++)	
  { // 扫描8bit字模数据的每一bit																
    for(BitCounter=0;BitCounter<8;BitCounter++)	
    { // 某一位为‘0’
      if((*ChinesePointer & (0x80 >> BitCounter)) == 0x00)
      {
        SPFD5408_Write_Data(CharBackColor);	// 写入背景颜色
      }
      else																	// 某一位为‘1’
      {
        SPFD5408_Write_Data(CharColor); 		// 写入字体颜色
      }
    }
    ChinesePointer++;												// 指向下一字模数据
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
// 函数功能: LCD写中文字符串函数
// 入口参数: x: 横向坐标, 由左到右分别是0~239. 
//					 y: 纵向坐标, 由上到下分别为0~319.
//				 	 CharColor: 字符的颜色. CharBackColor: 字符背景颜色 .
//         	 *sp: 相应中文的编码.
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_WriteChinese().   
// 注意事项: 无
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
// 函数功能: LCD写字符串函数, 可以英文字符和汉字字符混合写.
// 入口参数: x: 横向坐标, 由左到右分别是0~239.  
//					 y: 纵向坐标, 由上到下分别为0~319
//				 CharColor: 字符的颜色. 
//				CharBackColor: 字符背景颜色.
//         *s: 相应字符的编码.
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_WriteASCII(),  LCD_WriteChinese().  
// 注意事项:
//-----------------------------------------------------------------
void LCD_WriteString(
		u8 x, 																	// X轴坐标
		u16  y, 																// y轴坐标
		u16 CharColor, 													// 字体颜色
		u16 CharBackColor, 											// 背景颜色
		u8 *s 																	// 指向字符串的字符指针
	)
{
	u8 * pt;
	u8 tem;

	pt = s;
	while (*pt)
	{	// 要显示的字符为英文字符16X8
		if (*pt < 128)													
		{
			tem = *pt;
			// 调用显示ASCII码函数
			LCD_WriteASCII (x, y, CharColor, CharBackColor, tem);	
			pt += 1 ;
			// 判断屏幕方向, 水平方向为X轴方向
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  		
			// 显示坐标小于231, 向后移8 个像素点, 显示下一个字符
			if (x < 231)													
			{
				x += 8;
			} 
			else if (y < 303)		// 一行写满,水平方向从0 开始,竖直方向增加16个像素点									
			{
				x = 0;
				y += 16;
			}   
			else								// 水平方向和竖直方向都写满, 继续从(0, 0)开始显示																
			{
				x = 0;
				y = 0;
			}
			
#else											// 水平方向为y轴方向
			if (y<311)					// 水平方向坐标小于311, 像素点加8, 继续显示下一个字符
			{
				y += 8;
			}
			else if (x < 223)		// 如果竖直方向没写满, 水平方向从0开始显示, 竖直方向增加16个像素点
			{
				y = 0;
				x += 16;
			}
			else								// 水平方向和竖直方向都写满, 从(0, 0)开始显示
			{
				x = 0;
				y = 0;
			}
#endif 
		}
		else									// 显示的是中文字符16X16
		{	// 调用显示中文字符函数
		 	LCD_WriteChinese(x, y, CharColor, CharBackColor, pt);	
			pt += 2 ;						// 中文字符占用两个字节, 所以指针偏移加‘2’
			// 水平方向为X轴方向
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  		
			if (x<223)					// X坐标小于223, X方向增加16个像素点, 显示下一个字符
			{
				x += 16;
			} 
			else if (y < 303)		// y小于303, x方向从0开始, y增加16个像素点, 换行显示
			{
				x = 0;
				y += 16;
			}   
			else								// x方向和y方向都写满, 从(0, 0)开始显示
			{
				x = 0;
				y = 0;
			}     
#else											// 竖直方向为y轴方向
			if (y < 303)				// X坐标小于303, y方向增加16个像素点, 显示下一个字符
			{
				y += 16;
			}
			else if (x < 223)		// y小于223, y方向从0开始, x增加16个像素点, 换行显示
			{
				y = 0;
				x += 16;
			}
			else								// x方向和y方向都写满, 从(0, 0)开始显示
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
// 函数功能: LCD写1个中文函数(24X24)
// 入口参数: x: 横向坐标, 由左到右分别是0~239. 
// 					 y: 纵向坐标, 由上到下分别为0~319.
//				 CharColor: 字符的颜色, 
//				CharBackColor: 字符背景颜色.
//         	 *sp: 相应中文的编码.
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_SetBox(), SPFD5408_WR_Start(), 
//					 GetChineseCode24x24(), SPFD5408_Write_Data().  
// 注意事项:
//-----------------------------------------------------------------
void LCD_WriteChinese24x24(
		u8 x,																		// x轴坐标
		u16  y,																	// y轴坐标
		u16 CharColor,													// 字体颜色
		u16 CharBackColor,											// 背景颜色
		u8 *sp																	// 中文字符指针
	)
{

  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer24x24[72];								// 24x24字体缓存
	// 检测频幕显示方向	  
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox (x, y, x + 24, y + 24);    		// 设置窗口
#else
	LCD_SetBox (x, y, x + 24, y + 24);    
#endif 
  SPFD5408_WR_Start();											//	启动写数据
  GetChineseCode24x24(ChineseBuffer24x24, sp);// 得到中文字符编码
  ChinesePointer=ChineseBuffer24x24;
  // 24x24字体包括72个8bit数据 
  for(ByteCounter=0; ByteCounter<72; ByteCounter++)
  { //	字模数据的每一bit																		
    for(BitCounter=0;BitCounter<8;BitCounter++)				
    {																				// 若为‘0’																		
      if((*ChinesePointer & (0x80 >> BitCounter)) == 0x00)	
      {
        SPFD5408_Write_Data(CharBackColor);	// 显示背景颜色
      }
      else																	// 若为‘1’
      {
        SPFD5408_Write_Data(CharColor); 		// 显示字体颜色
      }
    }
    ChinesePointer++;												// 指向下一字模数据
  }
  SPFD5408_CS_Set;	  											// 取消片选
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
// 函数功能: LCD写中字符串函数(24X24)
// 入口参数: x: 横向坐标, 由左到右分别是0~239. 
//					 y: 纵向坐标, 由上到下分别为0~319.
//				 CharColor: 字符的颜色, 
//				CharBackColor: 字符背景颜色.
//         	 *sp: 相应中文的编码.
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_WriteChinese24x24().   
// 注意事项:
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
// 函数功能: LCD显示字符数值
// 入口参数: x: 横向坐标, 由左到右分别是0~239. 
//					 y: 纵向坐标, 由上到下分别为0~319.
//				 CharColor: 字符的颜色,
//				CharBackColor: 字符背景颜色.
//         	 udata: 字符数值.
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_WriteString().   
// 注意事项: (1) 显示的值为0~255之间
//				 	 (2) 数据显示格式: 右对齐
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
				temp[i - 1] = ' ';									// 高位显示空格
			else
				temp[2] = '0';											// 显示0
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
// 函数功能: LCD显示整形数值
// 入口参数: x: 横向坐标, 由左到右分别是0~239. 
//					 y: 纵向坐标, 由上到下分别为0~319.
//				 CharColor: 字符的颜色, 
// 			CharBackColor: 字符背景颜色.
//         	 udata: 整形数据数值.
// 返回参数: 无 
// 全局变量: 无
// 调用模块: LCD_WriteString().   
// 注意事项: (1) 显示的值为0~65535之间
//				 	 (2) 数据显示格式: 右对齐
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
				temp[i - 1] = ' ';									// 高位显示空格
			else
				temp[4] = '0';											// 显示0
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
// 函数功能: LCD显示整形数值
// 入口参数: x: 横向坐标, 由左到右分别是0~239. 
//					 y: 纵向坐标, 由上到下分别为0~319.
//				 CharColor: 字符的颜色, 
//				CharBackColor: 字符背景颜色.
//         	 udata: 长整形数据数值.
// 返回参数: 无 
// 全局变量: 无
// 调用模块: LCD_WriteString().   
// 注意事项: (1) 显示的值为0~4294967296之间
//				   (2) 数据显示格式: 右对齐
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
				temp[i - 1] = ' ';									// 高位显示空格
			else
				temp[9] = '0';											// 显示0
		}
	}
	temp[10] = '\0';
	LCD_WriteString (x, y, CharColor, CharBackColor, temp);
}

//-----------------------------------------------------------------
// 图形显示区域
//-----------------------------------------------------------------

//-----------------------------------------------------------------
//	void LCD_SetBox(
//	   u8 xStart,														// x方向起始点
//	   u16  yStart,													// y方向起始点
//	   u8 xStop,						  						  // x方向终止点
//	   u16  yStop						   							// y方向终止点
//	)
//-----------------------------------------------------------------
//
// 函数功能: LCD块选函数, 选定LCD上指定的矩形区域.
// 入口参数: xStart: x方向的起始点, ySrart: y方向的终止点
//         	 xStop:  x方向终止点,   yStop:  y方向终止点
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPFD5408_Set_Cursor(), SPFD5408_Write_Reg().   
// 注意事项: xStart和 yStart随着屏幕的旋转而改变，位置是矩形框的四个角
//-----------------------------------------------------------------
void LCD_SetBox(
   u8 xStart,                								// x方向起始点
   u16  yStart,															// y方向起始点
   u8 xStop,							   								// x方向终止点
   u16  yStop							   								// y方向终止点
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
	SPFD5408_Write_Reg (0x0050, xStart);						// 水平 GRAM起始位置
	SPFD5408_Write_Reg (0x0051, xStart + xLong - 1);// 水平GRAM终止位置
	SPFD5408_Write_Reg (0x0052, yStart);						// 垂直GRAM起始位置
	SPFD5408_Write_Reg (0x0053, yStart + yLong - 1);// 垂直GRAM终止位置 
}
 
//-----------------------------------------------------------------
//	void LCD_ColorBox(
//		u8 xStart,                       			// x方向的起始点
//		u16  yStart,													// y方向的起始点
//		u8 xStop,															// x方向的终止点
//		u16  yStop,														// y方向的终止点
//		u16 Color						    							// 填充颜色
//	)
//-----------------------------------------------------------------
//
// 函数功能: 向矩形区域填充颜色
// 入口参数: xStart: x方向的起始点, ySrart: y方向的终止点
//         	 xStop:  x方向终止点,   yStop:  y方向终止点
//				 Color: 填充颜色
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_SetBox(), SPFD5408_WR_Start(), SPFD5408_Write_Data().   
// 注意事项:
//-----------------------------------------------------------------
void LCD_ColorBox(
	u8 xStart,                 								// x方向的起始点
	u16  yStart,															// y方向的起始点
	u8 xStop,																	// x方向的终止点
	u16  yStop,																// y方向的终止点
	u16 Color						    									// 填充颜色
)
{
	u8 xLong;																	// 矩形x方向长度
	u16  yLong;						    								// 矩形y方向长度
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
//		u8 x,																	// 横坐标
//		u16  y,							 									// 纵坐标
//		u16 Color															// 写入的颜色
//	)
//-----------------------------------------------------------------
//
// 函数功能: 将指定区域擦为指定颜色
// 入口参数: x: x方向的起始点, 
//					 y: y方向的起始点,
//           color: 指定LCD全屏颜色 RGB(5-6-5).
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_SetBox(), SPFD5408_WR_Start(), SPFD5408_Write_Data().   
// 注意事项:
//-----------------------------------------------------------------
void LCD_ClearCharBox(
	u8 x,																			// 横坐标
	u16  y,							 											// 纵坐标
	u16 Color																	// 写入的颜色
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
// 函数功能: 用指定的颜色在指定的坐标位置上画出一个点
// 入口参数: x: 横向坐标, 由左到右分别是0~239. 
//					 y: 纵向坐标, 由上到下分别为0~319.
//	         Color: 描点的颜色.
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_ColorBox().   
// 注意事项:
//-----------------------------------------------------------------
void LCD_SetPoint(u8 x, u16 y, u16 Color)
{  
	LCD_ColorBox (x, y, x, y, Color);					// 可以设定一个点的大小
	//SPFD5408_Read_Pixel(X,Y,Color);
}

//-----------------------------------------------------------------
// void LCD_HLine(u8 x, u16 y, u16 Length, u16 Color)
//-----------------------------------------------------------------
//
// 函数功能: 用指定的颜色在指定位置上画出指定长度的一条横线 
// 入口参数: x: 横向起点坐标, 由左到右分别是0~239. 
//					 y: 纵向起点坐标, 由上到下分别为0~319.
//				 	Length: 画线长度. Color: 描点的颜色.
// 返回参数: 无 
// 全局变量: 无
// 调用模块: LCD_SetBox(),SPFD5408_WR_Start(),SPFD5408_Write_Data(). 
// 注意事项:
//-----------------------------------------------------------------
void LCD_HLine(u8 x, u16 y, u16 Length, u16 Color) 
{  																					// 判断屏幕方向
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox(x, y, x + Length + 1, y + 1);	// 设置窗口
#else
	LCD_SetBox(x, y, x + 1, y + Length + 1);
#endif 
	SPFD5408_WR_Start();											// 启动写数据
 	do
	{  
		SPFD5408_Write_Data(Color);							// 写入颜色数据						
		Length--;
	}while(Length);
	SPFD5408_CS_Set;													// 取消片选
}

//-----------------------------------------------------------------
//	void LCD_HDotLine(
//		u8 x, 																// x 起始坐标
//		u16 y, 																// y 起始坐标
//		u16 Length, 													// 长度
//		u16 Color,														// 线颜色
//		u16 CharBackColor											// 背景颜色
//	) 
//-----------------------------------------------------------------
//
// 函数功能: 用指定的颜色在指定位置上画出指定长度的一条横虚线
// 入口参数: x: 横向起点坐标, 由左到右分别是0~239. 
//					 y: 纵向起点坐标, 由上到下分别为0~319.
//				 Length: 画线长度.
//					Color: 描点的颜色.
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_SetBox(), SPFD5408_WR_Start(), SPFD5408_Write_Data().   
// 注意事项:
//-----------------------------------------------------------------
void LCD_HDotLine(
		u8 x, 																	// x 起始坐标
		u16 y, 																	// y 起始坐标
		u16 Length, 														// 长度
		u16 Color,															// 线颜色
		u16 CharBackColor												// 背景颜色
	) 
{  																					// 判断屏幕方向
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox(x, y, x + Length + 1, y + 1);	// 设置窗口
#else
	LCD_SetBox(x, y, x + 1, y + Length + 1);
#endif 
	SPFD5408_WR_Start();											// 启动写数据
	do
	{ 
		if ((Length % DOT) == 0)								// 间隔DOT点画虚线
		{
			SPFD5408_Write_Data(Color);						// 写入线颜色
		} 
		else																		// 其它点
		{
			SPFD5408_Write_Data(CharBackColor);		// 写入背景颜色	
		}
		Length--;
	}while(Length);
	SPFD5408_CS_Set;												 	// 取消片选
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
// 函数功能: 用指定的颜色在指定位置上画出指定长度的一条竖线
// 入口参数: x: 横向起点坐标, 由左到右分别是0~239.
//					 y: 纵向起点坐标, 由上到下分别为0~319.
//			    High: 画线长度. 
//				 Color: 描点的颜色.
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_SetBox(), SPFD5408_WR_Start(),SPFD5408_Write_Data().   
// 注意事项:
//-----------------------------------------------------------------
void LCD_RLine(
		u8 x,																		// x轴坐标 
		u16  y, 																// y轴坐标
		u16  High, 															// 线高度
		u16 Color																// 线颜色
	) 
{  																					// 判断屏幕显示方向
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox(x, y, x + 1, y + High + 1);		// 设置窗口
#else
	LCD_SetBox(x, y, x + High + 1, y + 1);
#endif 
	SPFD5408_WR_Start();											// 启动写数据
	do
	{  
		SPFD5408_Write_Data(Color);							// 写入颜色数据
		High--;
	}while(High);
	SPFD5408_CS_Set;													// 取消片选
}

//-----------------------------------------------------------------
//	void LCD_RDotLine(
//		u8 x, 																// x 起始坐标
//		u16 y, 																// y 起始坐标
//		u16 High, 														// 线高度
//		u16 Color,														// 线颜色
//		u16 CharBackColor											// 背景颜色
//	)
//-----------------------------------------------------------------
//
// 函数功能: 用指定的颜色在指定位置上画出指定长度的一条竖虚线
// 入口参数: x： 横向起点坐标, 由左到右分别是0~239. 
//					 y: 纵向起点坐标, 由上到下分别为0~319.
//			    High: 画线长度. 
//				 Color: 描点的颜色. 
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_SetBox(),SPFD5408_WR_Start(),SPFD5408_Write_Data().   
// 注意事项:
//-----------------------------------------------------------------
void LCD_RDotLine(
		u8 x, 																	// x 起始坐标
		u16 y, 																	// y 起始坐标
		u16 High, 															// 线高度
		u16 Color,															// 线颜色
		u16 CharBackColor												// 背景颜色
	) 
{  																					// 判断屏幕显示方向
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox(x, y, x + 1, y + High + 1);		// 设置窗口
#else
	LCD_SetBox(x, y, x + High + 1, y + 1);
#endif 
	SPFD5408_WR_Start();											// 启动写数据
	do
	{  
		if ((High % DOT) == 0)									// 间隔DOT点画虚线
		{
			SPFD5408_Write_Data(Color);						// 写入线颜色
		}
		else 																		// 其它点
		{
			SPFD5408_Write_Data(CharBackColor);		// 写入背景颜色	
		}
		High--;
	}while(High);
	SPFD5408_CS_Set;													// 取消片选
}

//-----------------------------------------------------------------
//	void LCD_Print_Box(
//		u16 x0,																// 左上角起始x轴坐标
//		u16 y0,																// 左上角起始y轴坐标
//		u16 x1,																// 右下角终止x轴坐标
//		u16 y1,																// 右下角终止y轴坐标
//		u16 Color															// 矩形框颜色
//	)
//-----------------------------------------------------------------
//
// 函数功能: 画方框函数
// 入口参数: x0: 左上角x起始坐标. y0: 左上角y起始坐标.
//				 x1: 右下角x终止坐标. y1: 右下角y终止坐标.
//				 Color: 方框的原色值.
// 返回参数: 无
// 全局变量: 无
// 调用模块: LCD_RLine(), LCD_HLine().   
// 注意事项:
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
																						// 竖屏
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	  
	High   = y1 - y0;
	Length = x1 - x0;
	LCD_RLine (x0, y0, High + 1, Color) ; 		// 竖线
	LCD_RLine (x1, y0, High + 1, Color) ;
	LCD_HLine (x0, y0, Length + 1, Color) ;		// 横线
	LCD_HLine (x0, y1, Length + 1, Color) ;
#else																				// 横屏
	High   = x1 - x0;
	Length = y1 - y0;
	LCD_RLine (x0, y0, High, Color); 					// 竖线
	LCD_RLine (x0, y1, High, Color);
	LCD_HLine (x0, y0, Length + 1, Color);		// 横线
	LCD_HLine (x1, y0, Length + 1, Color);
#endif 
}


//-----------------------------------------------------------------
// 图片显示区域
//-----------------------------------------------------------------

//-----------------------------------------------------------------
//	void LCD_DisPicture(
//		u8 x,																	// 显示起始X坐标 
//		u16 y, 																// 显示起始y坐标
//		u8 length, 														// 图片长度
//		u16 high, 														// 图片高度
//		u8 *s																	// 指向图片数据指针
//	)
//-----------------------------------------------------------------
//
// 函数功能: 用指定位置上显示事先定义的图片
// 入口参数: (x, y): 是开始点的坐标. 
//					 length: 图片长度. 
//						 high: 图片高度. 
//							 *S: 图片数组的指针 
// 返回参数: 无 
// 全局变量: 无
// 调用模块: LCD_SetBox(), SPFD5408_WR_Start(), SPFD5408_Write_Data().    
// 注意事项: 将要显示的图片事先定义在bmp.h中的pic[]数组中,
//				 如果想修改图片大小、内容，请修改bmp.h中的pic[]数组,
//				 建议用Image2Lcd软件将你要显示的图象自动转换为数组数据.
//-----------------------------------------------------------------
void LCD_DisPicture(
		u8 x,																		// 显示起始X坐标 
		u16 y, 																	// 显示起始y坐标
		u8 length, 															// 图片长度
		u16 high, 															// 图片高度
		u8 *s																		// 指向图片数据指针
	)
{
   u16 temp = 0 ;
	u16 tmp  = 0 ;
	u16 num  = 0 ;	
																						// 判断屏幕显示方向
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)	
	LCD_SetBox (x, y, x + length, y + high) ; // 设置窗口
#else
	LCD_SetBox(x, y, x + high, y + length);    
#endif 
	SPFD5408_WR_Start();											// 启动写数据
	num = length * high * 2 ;									// 计算写入数据个数
	do
	{  
	   temp = s[tmp + 1] ;										// 读取高8位数据
	   temp = temp << 8 ;											// 读取低8位数据
	   temp = temp | s[tmp] ;									// 将数据组合成一个16位的数据
	   SPFD5408_Write_Data (temp) ;						// 逐点显示
	   tmp += 2 ;															// 继续读取后两个数据
	}while (tmp < num) ;
	SPFD5408_CS_Set;													// 取消片选
}

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
