#pragma once
//#define ARDUINOJSON_USE_LONG_LONG 1 // we need to store long long

#include <WiFiClientSecure.h>

#include <WebSocketsClient.h>
#include <stdlib.h>
#include <map>
#include <ArduinoJson.h>
#include <Ticker.h>


enum MessageCodes {
    WAMP_MSGCODE_HELLO        = 1,
    WAMP_MSGCODE_WELCOME      = 2,
    WAMP_MSGCODE_ABORT        = 3,
    WAMP_MSGCODE_GOODBYE      = 6,

    WAMP_MSGCODE_ERROR        = 8,

    WAMP_MSGCODE_PUBLISH      = 16,
    WAMP_MSGCODE_PUBLISHED    = 17,

    WAMP_MSGCODE_SUBSCRIBE    = 32,
    WAMP_MSGCODE_SUBSCRIBED   = 33,
    WAMP_MSGCODE_UNSUBSCRIBE  = 34,
    WAMP_MSGCODE_UNSUBSCRIBED = 35,
    WAMP_MSGCODE_EVENT        = 36,

    WAMP_MSGCODE_CALL         = 48,
    WAMP_MSGCODE_RESULT       = 50,

    WAMP_MSGCODE_REGISTER     = 64,
    WAMP_MSGCODE_REGISTERED   = 65,
    WAMP_MSGCODE_UNREGISTER   = 66,
    WAMP_MSGCODE_UNREGISTERED = 67,
    WAMP_MSGCODE_INVOCATION   = 68,
    WAMP_MSGCODE_YIELD        = 70
};



class WAMPParam
{
public:
 
 int asInt() const { return atoi(buff); }
 const char* asString() const { return buff; }
 int storeData(int value) {
	 char str[2 + 8 * sizeof(value)];
     itoa(value, str, 10);
     storeData(str); 
	 }
 int storeData(float value) { 
    char str[33];
    dtostrf(value, 5, 3, str);
    storeData(str);
 
 }
 int storeData(long value) { }
 int storeData(double value) { }
 int storeData(char* str) { strcpy(buff,str);}
public:
    char    buff[500];
    size_t   len;
    size_t   buff_size;

};

typedef std::function<void(int,char * )> ErrorCallback;
typedef std::function<void(WAMPParam *)> TSubscribeParamCallback;
typedef std::function<WAMPParam *(WAMPParam *)> TSubscribeWAMPParamCallback;
typedef std::function<void(void *)> TSubscribeCallback;
typedef std::map<std::string, std::pair<std::string, TSubscribeWAMPParamCallback> > RPCMap;

typedef std::map<int, std::string > PubTable;
typedef std::map<int, std::pair<std::string, TSubscribeParamCallback> > SubTable;
typedef std::map<int, std::pair<std::string, TSubscribeWAMPParamCallback> > RPCTable;



typedef std::map<std::string, std::pair<std::string, TSubscribeParamCallback> > SubscribedMap;
 
typedef std::map<int, int > IntermediateMap;


class Autobahn :public WebSocketsClient {

public:
		char * realm;
		MessageCodes _wampstate;
		bool _connected = false;
		bool _rpcresult = false;
		Ticker tickerObject; 

		SubscribedMap SubscribeTable;
        IntermediateMap TempTable;
		RPCMap rpcTable;
		RPCTable rpctab;
		PubTable pubtable;
		SubTable subtable;
		
        void startSSL(const char *host, uint16_t port,   char * realm, const char * url = "/",const char * protocol = "wamp.2.json");
		void start(const char *host, uint16_t port,   char * realm, const char * url = "/",const char * protocol = "wamp.2.json");
 		void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
		void wampstatemachine(uint8_t * payload, size_t length);
		void onError(ErrorCallback errorCallback);
		JsonObject& GenerateRolesObject(JsonBuffer& buffer);
		void storeResult(const char * payload,size_t length);

		void  sendHello(const char * realm);
		void  sendMessage(const char * payload);
		void  sendJson(JsonArray& root);

