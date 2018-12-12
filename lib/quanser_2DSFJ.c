/*
    \mainpage
    \author Marcelo Vasques, Arthur Ribeiro e Matheus Tura
    \version 1.0
    \date 2018
*/

#include <stdio.h>		//Biblioteca de entrada e saída
#include <stdlib.h>		//Biblioteca de função padrão
#include <poll.h>		//Biblioteca usada para interrupções.
#include <signal.h>		//Biblioteca de sinais usada para abortar o programa.
#include <unistd.h>		//Biblioteca usada para sleep.
#include <fcntl.h>		//Biblioteca de manipulação de arquivos (usada nos pinos).
#include <math.h>		//Biblioteca de funções e constantes matemáticas.

/* Header da biblioteca aqui desenvolvida */
#include <quanser_2DSFJ.h>

/*	Pinos Utilizados
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

/** Estrutura para armazenar os arquivos que configuram os pinos GPIO **/
struct PinFiles {
	// Sensor de fim de curso esquerdo do braço.
	int end1;
	// Sensor de fim de curso direito do braço.
	int end2;
	// Sentido do movimento do braço.
	int dir;
	// Reset para o decodificador de quadratura.
	int rst;
	// Dados lidos do decodificador de quadratura com a contagem da posição.
	int decoder_pins[8];
};

/** Estrutura para armazenar os arquivos que configuram os pinos PWM **/
struct PwmFiles {
	int period;
	int duty_cycle;
	int enable;
};

/** Estrutura para armazenar os arquivos que configuram o pino ADC **/
struct AdcFiles {
	int voltage;
	int scale;
};

/** Declaração das estruturas aqui usadas **/
struct PinFiles gpios;
struct PwmFiles pwm1;
struct PwmFiles pwm2;
struct AdcFiles dc1;

/** Poll para uso de interrupções e sinal para encerrar o programa **/
struct pollfd event_end[2];
struct sigaction act;

// Variável que indica que o programa está rodando
static volatile int run = 1;

/** \brief Encerra o programa ao receber o sinal */
void quit(int signal) {
	run = 0;
}

