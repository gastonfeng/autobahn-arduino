#ifndef AUTOBAHNPROTOIMPL_H
#define AUTOBAHNPROTOIMPL_H

#include "WAMP.h"
#include "Session.h"
#include "JsonParser.h"
#include "AutobahnApi.h"
#include "Params.h"

typedef std::map<int, std::string > PubTable;
class cautobahnproto_impl{
	
public:
    cautobahnproto_impl(){};
    ~cautobahnproto_impl(){};
	void on_abort(uint8_t * payload, size_t length);
    void on_goodbye(uint8_t * payload, size_t length);
    void on_welcome(uint8_t * payload, size_t length);
	void on_errormsg(uint8_t * payload, size_t length);
    void on_published(uint8_t * payload, size_t length);
	void on_subscribed(uint8_t * payload, size_t length);
    void on_unsubscribed(uint8_t * payload, size_t length);
    void on_event(uint8_t * payload, size_t length);    
    void on_result(uint8_t * payload,size_t length);
    void on_registered(uint8_t * payload, size_t length);
    void on_unregistered(uint8_t * payload, size_t length);
    void on_invocation(uint8_t * payload, size_t length);
    void return_call(wamp_param  * message);

    void wampstatemachine(uint8_t * payload, size_t length);    
    void wampstatemachine(e_message_codes _wampstate,uint8_t * payload, size_t length);    
    void send_hello(const char * topic);
    template <typename... Args>  
	void publish(const char * topic,Args... values);
    void subscribe(const char * topic, t_wamp_cb cb);
	bool is_connected();
	void unsubscribe(const char * topic);
	bool is_subscribed(const char * topic);
	void rpc_register(const char * topic,t_wamp_cb cb); 
    template <typename... Args>  
	wamp_param * rpc_call(const char * topic,Args... values);	
	wamp_param * rpc_call(const char * topic);	
    wamp_param * rpc_call_async(const char * topic,t_cb cb);
    template <typename... Args>  
    wamp_param * rpc_call_async(const char * topic,t_cb cb,Args... values);

    virtual void notify_welcome( ) = 0;
    virtual void send_towire( const char * topic) = 0;
    virtual void loop() = 0;

    void setrealm( char * realm){ this->_session.realm = realm;};   

    int generate_request_id();

    bool _connected = false;
	csession _session;
	cjsonparser _parser;
	wamp_param pub_param;
	wamp_param sub_param;
	wamp_param rpc_param;

    e_message_codes _wampstate;
	PubTable publish_table;	
	
	bool add_publish_record(int reqId, const char *topic){
		publish_table.insert(std::make_pair(reqId, std::string(topic)));
	}	
};
void cautobahnproto_impl::wampstatemachine(uint8_t * payload, size_t length){
	_wampstate = this->_parser.get_message_type(payload);
	wampstatemachine(_wampstate,payload,length);
}

void cautobahnproto_impl::wampstatemachine(e_message_codes wampstate,uint8_t * payload, size_t length){

	 switch(wampstate)	 {
		case WAMP_MSGCODE_HELLO:
		 send_hello( this->_session.realm);
		break;	
		case WAMP_MSGCODE_WELCOME:
		 on_welcome(( uint8_t *)payload,length);
		break;
		case WAMP_MSGCODE_ABORT:
		 on_abort(payload,length);		
		break;			
		case WAMP_MSGCODE_GOODBYE: 
		on_goodbye(payload,length);
		break;
		case WAMP_MSGCODE_ERROR:
		on_errormsg(payload,length);
        break; 
		case WAMP_MSGCODE_PUBLISH:
		send_towire((char *)payload);
		break;        
		case WAMP_MSGCODE_PUBLISHED:
		on_published(payload,length);
        break;
		case WAMP_MSGCODE_SUBSCRIBE:
		send_towire((char *)payload);
		break; 		
		case WAMP_MSGCODE_SUBSCRIBED:
		on_subscribed(payload,length);
		break; 		
		case WAMP_MSGCODE_UNSUBSCRIBE:
		send_towire((char *)payload);		
		break;		
		case WAMP_MSGCODE_UNSUBSCRIBED:
		on_unsubscribed(payload,length);
		break;		
		case WAMP_MSGCODE_EVENT:
		on_event(payload, length);
		break;		
		case WAMP_MSGCODE_CALL:
		send_towire((char *)payload);
		break;		
		case WAMP_MSGCODE_RESULT:
		on_result(payload, length);
		break;		
		case WAMP_MSGCODE_REGISTER:
		send_towire((char *)payload);
		break;
		case WAMP_MSGCODE_REGISTERED:
		on_registered(payload,length);
		break; 		
		case WAMP_MSGCODE_UNREGISTER:
		send_towire((char *)payload);
		break;
		case WAMP_MSGCODE_UNREGISTERED:
		on_unregistered(payload,length);
		break;
		case WAMP_MSGCODE_INVOCATION:
		on_invocation(payload,length);
		break;
		case WAMP_MSGCODE_YIELD:
		send_towire((char *)payload);
		break;
	 }
}

bool cautobahnproto_impl::is_connected(){
	return this->_session._connected;
}

int cautobahnproto_impl::generate_request_id(){
    return rand();
}
void  cautobahnproto_impl::send_hello(const char * topic){
	send_towire(_parser.get_hello(topic));
}

void  cautobahnproto_impl::on_welcome(uint8_t * payload, size_t length){
	this->_session._sessionId = _parser.getsessionID(( char *)payload);
  	this->_session._connected = true;
  	notify_welcome();

}
void  cautobahnproto_impl::on_abort(uint8_t * payload, size_t length){
}

