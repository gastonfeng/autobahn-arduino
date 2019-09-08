#ifndef AUTOBAHNWS_H
#define AUTOBAHNWS_H
#include <Arduino.h>
#include <EthernetClient.h>
#include "AutobahnProtoImpl.h"
#include "AutobahnTCP.h"

#define PARSER JsonParser
class AutobahnWS: public AutobahnProtoImpl{
public:	
    AutobahnWS(){};
    ~AutobahnWS(){};
	void begin(EthernetClient &client,const char *host, uint16_t port,   char * realm, const char * url = "/",const char * protocol = "wamp.2.json");
	void beginssl(EthernetClient &client,const char *host, uint16_t port,   char * realm, const char * url = "/",const char * protocol = "wamp.2.json");
	void webSocketEvent(WStype_t type, uint8_t * payload, int length);
	virtual void send_towire(const char * payload);
    virtual void notify_welcome( );	
    virtual void loop();
    void on_join(t_cb cb);
    
	EthernetClient *_client;
	t_cb onjoin_cb = nullptr;
};

void AutobahnWS::begin(EthernetClient &client,const char *host, uint16_t port,   char * realm, const char * url ,const char * protocol){

    using namespace std::placeholders; // for `_1`
	this->_client = &client;
	this->setrealm(realm);
	// this->_client->onEvent( std::bind(&AutobahnWS::webSocketEvent , this, _1,_2,_3));
	// this->_client->setReconnectInterval(5000);
	// this->_client->begin(host, port, url, protocol);		
}
void AutobahnWS::beginssl(EthernetClient &client,const char *host, uint16_t port,   char * realm, const char * url ,const char * protocol){

    using namespace std::placeholders; // for `_1`
	this->_client = &client;
	this->setrealm(realm);
	// this->_client->onEvent( std::bind(&AutobahnWS::webSocketEvent , this, _1,_2,_3));
	// this->_client->setReconnectInterval(5000);
	// this->_client->beginSSL(host, port, url, protocol);		
}
void AutobahnWS::webSocketEvent(WStype_t type, uint8_t * payload, int length) {

    switch(type) {
        case WStype_DISCONNECTED:
		//_websocketstate = type;
	//	_connected = false;
        core_debug("[WSc] Disconnected!\n");
        break;
        case WStype_CONNECTED:
	      core_debug("[WSc] WStype_CONNECTED!\n");
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
    //core_debug("[send_towire] %s\n",payload);	
    this->_client->write(payload,strlen(payload));
}  
void AutobahnWS::on_join(t_cb cb){
	onjoin_cb = cb;

}  
void AutobahnWS::notify_welcome(){
    core_debug("on_join!\n");
    if( onjoin_cb != nullptr)
		onjoin_cb();
}  

void AutobahnWS::loop(){
	// this->_client->loop();
}

#endif
