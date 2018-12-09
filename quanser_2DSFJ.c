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
};

struct PwmFiles {
	FILE *period;
	FILE *duty_cycle;
	FILE *enable;
};

struct AdcFiles {
	FILE *voltage;
	FILE *scale;
};

struct PinFiles gpios;
struct PwmFiles pwm1;
struct PwmFiles pwm2;
struct AdcFiles dc1;

int getValueFromPin(FILE *file) {
	char s;
	int value;

	fseek(file, 0, SEEK_SET);
	fread(&s, sizeof(s), 1, file);

	if (s == '1')
		value = 1;
	else
		value = 0;

	return value;
}

char setBit(char byte, int value, int pos) {
	if (value)
		byte |= 1 << pos;
	else
		byte &= ~(1 << pos);

	return byte;
}

char readDecoder() {
	unsigned char data_read = 0x00;

	data_read = setBit(data_read, getValueFromPin(gpios.decoder_pins[0]), 0);
	data_read = setBit(data_read, getValueFromPin(gpios.decoder_pins[1]), 1);
	data_read = setBit(data_read, getValueFromPin(gpios.decoder_pins[2]), 2);
	data_read = setBit(data_read, getValueFromPin(gpios.decoder_pins[3]), 3);
	data_read = setBit(data_read, getValueFromPin(gpios.decoder_pins[4]), 4);
	data_read = setBit(data_read, getValueFromPin(gpios.decoder_pins[5]), 5);
	data_read = setBit(data_read, getValueFromPin(gpios.decoder_pins[6]), 6);
	data_read = setBit(data_read, getValueFromPin(gpios.decoder_pins[7]), 7);

	return data_read;
}

void openPinFiles() {
	// END1
	gpios.end1 = fopen("/sys/class/gpio/gpio11/value", "r");

	// END2
	gpios.end2 = fopen("/sys/class/gpio/gpio12/value", "r");

	// DIR
	gpios.dir = fopen("/sys/class/gpio/gpio13/value", "w");

	// PWM2
	pwm2.period = fopen("/sys/class/pwm/pwmchip0/device/pwm_period", "w");
	pwm2.duty_cycle = fopen("/sys/class/pwm/pwmchip0/pwm1/duty_cycle", "w");
	pwm2.enable = fopen("/sys/class/pwm/pwmchip0/pwm1/enable", "w");

	// RST
	gpios.rst = fopen("/sys/class/gpio/gpio6/value", "w");

	// PWM1
	pwm1.period = pwm2.period;
	pwm1.duty_cycle = fopen("/sys/class/pwm/pwmchip0/pwm3/duty_cycle", "w");
	pwm1.enable = fopen("/sys/class/pwm/pwmchip0/pwm3/enable", "w");

	// D0
	gpios.decoder_pins[0] = fopen("/sys/class/gpio/gpio1/value", "r");

	// D1
	gpios.decoder_pins[1] = fopen("/sys/class/gpio/gpio38/value", "r");

	// D2
	gpios.decoder_pins[2] = fopen("/sys/class/gpio/gpio40/value", "r");

	// D3
	gpios.decoder_pins[3] = fopen("/sys/class/gpio/gpio4/value", "r");

	// D4
	gpios.decoder_pins[4] = fopen("/sys/class/gpio/gpio10/value", "r");

	// D5
	gpios.decoder_pins[5] = fopen("/sys/class/gpio/gpio5/value", "r");

	// D6
	gpios.decoder_pins[6] = fopen("/sys/class/gpio/gpio15/value", "r");

	// D7
	gpios.decoder_pins[7] = fopen("/sys/class/gpio/gpio7/value", "r");

	// DC1
	dc1.voltage = fopen("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", "r");
	dc1.scale = fopen("/sys/bus/iio/devices/iio:device0/in_voltage0_scale", "w");
}

int getPWMDirection(float voltage) {
	int direction;

	if (voltage > 2.5f)
		direction = 1;
	else
		direction = 0;

	return direction;
}

float mapPWMVoltage(float voltage) {
	float newVoltage;

	return newVoltage;
}

/** \brief Seta a ciclo de trabalho do PWM */
void setPWMDutyCycle(FILE *file, int duty_cycle) {
	char s[20];

	sprintf(s, "%d", duty_cycle);

	fseek(file, 0, SEEK_SET);
    fwrite(s, sizeof(char), sizeof(s), file);
}

/** \brief Seta a tensão do motor */
void setMotorVoltage(double value) {
    /**
    \details Seta a tensão do motor com o valor especificado por parâmetro
    \param value Valor do tipo inteiro usado para definir a tensão do motor
    \return Esta função não possui retorno
    */
	double period = 1.0/GALILEO_FREQ * 1000000000;

	int cycle = (int)(value/MOTOR_VOLTAGE * 0.5 * period + 0.5 * period);

	if(cycle > period*0.9)
        cycle = period * 0.9;

	setPWMDutyCycle(pwm2.duty_cycle, cycle);
}

/** \brief Seta o período do PWM */
void setPWMPeriod(int period) {
	char s[20];

	sprintf(s, "%d", period);

	fseek(pwm2.period, 0, SEEK_SET);
    fwrite(s, sizeof(char), sizeof(s), pwm2.period);
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
	int motor_percentage;

	openPinFiles();

	setPWMPeriod(PWM_PERIOD);

	printf("Set percentage to control motor speed and direction (0-100):");
	scanf("%d", &motor_percentage);

	setPWMDutyCycle(pwm2.duty_cycle, motor_percentage);


	return 0;
}
