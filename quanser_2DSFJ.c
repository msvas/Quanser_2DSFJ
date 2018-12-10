/*
    \mainpage
    \author Marcelo Vasques, Arthur Ribeiro e Matheus Tura
    \version 1.0
    \date 2018
*/

#include <stdio.h>                  //Biblioteca de entrada e saída
#include <stdlib.h>                 //Biblioteca de função padrão
#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

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

/** Tensão lógica (5V) */
#define GALILEO_VOLTAGE 5
/** Tensão máxima (27V) */
#define MOTOR_VOLTAGE 27
/** Período do sinal de PWM */
#define PWM_FREQ 1000
/** Para conversão de unidades*/
#define NANO 1000000000
/** Valor de radianos por contagem do encoder do motor */
#define RADS_PER_COUNT 0.0015

struct PinFiles {
	int end1;
	int end2;
	int dir;
	int rst;
	int decoder_pins[8];
};

struct PwmFiles {
	int period;
	int duty_cycle;
	int enable;
};

struct AdcFiles {
	int voltage;
	int scale;
};

struct PinFiles gpios;
struct PwmFiles pwm1;
struct PwmFiles pwm2;
struct AdcFiles dc1;
struct pollfd event_end[2];
struct sigaction act;

static volatile int run = 1;

void quit(int signal) {
	run = 0;
}

int getPinValue(int file) {
	char s;
	int value;

	lseek(file, 0, SEEK_SET);
	read(file, &s, sizeof s);

	if (s == '1')
		value = 1;
	else
		value = 0;

	return value;
}

void setPinValue(int file, int value) {
	char s[20];

	sprintf(s, "%d", value);

	lseek(file, 0, SEEK_SET);
	write(file, &s, sizeof s);
}

char setBit(char byte, int value, int pos) {
	if (value)
		byte |= 1 << pos;
	else
		byte &= ~(1 << pos);

	return byte;
}

int readDecoder() {
	unsigned char data_read = 0x00;
	int value;

	data_read = setBit(data_read, getPinValue(gpios.decoder_pins[0]), 0);
	data_read = setBit(data_read, getPinValue(gpios.decoder_pins[1]), 1);
	data_read = setBit(data_read, getPinValue(gpios.decoder_pins[2]), 2);
	data_read = setBit(data_read, getPinValue(gpios.decoder_pins[3]), 3);
	data_read = setBit(data_read, getPinValue(gpios.decoder_pins[4]), 4);
	data_read = setBit(data_read, getPinValue(gpios.decoder_pins[5]), 5);
	data_read = setBit(data_read, getPinValue(gpios.decoder_pins[6]), 6);
	data_read = setBit(data_read, getPinValue(gpios.decoder_pins[7]), 7);

	value = data_read;

	return value;
}

void openPinFiles() {
	// END1
	gpios.end1 = open("/sys/class/gpio/gpio11/value", O_RDONLY);

	// END2
	gpios.end2 = open("/sys/class/gpio/gpio12/value", O_RDONLY);

	// DIR
	gpios.dir = open("/sys/class/gpio/gpio13/value", O_WRONLY);

	// PWM2
	pwm2.period = open("/sys/class/pwm/pwmchip0/device/pwm_period", O_WRONLY);
	pwm2.duty_cycle = open("/sys/class/pwm/pwmchip0/pwm1/duty_cycle", O_WRONLY);
	pwm2.enable = open("/sys/class/pwm/pwmchip0/pwm1/enable", O_WRONLY);

	// RST
	gpios.rst = open("/sys/class/gpio/gpio6/value", O_WRONLY);

	// PWM1
	pwm1.period = pwm2.period;
	pwm1.duty_cycle = open("/sys/class/pwm/pwmchip0/pwm3/duty_cycle", O_WRONLY);
	pwm1.enable = open("/sys/class/pwm/pwmchip0/pwm3/enable", O_WRONLY);

	// D0
	gpios.decoder_pins[0] = open("/sys/class/gpio/gpio1/value", O_RDONLY);

	// D1
	gpios.decoder_pins[1] = open("/sys/class/gpio/gpio38/value", O_RDONLY);

	// D2
	gpios.decoder_pins[2] = open("/sys/class/gpio/gpio40/value", O_RDONLY);

	// D3
	gpios.decoder_pins[3] = open("/sys/class/gpio/gpio4/value", O_RDONLY);

	// D4
	gpios.decoder_pins[4] = open("/sys/class/gpio/gpio10/value", O_RDONLY);

	// D5
	gpios.decoder_pins[5] = open("/sys/class/gpio/gpio5/value", O_RDONLY);

	// D6
	gpios.decoder_pins[6] = open("/sys/class/gpio/gpio15/value", O_RDONLY);

	// D7
	gpios.decoder_pins[7] = open("/sys/class/gpio/gpio7/value", O_RDONLY);

	// DC1
	dc1.voltage = open("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", O_RDONLY);
	dc1.scale = open("/sys/bus/iio/devices/iio:device0/in_voltage0_scale", O_WRONLY);
}

