
#include <WiFi.h>
#include <AutobahnWS.h>


char ssid[] = "";
char pass[] = "";
WebSocketsClient client1;
AutobahnWS abc;


void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
   abc.begin(client1,"192.168.0.28", 8080, "realm1", "/ws", "wamp.2.json" );
   while ( ! abc.is_connected())
   {
     abc.loop();
     Serial.printf(".\n");
     delay(500);
   }
  Serial.printf("*");
}

void loop() {
  if(!abc.is_connected()){
   while ( ! abc.is_connected())   {
     abc.loop();
     Serial.printf(".\n");
     delay(500);
   }
  }
  abc.publish("com.myapp.topic1","test",3,2,1,"test");
  abc.loop();
  delay(250); 

}
