//-----------------------------------------------------------------
// 程序描述:
//			集成SPI外设驱动程序
// 作　　者: 凌智电子
// 开始日期: 2014-01-29
// 完成日期: 2014-01-29
// 修改日期: 2014-02-23
// 版　　本:  V1.0.3
// 　- V1.0: SPI1初始化及读写函数,包括模拟IO方式和SPI外设两种通信方式
// - V1.0.1: 读写时序分开写
//	-V1.0.2: (2014-02-16)头文件中不包含其他头文件
//  -V1.0.3: (2014-2-23)模拟和集成SPI分开之TP部分修改
// 调试工具: 凌智STM32+FPGA电子系统设计开发板、2.8寸液晶触摸屏、
// 					 LZE_ST_LINK2
// 说　　明: 
//				1、SPI1(重映射)或SPI3:驱动XPT2046或SD卡
//					 SPI2:驱动FPGA
//					 SPI4:驱动W25Q16
//				2、除了SPI4是模拟SPI外，SPI1,SPI2,SPI3分为集成和模拟SPI
//				3、为保持各个SPI的独立性与特殊性以及可阅读性,以及对于以前模 
//					 块的兼容性,对于IO初始化,SPI配置,读写等函数分别使用独立的函数.
// 					 即本文件为其他使用到SPI功能的模块(SD,TP,FPGA,W25Q16)的公用文件
//
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include <stm32f10x.h>
#include "SPI.h"
#include "Delay.h"

//-----------------------------------------------------------------
// 全局变量定义
//-----------------------------------------------------------------
u16 Time = 6;																// 延时长度
u8 higth = 1;

//-----------------------------------------------------------------
// 初始化程序区
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// void GPIO_SPI_Configuration(u16 SPIx)
//-----------------------------------------------------------------
// 
// 函数功能: SPI IO口初始化
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 引脚重映射步骤:
//			1、使能被重映射到的IO端口时钟
//			2、使能被重映射的外设时钟
//			3、禁止JTAG功能
//			4、使能AFIO功能的时钟
//			5、引脚重映射
//
//    
//-----------------------------------------------------------------
void GPIO_SPI_Configuration(u16 SPIx)
{	
	switch(SPIx)
	{
		case SPI_1: GPIO_SPI1_Configuration();break;
		case SPI_2: GPIO_SPI2_Configuration();break;
		case SPI_3: GPIO_SPI3_Configuration();break;
		case SPI_4: GPIO_SPI4_Configuration();break;
		default:break;
	}
}

//-----------------------------------------------------------------
// void GPIO_SPI1_Configuration(void)
//-----------------------------------------------------------------
// 
// 函数功能: SPI1外设初始化
// 入口参数: 无 
// 返 回 值: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 无
//         SPI1外设要引脚重映射    
//-----------------------------------------------------------------
void GPIO_SPI1_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	// 使能被重新映射到的IO端口时钟
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB | 
													RCC_APB2Periph_GPIOD, ENABLE);
	// 禁止JTAG功能
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
 // 使用GPIO模拟的引脚配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;// CLK、MOSI
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			// 推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;							// 配置PB4:MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	// SPI的触屏功能使能引脚PD3,SPI的SD卡功能的使能引脚PD6
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	// 禁止SD卡和触屏占用SPI总线
	SPI1_SD_CS_Set;
	SPI1_TP_CS_Set;	
}
//-----------------------------------------------------------------
// void GPIO_SPI2_Configuration(void)
//-----------------------------------------------------------------
// 
// 函数功能: SPI2 IO配置
// 入口参数: 无 
// 返 回 值: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 引脚连接如下：
//		SPI2_SEL			PB.15				
//		SPI2_CS			  PB.14
//		SPI2_SCK			PB.13
//		SPI2_MISO			PB.12
//		SPI2_MOSI			PB.11 
//-----------------------------------------------------------------
void GPIO_SPI2_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// 使能PB时钟  
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
 	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12 | GPIO_Pin_13 | 
																 GPIO_Pin_14 | GPIO_Pin_11;;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  // 使用GPIO模拟的引脚配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;// 推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SPI_FPGA_MOSI_Set;	
	SPI_FPGA_SCL_Set;
	CS_CMD_Set;
	CS_DATA_Set;
}
//-----------------------------------------------------------------
// void GPIO_SPI3_Configuration(void)
//-----------------------------------------------------------------
// 
// 函数功能: SPI3 IO配置
// 入口参数: 无 
// 返 回 值: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项:   
//    可以使用SPI3驱动液晶触摸屏和SD卡，也可以使用重映射引脚的SPI1驱动,
//    同样的PB3,PB4与JTAG复用,要使用SPI3需要把JTAG功能禁止.
//-----------------------------------------------------------------
void GPIO_SPI3_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | 
												 RCC_APB2Periph_GPIOD,ENABLE);
	// 禁止JTAG功能
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
  // 使用GPIO模拟的引脚配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;// CLK、MOSI
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			// 推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;							// 配置PB4:MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	// SPI的触屏功能使能引脚PD3,SPI的SD卡功能的使能引脚PD6
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	// 禁止SD卡和触屏占用SPI总线
	SPI1_SD_CS_Set;
	SPI1_TP_CS_Set;	
}
//-----------------------------------------------------------------
// void GPIO_SPI4_Configuration(void)
//-----------------------------------------------------------------
//
// 函数功能: SPI4的I/O口配置函数
// 入口参数: 无
// 返 回 值: 无
// 全局变量: 无
// 调用模块: 无 
// 注意事项: 硬件连接如下
//      SPI4_CS   -----> PA.0   片选信号
//      SPI4_MISO -----> PA.1   主设备数据输入，从设备数据输出
//      SPI4_MOSI -----> PA.2   主设备数据输出，从设备数据输入
//      SPI4_CLK  -----> PA.3   时钟信号
//-----------------------------------------------------------------
void GPIO_SPI4_Configuration(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	//开启GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);          				
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  			// 推挽输出 	
	GPIO_Init(GPIOA, &GPIO_InitStructure);													
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  				 // 上拉输入 	
	GPIO_Init(GPIOA, &GPIO_InitStructure);			
	//拉高片选信号线/主机输出信号线/时钟线
	SPI4_CS_Set;																						
	SPI4_MOSI_Set;
	SPI4_CLK_Set;	
}