		void publish(std::string& topic, int);
		void publish(const char * topic, int);
		void publish(const char * topic, double);
		void publish(const char * topic ,const char * message);
		void publish_towire(JsonObject& options, const char * topic, JsonArray * args);
		void publish_towire( const char * topic,  const char *  args);

		void subscribe(const char * topic, TSubscribeCallback); 
		void subscribe(const char * topic, TSubscribeParamCallback);
		void unsubscribe(const char * topic); 
		void subscribe_towire(const char * topic, TSubscribeParamCallback); 

		
		void onPublished(uint8_t * payload, size_t length);
		void onSubscribed(uint8_t * payload, size_t length);
		void onInvocation(uint8_t * payload, size_t length);
		void  return_call(char * message);
		void  return_call(int val);

		void rpc_register(const char * topic,TSubscribeWAMPParamCallback); 
		void register_towire(const char * procedure);
		void onRegistered(uint8_t * payload, size_t length);
		void  onAbort(uint8_t * payload, size_t length);
		void  onErrorMsg(uint8_t * payload, size_t length);
		void  notifyUser(int errorcode, char * );
		bool  is_subscribed(const char * topic);
		bool  is_rpc_register(const char * topic);

		void handleWAMPEvent(uint8_t * payload, size_t length);
		WAMPParam * rpc_call(const char * topic,const char *fmt); 

		WAMPParam rx_buffer;
		void TriggerTimer();

  
	    int temp_Subscription_ID;
	    int temp_RequestID;
		unsigned int temp_SUBRequestID;
  	    int temp_registerID;
        const char * temp_topic;
        TSubscribeParamCallback _tempCallback;
		TSubscribeWAMPParamCallback rpc_callback;

		int GenerateRequestId();
		 
		MessageCodes  getMessageType(const uint8_t *  str);
		
		
		int  getRequestID(const char * str);
		int  getsessionID(const char * str);
		int sessionId;
		JsonObject *publish_options;
		bool isConnected();
		StaticJsonBuffer<600> jsonBuffer;
		
		ErrorCallback _errorcb;
};

 
void Autobahn::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
		//_websocketstate = type;
		_connected = false;
        //Serial.printf("[WSc] Disconnected!\n");
        break;
        case WStype_CONNECTED:
        {
 	      this->_wampstate = WAMP_MSGCODE_HELLO;	
		//_websocketstate = type;
		//	Serial.printf("[WSc] Connected to url:");
			this->wampstatemachine(payload,length);
        //    Serial.printf("[WSc] Connected to url: %s\n",  payload,length);
        }
        break;
        case WStype_TEXT:
		//Serial.printf("WStype_TEXT Connected to url: %s  %d\n",  payload);

		 _wampstate = getMessageType(payload);
	     wampstatemachine(payload,length);
        break;
        case WStype_BIN:
        //Serial.printf("[WSc] get binary length: %u\n", length);
        break;
    }

}

bool Autobahn::isConnected()
{
	return _connected;
}

int Autobahn::GenerateRequestId()
{
    return rand();
}

void Autobahn::publish(const char * topic, double a){
  publish_towire(topic,String(a,3).c_str());
}

void Autobahn::publish(const char * topic, int a){
  // publish_towire(topic,std::to_string(a).c_str());
 publish_towire(topic,String(a, DEC).c_str());
}

void Autobahn::publish(std::string& topic, int a){
  publish_towire(topic.c_str(),String(a, DEC).c_str());
}

void Autobahn::publish(const char * topic ,const char * message){
  publish_towire(topic,message);
}

void Autobahn::publish_towire( const char * topic,  const char *  args)
{
    StaticJsonBuffer<200> jsonBuffer;
	char b[512]; // TODO: This may need to be increased ?
    int reqId = GenerateRequestId();

	pubtable.insert(std::make_pair(reqId, std::string(topic)));
    JsonArray& root = jsonBuffer.createArray();
	//JsonArray& r = jsonBuffer.createArray();
    JsonObject& r = jsonBuffer.createObject();
	JsonArray& v = jsonBuffer.createArray();
	
	v.add(args);
    root.add((int)WAMP_MSGCODE_PUBLISH);
    root.add(reqId);
    root.add(r);
    root.add(topic);
    if (args != NULL)
    {
        root.add(v);
    }
    root.printTo(b, sizeof(b));
    this->_wampstate = WAMP_MSGCODE_PUBLISH;
	wampstatemachine((uint8_t *)b,512);
}


