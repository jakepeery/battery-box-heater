#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//System Settings
#define HEAT_ON 37.0 //temp in F to turn on the heaters - this is the temp of the box
#define HEAT_OFF 40.0 //temp in F to turn of the heaters - this is the temp of the box

#define MAT_OFF 70.0 //temp in F to turn of the individual mat heater - this is the temp of the mat/aluminum plate

#define LOW_SHUTOFF 34.0 //temp in F to turn off the charging capabilities - battery low limit is 32F
#define HI_SHUTOFF 110.0 //temp in F to turn off the charging capabilities - battery hi limit is 113F

#define DUAL_FAN 105.0 //temp in F to turn on the second cooling fan - this is the temp of the box
#define COOL_ON 100.0 //temp in F to turn on the main cooling fan - this is the temp of the box
#define COOL_OFF 95.0 //temp in F to turn on the main cooling fan - this is the temp of the box



//Screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Temp sensors
#define MatTempsBus  4
#define NumSensors   5

OneWire oneWire(MatTempsBus);


int numberOfMatTempSensors;
DeviceAddress tempDeviceAddress;

DallasTemperature sensors(&oneWire);

//GPO's
#define Heater1   27
#define Heater2   26
#define Heater3   25
#define Heater4   33

#define FAN1   19
#define FAN2   18

#define CHARGE_ENABLE   13
#define CHARGE_ENABLE2   23


// function to print a device address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}

void flashOutputs() {
  digitalWrite(Heater1, LOW);
  digitalWrite(Heater2, LOW);
  digitalWrite(Heater3, LOW);
  digitalWrite(Heater4, LOW);
  digitalWrite(FAN1, LOW);
  digitalWrite(FAN2, LOW);
  digitalWrite(CHARGE_ENABLE, LOW);
  digitalWrite(CHARGE_ENABLE2, LOW);

  
  digitalWrite(Heater1, HIGH);
  delay(100);
  digitalWrite(Heater2, HIGH);
  delay(100);
  digitalWrite(Heater3, HIGH);
  delay(100);
  digitalWrite(Heater4, HIGH);
  delay(100);
  digitalWrite(FAN1, HIGH);
  delay(100);
  digitalWrite(FAN2, HIGH);
  delay(100);
  digitalWrite(CHARGE_ENABLE, HIGH);
  delay(100);
  digitalWrite(CHARGE_ENABLE2, HIGH);
  delay(100);

  digitalWrite(Heater1, LOW);
  delay(100);
  digitalWrite(Heater2, LOW);
  delay(100);
  digitalWrite(Heater3, LOW);
  delay(100);
  digitalWrite(Heater4, LOW);
  delay(100);
  digitalWrite(FAN1, LOW);
  delay(100);
  digitalWrite(FAN2, LOW);
  delay(100);
  digitalWrite(CHARGE_ENABLE, LOW);
  delay(100);
  digitalWrite(CHARGE_ENABLE2, LOW);
  delay(1000);
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Serial.println("battery heater with DS18B20 sensors");
  
//-----------------------------------------------------------OLED------------------------------------
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();

//-----------------------------------------------------Outputs (GPO)-----------------------------------
  pinMode(Heater1, OUTPUT);
  pinMode(Heater2, OUTPUT);
  pinMode(Heater3, OUTPUT);
  pinMode(Heater4, OUTPUT);

  pinMode(FAN1, OUTPUT);
  pinMode(FAN2, OUTPUT);

  pinMode(CHARGE_ENABLE, OUTPUT);
  pinMode(CHARGE_ENABLE2, OUTPUT);

  flashOutputs();
  //set defaults
  digitalWrite(Heater1, LOW);
  digitalWrite(Heater2, LOW);
  digitalWrite(Heater3, LOW);
  digitalWrite(Heater4, LOW);

  digitalWrite(FAN1, LOW);
  digitalWrite(FAN2, LOW);

  digitalWrite(CHARGE_ENABLE, HIGH);
  digitalWrite(CHARGE_ENABLE2, HIGH);

//-----------------------------------------------------Temp Sensors--------------------------------------
  sensors.begin();
  numberOfMatTempSensors = sensors.getDeviceCount();
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfMatTempSensors, DEC);
  Serial.println(" devices.");

  sensors.setResolution(11);   
  //options are 9, 10, 11, or 12-bits, which correspond to 0.5°C, 0.25°C, 0.125°C, and 0.0625°C, respectively.
  //time for temp polling: 9-0.67s, 10-0.140s, 11-0.311s, 12-0.683s
  //time for full loop function to run (as of 3/31/23) 9-0.310s, 10-0.407s, 11-0.508s, 12-0.925s

  // Loop through each device, print out address
  for(int i=0;i<numberOfMatTempSensors; i++) {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
  
  Serial.println("setup End");
}



