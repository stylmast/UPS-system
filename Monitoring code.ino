  Program:      eth_websrv_SD_Ajax_gauge Voltmeter, Batt Amp, Charger Amp & Inverter Amp 

  Description:  Arduino web server web page displays Arduino
                analog value on a dial gauge.
                The web page is stored on the SD card.
                Ajax is used to update the analog value on the
                web page.
  
  Hardware:     Arduino Ten, should work with other Arduinos and
                compatible Ethernet shields.
                2Gb micro SD card formatted FAT16.
                
  Software:     Developed using Arduino 1.0.5 software
                Should be compatible with Arduino 1.0 +
                SD card contains web page called index.htm
  
  References:   - WebServer example by David A. Mellis and 
                  modified by Tom Igoe
                - SD card examples by David A. Mellis and
                  Tom Igoe
                - Ethernet library documentation:
                  http://arduino.cc/en/Reference/Ethernet
                - SD Card library documentation:
                  http://arduino.cc/en/Reference/SD
                - Gauge from:
                  https://github.com/Mikhus/canv-gauge

 Author:        S.A.Elford 8 Dec 2014

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   50

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(0, 0, 0,0); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80
File webFile;               // the web page file on the SD card
char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer

void setup()
{
    // disable Ethernet chip
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);

    Serial.begin(9600);       // for debugging
    
    // initialize SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("index.htm")) {
        Serial.println("ERROR - Can't find index.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found index.htm file.");
    
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    
}