void Autobahn::publish_towire(JsonObject& options, const char * topic, JsonArray * args)
{
    StaticJsonBuffer<200> jsonBuffer;
	char b[512]; // TODO: This may need to be increased ?
	
    JsonArray& root = jsonBuffer.createArray();
    root.add((int)WAMP_MSGCODE_PUBLISH);
    root.add(GenerateRequestId());
   // root.add(options);
    root.add(topic);
    if (args != NULL)
    {
        root.add(*args);
    }

	 
    root.printTo(b, sizeof(b));
    this->_wampstate = WAMP_MSGCODE_PUBLISH;
	wampstatemachine((uint8_t *)b,512);
}

bool Autobahn::is_rpc_register(const char * topic)
{
	RPCMap::iterator it;
	uint64_t subid = 0;
	
	  for (it =  rpcTable.begin(); it != rpcTable.end(); it++)
       {

           std::pair<std::string, TSubscribeWAMPParamCallback> val =  it->second;
		   //Serial.printf("SUB SUB %s %s %s\n",topic,(val.first).c_str(),it->first );	
		   
		   if( strcmp( topic,(val.first).c_str() ) == 0)
			   return true;

       }
	return false;

}

bool Autobahn::is_subscribed(const char * topic)
{
	SubscribedMap::iterator it;
	uint64_t subid = 0;
	
	  for (it =  SubscribeTable.begin(); it != SubscribeTable.end(); it++)
       {

           std::pair<std::string, TSubscribeParamCallback> val =  it->second;
		   // Serial.printf("SUB SUB %s %s %s\n",topic,(val.first).c_str(),it->first );	
		   
		   if( strcmp( topic,(val.first).c_str() ) == 0)
			   return true;

       }
	return false;	   
}


void Autobahn::subscribe_towire(const char * topic,TSubscribeParamCallback sendCallback)
{
    int reqId = GenerateRequestId();
	this->temp_SUBRequestID = reqId; 
	
	subtable.insert(std::make_pair(reqId,std::make_pair( std::string(topic),sendCallback)));
	Serial.printf("Req ID = %d \n", this->temp_SUBRequestID);
    StaticJsonBuffer<200> b;
	char buffer[512]; // TODO: This may need to be increased ?
    JsonArray& r = b.createArray();
	JsonObject& t = b.createObject();

    r.add((int)WAMP_MSGCODE_SUBSCRIBE);
    r.add(reqId);
    r.add(t);
    r.add(topic);
    this->_wampstate = WAMP_MSGCODE_SUBSCRIBE;
    r.printTo(buffer, sizeof(buffer));
	//TempTable[reqId] = 0;
	wampstatemachine((uint8_t *)buffer,512);
}

void Autobahn::subscribe(const char * topic,TSubscribeParamCallback sendCallback){
	
	//smap.insert( std::make_pair(topic, sendCallback));  
	this->temp_topic = topic;
	//this->temp_RequestID;
	this->_tempCallback =  sendCallback;
	subscribe_towire(topic,sendCallback);
}


void Autobahn::subscribe(const char * topic,TSubscribeCallback sendCallback){

}

void Autobahn::unsubscribe(const char * topic){
	

	SubscribedMap::iterator it;
	std::string substring;
	
	  for (it =  SubscribeTable.begin(); it != SubscribeTable.end(); it++)
       {
           std::pair<std::string, TSubscribeParamCallback> val =  it->second;
		   if(strcmp( topic, (val.first).c_str() ) == 0)
		   {
			    substring = std::string(it->first);
				SubscribeTable.erase(it);
				break;
		   }
		  // Serial.printf("SUB SUB %s %s \n",topic,(val.first).c_str() );		   
       }

	int reqId = GenerateRequestId();
	this->temp_RequestID = reqId; 
	
	char buffer[512]; // TODO: This may need to be increased ?


	this->_wampstate = WAMP_MSGCODE_UNSUBSCRIBE;  
	sprintf(buffer, "[%d, %d, %s ]",WAMP_MSGCODE_UNSUBSCRIBE, reqId, substring.c_str());
	//Serial.printf(" UNSUBSCRIBE %s  \n",buffer);		   

	wampstatemachine((uint8_t *)buffer,512);
 
}
void Autobahn::onAbort(uint8_t * payload, size_t length)
{
	notifyUser(WAMP_MSGCODE_ABORT,(char *) payload);
}

