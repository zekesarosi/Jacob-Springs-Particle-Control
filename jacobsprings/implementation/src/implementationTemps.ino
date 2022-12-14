/*
* Project Implementation
* Description: A script to control a walkin fridge freezer
* Author: Ezekiel Sarosi
* Date: November 30, 2022
*/


#include <OneWire.h>
#include <spark-dallas-temperature.h>
#include <string>

#define INVALID_SENSOR_INDEX 0xff
#define INVALID_TEMP 0x7fff
#define ADDRESS_LEN 8
#define WIRE_BUS 2
#define ADDR_LEN 8
#define COMPRESSOR_RELAY 7
#define FAN_RELAY 5


//?? NOTE: the large sections of commented code is the beginnings of a higher level wifi control


// switching original sensors 1 & 4 for better testing 
uint8_t sensor4[8] = { 0x28, 0xBB, 0xE0, 0x49, 0xF6, 0x96, 0x3C, 0x60 }; //sensors unique address declarations
uint8_t sensor2[8] = { 0x28, 0x29, 0x99, 0x49, 0xF6, 0x80, 0x3C, 0x67 };
uint8_t sensor1[8] = { 0x28, 0x58, 0x39, 0x81, 0xE3, 0xD8, 0x3C, 0xA0 };
uint8_t sensor3[8] = { 0x28, 0xDE, 0xCA, 0x81, 0xE3, 0x59, 0x3C, 0xFB };
uint8_t sensor5[8] = { 0x28, 0xA3, 0x0F, 0x49, 0xF6, 0xF5, 0x3C, 0x20 };


int freezerSensors[2] = { 0, 1 };
int fridgeSensors[2] = { 2, 3 };

// funcion protoypes that can be called from particle API, have to return an Int and be passed a string


int freezerBoundUpperF(String); // funcion prototype for setting upper freezer temp
int freezerBoundLowerF(String); // the lower temp prototype 

int fridgeBoundUpperF(String); //same for the fridge
int fridgeBoundLowerF(String);

int compressorOverrideToOn(String); // funcion prototype for overriding compressor state
int compressorOverrideToOff(String);

int fanOverrideToOn(String);
int fanOverrideToOn(String);

int resetOverrides(String);
int resetCustomTemps(String);

////////////////////////////////////////////////////////////////


int freezerUpperBoundF = 0; // default values for fridge / freezer bounds
int freezerLowerBoundF = -8;
int fridgeUpperBoundF = 38;
int fridgeLowerBoundF = 33;

// FLAGS /////////////

bool fanState = false; // flag that when true means component is on
bool compressorState = false; 

bool fanOverrideOn = false; // flag that when true means fan is meant to be always on
bool compressorOverrideOn = false; // flag that when true means compressor is meant to be always on

bool fanOverrideOff = false;
bool compressorOverrideOff = false; // flag that when true means compressor is meant to be always off

bool freezerCooling = false; // flags to improve cooling logic
bool fridgeCooling = false;

///////////////////////////////////////

double temps[5] = {}; // declaration for temperature array. Idexes are mapped 0-4 to sensor1-sensor5
int sensorNum; // declaration for amount of sensors detected


OneWire oneWire(WIRE_BUS); // inits the WIRE_BUS pin as the serial bus
DallasTemperature sensors(&oneWire); // passes the serial bus to dallas temperature lib




void setup() {
    Serial.begin(9600); // begins serial communicaiton 
    pinMode(COMPRESSOR_RELAY, OUTPUT); // inits pin as OUTPUT
    pinMode(FAN_RELAY, OUTPUT); // same
    
    //Serial.println("Starting...");
    sensors.begin(); // starts the dallas temp sensing
    sensorNum =  sensors.getDeviceCount(); // asks how many sensors are detected 
    
    Particle.variable("Number of Sensors", sensorNum); // publishes global vars to particle api, vars are updated after each loop
    Particle.variable("Freezer Temp 1 (yellow)", temps[0]);
    Particle.variable("Freezer Temp 2 (blue)", temps[1]);
    Particle.variable("Fridge Temp 1 (red)", temps[2]);
    Particle.variable("Fridge Temp 2 (green)", temps[3]);
    //Particle.variable("Compressor Temp (red)", temps[4]);
    Particle.variable("Fan State", fanState);
    Particle.variable("Compressor State", compressorState);
    Particle.variable("Freezer Upper Bound(F)", freezerUpperBoundF);
    Particle.variable("Freezer Lower Bound(F)", freezerLowerBoundF);
    Particle.variable("Fridge Upper Bound(F)", fridgeUpperBoundF);
    Particle.variable("Fridge Lower Bound(F)", fridgeLowerBoundF);
    Particle.function("Set Freezer High(F)", freezerBoundUpperF); // same but with functions
    Particle.function("Set Freezer Low(F)", freezerBoundLowerF);
    Particle.function("Set Fridge High(F)", fridgeBoundUpperF);
    Particle.function("Set Fridge Low(F)", fridgeBoundLowerF);
    Particle.function("Reset Temps to Default", resetCustomTemps);
    Particle.function("Override Compressor to On", compressorOverrideToOn);
    Particle.function("Override Compressor to Off", compressorOverrideToOff);
    Particle.function("Override Fan to On", fanOverrideToOn);
    Particle.function("Override Fan to Off", fanOverrideToOff);
    Particle.function("Reset Control Overrides", resetOverrides);
    
}

