#ifndef AUTOBAHNAPI_H
#define AUTOBAHNAPI_H
#include <stdlib.h>
#include <map>

class AutobahnApi{
	
public:	
#if 0
	typedef std::function<wamp_param *(wamp_param *)>  t_sub_wamp_cb;

	void init(Client &client);
	virtual void loop() = 0;
	virtual void option(wamp_option *opts) = 0;
	
    template <typename... Args>
	virtual void publish(const char * topic,Args... values) = 0;
	virtual void subscribe(const char * topic, t_sub_wamp_cb sendCallback) = 0;
	virtual void unsubscribe(const char * topic) = 0;
    virtual bool is_subscribed(const char * topic) = 0;	
	virtual void rpc_register(const char * topic,t_sub_wamp_cb sendCallback) = 0;
	virtual void rpc_unregister(const char * topic) = 0;
    virtual wamp_param * rpc_call(const char * topic,const char *fmt) = 0;
    virtual void disconnect() = 0;
    virtual bool is_connected() = 0;
    virtual void connect() = 0;
#endif    
};
#endif