//-----------------------------------------------------------------
// void SPI_Initialization(u16 SPIx)
//-----------------------------------------------------------------
// 
// 函数功能: SPI初始化
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 无
//    
//-----------------------------------------------------------------
void SPI_Initialization(u16 SPIx)
{
	switch(SPIx)
	{
		case SPI_1: SPI1_Initialization();break;
		case SPI_2: SPI2_Initialization();break;
		case SPI_3: SPI3_Initialization();break;
		case SPI_4: SPI4_Initialization();break;
		default:break;
	}	
}

//-----------------------------------------------------------------
// void SPI1_Initialization(void)
//-----------------------------------------------------------------
// 
// 函数功能: SPI1初始化
// 入口参数: 无 
// 返 回 值: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 无
    
//-----------------------------------------------------------------
void SPI1_Initialization(void)
{
}
//-----------------------------------------------------------------
// void SPI2_Initialization(void)
//-----------------------------------------------------------------
// 
// 函数功能: SPI2初始化
// 入口参数: 无 
// 返 回 值: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 无
    
//-----------------------------------------------------------------
void SPI2_Initialization(void)
{
}
//-----------------------------------------------------------------
// void SPI3_Initialization(void)
//-----------------------------------------------------------------
// 
// 函数功能: 系统外设初始化
// 入口参数: 无 
// 返 回 值: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 无
    
//-----------------------------------------------------------------
void SPI3_Initialization(void)
{	
	
}
// SPI4是自己模拟的，没有集成的SPI,故不用SPI 配置
//-----------------------------------------------------------------
// void SPI2_Initialization(void)
//-----------------------------------------------------------------
// 
// 函数功能: SPI4初始化
// 入口参数: 无 
// 返 回 值: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 无
    
//-----------------------------------------------------------------
void SPI4_Initialization(void)
{	
}
//-----------------------------------------------------------------
// void SPI_Configuration(u16 SPIx)
//-----------------------------------------------------------------
// 
// 函数功能: SPI配置
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 模拟SPI，无需SPI初始化
//    
//-----------------------------------------------------------------
void SPI_Configuration(u16 SPIx)
{
 	GPIO_SPI_Configuration(SPIx);							// 各引脚配置
}

//-----------------------------------------------------------------
// 功能程序区
//-----------------------------------------------------------------
 
//-----------------------------------------------------------------
// void SPI1_Write_Byte(unsigned char Data
//-----------------------------------------------------------------
//
// 函数功能: 用IO口模拟SPI1的写一个字节
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPI1_Delay().   
// 注意事项: 
//			(1) 只有用IO模拟SPI时序的方法才用的这些函数,
// 		(2) 注意SPI 主机发送从机接收引脚MOSI的数据有效是在时钟上升沿
//				 有效, 而主机接收丛集发送引脚MISO是在时钟下降沿数据有效.
//				 SPI 可以全双工通信, 但是在本处, 主机先发送命令给从机然后
//				 从从机发送回馈数据给主机, 有先后关系在调用此函数之前, 请
//				 将片选信号有效, 否则写数据不成功.
//-----------------------------------------------------------------
 void SPI1_Write_Byte(u8 Digital)
{
	u8 a=0;
	
	for(a=0;a<8;a++)
	{
		if(Digital &0x80)
			SPI1_MOSI_Set;
		else
			SPI1_MOSI_Clr;
		Digital<<=1;
		SPI1_CLK_Clr;
		SPI1_CLK_Set;
	}
}

