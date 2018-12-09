/*
    \mainpage
    \author Marcelo Vasques, Arthur Ribeiro e Matheus Tura
    \version 1.0
    \date 2018
*/

#include <stdio.h>                  //Biblioteca de entrada e saída
#include <stdlib.h>                 //Biblioteca de função padrão

#include <math.h>
#include "quanser_2DSFJ.h"

/*
		IO-0 -
    IO-1 -
    IO-2 -
    IO-3 -
    IO-4 -
    IO-5 -
    IO-6 -
    IO-7 -
    IO-8 -
    IO-9 -
    IO-10 -
    IO-11 -
    IO-12 -
    IO-13 -
    IO-14 -
    IO-15 -
    IO-16 -

*/

/** Usado para setar os bits mais significativos do contador. (COUNTHIGH = 0)*/
//#define BYTE_SELECTED_LOW 0
/** Usado para setar os bits menos significativos do contador. (COUNTLOW = 1)*/
//#define BYTE_SELECTED_HIGH 1
/** Frequência máxima da galileo*/
#define GALILEO_FREQ 1525
/** Voltagem máxima (27V) */
#define MOTOR_VOLTAGE 27
/** Número máximo de ranhuras*/
#define NUM_MARKS 4096
/** Para conversão de unidades*/
#define NANO 1000000000



/** Estrutura utilizada para ler os bits do contador e os sensores de fim de curso */
struct sHandles
{
    /** Usado para ler os bits do contador */
	FILE *fileBit[16];
	/** Usado para ler o fim de curso 1 */
	FILE *fileLim1;
	/** Usado para ler o fim de curso 2 */
	FILE *fileLim2;
};

/** \brief Seta a ciclo de trabalho do PWM */
void setPWMDutyCycle(int duty_cycle)
{
    /**
    \details Seta a ciclo de trabalho do PWM com o valor especificado por parâmetro
    \param duty_cycle Valor do tipo inteiro usado para definir o ciclo de trabalho do PWM
    \return
    */
    char tempStringDutyCycle[100];
    sprintf(tempStringDutyCycle, "echo %d > /sys/class/pwm/pwmchip0/pwm1/duty_cycle", duty_cycle);
    system(tempStringDutyCycle);
}

/** \brief Seta o pino que desabilita as saídas do MOSFET driver.*/
void setDIS(int value)
{
    /**
    \details Seta o pino que desabilita as saídas do MOSFET driver.
    \param value Valor do tipo inteiro usado para setar o pino.
    \return Esta função não possui retorno
    */

	if(value == 0){
	    system("echo 0 > /sys/class/gpio/gpio11/value");
	}
	else
	if(value == 1){
	    system("echo 1 > /sys/class/gpio/gpio11/value");
	}
}

/** \brief Seta o valor do pino de reset utilizado pelos contadores. */
void setReset(int value)
{
  /**
    \details Seta o valor do pino de reset utilizado pelos contadores (value = 1 reseta, escreve 0 no valor da gpio, pois ele é negado)
    \param value Valor do tipo inteiro usado para definir o valor do reset
    \return Esta função não possui retorno
    */
    if (value == 1)
        system("echo 0 > /sys/class/gpio/gpio6/value");
    else
    if (value == 0)
        system("echo 1 > /sys/class/gpio/gpio6/value");

}

/** \brief Inicializa: exporta as portas da galileo e abre os arquivos necessários para leitura */
handles *initialize(void)
{
	/**
	\details Inicializa.
    \return Estrutura com os ponteiros necessários para leitura
    */
	handles* temp = malloc(sizeof(handles));
  	return temp;
}



/** \brief Seta a voltagem do motor */
void setMotorVoltage(double value);
{
    /**
    \details Seta a voltagem do motor com o valor especificado por parâmetro
    \param value Valor do tipo inteiro usado para definir a voltagem do motor
    \return Esta função não possui retorno
    */
	double period = 1.0/GALILEO_FREQ * 1000000000;

	int cycle = (int)(value/MOTOR_VOLTAGE * 0.5 * period + 0.5 * period);

	if(cycle > period*0.9)
        cycle = period * 0.9;

	setPWMDutyCycle(cycle);
}

/** \brief Seta o período do PWM */
void setPWMPeriod(int period)
{
     /**
    \details Seta a período do PWM com o valor especificado por parâmetro
    \param period Valor do tipo inteiro usado para definir o período do PWM
    \return
    */
    char tempStringPeriod[100];
    sprintf(tempStringPeriod, "echo %d > /sys/class/pwm/pwmchip0/device/pwm_period", period);
    system(tempStringPeriod);
}


