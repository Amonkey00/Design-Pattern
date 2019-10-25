//
//  Shareable.h
//  Design Pattern
//
//  Created by double Z on 2019/10/24.
//  Copyright © 2019 double Z. All rights reserved.
//

#ifndef Shareable_h
#define Shareable_h

#include <iostream>
#include <cstring>
using namespace std;

class myString
{
public:
    friend ostream& operator<<(ostream& os, const myString& str);

private:
    struct StringValue {
        int refCount;
        char* data;
        bool shareable;     //@1
        StringValue(const char* initValue);
        ~StringValue();
    };
    StringValue* value = NULL;

public:
    myString() {}
    myString(const char* cstr);

    myString(const myString& rhs);
    ~myString();
    myString& operator=(const myString& rhs);

    char* get_c_str() const { return value->data; }
    
    ////////////////////////////////////////////////////////////////////////////////////
    const char& operator[](int index) const;
    char& operator[](int index);
};

ostream& operator<<(ostream& os, const myString& str)
{
    os << str.value->data;
    return os;
}

myString::StringValue::StringValue(const char* initValue)
    :refCount(1), shareable(true)       //@2
{
    data = new char[strlen(initValue) + 1];
    strcpy(data, initValue);
}

myString::StringValue::~StringValue()
{
    delete[] data;      //判断计数器为0的语句在外层做了
}

myString::myString(const char* cstr)
    :value(new StringValue(cstr)) {}     //唤起StringValue的构造函数

myString::myString(const myString& rhs)
{
    if(rhs.value->shareable){
        value = rhs.value;
        ++value->refCount;
    }
    else{
        value = new StringValue(rhs.value->data);
    }
}

myString::~myString()
{
    if (--value->refCount == 0) {
        delete value;       //唤起StringValue的析构函数
    }
}

myString& myString::operator=(const myString& rhs)
{
    if (value == rhs.value) {
        return *this;
    }

    if (value && --value->refCount == 0) {
        delete value;
    }

    //@5
    if(rhs.value->shareable){
        value = rhs.value;        //二者的类型都是指针
        ++value->refCount;        //递增this->value的refCount, rhs.value的refCount也同步变化
    }
    else{
        value = new StringValue(rhs.value->data);
    }
   
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
const char& myString::operator[](int index) const
{
    return value->data[index];      //这是const object唤起的const function，一定不会改动，即使很多人共享，也不用考虑COW
}

char& myString::operator[](int index)
{
    /* 悲观的假设每次都用于写 */
    if(value->refCount > 1){    //如果这个object是和其他人共享的，复制一份COW
        --value->refCount;      //自己被分割出去，所以引用减少一个
        value = new StringValue(value->data);   //用之前的data创建新的value
    }
    
    value->shareable = false;       //@4
    
    return value->data[index];      //此时的myString是一个绝对未被共享的
}



#endif /* Shareable_h */
