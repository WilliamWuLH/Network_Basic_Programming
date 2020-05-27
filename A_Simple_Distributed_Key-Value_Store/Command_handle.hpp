#ifndef _RESPHANDLE_
#define _RESPHANDLE_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

/*
    * @author: WilliamWuLH
    * @version: 2020.5.24
    * @see: Command
    * 用于处理命令信息
*/

const int max_key_number = 2048;
class Command
{
    public:
        int key_number;
        string operation;
        string key[max_key_number];
        string value;
    
    public:
        void RESP_handle(string &buf);

        string RESP_produce(string s);

};

#endif