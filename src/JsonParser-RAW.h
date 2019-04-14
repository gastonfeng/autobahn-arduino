#ifndef JSONPARSERRAW_H
#define JSONPARSERRAW_H
#include "Parser.h"
#include "WAMP.h"
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

class JsonParserRaw:public Parser {
public:	
	const char * get_hello(const char * topic);
	const char * get_publish(int reqId,const char * topic,const char *payload);
    const char * get_subscribe(int reqId,const char * topic);

    int getsubscriptionID(const uint8_t * data);
	int getsubscriberequestID(const uint8_t * data);
	int getsessionID(const char * data);
    int getrequestID(const uint8_t * data);
    MessageCodes getMessageType(const uint8_t *  str);
    const char *geteventpayload(const uint8_t * payload);
    int  getregistrationID(const uint8_t * data);
    int  getregisterID(const uint8_t * data);
    const char * get_rpc_register(int reqId,const char * procedure);
    std::string  getsubscriptionID_str(const uint8_t * data);
    std::string  getsubscriberequestID_str(const uint8_t * data);
    const char * get_result(std::string reqId,const char * payload);
    const char * get_result(int reqId,const char * payload);    
    std::string   getregistrationID_str(const uint8_t * data);
    std::string getregisterID_str(const uint8_t * data);
    const char * get_rpccall(int reqId,const char * topic, char *payload);
    //std::string  geteventpayload_str(const uint8_t * payload);
    const char * get_unsubscribe(int reqId,std::string subid);	
    const char * geteventpayload_rpc(const uint8_t * payload);

private:
//	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& GenerateRolesObject(JsonBuffer& buffer);
	char buffer[512];
};

JsonObject& JsonParserRaw::GenerateRolesObject(JsonBuffer& buffer)
{
    DynamicJsonBuffer jsonBuffer;	
    JsonObject& o = buffer.createObject();
    JsonObject& roles = buffer.createObject();
    roles["publisher"] = buffer.createObject();
    roles["subscriber"] = buffer.createObject();
	roles["caller"] = buffer.createObject();
    roles["callee"] = buffer.createObject();
    o["roles"] = roles;

    return o;
}

const char * JsonParserRaw::get_hello(const char * realm){
    DynamicJsonBuffer jsonBuffer;	
    JsonArray& root = jsonBuffer.createArray();
    root.add((int)WAMP_MSGCODE_HELLO);
    root.add(realm);
    root.add(GenerateRolesObject(jsonBuffer));
	root.printTo(buffer+4, sizeof(buffer));
	buffer[0] = 0x00;
    buffer[1] = 0x00;
    buffer[2] = 0x00;
    buffer[3] = strlen(buffer+4);
	return buffer;
}
const char * JsonParserRaw::get_result(int reqId,const char * payload){
	
    DynamicJsonBuffer jsonBuffer;		   
    JsonArray& root = jsonBuffer.createArray();
    JsonObject& r = jsonBuffer.createObject();
 	JsonArray& v = jsonBuffer.createArray();
   	root.add((int)WAMP_MSGCODE_YIELD);
    root.add(reqId);
    root.add(r);
    v.add(payload);
    root.add(v);
	root.printTo(buffer, sizeof(buffer));
	return buffer;

}
const char * JsonParserRaw::get_result(std::string reqId,const char * payload){
    DynamicJsonBuffer jsonBuffer;	
    JsonArray& root = jsonBuffer.createArray();
    JsonObject& r = jsonBuffer.createObject();
 	JsonArray& v = jsonBuffer.createArray();
   	root.add((int)WAMP_MSGCODE_YIELD);
    root.add(atoll(reqId.c_str()));
    root.add(r);
    v.add(payload);
    root.add(v);
	root.printTo(buffer, sizeof(buffer));
	return buffer;

}