void loop()
{
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                // buffer first part of HTTP request in HTTP_req array (string)
                // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
                if (req_index < (REQ_BUF_SZ - 1)) {
                    HTTP_req[req_index] = c;          // save HTTP request character
                    req_index++;
                }
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    // remainder of header follows below, depending on if
                    // web page or XML page is requested
                    // Ajax request - send XML file
                    if (StrContains(HTTP_req, "ajax_inputs")) {
                        // send rest of HTTP header
                        client.println("Content-Type: text/xml");
                        client.println("Connection: keep-alive");
                        client.println();
                        // send XML file containing input states
                        XML_response(client);
                    }
                    else {  // web page request
                        // send rest of HTTP header
                        client.println("Content-Type: text/html");
                        client.println("Connection: keep-alive");
                        client.println();
                        // send web page
                        webFile = SD.open("index.htm");        // open web page file
                        if (webFile) {
                            while(webFile.available()) {
                                client.write(webFile.read()); // send web page to client
                            }
                            webFile.close();
                        }
                    }
                    // display received HTTP request on serial port
                    Serial.print(HTTP_req);
                    // reset buffer index and all buffer elements to 0
                    req_index = 0;
                    StrClear(HTTP_req, REQ_BUF_SZ);
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

// send the XML file containing analog value
void XML_response(EthernetClient cl)
{
    int analog_val = 0;
    int analog_val1 = 0;      // Define analog input pin value to 0, Voltmeter
    int analog_val2 = 0;      // Define analog input pin value to 0, Battery current
    int analog_val3 = 0;      // Define analog input pin value to 0, Chager current
    int analog_val4 = 0;      // Define analog input pin value to 0, Inverter current
    char sample;
    const int numReadings = 20;
    const int voltPin = 0;  
    
    float dominator;
    int resistor1 = 12000;
    int resistor2 = 5000;
    int readings[numReadings];      // the readings from the analog input
    int index = 0;                  // the index of the current reading
    int total = 0;                  // the running total
    int average = 0;                // the average
    int inputPin0 = A0;              // Analog input pin Voltage value
    int inputPin1 = A1;              // Analog input pin Battery value
    int inputPin2 = A2;              // Analog input pin Charger value    
    int inputPin3 = A3;              // Analog input pin Inverter value
    //int average1 = 0;                // the average for input Voltage reading NOT IN US at the moment
    //int average2 = 0;                // the average for input Battery reading NOT IN US at the moment
   // int average3 = 0;                // the average for input Charger reading NOT IN US at the moment
   // int average4 = 0;                // the average for input Inverter reading NOT IN US at the moment
  
   // int voltage = 0;
    
   dominator = (float)resistor2 / (resistor1 + resistor2);
   for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
   
   float voltage; 
   voltage = analogRead(inputPin0);
   
   voltage = (voltage / 1024) * 4.86;    // Convert to actual voltage (0 - 5 Vdc)
   voltage = voltage / dominator;
   
   Serial.print("Volts: ");    // Output to serial
   Serial.println(voltage, 2);
   
   delay(1000);
   
    cl.print("<?xml version = \"1.0\" ?>");
    cl.println("<inputs>");
    analog_val = analogRead(inputPin0);  // read analog pin A0 UPS voltage
    cl.print("<analog>");
    cl.print(analog_val);
    cl.print("</analog>");
    analog_val = analogRead(1);  // read analog pin A1 battery current 50 Amp Bi directional
    cl.print("<analog>");
    cl.print(analog_val2);
    cl.print("</analog>");
    analog_val = analogRead(2);  // read analog pin charger current 50 amp Uni directional
    cl.print("<analog>");
    cl.print(analog_val3);
    cl.print("</analog>");
    analog_val = analogRead(3);  // read analog pin A2 inverter current 200 Amp Uni directional
    cl.print("<analog>");
    cl.print(analog_val4);
    cl.print("<analog>");
    cl.print("</inputs>");
    
   delay(1000);
/*  /// NOT IN USE at the moment
 total= total - readings[index];   		// subtract the last reading:      
  readings[index] = analogRead(0);      	// read from the sensor:
  total= total + readings[index]; 		// add the reading to the total:   
  index = index + 1;      		        // advance to the next position in the array:   
  if (index >= numReadings)    		        // if we're at the end of the array..
  index = 0;                  		        // ...wrap around to the beginning:
  average = total / numReadings;	        // calculate the average:
  Serial.print(average1);  		        // send it to the computer as ASCII digits
  delay(2000); 				        // delay in between reads for stability

total= total - readings[index];   		// subtract the last reading:      
  readings[index] = analogRead(1);	        // read from the sensor:
  total= total + readings[index]; 		// add the reading to the total:   
  index = index + 1;      		        // advance to the next position in the array:   
  if (index >= numReadings)    		        // if we're at the end of the array..
  index = 0;                  		        // ...wrap around to the beginning:
  average = total / numReadings;	        // calculate the average:
  Serial.print(average2);  		        // send it to the computer as ASCII digits
  delay(2000);
  
total= total - readings[index];   		// subtract the last reading:      
  readings[index] = analogRead(2);        	// read from the sensor:
  total= total + readings[index]; 		// add the reading to the total:   
  index = index + 1;      		        // advance to the next position in the array:   
  if (index >= numReadings)    		        // if we're at the end of the array..
  index = 0;                  		        // ...wrap around to the beginning:
  average = total / numReadings;	        // calculate the average:
  Serial.print(average3);  		        // send it to the computer as ASCII digits
  delay(2000);

total= total - readings[index];   		// subtract the last reading:      
  readings[index] = analogRead(3);        	// read from the sensor:
  total= total + readings[index]; 		// add the reading to the total:   
  index = index + 1;      		        // advance to the next position in the array:   
  if (index >= numReadings)    		        // if we're at the end of the array..
  index = 0;                  		        // ...wrap around to the beginning:
  average = total / numReadings;	        // calculate the average:
  Serial.print(average4);  		        // send it to the computer as ASCII digits
  delay(2000);
  
*/
 
double Current1 = currentSensor1(analogRead(inputPin1));  // Read  analog value
  Serial.print("Batt Amps:"); 
  printDouble(Current1, 2);                               // display Current, number of decimal places
  Serial.print(" A");
  Serial.print("   ");
  delay(2000);
 
double Current2 = currentSensor2(analogRead(inputPin2));  // Read  analog value
  Serial.print(", Charge Amps:"); 
  printDouble(Current2, 2);                               // display Current, number of decimal places
  Serial.print(" A");
  Serial.print("   ");
  delay(2000);
  
double Current3 = currentSensor3(analogRead(inputPin3));  // Read  analog value
  Serial.print(", Inverter Amps:"); 
  printDouble(Current2, 2);                               // display Current, number of decimal places
  Serial.print(" A");
  Serial.println("   ");
  delay(2000);
}

// --------------------------------------------------------------------------------------------------------

 Print decimal numbers

void printDouble(double val, byte precision) {

  Serial.print (int(val));                                     // Print int part
  if( precision > 0) {                                         // Print decimal part
    Serial.print(".");
    unsigned long frac, mult = 1;
    byte padding = precision -1;
    while(precision--) mult *=10;
    if(val >= 0) frac = (val - int(val)) * mult; else frac = (int(val) - val) * mult;
    unsigned long frac1 = frac;
    while(frac1 /= 10) padding--;
    while(padding--) Serial.print("0");
    Serial.println(frac,DEC) ;
  }
}

// ---------------------------------------------------------------
// Read 1.1V reference against AVcc

long readInternalVcc() {

  long result;
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);                                                    // Wait for Vref to settle
  ADCSRA |= _BV(ADSC);                                         // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result;                                  // Back-calculate AVcc in mV
  return result;
}

