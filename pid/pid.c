#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <quanser_2DSFJ.h>

#define KP 10
#define KI 10
#define KD 10
#define SAMPLE_FREQ 1000000

double min(double a, double b) {
	if (a <= b)
		return a;
	else
		return b;
}

int main() {
	float desired_angle = 0;
	float last_error = 0;
	float sample_period = 1/SAMPLE_FREQ;
	float degrees = 0;
	double angle;
	double error, error_area, error_speed;
	double u;

	printf("Desired position (in degrees): \n");
	scanf("%f",&degrees);

	desired_angle = degrees * (M_PI / 180);

	while(1) {
		angle = getEncoderRadiansData();

		// absolute error
		error = desired_angle - floor(angle);

		// integration by trapezoidal rule
		error_area = (min(error, last_error) + abs(error - last_error) / 2) * sample_period; // sample_period in nanoseconds

		// instant speed
		error_speed = (error - last_error) / sample_period;

		u = KP * error + KI * error_area + KD * error_speed;

		last_error = error;

		setMotorVoltage(u);
		usleep(1);
	}
}
