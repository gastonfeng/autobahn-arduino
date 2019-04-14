
#include <WiFi.h>
#include <AutobahnWS.h>


char ssid[] = "";
char pass[] = "";
WebSocketsClient client1;
AutobahnWS abc;

wamp_param * rpcproc_date(wamp_param *arg){
  Serial.printf("\ntopic 2 %s\n",arg->asString());
  arg->reset();
  arg->add("date");
  arg->add("1");

  return arg;
}
void onjoin(){
   Serial.printf("onjoin.\n");
   abc.rpc_register("my.com.date",rpcproc_date);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  abc.on_join(onjoin);
  abc.begin(client1,"192.168.0.28", 8080, "realm1", "/ws", "wamp.2.json" );
  while ( ! abc.is_connected()){
    abc.loop();
    Serial.printf(".\n");
    delay(500);
  }
  Serial.printf("*");
}

void loop() {
  if(!abc.is_connected()){
    
   while ( ! abc.is_connected()){
     abc.loop();
     Serial.printf(".\n");
     delay(500);
   }
  }
  abc.loop();
  delay(250);
}
