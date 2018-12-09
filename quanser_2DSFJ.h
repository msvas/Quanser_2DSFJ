/*
    \mainpage
    \author Hugo Constantinopolos, Marcelo Vasques, Renan Bertoluzzi
    \version 1.0
    \date 2018
*/

#ifndef lib_2DSFJ

#define lib_2DSFJ

/** \brief Estrutura utilizada para ler os bits do contador e os sensores de fim de curso  */
typedef struct sHandles handles;

// Assinatura das fun��es
handles *initialize(void);
void setPWMDutyCycle(int duty_cycle);
void setPWMPeriod(int period);
void setMotorVoltage(double value);
void enablePWM(void);
void disablePWM(void);
void setDIS(int value);
void setReset(int value);
void setSEL(int value);
void setMUX(int value);
void setENB(int value);

int readCounter(handles * input_values);
int readLimitSwitches(handles * h, int num);

double pid_controller(double *P_error, double *I_error, double
*D_error, double *error, double *prevError, double setPoint, double
processValue);

#endif







