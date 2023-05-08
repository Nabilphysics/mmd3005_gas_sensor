/*
Board: ESP32 Devboard
Sensor: MMD3005
Code: Syed Razwanul Haque (Nabil)
GitHub: https://www.github.com/Nabilphysics
Linkdin: https://www.linkedin.com/in/nabilphysics/
Position: Lead Robotics Engnieer
Company: Bengal Champ, Bangladesh 
*/

/*
 Datasheet: https://github.com/Nabilphysics/mmd3005_gas_sensor/blob/main/mmd3005GasSensor.ino
 Example Sensor Data in Hexadecimal
 AA00555C00008D0500007401000084020000000098E8010001000000000000000069D6000000BB64    = 1 - 80 , substring(78,80) = 64
 
 AA00A8610000DC0200004B030000BA0300000000204E010000A86100003075000050C3000000BB87  
-----------------------------------------------------------------------------------------------------------------------------------
     AA      |00|A861|0000|DC02 |0000| 4B03|0000|BA03 |0000|0000|204E|010000|A861|0000|3075|0000|  50C3  |000000|  BB    | 87     |
 ----------------------------------------------------------------------------------------------------------------------------------     
 Frame Header|  |Temp|    |volt1|    |volt2|    |volt3|    |    |NH3 |      |TVOC|    |H2S |    |humidity|      |endframe|checksum|
 ----------------------------------------------------------------------------------------------------------------------------------
 Conversion Example:
 NH3 -> 204E -> 4E20(Hexadecimal) -> 20000(decimal) ->20.000(Divided by 1000) in PPM 
 Temp -> A861 -> 61A8(Hexadecimal) ->25000(decimal) ->25.000(Divided by 1000) in Celsius
 Sensor Baud Rate: 38400, 8N1
 
 Sensor Connection : 
 -------------------------
 ESP32     | Sensor MMD3005
 -------------------------
 3.3V      | 3.3V
 GND       | GND
 GPIO16(RX)| Tx

Display Connection :
 -------------------------
 ESP32  | Sensor MMD3005
 -------------------------
 GPIO2  | RST
 GPIP15 | CE
 GPIO4  | DC
 GPIO23 | Din
 GPIO18 | Clk
 3.3V   | Vcc
 3.3V   | BL series with 220 ohm resistor
 GND    | GND 

 Good Resource: https://microcontrollerslab.com/nokia-5110-lcd-esp32-tutorial/
*/     
#include <HardwareSerial.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Serial output on/off based on debugFlag
#define debugFlag true //If true show output in Serial

#if debugFlag == true
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x) 
#define debugln(x)
#endif

HardwareSerial SerialPort(2); //UART2 of ESP32
Adafruit_PCD8544 nokiaDisplay = Adafruit_PCD8544(18,23,4,15,2); //Initialize nokia display 5110

String sensorData = "";
int i = 0;

void setup() {
  Serial.begin(115200); //Serial Port for PC Communication
  // set the data rate for the HardwareSerial port of ESP32
  SerialPort.begin(38400, SERIAL_8N1, 16, 17); //UART2 of ESP32
  nokiaDisplay.begin();

  nokiaDisplay.setContrast(50);

  nokiaDisplay.clearDisplay();
  nokiaDisplay.setTextSize(1);
  nokiaDisplay.setTextColor(BLACK);
  nokiaDisplay.setCursor(1,0);
  nokiaDisplay.println("");
  nokiaDisplay.println(" Bengal Champ");
  nokiaDisplay.println(" Gas Analyzer");
  nokiaDisplay.println("");
  
  //display.setTextSize(1);
  //display.println("bengalchamp.com");
  
  nokiaDisplay.display();
  delay(2000);
}

