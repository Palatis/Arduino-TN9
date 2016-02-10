// make the library verbose...
#define TN901_DEBUG

#include <TN901.h>

TN901 tn; // instanciate the object

// use this form if you don't want to call init()...
//TN901 tn2(/* data */ 11, /* clock */ 12, /* ack */ 13);

void setup()
{
  Serial.begin(115200);

  // don't call this if you're not using the default constructor
  tn.init(/* data */ 11, /* clock */ 12, /* ack */ 13);
}

void loop()
{
  time_t startMillis = millis();
  
  //tn.read(MODE_OT); // read only object temperature
  //tn.read(MODE_ET); // read only environment temperature
  //tn.read(MODE_OT | MODE_ET); // read both
  tn.read(); // read both, too.
  
  time_t endMillis = millis();
  
  Serial.print("conversion took ");
  Serial.print(endMillis - startMillis);
  Serial.println("ms");
  
  Serial.print("Object / Environment: ");
  Serial.print(tn.getObjectTemperature());
  Serial.print(" / ");
  Serial.println(tn.getEnvironmentTemperature());

  delay(2000);
}