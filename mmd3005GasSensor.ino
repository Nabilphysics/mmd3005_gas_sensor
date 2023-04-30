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
 Circuit: ESP32 3.3V to Sensor 3.3V , GND to GND, ESP32 Rx(GPIO 16) to Sensor TX
*/     
#include <HardwareSerial.h>
HardwareSerial SerialPort(2); //UART2 of ESP32

String sensorData = "";
int i = 0;

void setup() {
  Serial.begin(115200);
  // set the data rate for the HardwareSerial port of ESP32
  SerialPort.begin(38400, SERIAL_8N1, 16, 17);
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
    Serial.println(sensorData);
    extractData(sensorData);
    sensorData = "";
    }
  }
}


void extractData(String sensorData){
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
  Serial.println("------ NH3,H2S,TVOC Sensor - MMD3005 ------");
  //Temperature
  String temperatureHexString = temperature1 + temperature2;
  unsigned long int temperatureIntValue = strtol(temperatureHexString.c_str(), NULL, 16); // convert hex to int
  Serial.print("temperature: "); Serial.print(temperatureIntValue/1000); Serial.println(" celsius");
  //Ammonia
  String ammoniaHexString = amonia1 + amonia2;
  unsigned long int ammoniaIntValue = strtol(ammoniaHexString.c_str(), NULL, 16); // convert hex to int
  Serial.print("Ammonia: "); Serial.print(ammoniaIntValue/1000); Serial.println(" ppm");
  
  //Humidity
  String humidityHexString = humidity1+humidity2;
  unsigned long int humidityIntValue = strtol(humidityHexString.c_str(), NULL, 16); // convert hex to int
  Serial.print("Humidity: ");Serial.print(humidityIntValue/1000); Serial.println("");
  
  //TVOC
  String tvocHexString = tvoc1+tvoc2;
  unsigned long int tvocIntValue = strtol(tvocHexString.c_str(), NULL, 16); // convert hex to int
  Serial.print("TVOC: ");Serial.print(tvocIntValue/1000); Serial.println(" ppm");

  //H2S -  Hydrogen Sulfide
  String hydrogenSulfideHexString = hydrogenSulfide1 + hydrogenSulfide2;
  unsigned long int hydrogenSulfideIntValue = strtol(hydrogenSulfideHexString.c_str(), NULL, 16); // convert hex to int
  Serial.print("H2S: ");Serial.print(hydrogenSulfideIntValue/1000); Serial.println(" ppm");

}