// the loop function runs over and over again forever
void loop() {
  Serial.println("Loop Start");
  sensors.requestTemperatures(); 
  //Serial.println("After read temps");
  //Serial.println(numberOfMatTempSensors);
  // Loop through each device, print out temperature data
  float temps[NumSensors] = {500, 500, 500, 500, 500};
  for(int i=0;i<numberOfMatTempSensors; i++) {
    // Search the wire for address
    
    // Output the device ID
    if(sensors.getAddress(tempDeviceAddress, i)){
      // Get the data
      //Serial.print("Getting Temps----- ");
      float tempC = sensors.getTempC(tempDeviceAddress);
      temps[i] = (tempC * 9 / 5) + 32;
      //    Serial.println(temps[i]);
    }else{
      Serial.println("No Temp Sensors Found");
      temps[i] = 500;
    }
  }
  //put the temp sensors in order
  float evalTemps[NumSensors - 1] = {temps[1], temps[0], temps[4], temps[3]};
  int stat = evaluateTemp(evalTemps, temps[2]);
  drawScreen(temps[1], temps[0], temps[4], temps[3], temps[2], stat);
}




int evaluateTemp(float temps[NumSensors], float boxTemp) {
  //error check if a sensor is missing
  for (int i = 0; i <= NumSensors - 1; ++i) {
    if (temps[i] == 500.0) {
      Serial.println("Error no temp sensor found");
      return 0;
    }
  }
  
  int displayType = 0; //0=standby, 1=heating, 2=cooling, 3=dual cooling, 4=cold shutoff, 5=over temp shutoff
  if (boxTemp == 500.0){
    Serial.println("Error no BoxTemp sensor found");
    return 0;
  }
  for (int i = 0; i <= NumSensors - 1; ++i) {
    Serial.println(temps[i]);
    //handle heaters
    if (boxTemp < HEAT_ON) {
      displayType = 1;
      //regulate temp of the mats while in heater mode
      if (temps[i] > MAT_OFF) {
        heaterOff(i);
      } else {
        heaterOn(i);
      }
 
    }else if (boxTemp > HEAT_OFF) {
      heaterOff(i);
    }
  }


  //handle cooling fans
  if (boxTemp > DUAL_FAN) {
      coolOn();
      dualOn();
      displayType = 3;
  } else if (boxTemp > COOL_ON){
      coolOn();
      dualOff();
      displayType = 2;
  } else if (boxTemp < COOL_OFF) {
      coolOff();
      dualOff();
  }
 

  //handle shutoff temps
  if (boxTemp < LOW_SHUTOFF){
    chargeDisable();
    displayType = 4;
  }else if(boxTemp > HI_SHUTOFF){
    chargeDisable();
    displayType = 5;
  }else{
    chargeEnable();
  }

  return displayType;

}





void chargeEnable(){
  char buffer[64];
  Serial.print("Charge Enabled ");
  sprintf(buffer, "Pin# %d", CHARGE_ENABLE);
  Serial.println(buffer);
  digitalWrite(CHARGE_ENABLE, HIGH);
}
void chargeDisable(){
  char buffer[64];
  Serial.print("Charge Disabled ");
  sprintf(buffer, "Pin# %d", CHARGE_ENABLE);
  Serial.println(buffer);
  digitalWrite(CHARGE_ENABLE, LOW);
}



