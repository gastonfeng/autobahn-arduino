#ifndef SESSION_H
#define SESSION_H
#include <map>
#include "Params.h"

//typedef std::function<void(wamp_param *)> t_sub_wamp_cb;
//typedef std::function<wamp_param *(wamp_param *)> t_rpc_wamp_cb;
typedef std::function<wamp_param *(wamp_param *)> t_wamp_cb;
typedef std::function<void()> t_cb;

typedef std::map<int, std::string > PubTable;

typedef std::map<int, std::pair<std::string, t_wamp_cb> > t_temp_table;
typedef std::map<std::string, std::pair<std::string, t_wamp_cb> > t_sub_rpc_table;


class csession{
	public:
	char * realm;
	bool _connected = false;
	bool _rpcresult = false;

	int _sessionId = 0;
	//PubTable publish_table;	
	//PubTable::iterator pubtable_it;
	
	t_temp_table temp_sub_rpc_table;
	t_temp_table::iterator subrpc_it;    
	
	t_sub_rpc_table sub_rpc_table;
	t_sub_rpc_table::iterator sub_rpc_it;


    void reset(){
		
	}
	bool add_publish_record(int reqId, const char *topic){
		//publish_table.insert(std::make_pair(reqId, std::string(topic)));
	}
	
	bool del_publish_record(int reqId){
		//pubtable_it = publish_table.find(reqId);
	    //publish_table.erase (pubtable_it); 
	}

	bool add_temp_rpc_record(int reqId, const char *topic, t_wamp_cb cb){
		temp_sub_rpc_table.insert(std::make_pair(reqId,std::make_pair( std::string(topic),cb)));
	}	
	
	bool add_rpc_record(int registerid, std::string  registrationid){
	    subrpc_it = temp_sub_rpc_table.find(registerid);
	    this->sub_rpc_table.insert( std::make_pair(registrationid, subrpc_it->second)); 
	    temp_sub_rpc_table.erase(subrpc_it);	    
		return true;
	}	
	
	bool add_temp_subscribe_record(int reqId, const char *topic, t_wamp_cb cb){
		temp_sub_rpc_table.insert(std::make_pair(reqId,std::make_pair( std::string(topic),cb)));
	}	
	
	bool add_subscribe_record(int reqId, std::string subId){
        subrpc_it = temp_sub_rpc_table.find(reqId);
        std::pair<std::string, t_wamp_cb> val = temp_sub_rpc_table[reqId];
    	this->sub_rpc_table.insert( std::make_pair(subId , subrpc_it->second)); // TODO convert subId to int???
    	temp_sub_rpc_table.erase(subrpc_it);
		return true;
	}	

	t_wamp_cb get_subscribe_cb(std::string  reqid){
	    std::pair<std::string, t_wamp_cb> val = sub_rpc_table[ reqid];
	    return val.second;
	}

	t_wamp_cb get_rpc_cb(std::string reqid){
	    std::pair<std::string, t_wamp_cb> val = sub_rpc_table[reqid];
	    return val.second;
	}	
	
    bool is_subscribed(const char * topic){
	  for (sub_rpc_it =  sub_rpc_table.begin(); sub_rpc_it != sub_rpc_table.end(); sub_rpc_it++){
           std::pair<std::string, t_wamp_cb> val =  sub_rpc_it->second;   
		   if( strcmp( topic,(val.first).c_str() ) == 0)
			   return true;
       }
	   return false;			
    }

    std::string unsubscribe(const char * topic){
  	  std::string substring;
	
	  for (sub_rpc_it =  sub_rpc_table.begin(); sub_rpc_it != sub_rpc_table.end(); sub_rpc_it++){
           std::pair<std::string, t_wamp_cb> val =  sub_rpc_it->second;
		   if(strcmp( topic, (val.first).c_str() ) == 0){
   			    substring = std::string(sub_rpc_it->first);
				sub_rpc_table.erase(sub_rpc_it);
				break;
		   }
       }
	   return substring;
     }   
};
#endif