/** \brief Seta sinal de enable dos contadores e do multiplexador dos contadores */
void setENB(int cont)
{
    /**
    \details Seta o sinal enable, ativando a escolha do canal de encoder a ser passado para o contador.
                seta também os contadores.
    \param cont Valor do tipo inteiro usado para setar o contador
    \return Esta função não possui retorno
    */
	if(cont == 1){
	    system("echo 1 > /sys/class/gpio/gpio52/value"); //ativo
	}
	else
	if(cont == 0){
	    system("echo 0 > /sys/class/gpio/gpio52/value"); //flutuando
	}

}


/** \brief Seta os bits como mais ou menos significativos*/
void setSEL(int value)
{
    /**
    \details Seta os bits como mais ou menos significativos conforme o valor passado por parâmetro (1: mais significativos / 0: menos significativos)
    \param value Valor do tipo inteiro usado para setar os bits (mais ou menos significativos)
    \return Esta função não possui retorno
    */

	if(value == BYTE_SELECTED_LOW){
        system("echo 1 > /sys/class/gpio/gpio1/value");
	}
	else
	if(value == BYTE_SELECTED_HIGH){
	    system("echo 0 > /sys/class/gpio/gpio1/value");
	}
}


/** \brief Seta o valor do seletor do multiplexador.*/
void setMUX(int value){
    /**
    \details Seta o valor do seletor do multiplexador, selecionando qual encoder será lido no contador.
    \param value Valor do tipo inteiro usado para setar os bits (mais ou menos significativos)
    \return Esta função não possui retorno
    */
    if(value == 0){
        system("echo 1 > /sys/class/gpio/gpio38/value");
	}
	else
	if(value == 1){
	    system("echo 0 > /sys/class/gpio/gpio38/value");
	}
}


/** \brief Lê o contador*/
int readCounter(handles* input_values)
{
    /**
    \details Lê o contador
    \param input_values Estrutura do tipo 'handles' que possui os ponteiros para os arquivos utilizados para leitura
    \return Esta função retorna o valor do contador
    */
	int i, count=0;
	char c;

	for(i=0; i<16; i++){

		if (i == 0)
            setSEL(BYTE_SELECTED_LOW);
		else
		if (i == 8)
            setSEL(BYTE_SELECTED_HIGH);

		rewind(input_values->fileBit[i%8]);
		c = fgetc(input_values->fileBit[i%8]);
		count = count + atoi(&c)*(1<<i);
	}
	return count;
}

/** \brief Lê os valores de fim de curso*/
int readLimitSwitches(handles *h, int num) //LER FIM DE CURSO
{
    /**
    \details Lê o sensor de fim de curso representado pelo valor do parâmetro 'num'
	\param h Estrutura do tipo 'handles' que possui os ponteiros para os arquivos utilizados para leitura
	\param num Valor do tipo inteiro que define qual sensor será lido
    \return Esta função retorna o valor do sensor lido, ou -1 quando o sensor não existe
    */
    char c;
    rewind(h->fileLim1);
    rewind(h->fileLim2);
	if (num == 1)
	{
		c = fgetc(h->fileLim1);
 		return atoi(&c);
	}
	if (num == 2)
	{
        c = fgetc(h->fileLim2);
		return atoi(&c);
	}

	return -1;
}
/** \brief  Habilita o PWM */
void enablePWM(void)
{
    /**
    \details Habilita o PWM escrevendo 1 em /sys/class/pwm/pwmchip0/pwm1/enable
    \return Esta função não possui retorno
    */
    system("echo 1 > /sys/class/pwm/pwmchip0/pwm1/enable");
    setDIS(1);
}
/** \brief  Desabilita o PWM */
void disablePWM(void)
{
    /**
    \details Desabilita o PWM escrevendo 0 em /sys/class/pwm/pwmchip0/pwm1/enable
    \return Esta função não possui retorno
    */
    system("echo 0 > /sys/class/pwm/pwmchip0/pwm1/enable");
    setDIS(0);
}

/** \brief  Calcula o pid */
double pid(double *P_error, double *I_error, double *D_error, double *error, double *prevError, double setPoint, double processValue) {
    *prevError = *error;
    *error = setPoint - processValue;

    *P_error = *error;
    *I_error += *prevError;
    *D_error = *error - *prevError;


    return 0.1 * *P_error + 0.3 * *I_error + 0.02 * *D_error;
}