void loop() {
    sensorNum =  sensors.getDeviceCount(); // checks how many sensors are connected
    // Serial.print("Device count: ");
    // Serial.println(sensorNum);
    
    
    //Serial.print(" Requesting temperatures..."); 
    sensors.requestTemperatures(); // ask sensors for temps
    //Serial.println("Temperatures recieved"); 
    //Serial.println("Updating Temperatures");
    updateAll(); // update and cout all temperature values


    /*
    while ( checkError() ) {
        coolingLoop();
    }
    */
    //compressorlogic is commented out because we dont want to run the compressor during testing
    //compressorLogic(); // runs the compressor 
    fanLogic(); // and fan logic


    //compressorControl(compressorState); // passes the desired state to the control function
    fanControl(fanState);

    
    
    // if (!retryRunning && !Particle.connected()) { // if we have not already scheduled a retry and are not connected
    //     Serial.println("schedule");
    //     stopTimer.start();         // set timeout for auto-retry by system
    //     retryRunning = true;
    //     retryTimer.start();        // schedula a retry
    // }
    
    
    //commeting out the delay will allow for the code to run at maximum speed which produces a better graph
    // delay(3000); // waits 3 seconds
}

bool checkError(){
    sensors.requestTemperatures(); // update sensors
    updateAll();
    
    if (!compressorOverrideOn && !compressorOverrideOff && !fanOverrideOn && !fanOverrideOff){
        if ( sensors.getDeviceCount() == 0 ){
            return true;
        }
        else if ( temps[0] == -1000 && temps[1] == -1000 ){
            return true;
        }
        else if ( temps[2] == -1000 && temps[3] == -1000 ){
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }

}

void coolingLoop(){
    compressorState = true;
    fanState = true;
    int interval_to_five = 60; // 5*60 = 300 seconds per state.
    compressorControl(compressorState);
    fanControl(fanState);
    
    while ( checkError() ){
        compressorControl(true);
        fanControl(true);
        while ( checkError() || interval_to_five > 0 ){ //on loop
            delay(5000); // wont be perfectly five seconds because of logic times. 
            interval_to_five = interval_to_five - 5;
        }
        if ( checkError() ){
            break;
        }
        else {
            compressorControl(false);
            fanControl(false);
            while ( checkError() || interval_to_five < 60 ){
                delay(5000);
                interval_to_five = interval_to_five + 5;
            }
        }
        
    }
//    Serial.println("Cooling Loop Broken");
    

}


void compressorLogic(){
    if ( !compressorOverrideOn && !compressorOverrideOff) { // first checks if any overrides are active
        if ( freezerCooling ){
            freezerCooling = false;
            for (int j = 0; j < 2; j++){
                if ( temps[j] != -1000 ){
                    if ( temps[j] > (freezerUpperBoundF - ((freezerUpperBoundF - freezerLowerBoundF) * (3.0/4))) ) {
                        freezerCooling = true;
                       // Serial.println("Cooling Loop will continue");
                        break;
                    }
                }
            }
        }
        if ( !freezerCooling ){
            for (int i = 2; i < 4; i++) { // loops through first two temps
               if ( temps[i] >= freezerUpperBoundF ){
                    //Serial.println("Cooling loop activated");
                    compressorState = true; // if the temp is greater than the upper bound compressor state is set to true
                    freezerCooling = true;
                    break; // breaks if any of the sensors are greater.
                }
                else {
                    compressorState = false;
                } 

            }
        }

    } 
    else if ( compressorOverrideOn ){
        compressorState = true; // if the overrideOn is set to true then activate the compressor
    }
    else if ( compressorOverrideOff ){
        compressorState = false; // same but opposite
    }
}

// ignore fan logic 
void fanLogic(){
    if ( !fanOverrideOn && !fanOverrideOff) { // first checks if any overrides are active
        if ( fridgeCooling ){
            fridgeCooling = false;
            for (int j = 2; j < 4; j++){
                if ( temps[j] != -1000 ){
                    if ( temps[j] > (fridgeUpperBoundF - ((fridgeUpperBoundF - fridgeLowerBoundF) * (3.0/4))) ) {
                        fridgeCooling = true;
                       // Serial.println("Cooling Loop will continue");
                        break;
                    }
                }
            }
        }
        if ( !fridgeCooling ){
            for (int i = 2; i < 4; i++) { // loops through first two temps
               if ( temps[i] >= fridgeUpperBoundF ){
                    //Serial.println("Cooling loop activated");
                    fanState = true; // if the temp is greater than the upper bound compressor state is set to true
                    fridgeCooling = true;
                    break; // breaks if any of the sensors are greater.
                }
                else {
                    fanState = false;
                } 

            }
        }

    } 
    else if ( fanOverrideOn ){
        fanState = true; // if the overrideOn is set to true then activate the compressor
    }
    else if ( fanOverrideOff ){
        fanState = false; // same but opposite
    }
}



void fanControl(bool control) {
    if ( control ){
        digitalWrite(FAN_RELAY, HIGH); // sets pin 5 to HIGH (3.3V)
    }
    else {
        digitalWrite(FAN_RELAY, LOW); // sets to LOW (0 V)
    }
}


void compressorControl(bool control) {
    if ( control ){
        digitalWrite(COMPRESSOR_RELAY, HIGH);
    }
    else {
        digitalWrite(COMPRESSOR_RELAY, LOW);
    }
}


void updateAll() {   //updates all 5 sensors
    //Serial.print("1, ");
    temps[0] = updateTemperature(sensor1);
  
    //Serial.print("2, ");
    temps[1] = updateTemperature(sensor2);
  
    //Serial.print("3, ");
    temps[2] = updateTemperature(sensor3);
    
    //Serial.print("4, ");
    temps[3] = updateTemperature(sensor4);
    
    //Serial.print("5, ");
    temps[4] = updateTemperature(sensor5);
    Serial.println("");
}


double updateTemperature(DeviceAddress deviceAddress) { // askes each sensor for temperature and converts value to (F)
    if ( sensors.isConnected(deviceAddress) ) {
        double tempF = sensors.getTempF(deviceAddress);
        Serial.print(tempF);
        Serial.print(", ");
        //Serial.println(" F");
        return tempF;
    }
    else {
        Serial.print("-1000");
        Serial.print(", ");
        return -1000;
    }
}



int compressorOverrideToOn(String command){ // override funcions for compressorOn / Off and the fan 
    compressorOverrideOff = false;
    compressorOverrideOn = true;
    return 1;
}

int compressorOverrideToOff(String command){
    compressorOverrideOff = true;
    compressorOverrideOn = false;
    return 1;
}

int fanOverrideToOn(String command){
    fanOverrideOff = false;
    fanOverrideOn = true;
    return 1;
}

int fanOverrideToOff(String command){
    fanOverrideOff = true;
    fanOverrideOn = false;
    return 1;
}


//////////////////////////////////////////////



int resetOverrides(String command){ //reset overrides to default no override
    fanOverrideOn = false;
    compressorOverrideOn = false;
    fanOverrideOff = false;
    compressorOverrideOff = false;
    return 1;
}

int resetCustomTemps(String command){ //reset custom temps to the default temps
    freezerUpperBoundF = 0;
    freezerLowerBoundF = -8;
    fridgeUpperBoundF = 38;
    fridgeLowerBoundF = 33;
    return 1;
}


int freezerBoundUpperF(String command){
    freezerUpperBoundF = command.toFloat(); // takes the funcion input converts to float and then truncates to int for temperature bounds
    return freezerUpperBoundF;
}
int freezerBoundLowerF(String command){
    freezerLowerBoundF = command.toFloat(); //mostly the same as above 
    return freezerLowerBoundF;
}

int fridgeBoundUpperF(String command){
    fridgeUpperBoundF = command.toFloat();
    return fridgeUpperBoundF;
}
int fridgeBoundLowerF(String command){
    fridgeLowerBoundF = command.toFloat();
    return fridgeLowerBoundF;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////