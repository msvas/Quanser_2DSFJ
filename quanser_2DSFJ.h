/*
    \mainpage
    \author Marcelo Vasques, Arthur Ribeiro e Matheus Tura
    \version 1.0
    \date 2018
*/

#ifndef quanser_2DSFJ

#define quanser_2DSFJ

// Assinatura das funções
int getValueFromPin(FILE *file);
char setBit(char byte, int value, int pos);
char readDecoder();
void openPinFiles();
int getPWMDirection(float voltage);
float mapPWMVoltage(float voltage);
void setPWMDutyCycle(FILE *file, int duty_cycle);
void setMotorVoltage(double value);
void setPWMPeriod(int period);
double pid_controller(double *P_error, double *I_error, double
*D_error, double *error, double *prevError, double setPoint, double
processValue);

#endif
