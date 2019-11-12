/*																																								*/
/*文件名：main.c																																		*/
/*时间 ：2018年9月18日     九 一八 勿忘国耻																					*/
/*基本功能 ：称重数据处理 																													*/
/*					先获取一个当前重量，作为一个参考，再获取第二次重量，两次差值作为去皮清零	*/
/*					                   精度    24位   128增益															*/
/*					修正值与精度的关系  1mV *（2^24） * 128 = 2147483648  214.7483648			*/
/*					校准后的修正值写入 Flash 																							*/
/*串口发送数据作为调节指令 1: 去皮  2: g kg ct 单位切换  3:秤台校准  4：查看校准值		*/
#include "main.h"

/************************* 主函数 **************************/
int main(void)
{
/****************** 配置系统时钟为 72M *********************/      	
	SystemInit();
/**************  USART2 config 115200 8-N-1  **************/		
	USART2_Config();
/*********************  HX711端口配置 *********************/
	GPIO_INIT();
/*********************  秤台数值清零  *********************/
  GetRoughWeight();
/********************* 读取秤台校准值 *********************/
	GetFlashParameter();
	if(Correction == 0xFFFFFFFF)
		{
			printf("/*** 秤台未校准 ***/ \r\n");
			Correction = SensorAccuracy1mV;//缺省设置
			printf("校准值 Correction = %f \r\n",Correction);
		}
/*********************  清零成功检测  *********************/
	printf("/**** STM32F103C8T6 HX711电子秤 ****/\r\n");
	printf("/********* 请输入串口指令  *********/\r\n");
	printf("/********* 1 称重         *********/\r\n");
	printf("/********* 2 去皮         *********/\r\n");
	printf("/********* 3 转换单位     *********/\r\n");
	printf("/********* 4 秤台校准     *********/\r\n");
	printf("/********* 5 查看校准值   *********/\r\n");
/****************************************/	
	while(1)
		{
			function = 0;
			if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
			  {
				 USART_ClearFlag(USART2, USART_FLAG_RXNE);
				 USART_ClearITPendingBit(USART2,USART_IT_RXNE);
				 function = USART_ReceiveData(USART2);
			  }
			if(function)
			 {
				switch( function )
				 {
					case 1 ://称重
						Weighing();
						break;
					case 2 ://去皮
						GetRoughWeight();
					  Weighing();
						break;
					case 3 ://切换单位
						if(++unit >= 3)unit = 0;
					  Weighing();					
						break;
					case 4 ://秤台校准
						WeighingCorrection();
						break;
					case 5 ://查看校准值
						printf("校准值 Correction = %f \r\n",Correction);
						break;
					default:
						printf("***** 无效指令 *****\r\n");
						break;
				 }
			 }
		}
}
/**************     获取毛皮重量   **************/
void GetRoughWeight(void)
{
	repeat:
		first_weight = FilterCount();
		delay_ms(1000);
		delay_ms(1000);/*  2S 后 数据稳定，说明传感器基本稳定 实际灵敏度高了 一点都不稳定  */
		rough_weight = FilterCount();
	if(first_weight/50 != rough_weight/50)
	goto repeat;
}
/***************   获取实际重量  *********************/
void Weighing(void)
{
	actual_weight = FilterCount();
	actual_weight = actual_weight - rough_weight;
/*****************  单位为千克 kg	 *****************/
	if(unit == 0)
		{
			weight = ((float)actual_weight / Correction);
			if(weight > 10000)weight = 0;
			printf("当前重量 = %.2f kg\r\n",weight);
		}
/*****************  单位为克  g	  *****************/
	if(unit == 1)
		{
			weight = ((float)actual_weight / Correction) *1000;
			if(weight > 10000)weight = 0;
			printf("当前重量 = %.2f g\r\n",weight);
		}
/*****************  单位为克拉 ct	 *****************/
	if(unit == 2)
		{
			weight = ((float)actual_weight / Correction) * 5000;
			if(weight > 10000)weight = 0;
			printf("当前重量 = %.3f ct\r\n",weight);
		}	
}
/*********************** 秤台校准 ***********************/
void WeighingCorrection(void)
{
	printf("请移除秤台上所有异物…… \r\n");
	GetRoughWeight();//获取皮重
	actual_weight = FilterCount();	
	actual_weight = actual_weight - rough_weight;//秤台清零 获取当前的 “0”
	printf("请放置 1 kg 标准砝码…… \r\n");
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	actual_weight = FilterCount();//获取 1kg 时 hx711 的值
	Correction	= (float)((actual_weight - rough_weight) / 1.000);//获取修正值
	Correction *= 1000;
	SaveFlashParameter();//修正值写入 Flash
	GetFlashParameter(); //将修正值读出，以作使用
	Correction /= 1000;
	printf("校准值 Correction  = %f \r\n",Correction);
}