double getEncoderData() {
	int count;

	count = readDecoder();

	return RADS_PER_COUNT * count;
}

int getPWMDirection(float voltage) {
	int direction;

	if (voltage > 2.5f)
		direction = 1;
	else
		direction = 0;

	return direction;
}

/** \brief Seta a ciclo de trabalho do PWM */
void setPWMDutyCycle(int file, int duty_cycle) {
	char s[20];

	sprintf(s, "%d", duty_cycle);

	lseek(file, 0, SEEK_SET);
    write(file, &s, sizeof(s));
}

/** \brief Seta a tensão do motor */
void setMotorVoltage(double value) {
    /**
    \details Seta a tensão do motor com o valor especificado por parâmetro
    \param value Valor do tipo inteiro usado para definir a tensão do motor
    \return Esta função não possui retorno
    */
	double period = 1.0/PWM_FREQ * NANO;

	if (value > MOTOR_VOLTAGE)
		value = MOTOR_VOLTAGE;

	int cycle = (int)(value/MOTOR_VOLTAGE * period);

	if(cycle > period*0.9)
        cycle = period * 0.9;

	setPWMDutyCycle(pwm2.duty_cycle, cycle);
}

/** \brief Seta o período do PWM */
void setPWMPeriod(int period) {
	char s[20];

	sprintf(s, "%d", period);

	lseek(pwm2.period, 0, SEEK_SET);
    write(pwm2.period, s, sizeof(s));
}

int reachedEnd() {
	int reached = 0;

	if (getPinValue(gpios.end1))
		reached = 1;
	else if (getPinValue(gpios.end2))
		reached = 1;

	return reached;
}

int endLeft() {
	int reached = 0;

	if (getPinValue(gpios.end1))
		reached = 1;

	return reached;
}

int endRight() {
	int reached = 0;

	if (getPinValue(gpios.end2))
		reached = 1;

	return reached;
}

void interruptionsReachedEnd() {
	unsigned char c;

	event_end[0].fd = gpios.end1;
	event_end[1].fd = gpios.end2;

	/* Limpa valores */
    read(event_end[0].fd, &c, 1);
	read(event_end[1].fd, &c, 1);

	event_end[0].events = POLLPRI;
	event_end[1].events = POLLPRI;

	act.sa_handler = quit;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);

	poll(event_end, 2, -1);
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

void initialize() {
	openPinFiles();
	setPWMPeriod(1/PWM_FREQ * NANO);
	setPWMDutyCycle(pwm1.duty_cycle, 0);
	setPWMDutyCycle(pwm2.duty_cycle, 0);
	setPinValue(pwm1.enable, 1);
	setPinValue(pwm2.enable, 1);
	setPinValue(gpios.rst, 1);
	usleep(1000);
	setPinValue(gpios.rst, 0);
}

int main() {
	int motor_percentage;
	int direction;

	initialize();

	printf("Set percentage to control motor speed and direction (0-100):");
	scanf("%d", &motor_percentage);

	direction = getPWMDirection(GALILEO_VOLTAGE * motor_percentage * 0.01);

	setPWMDutyCycle(pwm2.duty_cycle, motor_percentage);
	setPinValue(gpios.dir, direction);

	interruptionsReachedEnd();

	setPWMDutyCycle(pwm2.duty_cycle, 0);

	return 0;
}
