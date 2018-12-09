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
	IO0 - GPIO - END1
    IO1 - GPIO - END2
    IO2 - GPIO - DIR
    IO3 - PWM - PWM2
    IO4 - GPIO - RST
    IO5 - PWM - PWM1
    IO6 - GPIO - D0
    IO7 - GPIO - D1
    IO8 - GPIO - D2
    IO9 - GPIO - D3
    IO10 - GPIO - D4
    IO11 - GPIO - D5
    IO12 - GPIO - D6
    IO13 - GPIO - D7
    IO14 - ADC - DC1
*/

/** Usado para setar os bits mais significativos do contador. (COUNTHIGH = 0)*/
//#define BYTE_SELECTED_LOW 0
/** Usado para setar os bits menos significativos do contador. (COUNTLOW = 1)*/
//#define BYTE_SELECTED_HIGH 1
/** Frequência máxima da galileo*/
#define GALILEO_FREQ 1525
/** Tensão máxima (27V) */
#define MOTOR_VOLTAGE 27
/** Período do sinal de PWM */
#define PWM_PERIOD 100

/** Número máximo de ranhuras*/
//#define NUM_MARKS 4096
/** Para conversão de unidades*/
//#define NANO 1000000000

struct PinFiles {
	FILE *end1;
	FILE *end2;
	FILE *dir;
	FILE *rst;
	FILE *decoder_pins[8];
}

struct PwmFiles {
	FILE *period;
	FILE *dut_cycle;
	FILE *enable;
}

struct AdcFiles {
	FILE *voltage;
	FILE *scale;
}

char readDecoder() {

}

void openPinFiles() {
	// END1
	open("/sys/class/gpio/gpio11/value", O_RDONLY);

	// END2
	open("/sys/class/gpio/gpio12/value", O_RDONLY);

	// DIR
	open("/sys/class/gpio/gpio13/value", O_WRONLY);

	// PWM2
	open("/sys/class/pwm/pwmchip0/device/pwm_period", O_WRONLY);
	open("/sys/class/pwm/pwmchip0/pwm1/duty_cycle", O_WRONLY);
	open("/sys/class/pwm/pwmchip0/pwm1/enable", O_WRONLY);

	// RST
	open("/sys/class/gpio/gpio6/value", O_WRONLY);

	// PWM1
	open("/sys/class/pwm/pwmchip0/pwm3/duty_cycle", O_WRONLY);
	open("/sys/class/pwm/pwmchip0/pwm3/enable", O_WRONLY);

	// D0
	open("/sys/class/gpio/gpio1/value", O_RDONLY);

	// D1
	open("/sys/class/gpio/gpio38/value", O_RDONLY);

	// D2
	open("/sys/class/gpio/gpio40/value", O_RDONLY);

	// D3
	open("/sys/class/gpio/gpio4/value", O_RDONLY);

	// D4
	open("/sys/class/gpio/gpio10/value", O_RDONLY);

	// D5
	open("/sys/class/gpio/gpio5/value", O_RDONLY);

	// D6
	open("/sys/class/gpio/gpio15/value", O_RDONLY);

	// D7
	open("/sys/class/gpio/gpio7/value", O_RDONLY);

	// DC1
	open("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", O_RDONLY);
	open("/sys/bus/iio/devices/iio:device0/in_voltage0_scale", O_WRONLY);
}

int readLimSwitches() {

}

int getPWMDirection(float voltage) {

}

float mapPWMVoltage(float voltage) {
	float newVoltage;

	return newVoltage;
}

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

int main() {
	char decodedData;

	setPWMPeriod(PWM_PERIOD);

}
