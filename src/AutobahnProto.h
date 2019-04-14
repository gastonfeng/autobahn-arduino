#ifndef AUTOBAHNPROTO_H
#define AUTOBAHNPROTO_H
#include "WAMP.h"

class AutobahnProto{

public:
    void wampstatemachine(MessageCodes _wampstate,uint8_t * payload, size_t length);
    
    
    void  onWelcome(uint8_t * payload, size_t length);
	void  onAbort(uint8_t * payload, size_t length);
    void  onGoodBye(uint8_t * payload, size_t length);
	void  onErrorMsg(uint8_t * payload, size_t length);
    void  onPublished(uint8_t * payload, size_t length);
	void  onSubscribed(uint8_t * payload, size_t length);
    void  onUnsubscribed(uint8_t * payload, size_t length);
    void  onEvent(uint8_t * payload, size_t length);
    void  onResult(const char * payload,size_t length);
    void  onRegistered(uint8_t * payload, size_t length);
    void  onInvocation(uint8_t * payload, size_t length);

    
    // virtual
    void sendHello()
    void notifyUser(int errorcode,char * message );
    void publish_towire( const char * topic,  const char *  args);
    void subscribe_towire(const char * topic,TSubscribeParamCallback sendCallback);
    void unsubscribe_towire(const char * topic);
    WAMPParam * rpc_call_towire(const char * topic,const char *param)
    void register_towire(const char * procedure)
    void return_call(char * message);

private:
    MessageCodes _wampstate;
    bool _connected = false;
    StaticJsonBuffer<400> jsonBuffer;
	
};


void AutobahnProto::wampstatemachine(MessageCodes wampstate,uint8_t * payload, size_t length){
	 switch(wampstate)	 {
		case WAMP_MSGCODE_HELLO:
		 this->sendHello();
		break;
		case WAMP_MSGCODE_WELCOME:
		 this->onWelcome( char *)payload,length);
		break;
		case WAMP_MSGCODE_ABORT:
		onAbort(payload,length);		
		break;
		case WAMP_MSGCODE_GOODBYE: 
		onGoodBye(payload,length);
		break; 
		case WAMP_MSGCODE_ERROR:
		onErrorMsg(payload,length);
        break; 
		case WAMP_MSGCODE_PUBLISH:
		sendMessage((char *)payload);
		break;
		case WAMP_MSGCODE_PUBLISHED:
		onPublished(payload,length);
        break;
		case WAMP_MSGCODE_SUBSCRIBE:
		sendMessage((char *)payload);
		break;
		case WAMP_MSGCODE_SUBSCRIBED:
		onSubscribed(payload,length);
		break;
		case WAMP_MSGCODE_UNSUBSCRIBE:
		sendMessage((char *)payload);		
		break;
		case WAMP_MSGCODE_UNSUBSCRIBED:
		onUnsubscribed(payload,length);
		break;
		case WAMP_MSGCODE_EVENT:
		onEvent(payload, length);
		break;
		case WAMP_MSGCODE_CALL:
		sendMessage((char *)payload);
		break;
		case WAMP_MSGCODE_RESULT:
		onResult((char *)payload, length);
        _rpcresult = true;
		break;
		case WAMP_MSGCODE_REGISTER:
		sendMessage((char *)payload);
		break;
		case WAMP_MSGCODE_REGISTERED:
		onRegistered(payload,length);
		break;
		case WAMP_MSGCODE_UNREGISTER:
		sendMessage((char *)payload);
		break;
		case WAMP_MSGCODE_UNREGISTERED:
		//Serial.printf("WAMP_MSGCODE_UNREGISTERED\n");
		break;
		case WAMP_MSGCODE_INVOCATION:
		this->temp_RequestID  = getRequestID((char *)payload);
		//Serial.printf("WAMP_MSGCODE_INVOCATION\n");
		onInvocation(payload,length);
		break;
		case WAMP_MSGCODE_YIELD:
		sendMessage((char *)payload);
		break;
	 }
	 	
}

#endif
