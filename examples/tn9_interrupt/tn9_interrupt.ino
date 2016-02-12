#define TN901_DEBUG
#include <TN901.h>

TN901 tn;

// isr func for attachInterrupt(), supports multiple TN901s on different pin
void tn_isr() {
  tn.processIsr();
}

void setup() {
  Serial.begin(115200);

  // clk must be on a interrupt pin for interrupt support
  tn.init(/* data */ 11, /* clk */ 2, /* ack */ 13);

  if (tn.startConversion(tn_isr))
    Serial.println("registered interrupt");
  else
    Serial.println("interrupt not supported on clk pin.");
}

void loop() {
  // the conversion is done continuesly
  if (tn.isEnvironmentTemperatureUpdated()) {
    Serial.print("Env: ");
    // reading it reset the updated flag
    Serial.println(tn.getEnvironmentTemperature());
  }

  if (tn.isObjectTemperatureUpdated()) {
    Serial.print("Obj: ");
    Serial.println(tn.getObjectTemperature());
  }
  
  // stop the conversion if you don't need it anymore
  //tn.endConversion();
}