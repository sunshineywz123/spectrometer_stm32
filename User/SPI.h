//-----------------------------------------------------------------
// ģ��SPI�ӿڵײ�����ͷ�ļ�
// ͷ�ļ���: SPI.h
// ��    ��: ���ǵ���
// ��дʱ��: 2014-01-29
// �޸�����: 2014-02-23
// �桡����:  V1.0.3
// ��- V1.0: SPI1��ʼ������д����,����ģ��IO��ʽ��SPI��������ͨ�ŷ�ʽ
// - V1.0.1: ��дʱ��ֿ�д
//	-V1.0.2: (2014-02-16)ͷ�ļ��в���������ͷ�ļ�
//  -V1.0.3: (2014-2-23)ģ��ͼ���SPI�ֿ�֮TP�����޸�
//-----------------------------------------------------------------

#ifndef _SPI_H
#define _SPI_H

//-----------------------------------------------------------------
// �ⲿ��������
//-----------------------------------------------------------------
extern u16 Time;
extern u8 higth;

//-----------------------------------------------------------------
// �궨��
//-----------------------------------------------------------------
// ʹ�ü���SPI����/��GPIOģ��SPI�л�����
#define SPI_GPIO
// ����SPIͨ���ٶ�
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

#define		SPI4_DATA_WIDTH 32	//���巢�����ݵĿ��---ע��:���Ҫ��������λ��,��Ӧ�ķ��ͺͽ������ݳ���ҲҪ����
#define		SPI4_CMD_WIDTH  8   //��ַλ���

//-----------------------------------------------------------------
// λ����
//-----------------------------------------------------------------
#ifdef SPI_GPIO
	// SPI1 IO ����
	#define SPI1_MOSI_Set		GPIO_SetBits(GPIOB,GPIO_Pin_5)
	#define SPI1_MOSI_Clr		GPIO_ResetBits(GPIOB,GPIO_Pin_5)
	#define SPI1_CLK_Set		GPIO_SetBits(GPIOB,GPIO_Pin_3)
	#define SPI1_CLK_Clr		GPIO_ResetBits(GPIOB,GPIO_Pin_3)
	#define SPI1_MISO				GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
	// SPI2IO����
	#define SPI_FPGA_MISO 		(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15))        //������:��Ƭ�����,FPGA����(spi_sdi)
	#define SPI_FPGA_SCL_Set 	(GPIO_SetBits(GPIOB,GPIO_Pin_14))   				//ʱ����,FPGA(spi_scl)
	#define SPI_FPGA_SCL_Clr	(GPIO_ResetBits(GPIOB,GPIO_Pin_14))
	#define CS_CMD_Set 				(GPIO_SetBits(GPIOB,GPIO_Pin_13))    	 			//Ƭѡ����,FPGA(spi_cs_Cmd)
	#define CS_CMD_Clr 				(GPIO_ResetBits(GPIOB,GPIO_Pin_13))
	#define CS_DATA_Set 			(GPIO_SetBits(GPIOB,GPIO_Pin_12)) 		 			//Ƭѡ����cs_data
	#define CS_DATA_Clr 			(GPIO_ResetBits(GPIOB,GPIO_Pin_12))
	#define SPI_FPGA_MOSI_Set (GPIO_SetBits(GPIOB,GPIO_Pin_11))  					//������:��Ƭ������,FPGA���(spi_sdo)
	#define SPI_FPGA_MOSI_Clr (GPIO_ResetBits(GPIOB,GPIO_Pin_11))

	// SPI4 IO ����
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
// SPIʱ����ʱ
#define SPI_Delay(n)		Delay_1us(n)	

//-----------------------------------------------------------------
// ���ϼ������Ľӿ�:�ⲿ��������
//-----------------------------------------------------------------
// SPI IO����
extern void GPIO_SPI_Configuration(u16 SPIx);
extern void GPIO_SPI1_Configuration(void);
extern void GPIO_SPI2_Configuration(void);
extern void GPIO_SPI3_Configuration(void);
extern void GPIO_SPI4_Configuration(void);
// SPI ��ʼ��
extern void SPI_Initialization(u16 SPIx);
extern void SPI1_Initialization(void);
extern void SPI2_Initialization(void);
extern void SPI3_Initialization(void);
extern void SPI4_Initialization(void);
// SPI ����
extern void SPI_Configuration(u16 SPIx);
// SPI1��д����
extern void SPI1_Write_Byte(u8 Digital);
extern u8 SPI1_Read_Byte(void);
extern u8 SPI_Read_Write_Data(SPI_TypeDef* SPIx,u8 Digital);
extern void SPI_Set_Speed(SPI_TypeDef* SPIx,u8 Level);
// SPI2��д����
extern void SP2_Send_Data(u32 Send_Data);
extern void SPI2_Send_Cmd(u8 Send_Data);
extern u32	SPI2_Rece_Data(void);
extern void SPI2_Send_Cmd_Data(u8 Cmd,u32 Send_Data);
extern u32	SPI2_Rece_Cmd_Data(u8 Cmd);
// SPI4��д����
extern void SPI4_Write_Byte(u8 Data);
extern u8 SPI4_Read_Byte(void);
extern u8 SPI4_Write_Read(u8 Digital);

#endif

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