//-----------------------------------------------------------------
// unsigned char SPI1_Read_Byte(void) 
//-----------------------------------------------------------------
//
// 函数功能: 用IO口模拟SPI1的读一个字节
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPI1_Delay().   
// 注意事项:
//		(1) 只有用IO模拟SPI时序的方法才用的这些函数,
// 		(2) 注意SPI 主机发送从机接收引脚MOSI的数据有效是在时钟上升沿
//				 有效, 而主机接收丛集发送引脚MISO是在时钟下降沿数据有效.
//				 SPI 可以全双工通信, 但是在本处, 主机先发送命令给从机然后
//				 从从机发送回馈数据给主机, 有先后关系在调用此函数之前, 请
//				 将片选信号有效, 否则写数据不成功.
//-----------------------------------------------------------------
u8 SPI1_Read_Byte(void) 
{
	unsigned char a=0,b=0;
	for(a=8;a>0;a--)
	{
		SPI1_CLK_Set;
		if (higth == 0)
		{
			SPI1_CLK_Clr; 		
		}
		else
		{
			SPI_Delay(Time);	
			SPI1_CLK_Clr; 		
			SPI_Delay(Time);
		}
		b <<= 1;
		if(SPI1_MISO)
			b |= 0x01;
	}
	SPI1_CLK_Set;

	return (b);	
}

//-----------------------------------------------------------------
// u8 SPI_Read_Write_Data(SPI_TypeDef* SPIx,u8 Digital)
//-----------------------------------------------------------------
// 
// 函数功能: SPI读写一个字节
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 
//    
//-----------------------------------------------------------------
u8 SPI_Read_Write_Data(SPI_TypeDef* SPIx,u8 Digital)
{
	u8 Receivedata=0;
 
	SPI1_Write_Byte(Digital);
	Receivedata = SPI1_Read_Byte();

	return Receivedata; 
}
//-----------------------------------------------------------------
// void SPI_Set_Speed(SPI_TypeDef* SPIx,u8 Level)
//-----------------------------------------------------------------
// 
// 函数功能: 改变SPI的通信速度
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 如果是集成外设，Level:0-7;
//					 如果是模拟IO，可以为任意值
//    
//-----------------------------------------------------------------
void SPI_Set_Speed(SPI_TypeDef* SPIx,u8 Level)
{
 	Time = Level;		
}
//-----------------------------------------------------------------
// SPI2功能程序
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// void SPI2_Send_Cmd(u8 Send_Data)
//-----------------------------------------------------------------
//
// 函数功能: 发送8位的命令(默认)
// 入口参数: 要发送的命令
// 返 回 值: 无
// 全局变量: 无
// 调用模块: 无     
// 注意事项: 时钟是上升沿的时候发送命令片选低电平有效有效
//-----------------------------------------------------------------
void SPI2_Send_Cmd(u8 Send_Data)
{	
	u8	i;
	
	CS_CMD_Clr;																// 片选选中
	for(i=0;i<SPI4_CMD_WIDTH;i++)
	{		
		if((Send_Data&0x80) == 0x80) 
		{
			SPI_FPGA_MOSI_Set;
		}
		else 
		{
			SPI_FPGA_MOSI_Clr;
		}				
		Send_Data <<= 1;												// 高位在前
		SPI_FPGA_SCL_Clr;
		SPI_FPGA_SCL_Set;												// 上升沿发送数据
	}
	CS_CMD_Set;																// 片选拉高
}

//-----------------------------------------------------------------
// void SP2_Send_Data(u32 Send_Data)
//-----------------------------------------------------------------
//
// 函数功能: spi2发送数据
// 入口参数: 待发的数据
// 返 回 值: 无
// 全局变量: 无
// 调用模块: 无 
// 注意事项: 时钟是上升沿的时候发送数据,片选低电平有效有效
//					 数据类型是32位无符号长整形,数据类型可以根据需要更改      
//-----------------------------------------------------------------
void SP2_Send_Data(u32 Send_Data)
{	
	u8	i;
	
	CS_DATA_Clr;															// 片选选中
	for(i=0;i<SPI4_DATA_WIDTH;i++)
	{ 
		if((Send_Data&0x80000000) == 0x80000000)
		{
			SPI_FPGA_MOSI_Set;
		}
		else
		{
			SPI_FPGA_MOSI_Clr;
		}				
		Send_Data <<= 1				;									// 高位在前
		SPI_FPGA_SCL_Clr;
		SPI_FPGA_SCL_Set;												// 上升沿发送数据
	}
	CS_DATA_Set;														  // 片选拉高
}

