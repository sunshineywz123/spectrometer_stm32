//-----------------------------------------------------------------
// 模拟SPI接口底层驱动头文件
// 头文件名: SPI.h
// 作    者: 凌智电子
// 编写时间: 2014-01-29
// 修改日期: 2014-02-23
// 版　　本:  V1.0.3
// 　- V1.0: SPI1初始化及读写函数,包括模拟IO方式和SPI外设两种通信方式
// - V1.0.1: 读写时序分开写
//	-V1.0.2: (2014-02-16)头文件中不包含其他头文件
//  -V1.0.3: (2014-2-23)模拟和集成SPI分开之TP部分修改
//-----------------------------------------------------------------

#ifndef _SPI_H
#define _SPI_H

//-----------------------------------------------------------------
// 外部变量声明
//-----------------------------------------------------------------
extern u16 Time;
extern u8 higth;

//-----------------------------------------------------------------
// 宏定义
//-----------------------------------------------------------------
// 使用集成SPI外设/用GPIO模拟SPI切换开关
#define SPI_GPIO
// 定义SPI通信速度
#define SPI_SPEED_2	   0
#define SPI_SPEED_4	   1
#define SPI_SPEED_8	   2
#define SPI_SPEED_16   3
#define SPI_SPEED_32   4
#define SPI_SPEED_64   5
#define SPI_SPEED_128  6
#define SPI_SPEED_256  7

#define SPI_1       		  0x0001
#define SPI_2       		  0x0002
#define SPI_3       		  0x0003
#define SPI_4       		  0x0004

#define		SPI4_DATA_WIDTH 32	//定义发送数据的宽度---注意:如果要更改数据位宽,对应的发送和接收数据程序也要更改
#define		SPI4_CMD_WIDTH  8   //地址位宽度

//-----------------------------------------------------------------
// 位操作
//-----------------------------------------------------------------
#ifdef SPI_GPIO
	// SPI1 IO 操作
	#define SPI1_MOSI_Set		GPIO_SetBits(GPIOB,GPIO_Pin_5)
	#define SPI1_MOSI_Clr		GPIO_ResetBits(GPIOB,GPIO_Pin_5)
	#define SPI1_CLK_Set		GPIO_SetBits(GPIOB,GPIO_Pin_3)
	#define SPI1_CLK_Clr		GPIO_ResetBits(GPIOB,GPIO_Pin_3)
	#define SPI1_MISO				GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
	// SPI2IO操作
	#define SPI_FPGA_MISO 		(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15))        //数据线:单片机输出,FPGA输入(spi_sdi)
	#define SPI_FPGA_SCL_Set 	(GPIO_SetBits(GPIOB,GPIO_Pin_14))   				//时钟线,FPGA(spi_scl)
	#define SPI_FPGA_SCL_Clr	(GPIO_ResetBits(GPIOB,GPIO_Pin_14))
	#define CS_CMD_Set 				(GPIO_SetBits(GPIOB,GPIO_Pin_13))    	 			//片选命令,FPGA(spi_cs_Cmd)
	#define CS_CMD_Clr 				(GPIO_ResetBits(GPIOB,GPIO_Pin_13))
	#define CS_DATA_Set 			(GPIO_SetBits(GPIOB,GPIO_Pin_12)) 		 			//片选数据cs_data
	#define CS_DATA_Clr 			(GPIO_ResetBits(GPIOB,GPIO_Pin_12))
	#define SPI_FPGA_MOSI_Set (GPIO_SetBits(GPIOB,GPIO_Pin_11))  					//数据线:单片机输入,FPGA输出(spi_sdo)
	#define SPI_FPGA_MOSI_Clr (GPIO_ResetBits(GPIOB,GPIO_Pin_11))

	// SPI4 IO 操作
	#define SPI4_CS_Set      (GPIO_SetBits(GPIOA,GPIO_Pin_0))
	#define SPI4_CS_Clr      (GPIO_ResetBits(GPIOA,GPIO_Pin_0))
	#define SPI4_MISO        (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)) 
	#define SPI4_MOSI_Set    (GPIO_SetBits(GPIOA,GPIO_Pin_2))
	#define SPI4_MOSI_Clr    (GPIO_ResetBits(GPIOA,GPIO_Pin_2))
	#define SPI4_CLK_Set     (GPIO_SetBits(GPIOA,GPIO_Pin_3))
	#define SPI4_CLK_Clr     (GPIO_ResetBits(GPIOA,GPIO_Pin_3))
	
#endif
	#define SPI1_TP_CS_Set	GPIO_SetBits(GPIOD,GPIO_Pin_3)
	#define SPI1_TP_CS_Clr	GPIO_ResetBits(GPIOD,GPIO_Pin_3)
	#define SPI1_SD_CS_Set  GPIO_SetBits(GPIOD,GPIO_Pin_6)
	#define SPI1_SD_CS_Clr	GPIO_ResetBits(GPIOD,GPIO_Pin_6)
// SPI时序延时
#define SPI_Delay(n)		Delay_1us(n)	

//-----------------------------------------------------------------
// 与上级函数的接口:外部函数声明
//-----------------------------------------------------------------
// SPI IO配置
extern void GPIO_SPI_Configuration(u16 SPIx);
extern void GPIO_SPI1_Configuration(void);
extern void GPIO_SPI2_Configuration(void);
extern void GPIO_SPI3_Configuration(void);
extern void GPIO_SPI4_Configuration(void);
// SPI 初始化
extern void SPI_Initialization(u16 SPIx);
extern void SPI1_Initialization(void);
extern void SPI2_Initialization(void);
extern void SPI3_Initialization(void);
extern void SPI4_Initialization(void);
// SPI 配置
extern void SPI_Configuration(u16 SPIx);
// SPI1读写函数
extern void SPI1_Write_Byte(u8 Digital);
extern u8 SPI1_Read_Byte(void);
extern u8 SPI_Read_Write_Data(SPI_TypeDef* SPIx,u8 Digital);
extern void SPI_Set_Speed(SPI_TypeDef* SPIx,u8 Level);
// SPI2读写函数
extern void SP2_Send_Data(u32 Send_Data);
extern void SPI2_Send_Cmd(u8 Send_Data);
extern u32	SPI2_Rece_Data(void);
extern void SPI2_Send_Cmd_Data(u8 Cmd,u32 Send_Data);
extern u32	SPI2_Rece_Cmd_Data(u8 Cmd);
// SPI4读写函数
extern void SPI4_Write_Byte(u8 Data);
extern u8 SPI4_Read_Byte(void);
extern u8 SPI4_Write_Read(u8 Digital);

#endif

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
