#pragma once


///
typedef struct {
	bool connected = true;  //mobile connection
	time_t epoch = 0; //current time taken from modem
	float tankLevel_1 = 0;
	float tankLevel_2 = 0;
	bool mainFlow = false;
	bool mainValve = false;
	float temperature = 0;
} SensorValues_t;
 
///
typedef struct {
	float tank1 = 0;
	float tank2 = 0;
	float temp1 = 0;
 	float temp2 = 0;
  	float temp3 = 0;
} testValues_t;  

///
typedef struct {
	bool connected = true;  //mobile connection
	time_t epoch = 0; //current time taken from modem
} Status_t;