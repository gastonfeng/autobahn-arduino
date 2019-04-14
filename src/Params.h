#ifndef PARAMS_H
#define PARAMS_H

class wamp_param{
	
public:
 
 int asInt() const { return atoi(buff); }
 const char* asString() const { if(len != 0)return buff; else return ""; }
 wamp_param *get(int index){};
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
 int storeData(char* str) { 
	 if(str != NULL){
	     strcpy(buff,str);
	     this->len += strlen(str);
	 }
 }

void reset() { 
		this->len = 0;
		this->buff[0] = '\0';
		memset(buff,0,500);
	}
 

template <typename T, typename... Args> 
void add(T head, Args... tail){ 
	add(head) ; 
    add(tail...) ; 
} 
template <typename T> 
void add(T head){ 
 	
	add1(head) ; 
}    
void add1(const char* b, size_t l)
{

    if (this->len + l > buff_size)
        return;

    memcpy(this->buff+len, b, l);
    this->len += l;    
    this->buff[len++] = ' ';
  

}


void add1(const char* str)
{
    if (str == NULL) {
        buff[this->len++] = '\0';
        return;
    }
    add1(str, strlen(str));
}
 


void add1(int value) {
    char str[2 + 8 * sizeof(value)];
    itoa(value, str, 10);
    add(str);
}

void add1(unsigned int value)
{
	char str[1 + 8 * sizeof(value)];
	utoa(value, str, 10);
	add(str);
}

void add1(float value){
	char str[33];
	dtostrf(value, 5, 3, str);
	add(str);

}
#if 0
#endif
    char    buff[500];
    size_t   len = 0;
    size_t   buff_size = 500;

};
#endif