void  cautobahnproto_impl::on_goodbye(uint8_t * payload, size_t length){
}

void  cautobahnproto_impl::on_errormsg(uint8_t * payload, size_t length){
}

void  cautobahnproto_impl::on_published(uint8_t * payload, size_t length){
	int var = _parser.getrequestID(payload);	
	this->_session.del_publish_record(var);
}  
 void  cautobahnproto_impl::on_subscribed(uint8_t * payload, size_t length){     
	 std::string subid = _parser.get_subscription_id_str(payload);
     int reqid = _parser.get_subscribe_request_id(payload);
	 this->_session.add_subscribe_record(reqid,subid);	
}
 

void  cautobahnproto_impl::on_unsubscribed(uint8_t * payload, size_t length){
}
 

void  cautobahnproto_impl::on_event(uint8_t * payload, size_t length){
	std::string reqId = _parser.get_subscribe_request_id_str( payload);
	const char* buf = _parser.geteventpayload(payload);
	sub_param.storeData(const_cast<char*>(buf));
	t_wamp_cb cb = _session.get_subscribe_cb(reqId);
  	cb((wamp_param  *)&sub_param );
} 
 void cautobahnproto_impl::on_result(uint8_t * payload,size_t length){
 	const char* buf = _parser.geteventpayload_rpc(payload);
    rpc_param.storeData(const_cast<char*>(buf));  	
     _session._rpcresult = true;
}  
 
void  cautobahnproto_impl::on_registered(uint8_t * payload, size_t length){
	 std::string regnid = _parser.get_registration_id_str(payload);
     int reqid = _parser.get_register_id(payload);
	 this->_session.add_rpc_record(reqid,regnid);
}
void  cautobahnproto_impl::on_unregistered(uint8_t * payload, size_t length){

	
}
void  cautobahnproto_impl::on_invocation(uint8_t * payload, size_t length){
	
	std::string reqId = _parser.get_register_id_str( payload);
	std::string regId = _parser.get_registration_id_str( payload);

    rpc_param.reset();
	const char * buf = _parser.geteventpayload(payload);
    if( buf != NULL){   	
        rpc_param.add1(const_cast<char*>(buf));
	}
	t_wamp_cb  cb = _session.get_rpc_cb(regId);
  	cb((wamp_param  *)&rpc_param );
	send_towire( _parser.get_result(reqId,rpc_param.asString()));

}
void  cautobahnproto_impl::return_call(wamp_param  * message)
{ 
}
 

template <typename... Args>  
void cautobahnproto_impl::publish(const char * topic,Args... values){
	pub_param.reset();
	pub_param.add(values...);   
	int reqId = generate_request_id();
	const char * tmp = pub_param.asString();
	send_towire( _parser.get_publish(reqId,topic, tmp));		
}

void cautobahnproto_impl::subscribe(const char * topic, t_wamp_cb cb){
	
	int reqId = generate_request_id();
	this->_session.add_temp_subscribe_record(reqId, topic, cb);		
    send_towire( _parser.get_subscribe(reqId,topic));
} 
void cautobahnproto_impl::unsubscribe(const char * topic){
	char buffer[512];  // TODO reimplement
	std::string subid = this->_session.unsubscribe(topic);
	int reqId = generate_request_id();
	send_towire(_parser.get_unsubscribe(reqId,subid));
} 

bool cautobahnproto_impl::is_subscribed(const char * topic){
	return this->_session.is_subscribed(topic);
} 
void cautobahnproto_impl::rpc_register(const char * topic, t_wamp_cb cb){
    int reqId = generate_request_id();
    
	this->_session.add_temp_rpc_record(reqId, topic, cb);
	send_towire(_parser.get_rpc_register(reqId,topic));		
}
wamp_param * cautobahnproto_impl::rpc_call(const char * topic){
	return rpc_call(topic,nullptr);
}

wamp_param * cautobahnproto_impl::rpc_call_async(const char * topic,t_cb cb){
	return rpc_call_async(topic,cb,nullptr);
}

template <typename... Args>  
wamp_param * cautobahnproto_impl::rpc_call_async(const char * topic,t_cb cb,Args... values){
	rpc_param.reset();
	int breakout_counter = 0;
	char * tmp;
	if(sizeof...(values) != 0)	{
	    rpc_param.add(values...);
    	tmp = const_cast<char*>(rpc_param.asString());

	}else{
		tmp = NULL;
	}
	int reqId = generate_request_id();
	
    //this->_session.add_publish_record(reqId, topic);
	send_towire( _parser.get_rpccall(reqId,topic, tmp));	
}

template <typename... Args>  
wamp_param * cautobahnproto_impl::rpc_call(const char * topic,Args... values){
	
	rpc_param.reset();
	int breakout_counter = 0;
	char * tmp;
	if(sizeof...(values) != 0)	{
	    rpc_param.add(values...);
    	tmp = const_cast<char*>(rpc_param.asString());

	}else{
		tmp = NULL;
	}
	int reqId = generate_request_id();
    //this->_session.add_publish_record(reqId, topic);
	send_towire( _parser.get_rpccall(reqId,topic, tmp));	
	while( _session._rpcresult != true)
	{
		breakout_counter++;
		this->loop();
		if(breakout_counter > 100){
			breakout_counter = 0;
			rpc_param.reset();
			break;		
		}
	}	
	_session._rpcresult = false;
	return &rpc_param;
}

#endif
