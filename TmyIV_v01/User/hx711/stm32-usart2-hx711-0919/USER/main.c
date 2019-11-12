/*																																								*/
/*�ļ�����main.c																																		*/
/*ʱ�� ��2018��9��18��     �� һ�� ��������																					*/
/*�������� ���������ݴ��� 																													*/
/*					�Ȼ�ȡһ����ǰ��������Ϊһ���ο����ٻ�ȡ�ڶ������������β�ֵ��ΪȥƤ����	*/
/*					                   ����    24λ   128����															*/
/*					����ֵ�뾫�ȵĹ�ϵ  1mV *��2^24�� * 128 = 2147483648  214.7483648			*/
/*					У׼�������ֵд�� Flash 																							*/
/*���ڷ���������Ϊ����ָ�� 1: ȥƤ  2: g kg ct ��λ�л�  3:��̨У׼  4���鿴У׼ֵ		*/
#include "main.h"

/************************* ������ **************************/
int main(void)
{
/****************** ����ϵͳʱ��Ϊ 72M *********************/      	
	SystemInit();
/**************  USART2 config 115200 8-N-1  **************/		
	USART2_Config();
/*********************  HX711�˿����� *********************/
	GPIO_INIT();
/*********************  ��̨��ֵ����  *********************/
  GetRoughWeight();
/********************* ��ȡ��̨У׼ֵ *********************/
	GetFlashParameter();
	if(Correction == 0xFFFFFFFF)
		{
			printf("/*** ��̨δУ׼ ***/ \r\n");
			Correction = SensorAccuracy1mV;//ȱʡ����
			printf("У׼ֵ Correction = %f \r\n",Correction);
		}
/*********************  ����ɹ����  *********************/
	printf("/**** STM32F103C8T6 HX711���ӳ� ****/\r\n");
	printf("/********* �����봮��ָ��  *********/\r\n");
	printf("/********* 1 ����         *********/\r\n");
	printf("/********* 2 ȥƤ         *********/\r\n");
	printf("/********* 3 ת����λ     *********/\r\n");
	printf("/********* 4 ��̨У׼     *********/\r\n");
	printf("/********* 5 �鿴У׼ֵ   *********/\r\n");
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
					case 1 ://����
						Weighing();
						break;
					case 2 ://ȥƤ
						GetRoughWeight();
					  Weighing();
						break;
					case 3 ://�л���λ
						if(++unit >= 3)unit = 0;
					  Weighing();					
						break;
					case 4 ://��̨У׼
						WeighingCorrection();
						break;
					case 5 ://�鿴У׼ֵ
						printf("У׼ֵ Correction = %f \r\n",Correction);
						break;
					default:
						printf("***** ��Чָ�� *****\r\n");
						break;
				 }
			 }
		}
}
/**************     ��ȡëƤ����   **************/
void GetRoughWeight(void)
{
	repeat:
		first_weight = FilterCount();
		delay_ms(1000);
		delay_ms(1000);/*  2S �� �����ȶ���˵�������������ȶ� ʵ�������ȸ��� һ�㶼���ȶ�  */
		rough_weight = FilterCount();
	if(first_weight/50 != rough_weight/50)
	goto repeat;
}
/***************   ��ȡʵ������  *********************/
void Weighing(void)
{
	actual_weight = FilterCount();
	actual_weight = actual_weight - rough_weight;
/*****************  ��λΪǧ�� kg	 *****************/
	if(unit == 0)
		{
			weight = ((float)actual_weight / Correction);
			if(weight > 10000)weight = 0;
			printf("��ǰ���� = %.2f kg\r\n",weight);
		}
/*****************  ��λΪ��  g	  *****************/
	if(unit == 1)
		{
			weight = ((float)actual_weight / Correction) *1000;
			if(weight > 10000)weight = 0;
			printf("��ǰ���� = %.2f g\r\n",weight);
		}
/*****************  ��λΪ���� ct	 *****************/
	if(unit == 2)
		{
			weight = ((float)actual_weight / Correction) * 5000;
			if(weight > 10000)weight = 0;
			printf("��ǰ���� = %.3f ct\r\n",weight);
		}	
}
/*********************** ��̨У׼ ***********************/
void WeighingCorrection(void)
{
	printf("���Ƴ���̨����������� \r\n");
	GetRoughWeight();//��ȡƤ��
	actual_weight = FilterCount();	
	actual_weight = actual_weight - rough_weight;//��̨���� ��ȡ��ǰ�� ��0��
	printf("����� 1 kg ��׼���롭�� \r\n");
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	actual_weight = FilterCount();//��ȡ 1kg ʱ hx711 ��ֵ
	Correction	= (float)((actual_weight - rough_weight) / 1.000);//��ȡ����ֵ
	Correction *= 1000;
	SaveFlashParameter();//����ֵд�� Flash
	GetFlashParameter(); //������ֵ����������ʹ��
	Correction /= 1000;
	printf("У׼ֵ Correction  = %f \r\n",Correction);
}
