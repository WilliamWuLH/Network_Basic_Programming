#include "Command_handle.hpp"

/*
    * @author: WilliamWuLH
    * @version: 2020.5.24
    * @see: Command
    * 处理 RESP 信息，提取出 RESP 信息中的 key 和 value
*/
void Command::RESP_handle(string &buf){
    stringstream s;
    string msg;
    operation = "";
    key_number = 0;
    key[0] = "";
    value = "";
    s << buf;
    while(s >> msg){
        if(msg == "SET"){
            operation = "SET";
            s >> msg;
            s >> key[0];
            s >> msg;
            s >> value;
            while(s >> msg){
                if(msg[0] != '$'){
                    value += (" " + msg);
                }
            }
            break;
        }
        else if(msg == "GET"){
            operation = "GET";
            key_number = 0;
            while(s >> msg){
                if(msg[0] != '$'){
                    key[key_number] = msg;
                    key_number++;
                }
            }
            break;
        }
        else if(msg == "DEL"){
            operation = "DEL";
            key_number = 0;
            while(s >> msg){
                if(msg[0] != '$'){
                    key[key_number] = msg;
                    key_number++;
                }
            }
            break;
        }
    }
}

/*
    * @author: WilliamWuLH
    * @version: 2020.5.24
    * @see: Command
    * 将命令转化为 RESP 协议的形式
*/
string Command::RESP_produce(string s){
    stringstream ss;
    string msg;
    vector<string> data;
    string result = "*";
    ss << s;
    while(ss >> msg){
        data.push_back(msg);
    }
    //*4\r\n$3\r\nSET\r\n$7\r\nCS06142\r\n$5\r\nCloud\r\n$9\r\nComputing\r\n
    int len = data.size();
    ss.str("");
    ss.clear();
    ss << len;
    string slen;
    slen = ss.str();
    result += slen + "\r\n";
    for(int i=0; i<(int)data.size(); i++){
        //" "
        if( data[0] == "SET" && i == 2 )
            data[i] = data[i].substr(1,data[i].length()-1);
        else if( data[0] == "SET" && i == (int)data.size()-1 )
            data[i] = data[i].substr(0,data[i].length()-1);
        //
        len = data[i].length();
        ss.str("");
        ss.clear();
        ss << len;
        slen = ss.str();
        result += "$" + slen + "\r\n";
        result += data[i] + "\r\n";
    }
    // cout <<result;
    return result;
}