void Autobahn::onErrorMsg(uint8_t * payload, size_t length)
{
	notifyUser( WAMP_MSGCODE_ERROR, (char *)  payload);
}



void Autobahn::notifyUser(int errorcode,char * message )
{
	this->_errorcb(errorcode,message);

}

 void  Autobahn::onSubscribed(uint8_t * payload, size_t length)
 {
	 // Serial.printf("onSubscribed  %s  %d \n", payload,length );

    DynamicJsonBuffer jsonBuffer;
	JsonArray& root = jsonBuffer.parseArray(payload);
	SubTable::iterator it;
	 long var = root[2];
	 uint64_t var1 = 0;
	 var1 = strtoull(root[2].as<char*>(), NULL , 10);
	//Serial.printf("onSubscribed  val %s  \n", root[2].as<char*>() );

	if (!root.success())
    {
        // Ignore this message ?
        return ;
	}	

	it = subtable.find(root[1]);
	this->SubscribeTable.insert( std::make_pair(std::string(root[2].as<char*>()), it->second));
	subtable.erase(it);

	return ;
	
 }
 
 void  Autobahn::onPublished(uint8_t * payload, size_t length)
 {
	DynamicJsonBuffer jsonBuffer;
	JsonArray& root = jsonBuffer.parseArray(payload);
	PubTable::iterator it;
	int var = root[2];
	it = pubtable.find(var);
	pubtable.erase (it); 
	 
 }
 
 
void  Autobahn::return_call(char * message)
{ 
	 
    StaticJsonBuffer<200> b;
	char buffer[512]; // TODO: This may need to be increased ?
    JsonArray& r = b.createArray();
	JsonObject& empty = b.createObject();
	
	JsonArray& data = b.createArray();
    r.add((int)WAMP_MSGCODE_YIELD);
    r.add(this->temp_RequestID);
    r.add(empty);
    data.add(message);
	r.add(data);
    this->_wampstate = WAMP_MSGCODE_YIELD;
    r.printTo(buffer, sizeof(buffer));
	sendMessage(buffer);
}
 
 
 void  Autobahn::return_call(int length)
{
   
	 
    StaticJsonBuffer<200> b;
	char buffer[512]; // TODO: This may need to be increased ?
    JsonArray& r = b.createArray();
    r.add((int)WAMP_MSGCODE_YIELD);
    r.add(this->temp_RequestID);
    r.add(this->publish_options);
    r.add(length);
    this->_wampstate = WAMP_MSGCODE_YIELD;
    r.printTo(buffer, sizeof(buffer));
	sendMessage(buffer);
}
 
void Autobahn::rpc_register(const char * topic, TSubscribeWAMPParamCallback sendCallback)
{
	this->rpc_callback = sendCallback;
	this->temp_topic = topic;
	register_towire(topic);
}

void Autobahn::register_towire(const char * procedure)
{
    int reqId = GenerateRequestId();
	char buffer[512]; // TODO: This may need to be increased ?
	rpctab.insert( std::make_pair(reqId, make_pair(std::string(this->temp_topic),this->rpc_callback)));
    StaticJsonBuffer<200> b;
    JsonArray& r = b.createArray();
	JsonObject& empty = b.createObject();
    r.add((int)WAMP_MSGCODE_REGISTER );
    r.add(reqId);
    r.add(empty);
    r.add(procedure);
	r.printTo(buffer, sizeof(buffer));
    this->_wampstate = WAMP_MSGCODE_REGISTER;
	this->temp_RequestID  = reqId;
	wampstatemachine((uint8_t *)buffer,512);	
}
 
 