//-------------------------------------------------------------

// Calculate current 50 Amp Bi sensor

double currentSensor1(int RawADC) {

  int    Sensitivity    = 40; // mV/A
  long   InternalVcc    = readInternalVcc();
  double ZeroCurrentVcc = InternalVcc / 2;
  double SensedVoltage  = (RawADC * InternalVcc) / 1024;
  double Difference     = SensedVoltage - ZeroCurrentVcc;
  double SensedCurrent  = Difference / Sensitivity;
  Serial.print("Charger ");
  Serial.print(RawADC);
  /*Serial.print("/1024");
  Serial.print(", S V: ");
  printDouble(SensedVoltage, 1);
  Serial.print("mV");
  Serial.print(", 0A at: ");
  printDouble(ZeroCurrentVcc, 1);
 */
  Serial.print("mV");
  return SensedCurrent;                                        // Return the Current
       
}

  // Calculate current with 50 Amp Uni sensor

double currentSensor2(int RawADC) {

  int    Sensitivity    = 60; // mV/A
  long   InternalVcc    = readInternalVcc();
  double ZeroCurrentVcc = InternalVcc * 0.12;
  double SensedVoltage  = (RawADC * InternalVcc) / 1024;
  double Difference     = SensedVoltage - ZeroCurrentVcc;
  double SensedCurrent  = Difference / Sensitivity;
  Serial.print("Batt Bank ");
  Serial.print(RawADC);
  /*Serial.print("/1024");
  Serial.print(", S V: ");
  printDouble(SensedVoltage, 1);
  Serial.print("mV");
  Serial.print(", 0A at: ");
  printDouble(ZeroCurrentVcc, 1);
  */
  Serial.print("mV,");
  return SensedCurrent;                                        // Return the Current
       
}

// Calculate current with 200 Amp Uni sensor

double currentSensor3(int RawADC) {

  int    Sensitivity    = 20; // mV/A
  long   InternalVcc    = readInternalVcc();
  double ZeroCurrentVcc = InternalVcc * 0.12;    
  double SensedVoltage  = (RawADC * InternalVcc) / 1024;
  double Difference     = SensedVoltage - ZeroCurrentVcc;
  double SensedCurrent  = Difference / Sensitivity;
  Serial.print("Inverter ");
  Serial.print(RawADC);
  /*Serial.print("/1024");
  Serial.print(", S V: ");
  printDouble(SensedVoltage, 1);
  rial.print("mV");
  Serial.print(", 0A at: ");
  printDouble(ZeroCurrentVcc, 1);
  */
  Serial.println("mV,");
  return SensedCurrent;                                        // Return the Current
*/
}
// ------------------------------------------------------------------------       


// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}
