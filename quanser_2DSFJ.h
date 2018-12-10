/*
    \mainpage
    \author Marcelo Vasques, Arthur Ribeiro e Matheus Tura
    \version 1.0
    \date 2018
*/

#ifndef quanser_2DSFJ

#define quanser_2DSFJ

// Assinatura das funções
void quit(int signal);
int getPinValue(int file);
void setPinValue(int file, int value);
char setBit(char byte, int value, int pos);
int readDecoder();
void openPinFiles();
double getEncoderData();
int getPWMDirection(float voltage);
void setPWMDutyCycle(int file, int duty_cycle);
void setMotorVoltage(double value);
void setPWMPeriod(int period);
int reachedEnd();
int endLeft();
int endRight();
void interruptionsReachedEnd();
double pid_controller(double *P_error, double *I_error, double
*D_error, double *error, double *prevError, double setPoint, double
processValue);
void initialize();

#endif
