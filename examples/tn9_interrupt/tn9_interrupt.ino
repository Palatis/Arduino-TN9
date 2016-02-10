#define TN901_DEBUG
#include <TN901.h>

TN901 tn;

// isr func for attachInterrupt(), supports multiple TN901s on different pin
void tn_isr() {
  tn.processIsr();
}

void onEnvironmentTemperature() {
  Serial.print("Env: ");
  Serial.println(tn.getEnvironmentTemperature());
}

void onObjectTemperature() {
  Serial.print("Obj: ");
  Serial.println(tn.getObjectTemperature());
}

void setup() {
  Serial.begin(115200);

  // clk must be on a interrupt pin for interrupt support
  tn.init(/* data */ 11, /* clk */ 2, /* ack */ 13);

  // these functions are called on conversion done
  tn.attachEnvironmentInterrupt(onEnvironmentTemperature);
  tn.attachObjectInterrupt(onObjectTemperature);

  if (tn.startConversion(tn_isr))
    Serial.println("registered interrupt");
  else
    Serial.println("interrupt not supported on clk pin.");
}

void loop() {
  // do your useful stuff...
  Serial.println("Blah");
  delay(3000);
  
  // unregister the conversion done callback
  //tn.attachEnvironmentInterrupt(NULL);
  //tn.attachObjectInterrupt(NULL);
  
  // stop the conversion if you don't need it anymore
  //tn.endConversion();
}