/** \brief Lê o pino e retorna seu valor */
int getPinValue(int file) {
	/**
	\details Usa o pino aberto no parâmetro para ler o valor.
	\param file Arquivo do pino a ser lido.
	\return Valor lido do arquivo do pino.
	*/
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

/** \brief Escreve valor no pino */
void setPinValue(int file, int value) {
	/**
	\details Usa o pino aberto no parâmetro para escrever o valor passado.
	\param file Arquivo do pino a ser escrito.
	\param value Valor a ser escrito.
	\return Não há retorno.
	*/
	char s[20];

	sprintf(s, "%d", value);

	lseek(file, 0, SEEK_SET);
	write(file, &s, sizeof s);
}

/** \brief Altera o valor de um bit do char */
char setBit(char byte, int value, int pos) {
	/**
	\details Seta em 1 ou 0 um bit escolhido do byte de char.
	\param byte Byte a ser alterado.
	\param value Valor de bit a ser escrito.
	\param pos Posição do byte a ter bit alterado.
	\return Novo byte em char.
	*/
	if (value)
		byte |= 1 << pos;
	else
		byte &= ~(1 << pos);

	return byte;
}

/** \brief Lê os 8 pinos informados pelo decoder. */
int readDecoder() {
	/**
	\details Lê cada pino, que representa um bit, da saída do decoder e cria um byte.
	\return Byte formado pelos 8 pinos em um char.
	*/
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

/** \brief Abre todos os arquivos dos pinos configurados. */
void openPinFiles() {
	/**
	\details Abre e guarda nas estruturas todos os arquivos dos pinos usados no projeto.
	\return Não há retorno.
	*/

	// END1
	if((gpios.end1 = open("/sys/class/gpio/gpio11/value", O_RDONLY)) < 0)
		printf("Error opening end1\n");

	// END2
	if((gpios.end2 = open("/sys/class/gpio/gpio12/value", O_RDONLY)) < 0)
		printf("Error opening end2\n");

	// DIR
	if((gpios.dir = open("/sys/class/gpio/gpio13/value", O_WRONLY)) < 0)
		printf("Error opening dir\n");

	// PWM2
	if((pwm2.period = open("/sys/class/pwm/pwmchip0/device/pwm_period", O_WRONLY)) < 0)
		printf("Error opening pwm period\n");
	if((pwm2.duty_cycle = open("/sys/class/pwm/pwmchip0/pwm1/duty_cycle", O_WRONLY)) < 0)
		printf("Error opening pwm1 duty cycle\n");
	if((pwm2.enable = open("/sys/class/pwm/pwmchip0/pwm1/enable", O_WRONLY)) < 0)
		printf("Error opening pwm1 enable\n");

	// RST
	if((gpios.rst = open("/sys/class/gpio/gpio6/value", O_WRONLY)) < 0)
		printf("Error opening rst\n");

	// PWM1
	pwm1.period = pwm2.period;
	if((pwm1.duty_cycle = open("/sys/class/pwm/pwmchip0/pwm3/duty_cycle", O_WRONLY)) < 0)
		printf("Error opening pwm3 duty cycle\n");
	if((pwm1.enable = open("/sys/class/pwm/pwmchip0/pwm3/enable", O_WRONLY)) < 0)
		printf("Error opening pwm3 enable\n");

	// D0
	if((gpios.decoder_pins[0] = open("/sys/class/gpio/gpio1/value", O_RDONLY)) < 0)
		printf("Error opening d0\n");

	// D1
	if((gpios.decoder_pins[1] = open("/sys/class/gpio/gpio38/value", O_RDONLY)) < 0)
		printf("Error opening d1\n");

	// D2
	if((gpios.decoder_pins[2] = open("/sys/class/gpio/gpio40/value", O_RDONLY)) < 0)
		printf("Error opening d2\n");

	// D3
	if((gpios.decoder_pins[3] = open("/sys/class/gpio/gpio4/value", O_RDONLY)) < 0)
		printf("Error opening d3\n");

	// D4
	if((gpios.decoder_pins[4] = open("/sys/class/gpio/gpio10/value", O_RDONLY)) < 0)
		printf("Error opening d4\n");

	// D5
	if((gpios.decoder_pins[5] = open("/sys/class/gpio/gpio5/value", O_RDONLY)) < 0)
		printf("Error opening d5\n");

	// D6
	if((gpios.decoder_pins[6] = open("/sys/class/gpio/gpio15/value", O_RDONLY)) < 0)
		printf("Error opening d6\n");

	// D7
	if((gpios.decoder_pins[7] = open("/sys/class/gpio/gpio7/value", O_RDONLY)) < 0)
		printf("Error opening d7\n");

	// DC1
	if((dc1.voltage = open("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", O_RDONLY)) < 0)
		printf("Error opening dc1\n");
	if((dc1.scale = open("/sys/bus/iio/devices/iio:device0/in_voltage0_scale", O_WRONLY)) < 0)
		printf("Error opening dc1 scale\n");
}

/** \brief Converte para radianos o valor lido do decoder. */
double getEncoderRadiansData() {
	/**
	\details Usa a constante do motor para converter para radianos a contagem do decoder.
	\return Posição do braço em radianos.
	*/
	int count;

	count = readDecoder();

	return RADS_PER_COUNT * count;
}

/** \brief Usa a tensão da galileo para verificar a direção do movimento do braço. */
int getPWMDirection(float voltage) {
	/**
	\details Usa a tensão da galileo para verificar a direção do movimento do braço.
	\param voltage Tensão lógica da Galileo em DC, relativa ao PWM e o duty cycle.
	\return O sentido do movimento em 1 ou 0.
	*/
	int direction;

	if (voltage > 2.5f)
		direction = 1;
	else
		direction = 0;

	return direction;
}

/** \brief Seta a ciclo de trabalho do PWM */
void setPWMDutyCycle(int file, int duty_cycle) {
	/**
	\details Usa o arquivo do PWM para setar o ciclo do trabalho no arquivo.
	\param file Arquivo do PWM com o ciclo de trabalho.
	\param duty_cycle Ciclo de trabalho em nanossegundos.
	\return Sem retorno.
	*/
	char s[20];

	sprintf(s, "%d", duty_cycle);

	printf("Setting pwm duty cycle to %s\n", s);

	lseek(file, 0, SEEK_SET);
    write(file, &s, sizeof(s));
}

/** \brief Seta a tensão do motor */
void setMotorVoltage(double value) {
    /**
    \details Seta a tensão do motor com o valor especificado por parâmetro.
    \param value Valor do tipo double usado para definir a tensão do motor.
    \return Esta função não possui retorno
    */
	int period = (1.0 / PWM_FREQ) * NANO; //Período é o inverso da frequência.

	if (value > MOTOR_VOLTAGE)
		value = MOTOR_VOLTAGE;
	if (value < 0)
		value = 0;

	// Usa a fração da tensão desejada para setar o período do ciclo.
	int cycle = (int)((value / MOTOR_VOLTAGE) * period);

	printf("Setting pwm cycle to %d, period is %d\n", cycle, period);
	setPWMDutyCycle(pwm2.duty_cycle, cycle);
}

/** \brief Seta o período do PWM */
void setPWMPeriod(int period) {
	/**
	\details Altera o período total do PWM.
	\param period Período em nanossegundos.
	\return Sem retorno.
	*/
	char s[20];

	sprintf(s, "%d", period);

	printf("Setting pwm period to %d\n", period);
	lseek(pwm2.period, 0, SEEK_SET);
    write(pwm2.period, s, sizeof(s));
}

/** \brief Função para checar se atingiu um fim de curso. */
int reachedEnd() {
	/**
	\details Lê os dois sensores de fim de curso e informa se atingiu algum.
	\return 1 se atingiu, 0 se não.
	*/
	int reached = 0;

	if (!getPinValue(gpios.end1))
		reached = 1;
	else if (!getPinValue(gpios.end2))
		reached = 1;

	return reached;
}

/** \brief Função para checar se atingiu o fim de curso esquerdo. */
int endLeft() {
	/**
	\details Lê o sensor de fim de curso esquerdo e informa se foi atingido.
	\return 1 se atingiu, 0 se não.
	*/
	int reached = 0;

	if (!getPinValue(gpios.end1))
		reached = 1;

	return reached;
}

/** \brief Função para checar se atingiu o fim de curso direito. */
int endRight() {
	/**
	\details Lê o sensor de fim de curso direito e informa se foi atingido.
	\return 1 se atingiu, 0 se não.
	*/
	int reached = 0;

	if (!getPinValue(gpios.end2))
		reached = 1;

	return reached;
}

/** \brief Função que usa interrupções para checar se atingiu algum dos fins de curso. */
void interruptionsReachedEnd() {
	/**
	\details Função que usa interrupções para checar se atingiu algum dos
	fins de curso, ficando travado enquanto não atinge.
	\return Sem retorno.
	*/
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
double pid(double dt, double *P_error, double *I_error, double *D_error, double *error, double *prevError, double setPoint, double processValue) {
    *prevError = *error;
    *error = setPoint - processValue;

    *P_error = *error;
    *I_error += *prevError * dt;
    *D_error = (*error - *prevError) / dt;
    return 0.1 * *P_error + 0.2 * *I_error + 0.0005 * *D_error;
}

/** \brief  Muda o valor do reset do decoder. */
void setRst(int value) {
	setPinValue(gpios.rst, value);
}

/** \brief  Muda o valor da direção do PWM. */
void setDir(int value) {
	setPinValue(gpios.dir, value);
}

/** \brief  Inicializa os pinos com valores seguros. */
void initialize() {
	openPinFiles();
	setPWMPeriod((1.0 / PWM_FREQ) * NANO);
	setPWMDutyCycle(pwm1.duty_cycle, 0);
	setPWMDutyCycle(pwm2.duty_cycle, 0);
	setPinValue(pwm1.enable, 1);
	setPinValue(pwm2.enable, 1);
	setPinValue(gpios.rst, 1);
	usleep(1000);
	setPinValue(gpios.rst, 0);
	usleep(1000);
}

/*
int main() {
	int motor_percentage;
	int direction;

	initialize();

	//direction = getPWMDirection(GALILEO_VOLTAGE * motor_percentage * 0.01);
	setPinValue(gpios.dir, 1);

	setMotorVoltage(13.5);

	printf("Set voltage to control motor speed and direction (0-27):");
	scanf("%d", &motor_percentage);
	//setPWMDutyCycle(pwm2.duty_cycle, motor_percentage);
	//setPinValue(gpios.dir, direction);

	//interruptionsReachedEnd();

	//setPWMDutyCycle(pwm2.duty_cycle, 0);


	return 0;
}
*/
