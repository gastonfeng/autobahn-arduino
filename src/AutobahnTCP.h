#ifndef AUTOBAHNTCP_H
#define AUTOBAHNTCP_H

#include "AutobahnProtoImpl.h"
  typedef enum {
      WStype_ERROR,
      WStype_DISCONNECTED,
      WStype_CONNECTED,
      WStype_TEXT,
      WStype_BIN,
        WStype_FRAGMENT_TEXT_START,
        WStype_FRAGMENT_BIN_START,
        WStype_FRAGMENT,
        WStype_FRAGMENT_FIN,
  } WStype_t;


#define PARSER JsonParser
class AutobahnTCP: public AutobahnProtoImpl{
public:	
    AutobahnTCP(){};
    ~AutobahnTCP(){};
	void begin(WiFiClient &client,const char *host, uint16_t port,   char * realm, const char * url = "/",const char * protocol = "wamp.2.json");
	void beginssl(WiFiClient &client,const char *host, uint16_t port,   char * realm, const char * url = "/",const char * protocol = "wamp.2.json");
	void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
	virtual void send_towire(const char * payload);
    virtual void loop();
    void  send_header();

	WiFiClient *_client;
};

void AutobahnTCP::begin(WiFiClient &client,const char *host, uint16_t port,   char * realm, const char * url ,const char * protocol){
    Serial.printf("[Soc] begin 1!\n");
	this->_client = &client;
    Serial.printf("[Soc] begin 2!\n");	
	this->setrealm(realm);
    Serial.printf("[Soc] begin 3!\n");	
	if( this->_client->connect(host, port)){
		 this->_session._connected = true;
   	     webSocketEvent(WStype_CONNECTED,NULL,0);
	}
    Serial.printf("[Soc] begin 4!\n");	
}
void AutobahnTCP::beginssl(WiFiClient &client,const char *host, uint16_t port,   char * realm, const char * url ,const char * protocol){
#if 0
    using namespace std::placeholders; // for `_1`
	this->_client = &client;
	this->setrealm(realm);
	this->_client->onEvent( std::bind(&AutobahnWS::webSocketEvent , this, _1,_2,_3));
	this->_client->setReconnectInterval(5000);
	this->_client->beginSSL(host, port, url, protocol);	
#endif

}
void AutobahnTCP::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
   Serial.printf("Event: webSocketEvent %d %d  !\n",type, length);        

    switch(type) {
        case WStype_DISCONNECTED:
		//_websocketstate = type;
	 	this->_session._connected =false;
        Serial.printf("[Soc] Disconnected!\n");
        break;
        case WStype_CONNECTED:
	      Serial.printf("[Soc] WStype_CONNECTED!\n");		  
		  send_header();
		  this->wampstatemachine(WAMP_MSGCODE_HELLO,payload,length);
        break;
        case WStype_TEXT:
	      wampstatemachine(payload,length);
        break;
        case WStype_BIN:
        break;
    }
}
void AutobahnTCP::send_header(){
	uint8_t payload[5] = { 0x7F, 0xf1, 0x00,0x00};
	int len = 0;
   this->_client->write(payload,4);   
    delay(50);

	for(int c = 0; c < 300; c++){
		delay(25);
		len = this->_client->available();
		Serial.printf("[loop] %d!\n",len);
		if(len > 0)
		    break;
	}	
    
    for ( int c = 0; c < len; c++){
	  Serial.printf("[read loop] %x!\n",this->_client->read());
    }

}
void AutobahnTCP::send_towire(const char * payload){
   Serial.printf("[WSc] send_towire %s!\n",payload);
   this->_client->write(payload);   
}  
void AutobahnTCP::loop(){
  if (!this->_client->connected()) {
	  this->_client->stop();
	  webSocketEvent(WStype_DISCONNECTED,NULL,0);
  }
  
 // if (len) {
//	  String line = this->_client->readStringUntil('\r');
//	  webSocketEvent(WStype_TEXT,(uint8_t *)line.c_str(),len);
 // }

}

#endif