void loop() {
 
  if (SerialPort.available() > 0) {
    char inputData = SerialPort.read();
    char inputHexChar[3];
  
    sprintf(inputHexChar, "%02X", inputData); // Hex to Char Conversion
    
    String inputHexString = String(inputHexChar); // Char to String Conversion

    if(inputHexString.startsWith("AA")){ // Match The Sensor Header Data and start reading to avoid corrupted data
      i = 0; 
    }
    sensorData = sensorData + inputHexString ; // Acquire full data frame
    i = i + 1;
    
    if(i > 39){ //Print when full received
    debugln(sensorData);
    extractAndShowData(sensorData);
    sensorData = "";
    }
  }
}


void extractAndShowData(String sensorData){
  // This function will convert and print the data  
  String temperature1 = sensorData.substring(6,8); // See conversion example in the start of this code 
  String temperature2 = sensorData.substring(4,6);
 

  String amonia1 = sensorData.substring(42, 44);
  String amonia2 = sensorData.substring(40, 42);
  
  String humidity1 = sensorData.substring(68,70);
  String humidity2 = sensorData.substring(66,68);

  String tvoc1 = sensorData.substring(52,54);
  String tvoc2 = sensorData.substring(50,52);

  String hydrogenSulfide1 = sensorData.substring(60,62);
  String hydrogenSulfide2 = sensorData.substring(58,60);
  
  // Sensor Data Print
  debugln("------ NH3,H2S,TVOC Sensor - MMD3005 ------");
  //Temperature
  String temperatureHexString = temperature1 + temperature2;
  unsigned long int temperatureIntValue = strtol(temperatureHexString.c_str(), NULL, 16); // convert hex to int
  float temperature = temperatureIntValue/float(1000);
  debug("temperature: "); debug(temperature); debugln(" celsius");
  
  
  //Ammonia
  String ammoniaHexString = amonia1 + amonia2;
  unsigned long int ammoniaIntValue = strtol(ammoniaHexString.c_str(), NULL, 16); // convert hex to int
  int ammonia = ammoniaIntValue/int(1000);
  debug("Ammonia: "); debug( ammonia); debugln(" ppm");
  
  //Humidity
  String humidityHexString = humidity1+humidity2;
  unsigned long int humidityIntValue = strtol(humidityHexString.c_str(), NULL, 16); // convert hex to int
  int humidity = humidityIntValue/int(1000);
  debug("Humidity: ");debug(humidity); debugln("");
  
  //TVOC
  String tvocHexString = tvoc1+tvoc2;
  unsigned long int tvocIntValue = strtol(tvocHexString.c_str(), NULL, 16); // convert hex to int
  int tvoc = tvocIntValue/int(1000);
  debug("TVOC: ");debug(tvoc); debugln(" ppm");

  //H2S -  Hydrogen Sulfide
  String hydrogenSulfideHexString = hydrogenSulfide1 + hydrogenSulfide2;
  unsigned long int hydrogenSulfideIntValue = strtol(hydrogenSulfideHexString.c_str(), NULL, 16); // convert hex to int
  int hydrogenSulfide = hydrogenSulfideIntValue/int(1000);
  debug("H2S: ");debug(hydrogenSulfide); debugln(" ppm");

  nokiaDisplay.clearDisplay();
  nokiaDisplay.setTextSize(1);
  nokiaDisplay.setTextColor(BLACK);
  nokiaDisplay.setCursor(0,0);
  
  nokiaDisplay.print("Temp: ");
  nokiaDisplay.print(temperature);
  nokiaDisplay.println(" C");

  nokiaDisplay.print("Humidity: ");
  nokiaDisplay.print(humidity);
  nokiaDisplay.println(" %");

  nokiaDisplay.print("NH3: ");
  nokiaDisplay.print(ammonia);
  nokiaDisplay.println(" ppm");

  nokiaDisplay.print("TVOC: ");
  nokiaDisplay.print(tvoc);
  nokiaDisplay.println(" ppm");

  nokiaDisplay.print("H2S: ");
  nokiaDisplay.print(hydrogenSulfide);
  nokiaDisplay.println(" ppm");

  nokiaDisplay.println(" Bengal Champ");

  nokiaDisplay.display();

}