MessageCodes  Autobahn::getMessageType(const uint8_t *  str)
{
    DynamicJsonBuffer jsonBuffer;
	JsonArray& root = jsonBuffer.parseArray(str);
	if (!root.success())
    {
        // Ignore this message ?
        return WAMP_MSGCODE_ERROR;
	}	
	MessageCodes command = static_cast<MessageCodes>((int)(root[0]));
	return command;
}

void Autobahn::TriggerTimer()
{
	
}



WAMPParam * Autobahn::rpc_call(const char * topic,const char *param)
{
 	int reqId = GenerateRequestId();
	char buffer[512]; // TODO: This may need to be increased ?
    StaticJsonBuffer<200> b;
    JsonArray& r = b.createArray();
	JsonObject& t = b.createObject();
	JsonArray& v = b.createArray();
	
	//v.add(args);
    r.add((int)WAMP_MSGCODE_CALL );
    r.add(reqId);	
	r.add(t);
	r.add(topic);
	if(strlen(param) != 0)
		v.add(param);
	r.add(v);
	
	r.printTo(buffer, sizeof(buffer));
    this->_wampstate = WAMP_MSGCODE_CALL;
	this->temp_RequestID  = reqId;
	//tickerObject.attach_ms(100, std::bind(&Autobahn::TriggerTimer));
	wampstatemachine((uint8_t *)buffer,512);	
	while( _rpcresult != true)
	{
		this->loop();
		
	}
	return &rx_buffer;
	
}

void  Autobahn::handleWAMPEvent(uint8_t * payload, size_t length)
{
    DynamicJsonBuffer jsonBuffer;
	JsonArray& root = jsonBuffer.parseArray(payload); 
	
	JsonArray& now = root[4];
	const char* value2 = now.get<const char*>(0); // returns NULL

  	this->rx_buffer.storeData(const_cast<char*>(value2));
	std::pair<std::string, TSubscribeParamCallback> val = SubscribeTable[ std::string(root[1].as<char*>())];
  	val.second((WAMPParam *) &this->rx_buffer);

}

void Autobahn::onError(ErrorCallback errorCallback)
{
	this->_errorcb = errorCallback;
}

void Autobahn::startSSL(const char *host, uint16_t port,   char * realm1, const char * url, const char * protocol) {
    using namespace std::placeholders; // for `_1`

	this->realm = realm1;
	this->onEvent( std::bind(&Autobahn::webSocketEvent , this, _1,_2,_3));
	this->setReconnectInterval(5000);
	this->beginSSL(host, port, url, protocol);	
}

void Autobahn::start(const char *host, uint16_t port,   char * realm1, const char * url, const char * protocol) {
    using namespace std::placeholders; // for `_1`

	this->realm = realm1;
	this->onEvent( std::bind(&Autobahn::webSocketEvent , this, _1,_2,_3));
	this->setReconnectInterval(5000);
	this->begin(host, port, url, protocol);	
}

  
JsonObject& Autobahn::GenerateRolesObject(JsonBuffer& buffer)
{
    JsonObject& o = buffer.createObject();
    JsonObject& roles = buffer.createObject();
    roles["publisher"] = buffer.createObject();
    roles["subscriber"] = buffer.createObject();
	roles["caller"] = buffer.createObject();
    roles["callee"] = buffer.createObject();
    o["roles"] = roles;

    return o;
}

  
void Autobahn::storeResult(const char * payload,size_t length)
{
    // Serial.printf("[WSc] get text: %s\n", payload);
 	DynamicJsonBuffer jsonBuffer;    
 	JsonArray& root = jsonBuffer.parseArray(payload); 
	JsonArray& now = root[3];	
	const char* value  = now.get<const char*>(0); // returns NULL
  	this->rx_buffer.storeData(const_cast<char*>(value));	
  	
}  
  
void Autobahn::sendMessage(const char * payload)
{
    //Serial.printf("[WSc] send text: %s\n", payload);
    this->sendTXT(payload);
}  