const char * JsonParserRaw::get_publish(int reqId,const char * topic,const char *payload){
    DynamicJsonBuffer jsonBuffer;
    JsonArray& root = jsonBuffer.createArray();
    JsonObject& r = jsonBuffer.createObject();
	JsonArray& v = jsonBuffer.createArray();
	
	v.add(payload);
    root.add((int)WAMP_MSGCODE_PUBLISH);
    root.add(reqId);
    root.add(r);
    root.add(topic);
    if (payload != NULL){
        root.add(v);
    }
   	root.printTo(buffer, sizeof(buffer));
   	jsonBuffer.clear();
	return buffer;
}
const char * JsonParserRaw::get_subscribe(int reqId,const char * topic){
    DynamicJsonBuffer jsonBuffer;
    JsonArray& root = jsonBuffer.createArray();
    JsonObject& r = jsonBuffer.createObject();
    root.add((int)WAMP_MSGCODE_SUBSCRIBE);
    root.add(reqId);
    root.add(r);
    root.add(topic);
    root.printTo(buffer, sizeof(buffer));
   	jsonBuffer.clear();
	return buffer;
}
const char * JsonParserRaw::get_unsubscribe(int reqId,std::string subid){	
    sprintf(buffer, "[%d, %d, %s ]",WAMP_MSGCODE_UNSUBSCRIBE, reqId, subid.c_str());
    return buffer;
}
const char * JsonParserRaw::get_rpc_register(int reqId,const char * procedure){

    DynamicJsonBuffer jsonBuffer;	
    JsonArray& root = jsonBuffer.createArray();
	JsonObject& r = jsonBuffer.createObject();
    root.add((int)WAMP_MSGCODE_REGISTER );
    root.add(reqId);
    root.add(r);
    root.add(procedure);
    root.printTo(buffer, sizeof(buffer));
   	jsonBuffer.clear();    
	return buffer;
}

const char * JsonParserRaw::get_rpccall(int reqId,const char * topic, char *payload){
    DynamicJsonBuffer jsonBuffer;
    JsonArray& root = jsonBuffer.createArray();
    JsonObject& r = jsonBuffer.createObject();
	JsonArray& v = jsonBuffer.createArray();
	if(payload != NULL)
	    v.add(payload);
    root.add((int)WAMP_MSGCODE_CALL);
    root.add(reqId);
    root.add(r);
    root.add(topic);
    if (payload != NULL){
        root.add(v);
    }
   	root.printTo(buffer, sizeof(buffer));
   	jsonBuffer.clear();   	
	return buffer;
}
int  JsonParserRaw::getsessionID(const char * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	int id = ((int)(root[0]));
   	jsonBuffer.clear();	
	return id;
}
int  JsonParserRaw::getrequestID(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	int id = ((int)(root[2]));
   	jsonBuffer.clear();	
	return id;	
}
int  JsonParserRaw::getsubscriberequestID(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	int id = ((int)(root[1]));
   	jsonBuffer.clear();	
	return id;	
}
std::string  JsonParserRaw::getsubscriberequestID_str(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	return std::string(root[1].as<char*>());	
}

std::string   JsonParserRaw::getsubscriptionID_str(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	 return std::string(root[2].as<char*>());		
}

int  JsonParserRaw::getsubscriptionID(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	int id = ((int)(root[2]));
   	jsonBuffer.clear();	
	return id;	
}
int  JsonParserRaw::getregisterID(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	int id = ((int)(root[1]));
   	jsonBuffer.clear();	
	return id;	
}
std::string  JsonParserRaw::getregisterID_str(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	return std::string(root[1].as<char*>());	
}

int  JsonParserRaw::getregistrationID(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	int id = ((int)(root[2]));
   	jsonBuffer.clear();	
	return id;	
}
std::string   JsonParserRaw::getregistrationID_str(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	return std::string(root[2].as<char*>());	

}
MessageCodes  JsonParserRaw::getMessageType(const uint8_t *  str)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(str);
	if (!root.success())
    {
        return WAMP_MSGCODE_ERROR;
	}	
	MessageCodes command = static_cast<MessageCodes>((int)(root[0]));
   	jsonBuffer.clear();	
	return command;
}
#if 0
std::string JsonParserRaw::geteventpayload_str(const uint8_t * payload){
	
	JsonArray& root = jsonBuffer.parseArray(payload); 	
	JsonArray& now = root[4];
	std::string value = now.get<std::string>(0); // returns NULL
	return value;
}
#endif
const char * JsonParserRaw::geteventpayload(const uint8_t * payload){
	
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(payload); 	
	JsonArray& now = root[4];
	const char* value = now.get<const char*>(0); // returns NULL
   	jsonBuffer.clear();	
	return value;
}
const char * JsonParserRaw::geteventpayload_rpc(const uint8_t * payload){
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(payload); 	
	JsonArray& now = root[3];
	const char* value = now.get<const char*>(0); // returns NULL
   	jsonBuffer.clear();	
	return value;
}
#endif