void coolOn(){
  char buffer[64];
  Serial.print("FAN1 On ");
  sprintf(buffer, "Pin# %d", FAN1);
  Serial.println(buffer);
  digitalWrite(FAN1, HIGH);
}
void dualOn(){
  char buffer[64];
  Serial.print("FAN2 On ");
  sprintf(buffer, "Pin# %d", FAN2);
  Serial.println(buffer);
  digitalWrite(FAN2, HIGH);
}
void coolOff(){
  char buffer[64];
  Serial.print("FAN1 Off ");
  sprintf(buffer, "Pin# %d", FAN1);
  Serial.println(buffer);
  digitalWrite(FAN1, LOW);
}
void dualOff(){
  char buffer[64];
  Serial.print("FAN2 Off ");
  sprintf(buffer, "Pin# %d", FAN2);
  Serial.println(buffer);
  digitalWrite(FAN2, LOW);
}



void heaterOn(int heater) {
  char buffer[64];
  sprintf(buffer, "heater #%d on", heater);
  Serial.println(buffer);
  if (heater == 1) {
    digitalWrite(Heater1, HIGH);
  }else if (heater == 2){
    digitalWrite(Heater2, HIGH);
  }else if (heater == 3){
    digitalWrite(Heater3, HIGH);
  }else if (heater == 4){
    digitalWrite(Heater4, HIGH);
  }
}
void heaterOff(int heater) {
  char buffer[64];
  sprintf(buffer, "heater #%d off", heater);
  Serial.println(buffer);
  if (heater == 1) {
    digitalWrite(Heater1, LOW);
  }else if (heater == 2){
    digitalWrite(Heater2, LOW);
  }else if (heater == 3){
    digitalWrite(Heater3, LOW);
  }else if (heater == 4){
    digitalWrite(Heater4, LOW);
  }
}




void drawScreen(float tmp1, float tmp2, float tmp3, float tmp4, float boxTemp, int displayType) {
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.setTextSize(2);
  display.println(F("Box Temp"));
  display.setTextSize(2);

  char boxTempPrint[64];
  sprintf(boxTempPrint, " %.1f%cF", boxTemp, (char)9);
  display.println(boxTempPrint);
  
  display.setTextSize(1);
  display.println(F("Battery Temps"));

  //handles two vs 3 digit temps for left alignment
  char buffer[256];
  if (tmp1 >= 100.0) {
    sprintf(buffer, " %.1f%cF    %.1f%cF", tmp1, (char)9, tmp2, (char)9);
  } else {
    sprintf(buffer, " %.1f%cF%s    %.1f%cF", tmp1, (char)9, " ", tmp2, (char)9);
  }
  display.println(buffer);

  //handles two vs 3 digit temps for left alignment
  char buffer2[256];
  if (tmp3 >= 100.0) {
    sprintf(buffer2, " %.1f%cF    %.1f%cF", tmp3, (char)9, tmp4, (char)9);
  } else {
    sprintf(buffer2, " %.1f%cF%s    %.1f%cF", tmp3, (char)9, " ", tmp4, (char)9);
  }
  display.println(buffer2);

  //State of the system (heating, cooling, standby, etc)

  char stat[64];
  switch(displayType) {
  case 1:
    strcpy(stat, "Heating");
    //char str[64] = "Heating";
    break;
  case 2:
    strcpy(stat, "Cooling");
    //char str[64] = "Cooling";
    break;
  case 3:
    strcpy(stat, "Dual Fan Cooling");
    //char str[64] = "Dual Fan Cooling";
    break;
  case 4:
    strcpy(stat, "Cold Shutoff");
    //char str[64] = "Cold Shutoff";
    break;
  case 5:
    strcpy(stat, "Over Temp Shutoff");
    //char str[64] = "Over Temp Shutoff";
    break;
  case 6:
    strcpy(stat, "Sensor Error");
    //char str[64] = "Sensor Error";
    break;
  default:
    strcpy(stat, "standby");
    //char str[64] = "standby";
  }
  display.println(stat);
  
  display.display();
}