void Autobahn::sendJson(JsonArray& root)
{
	char buffer[512];
	root.printTo(buffer, sizeof(buffer));
	sendMessage(buffer);
}
void Autobahn::sendHello(const char * realm)
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonArray& root = jsonBuffer.createArray();
    root.add((int)WAMP_MSGCODE_HELLO);
    root.add(realm);
    root.add(GenerateRolesObject(jsonBuffer));
	//root.add(wamp_c_option);
	sendJson(root);
 
}
 
 
 
 
int  Autobahn::getsessionID(const char * str)
{
    DynamicJsonBuffer jsonBuffer;
	JsonArray& root = jsonBuffer.parseArray(str);
	if (!root.success())
    {
        // Ignore this message ?
        return WAMP_MSGCODE_ERROR;
	}	
	int id = ((int)(root[0]));
	return id;
}

 
int  Autobahn::getRequestID(const char * str)
{
    DynamicJsonBuffer jsonBuffer;
	JsonArray& root = jsonBuffer.parseArray(str);
	if (!root.success())
    {
        // Ignore this message ?
        return WAMP_MSGCODE_ERROR;
	}	
	int id = ((int)(root[1]));
	return id;
}
 
void  Autobahn::onRegistered(uint8_t * payload, size_t length)
{
    DynamicJsonBuffer jsonBuffer;
	JsonArray& root = jsonBuffer.parseArray(payload);
	RPCTable::iterator it;

	if (!root.success())
    {
        // Ignore this message ?
        return ;
	}	
	it = rpctab.find(root[1]);
	this->rpcTable.insert( std::make_pair(std::string(root[2].as<char*>()), it->second)); 
	rpctab.erase(it);

	return ;
	
}
 
 void  Autobahn::onInvocation(uint8_t * payload, size_t length)
{
	
    DynamicJsonBuffer jsonBuffer;
	JsonArray& root = jsonBuffer.parseArray(payload); 
	JsonArray& now = root[4];	 
	const char* value2 = now.get<const char*>(0); // returns NULL
  	this->rx_buffer.storeData(const_cast<char*>(value2));	
	WAMPParam * tmpwamp;
	std::pair<std::string, TSubscribeWAMPParamCallback > val = rpcTable[ std::string(root[2].as<char*>()) ];
	tmpwamp = val.second((WAMPParam *) &this->rx_buffer);	
	return_call(this->rx_buffer.buff);
}



 void  Autobahn::wampstatemachine(uint8_t * payload, size_t length)
 {
	 switch(this->_wampstate)
	 {
		case WAMP_MSGCODE_HELLO:
		 this->sendHello(this->realm);
		break;
		case WAMP_MSGCODE_WELCOME:
		this->sessionId = getsessionID(( char *)payload);
		//Serial.printf("%d\n",this->sessionId);
  	    _connected = true;
		//Serial.printf("WAMP_MSGCODE_WELCOME\n");
		break;
		case WAMP_MSGCODE_ABORT:
		_connected = false;  
		//Serial.printf("WAMP_MSGCODE_ABORT\n");	
		onAbort(payload,length);		
		break;
		case WAMP_MSGCODE_GOODBYE: 
		//Serial.printf("WAMP_MSGCODE_GOODBYE\n");
		_connected = false;
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
		//Serial.printf("WAMP_MSGCODE_UNSUBSCRIBED %s\n",payload);	
		break;
		case WAMP_MSGCODE_EVENT:
		handleWAMPEvent(payload, length);
		break;
		case WAMP_MSGCODE_CALL:
		sendMessage((char *)payload);
		break;
		case WAMP_MSGCODE_RESULT:
		//Serial.printf("WAMP_MSGCODE_RESULT\n");		
		storeResult((char *)payload, length);
		//Serial.printf("WAMP_MSGCODE_RESULT\n");
        _rpcresult = true;
		break;
		case WAMP_MSGCODE_REGISTER:
		//Serial.printf("WAMP_MSGCODE_REGISTER\n");
		sendMessage((char *)payload);
		break;
		case WAMP_MSGCODE_REGISTERED:
		//Serial.printf("WAMP_MSGCODE_REGISTERED\n");
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