//-----------------------------------------------------------------
// u32	SPI2_Rece_Data(void)
//-----------------------------------------------------------------
//
// 函数功能: spi接收数据---数据类型是32位无符号长整形,数据类型
// 入口参数: 无
// 返 回 值: 接收到的数据
// 全局变量: 无
// 调用模块: 无 
// 注意事项: 时钟是下降接收数据,片选下降沿有效
//-----------------------------------------------------------------
u32	SPI2_Rece_Data(void)
{
	u8	i;
	u32 Data_Buf=0;														// 接收到得数据
	
	CS_DATA_Clr;															// 片选选中
	for(i=0;i<SPI4_DATA_WIDTH;i++)
	{
		SPI_FPGA_SCL_Set;	
		SPI_FPGA_SCL_Clr;												// 下降沿接收数据	
		
		Data_Buf<<=1;														// 左移	
		Data_Buf = Data_Buf | SPI_FPGA_MISO;		// 接收数据
	}
	CS_DATA_Set;															// 片选拉高
	
	return Data_Buf;
}

//-----------------------------------------------------------------
// void SPI2_Send_Cmd_Data(u8 Cmd,u32 Send_Data)
//-----------------------------------------------------------------
//
// 函数功能: 先发送命令,在发送数据
// 入口参数: 要发送的命令,要发送的数据
// 返 回 值:
// 全局变量: 无
// 调用模块: 无 
// 注意事项:
//-----------------------------------------------------------------
void SPI2_Send_Cmd_Data(u8 Cmd,u32 Send_Data)
{
	SPI2_Send_Cmd(Cmd);
	SP2_Send_Data(Send_Data);
}

//-----------------------------------------------------------------
// u32	SPI2_Rece_Cmd_Data(u8 Cmd)
//-----------------------------------------------------------------
//
// 函数功能: 先发送命令,在接收数据
// 入口参数: 要发送的命令
// 返 回 值: 接收到的数据
// 全局变量: 无
// 调用模块: 无 
// 注意事项:
//-----------------------------------------------------------------
u32	SPI2_Rece_Cmd_Data(u8 cmd)
{
	SPI2_Send_Cmd(cmd);
	return(SPI2_Rece_Data());
}

//-----------------------------------------------------------------
// SPI4功能程序区
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// void SPI4_Write_Byte(u8 Data)
//-----------------------------------------------------------------
//
// 函数功能: I/O模拟SPI写1字节数据
// 入口参数: 发送的1字节数据
// 返 回 值: 无
// 全局变量: 无
// 调用模块: 无 
// 注意事项: SPI基本操作，输出高位在前
//-----------------------------------------------------------------
void SPI4_Write_Byte(u8 Data) 
{
	u8 i;

	for(i=0;i<8;i++)
	{	
		SPI4_CLK_Clr;					 
		if((Data&0x80) == 0x80)									// 处理得到一位并发送
		{
			SPI4_MOSI_Set;
		}			
		else
		{
			SPI4_MOSI_Clr;
		}		
		Data <<= 1;	          									// 数据左移1位，为下次发送做准备		
		SPI4_CLK_Set;														// 上升沿写入数据 	
	}	
}

//-----------------------------------------------------------------
// u8 SPI4_Read_Byte(void)
//-----------------------------------------------------------------
//
// 函数功能: I/O模拟SPI读1字节
// 入口参数: 无
// 返 回 值: 接收的1节数据
// 全局变量: 无
// 调用模块: 无 
// 注意事项: SPI基本操作，输入高位在前
//-----------------------------------------------------------------
u8 SPI4_Read_Byte(void) 
{
	u8 i,data;

	for(i=0;i<8;i++)
	{		
		data <<= 1;                             // 接收下一个数据前先左移一位，
																						// 准备接收下一个数据
		SPI4_CLK_Set;														// CLK的下降沿MISO端数据有效		
		SPI4_CLK_Clr;
		if(SPI4_MISO == 0x01)      							// 按高位在前顺序逐位接收
		{																				// MISO端数据并缓存在data中
			data |= 0x01;
		}
	}
	return data;	            								// 返回1字节数据
}

//-----------------------------------------------------------------
// u8 SPI4_Write_Read(u8 Digital)
//-----------------------------------------------------------------
//
// 函数功能: 模拟SPI4写1字节数据，在接收1字节数据
// 入口参数: 发送数据
// 返 回 值: 接收数据
// 全局变量: 无
// 调用模块: 无 
// 注意事项: 无
//-----------------------------------------------------------------
u8 SPI4_Write_Read(u8 Digital)
{
	SPI4_Write_Byte(Digital); 		
	return  SPI4_Read_Byte();
}	

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
