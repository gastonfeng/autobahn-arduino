#ifndef AUTOBAHNWS_H
#define AUTOBAHNWS_H
#include <WebSocketsClient.h>
#include "AutobahnProtoImpl.h"


#define PARSER JsonParser
class AutobahnWS: public cautobahnproto_impl{
public:	
    AutobahnWS(){};
    ~AutobahnWS(){};
	void begin(WebSocketsClient &client,const char *host, uint16_t port,   char * realm, const char * url = "/",const char * protocol = "wamp.2.json");
	void beginssl(WebSocketsClient &client,const char *host, uint16_t port,   char * realm, const char * url = "/",const char * protocol = "wamp.2.json");
	void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
	virtual void send_towire(const char * payload);
    virtual void notify_welcome( );	
    virtual void loop();
    void on_join(t_cb cb);
    
	WebSocketsClient *_client;
	t_cb onjoin_cb = nullptr;
};

void AutobahnWS::begin(WebSocketsClient &client,const char *host, uint16_t port,   char * realm, const char * url ,const char * protocol){

    using namespace std::placeholders; // for `_1`
	this->_client = &client;
	this->setrealm(realm);
	this->_client->onEvent( std::bind(&AutobahnWS::webSocketEvent , this, _1,_2,_3));
	this->_client->setReconnectInterval(5000);
	this->_client->begin(host, port, url, protocol);		
}
void AutobahnWS::beginssl(WebSocketsClient &client,const char *host, uint16_t port,   char * realm, const char * url ,const char * protocol){

    using namespace std::placeholders; // for `_1`
	this->_client = &client;
	this->setrealm(realm);
	this->_client->onEvent( std::bind(&AutobahnWS::webSocketEvent , this, _1,_2,_3));
	this->_client->setReconnectInterval(5000);
	this->_client->beginSSL(host, port, url, protocol);		
}
void AutobahnWS::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
		//_websocketstate = type;
	//	_connected = false;
        Serial.printf("[WSc] Disconnected!\n");
        break;
        case WStype_CONNECTED:
	      Serial.printf("[WSc] WStype_CONNECTED!\n");
		  this->wampstatemachine(WAMP_MSGCODE_HELLO,payload,length);
        break;
        case WStype_TEXT:
	      wampstatemachine(payload,length);
        break;
        case WStype_BIN:
        break;
    }

}
void AutobahnWS::send_towire(const char * payload){
    //Serial.printf("[send_towire] %s\n",payload);	
    this->_client->sendTXT(payload);
}  
void AutobahnWS::on_join(t_cb cb){
	onjoin_cb = cb;

}  
void AutobahnWS::notify_welcome(){
    Serial.printf("on_join!\n");
    if( onjoin_cb != nullptr)
		onjoin_cb();
}  

void AutobahnWS::loop(){
	this->_client->loop();
}

#endif
