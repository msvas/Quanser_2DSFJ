#include <stdio.h>
#include <unistd.h>
#include <quanser_2DSFJ.h>

#define MOTOR_VOLTAGE 27

/* Programa de teste PID: Gira o braco ate o fim de curso para ter um lugar fixo inicial, e então gira o braco 180Cº (PI rad)*/
int main(void) {

	// inicializacao das variaveis de controle do pid
	double P_error 	 = 0;
	double I_error 	 = 0;
	double D_error 	 = 0;
	double prevError = 0;
	double error 	 = 0;
	double inputValue   = 0;
	int newVoltage   = 0;
	double setPoint  = 6.11; // posição desejada a partir da posicao inicial

	initialize();

	setMotorVoltage(5.0); // o robo vai mexendo para um lado ate chegar num dos limites

	while(!reachedEnd()) { }

	setMotorVoltage(0); // parar motor

	//Resetar contador
	setRst(1);
	usleep(10);
	setRst(0);

	while(1) {
		inputValue = getEncoderRadiansData();
		newVoltage = pid(0.01, &P_error, &I_error, &D_error, &error, &prevError, setPoint, inputValue);

		if (newVoltage >   MOTOR_VOLTAGE)
			newVoltage =   MOTOR_VOLTAGE;

		else
			if (newVoltage < -MOTOR_VOLTAGE)
				newVoltage = -MOTOR_VOLTAGE;

		setMotorVoltage(newVoltage);

		if(reachedEnd())
			setMotorVoltage(0);
	}

	return 0;

}
