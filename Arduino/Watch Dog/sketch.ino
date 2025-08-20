#include <Ticker.h>
Ticker secondTick;
int wdcCount = 0;
long rsCount = 1; //every ... sec count for watchdog reset
volatile bool flag = false ;
volatile int i = 1;
void setup() {
  Serial.begin(115200);
  for(int l = 1 ; l <= 8 ; l++){
  pinMode(l, OUTPUT);

  }blob:https://www.facebook.com/2986cea0-0377-4654-8b6c-dbd0e9140a19$0
  secondTick.attach(1, ISRWDC);
}

void loop() {

  flag = false;
  wdcCount = 0; // Reset watchdog
  Serial.print("wdc:");
  Serial.println(wdcCount);

  digitalWrite(i, HIGH);

  while(flag != true){
  }

}
//----------------------------Watchdog---------------------
void ISRWDC(){
  wdcCount++;

  if(wdcCount > rsCount){
    digitalWrite(i, LOW);
    Serial.println("Rebooting!!!");
    Serial.print(i);
    i++;
    flag = true ;

    if(i == 9){
      i=1;
    }

  }
}