#ifndef SENSORS_H_
#define SENSORS_H_


extern void Gyro_Config(void);
extern void Gyro_ReadAngRate (float* pfData);
extern void Compass_Config(void);
extern void Compass_ReadAcc(float* pfData);
extern void Compass_ReadMag (float* pfData);

#endif /* SENSORS_H_ */
