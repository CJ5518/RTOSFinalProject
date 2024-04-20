//Spencer Butler, CS 452, Final Project
#ifndef hdcWrapper_hpp
#define hdcWrapper_hpp 

#define HDC1080ADDRESS 0x40

//Stores temperature in t and humidity in h
//Temperature is degrees Celsius
//Humidity is between 0 and 1
//First function call initializes the sensor rather than returning values
void getTempHumid(double &t, double &h);

//Converts degrees Celsius to Fahrenheit
double degCToF(double degC);

#endif
