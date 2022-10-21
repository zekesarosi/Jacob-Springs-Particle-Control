#include <spark-dallas-temperature.h>
#include <string>
#include <sstream>
#include <OneWire.h>

#define INVALID_SENSOR_INDEX 0xff
#define INVALID_TEMP 0x7fff
#define ADDRESS_LEN 8
#define WIRE_BUS 4
#define ADDR_LEN 8


//?? NOTE: the large sections of commented code is the beginnings of a higher level wifi control

// SYSTEM_MODE(SEMI_AUTOMATIC);
// SYSTEM_THREAD(ENABLED);


uint8_t sensor1[8] = { 0x28, 0xA3, 0x0F, 0x49, 0xF6, 0xF5, 0x3C, 0x20 }; //sensor unique name declaration
uint8_t sensor2[8] = { 0x28, 0x29, 0x99, 0x49, 0xF6, 0x80, 0x3C, 0x67 };
uint8_t sensor3[8] = { 0x28, 0x58, 0x39, 0x81, 0xE3, 0xD8, 0x3C, 0xA0 };
uint8_t sensor4[8] = { 0x28, 0xDE, 0xCA, 0x81, 0xE3, 0x59, 0x3C, 0xFB };
uint8_t sensor5[8] = { 0x28, 0xBB, 0xE0, 0x49, 0xF6, 0x96, 0x3C, 0x60 };


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


// const uint32_t msRetryDelay = 5*60000; // retry every 5min
// const uint32_t msRetryTime  =   30000; // stop trying after 30sec

// bool   retryRunning = false;
// Timer retryTimer(msRetryDelay, retryConnect);  // timer to retry connecting
// Timer stopTimer(msRetryTime, stopConnect);



void setup() {
    Serial.begin(9600); // begins serial communicaiton 
    pinMode(8, OUTPUT); // inits pin 8 as OUTPUT
    pinMode(5, OUTPUT); // same
    
    Serial.println("Starting...");
    sensors.begin(); // starts the dallas temp sensing
    sensorNum =  sensors.getDeviceCount(); // asks how many sensors are detected 
    
    //   Particle.connect();
    // if (!waitFor(Particle.connected, msRetryTime)) {
    //     WiFi.off();                // no luck, no need for WiFi
    // }
    
    Particle.variable("Number of Sensors", sensorNum); // publishes global vars to particle api, vars are updated after each loop
    Particle.variable("Freezer Temp 1 (yellow)", temps[0]);
    Particle.variable("Freezer Temp 2 (blue)", temps[1]);
    Particle.variable("Fridge Temp 1 (orange)", temps[2]);
    Particle.variable("Fridge Temp 2 (green)", temps[3]);
    Particle.variable("Compressor Temp (red)", temps[4]);
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
    Serial.print(" Requesting temperatures..."); 
    sensors.requestTemperatures(); // ask sensors for temps
    Serial.println("Temperatures recieved"); 
    Serial.println("Updating Temperatures");
    updateAll(); // update and cout all temperature values
    
    
    compressorLogic(); // runs the compressor 
    fanLogic(); // and fan logic

    
    compressorControl(compressorState); // passes the desired state to the control function
    fanControl(fanState);
    
    
    // if (!retryRunning && !Particle.connected()) { // if we have not already scheduled a retry and are not connected
    //     Serial.println("schedule");
    //     stopTimer.start();         // set timeout for auto-retry by system
    //     retryRunning = true;
    //     retryTimer.start();        // schedula a retry
    // }
    
    
    
    delay(3000); // waits 3 seconds
}

void compressorLogic(){
    if ( !compressorOverrideOn && !compressorOverrideOff) { // first checks if any overrides are active
        if ( freezerCooling ){
            freezerCooling = false;
            for (int j = 0; j < 2; j++){
                if ( temps[j] > (freezerUpperBoundF - ((freezerUpperBoundF - freezerLowerBoundF) * (3.0/4))) ) {
                    freezerCooling = true;    
                    break;
                }
            }            
        }
        if ( !freezerCooling ){
            for (int i = 0; i < 2; i++) { // loops through first two temps
                if ( temps[i] >= freezerUpperBoundF ){
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
        for (int i = 2; i < 4; i++) { // loops temps 2-3
            if ( fridgeCooling ) {
                if ( temps[i] > (fridgeUpperBoundF - ((fridgeUpperBoundF - fridgeLowerBoundF) * (3/4))) ) {
                    break;
                }
            }
            fridgeCooling = false;
            if ( temps[i] >= fridgeUpperBoundF ){
                fanState = true; // if the temp is greater than the upper bound compressor state is set to true
                fridgeCooling = true;
                break; // breaks if any of the sensors are greater.
            }
            else {
                fanState = false;
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
        digitalWrite(5, HIGH); // sets pin 5 to HIGH (3.3V)
    }
    else {
        digitalWrite(5, LOW); // sets to LOW (0 V)
    }
}


void compressorControl(bool control) {
    if ( control ){
        digitalWrite(8, HIGH);
    }
    else {
        digitalWrite(8, LOW);
    }
}


void updateAll() {   //updates all 5 sensors
    Serial.println("Sensor 1: ");
    temps[0] = updateTemperature(sensor1);
  
    Serial.println("Sensor 2: ");
    temps[1] = updateTemperature(sensor2);
  
    Serial.println("Sensor 3: ");
    temps[2] = updateTemperature(sensor3);
    
    Serial.println("Sensor 4: ");
    temps[3] = updateTemperature(sensor4);
    
    Serial.println("Sensor 5: ");
    temps[4] = updateTemperature(sensor5);
}


double updateTemperature(DeviceAddress deviceAddress) { // askes each sensor for temperature and converts value to (F)
    double tempC = sensors.getTempC(deviceAddress);
    Serial.print(tempC);
    Serial.print((char)176);
    Serial.print("C  |  ");
    Serial.print(DallasTemperature::toFahrenheit(tempC));
    // double tempF = DallasTemperature::toFahrenheit(tempC);
    Serial.print((char)176);
    Serial.println("F");
    return DallasTemperature::toFahrenheit(tempC);
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


//////



int resetOverrides(String command){ //reset overrides to no override
    fanOverrideOn = false;
    compressorOverrideOn = false;
    fanOverrideOff = false;
    compressorOverrideOff = false;
    return 1;
}

int resetCustomTemps(String command){ //reset custom temps to the default
    freezerUpperBoundF = 0;
    freezerLowerBoundF = -8;
    fridgeUpperBoundF = 38;
    fridgeLowerBoundF = 33;
    return 1;
}


int freezerBoundUpperF(String command){
    freezerUpperBoundF = command.toFloat(); // takes the funcion input convers to float and then truncates to int for temperature bounds
    return freezerUpperBoundF;
}
int freezerBoundLowerF(String command){
    freezerLowerBoundF = command.toFloat(); 
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

// void retryConnect()
// {
//   if (!Particle.connected())   // if not connected to cloud
//   {
//     Serial.println("reconnect");
//     stopTimer.start();         // set of the timout time
//     WiFi.on();
//     Particle.connect();        // start a reconnectino attempt
//   }
//   else                         // if already connected
//   {
//     Serial.println("connected");
//     retryTimer.stop();         // no further attempts required
//     retryRunning = false;
//   }
// }

// void stopConnect()
// {
//     Serial.println("stopped");

//     if (!Particle.connected()) // if after retryTime no connection
//       WiFi.off();              // stop trying and swith off WiFi
//     stopTimer.stop();
// }
