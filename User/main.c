//-----------------------------------------------------------------
// 程序描述: 
//		- 结合FPGA实现等精度测频：使用spi通信，从FPGA读取数据，
//      计算显示标准计数个数、待测计数、频率、周期、高低电平时间、占空比
// 作    者: 凌智电子
// 开始日期: 2014-02-18
// 完成日期: 2014-02-18
// 修改日期:
// 版    本: V1.0
//   - V1.0:
// 调试工具: 凌智STM32+FPGA电子系统设计开发板、LZE_ST LINK2
//            2.8寸液晶、示波器、信号源
// 说    明:
//    - 调试使用的系统时钟频率Fsysclk=72MHz；
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// 头文件包含
//-----------------------------------------------------------------
#include <stm32f10x.h>
#include "Delay.h"
#include "Freq_Test.h"
#include "SPI.h"
#include "SPFD5408.h"

//-----------------------------------------------------------------
// 主程序
//-----------------------------------------------------------------
int main(void)
{
	
	SPFD5408_Init();
	SPFD5408_Clear_Window(BLACK);
	SPI_Configuration(SPI_2);									// MCU与FPGA的SPI通信初始化
	LCD_WriteChinese24x24string(32, 22,YELLOW, BLACK, "简易数字频率计");
	LCD_WriteString(14,74,YELLOW,BLACK,"标准计数:                ");
	LCD_WriteString(14,101,YELLOW,BLACK,"待测计数:                ");
	LCD_WriteString(14,128,YELLOW,BLACK,"频    率:                Hz"); 
	LCD_WriteString(14,155,YELLOW,BLACK,"周    期:                us");
	LCD_WriteString(14,182,YELLOW,BLACK,"高电平  :                us");
	LCD_WriteString(14,209,YELLOW,BLACK,"低电平  :                us");
	LCD_WriteString(14,236,YELLOW,BLACK,"占空比  :                %");
	while(1)
	{
 		Frequency_Calculations_Show();	   			// 读取数据、计数、显示
	  Delay_50ms(20);					   						  // 延时1s，即1s刷屏一次
	}
}

//-----------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------
