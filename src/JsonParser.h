#ifndef JSONPARSER_H
#define JSONPARSER_H
#include "Parser.h"
#include "WAMP.h"
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>

class cjsonparser:public Parser {
public:	
	const char * get_hello(const char * topic);
	const char * get_publish(int reqId,const char * topic,const char *payload);
    const char * get_subscribe(int reqId,const char * topic);

    int get_subscription_id(const uint8_t * data);
	int get_subscribe_request_id(const uint8_t * data);
	int getsessionID(const char * data);
    int getrequestID(const uint8_t * data);
    e_message_codes get_message_type(const uint8_t *  str);
    const char *geteventpayload(const uint8_t * payload);
    int  get_registration_id(const uint8_t * data);
    int  get_register_id(const uint8_t * data);
    const char * get_rpc_register(int reqId,const char * procedure);
    std::string  get_subscription_id_str(const uint8_t * data);
    std::string  get_subscribe_request_id_str(const uint8_t * data);
    const char * get_result(std::string reqId,const char * payload);
    const char * get_result(int reqId,const char * payload);    
    std::string   get_registration_id_str(const uint8_t * data);
    std::string get_register_id_str(const uint8_t * data);
    const char * get_rpccall(int reqId,const char * topic, char *payload);
    const char * get_unsubscribe(int reqId,std::string subid);	
    const char * geteventpayload_rpc(const uint8_t * payload);

private:
	JsonObject& GenerateRolesObject(JsonBuffer& buffer);
	char buffer[512];
};

JsonObject& cjsonparser::GenerateRolesObject(JsonBuffer& buffer)
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

const char * cjsonparser::get_hello(const char * realm){
    DynamicJsonBuffer jsonBuffer;	
    JsonArray& root = jsonBuffer.createArray();
    root.add((int)WAMP_MSGCODE_HELLO);
    root.add(realm);
    root.add(GenerateRolesObject(jsonBuffer));
	root.printTo(buffer, sizeof(buffer));
	return buffer;
}
const char * cjsonparser::get_result(int reqId,const char * payload){
	
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
const char * cjsonparser::get_result(std::string reqId,const char * payload){
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

const char * cjsonparser::get_publish(int reqId,const char * topic,const char *payload){
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
const char * cjsonparser::get_subscribe(int reqId,const char * topic){
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
const char * cjsonparser::get_unsubscribe(int reqId,std::string subid){	
    sprintf(buffer, "[%d, %d, %s ]",WAMP_MSGCODE_UNSUBSCRIBE, reqId, subid.c_str());
    return buffer;
}
const char * cjsonparser::get_rpc_register(int reqId,const char * procedure){

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

const char * cjsonparser::get_rpccall(int reqId,const char * topic, char *payload){
    DynamicJsonBuffer jsonBuffer;
    JsonArray& root = jsonBuffer.createArray();
    JsonObject& r = jsonBuffer.createObject();
	JsonArray& v = jsonBuffer.createArray();

	if(strlen(payload)  > 0){
		char delim[] = " ";
		char *ptr = strtok(payload, delim);		
		while(ptr != NULL){
			v.add(ptr);
			ptr = strtok(NULL, delim);
		}
	}

    root.add((int)WAMP_MSGCODE_CALL);
    root.add(reqId);
    root.add(r);
    root.add(topic);
	if(strlen(payload)  > 0){
        root.add(v);
    }
   	root.printTo(buffer, sizeof(buffer));
   	jsonBuffer.clear();   	
	return buffer;
}
int  cjsonparser::getsessionID(const char * data)
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
int  cjsonparser::getrequestID(const uint8_t * data)
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
int  cjsonparser::get_subscribe_request_id(const uint8_t * data)
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
std::string  cjsonparser::get_subscribe_request_id_str(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	return std::string(root[1].as<char*>());	
}

std::string   cjsonparser::get_subscription_id_str(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	 return std::string(root[2].as<char*>());		
}

int  cjsonparser::get_subscription_id(const uint8_t * data)
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
int  cjsonparser::get_register_id(const uint8_t * data)
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
std::string  cjsonparser::get_register_id_str(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	return std::string(root[1].as<char*>());	
}

int  cjsonparser::get_registration_id(const uint8_t * data)
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
std::string   cjsonparser::get_registration_id_str(const uint8_t * data)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(data);
	if (!root.success()){
        return 0;
	}	
	return std::string(root[2].as<char*>());	

}
e_message_codes  cjsonparser::get_message_type(const uint8_t *  str)
{
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(str);
	if (!root.success())
    {
        return WAMP_MSGCODE_ERROR;
	}	
	e_message_codes command = static_cast<e_message_codes>((int)(root[0]));
   	jsonBuffer.clear();	
	return command;
}
#if 0
std::string cjsonparser::geteventpayload_str(const uint8_t * payload){
	
	JsonArray& root = jsonBuffer.parseArray(payload); 	
	JsonArray& now = root[4];
	std::string value = now.get<std::string>(0); // returns NULL
	return value;
}
#endif
const char * cjsonparser::geteventpayload(const uint8_t * payload){
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(payload); 	
	JsonArray& now = root[4];
	const char* value = now.get<const char*>(0); // returns NULL
   	jsonBuffer.clear();	
	return value;
}
const char * cjsonparser::geteventpayload_rpc(const uint8_t * payload){
    DynamicJsonBuffer jsonBuffer;	
	JsonArray& root = jsonBuffer.parseArray(payload); 	
	JsonArray& now = root[3];
	const char* value = now.get<const char*>(0); // returns NULL
   	jsonBuffer.clear();	
	return value;
}
#endif
