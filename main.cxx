#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <sstream>

class value {
    private:
        enum type {
            unknown,
            string,
            integer,
            boolean,
            array,
            object
        };  
        type t;
        std::string s;
        int d;
        bool b;
    public:
        value() : t(unknown) {};
        value(std::string str) : t(string), s(str) {};
        value(int dv) : t(integer), d(dv) {};
        value(bool bv) : t(boolean), b(bv) {};
        ~value() {};
        const std::string& get_str() { return s; };
        const int& get_integer() { return d; };
        const bool& get_boolean() { return b; };
};

int skip_whitespaces(std::string& s, int i) {
    while(s[i] == ' ' && i < s.length() - 1) i++;
    return i;
}

int parse_string(std::string& s, std::map<std::string, value>& values, std::string& key, int i) {
    int vi_start = i;
    int vi_end = i;
    std::string v;

    std::cout << "parse_string: " << key << std::endl;

    while(s[vi_start] != '"' && vi_start < s.length() - 1) vi_start++;
    vi_end = vi_start + 1;
    while(s[vi_end] != '"' && vi_end < s.length() - 1) vi_end++;

    v = s.substr(vi_start+1, vi_end-vi_start-1);
    values[key] = value(v);

    vi_end = skip_whitespaces(s, vi_end + 1);

    return vi_end + 1;
}

int parse_integer(std::string& s, std::map<std::string, value>& values, std::string& key, int i) {
    int si = i;
    int v = 0;
    std::string substr;
    std::istringstream ss;

    std::cout << "parse_integer: " << key  << std::endl;
    
    if (s[i] < 0x30 || s[i] > 0x39) {
        std::cout << "error!!(parse_integer)" << std::endl;
        return 0;
    }

    while((s[i] >= '0' && s[i] <= '9') && i < s.length() - 1) i++;

    substr = s.substr(si, i-si);
    ss = std::istringstream(substr);
    ss >> v;
    values[key] = value(v);

    i = skip_whitespaces(s, i);

    return i+1;
    
}

int get_key(std::string& s, std::string& key, int ki_start) {
    int ki_end = ki_start;

    while(s[ki_start] != '"' && ki_start < s.length() - 1) ki_start++;
    ki_end = ki_start + 1;
    while(s[ki_end] != '"' && ki_end < s.length() - 1) ki_end++;
    
   key = s.substr(ki_start+1, ki_end-ki_start-1);

   return ki_end + 1;
}

void parse(std::string& s, std::map<std::string, value>& values, int i=0) {
    int ki_start = i;
    int ki_end = i;
    int current_i = i;
    std::string key;
    
    while(s[current_i] != '\0') {
        current_i = skip_whitespaces(s, current_i);
        ki_start = current_i;
        ki_end = current_i;

        while(s[ki_start] != '"' && ki_start < s.length() - 1) ki_start++;
        ki_end = ki_start + 1;
        while(s[ki_end] != '"' && ki_end < s.length() - 1) ki_end++;

        current_i = get_key(s, key, ki_start);
    
        current_i = skip_whitespaces(s, ki_end + 1);
        
        if (s[current_i] == ':') {
            current_i = skip_whitespaces(s, current_i + 1);
          
            if (s[current_i] == '"') {
                current_i = parse_string(s, values, key, current_i);
            } else if (s[current_i] >= 0x30 && s[current_i] <= 0x39) {
                current_i = parse_integer(s, values, key, current_i);
            } else if (s.substr(current_i, 4) == "true")  { 
                current_i += 4;
                values[key] = value(true);
            } else if (s.substr(current_i, 4) == "false") {
                current_i += 4;
                values[key] = value(false);
            } else {
                std::cout << "else: " << s[current_i] << std::endl;
                break;
            }
        } else {
            std::cout << "not : " << std::endl;
        }
    }
}

int main() {
    std::string json = R"({"hoge": "huga", "hogeint": 5, "hogebool": true,"piyo": "piyoyo", "hugaint": 27892})";
    std::map<std::string, value> values;

    parse(json, values);

    std::cout << "======================" << std::endl;
    std::cout << "hoge: " << values["hoge"].get_str() << std::endl;
    std::cout << "piyo: " << values["piyo"].get_str() << std::endl;
    std::cout << "hogeint: " << values["hogeint"].get_integer() << std::endl;
    std::cout << "hugaint: " << values["hugaint"].get_integer() << std::endl;
    std::cout << "hogebool: " << (values["hogebool"].get_boolean() ? "true" : "false") << std::endl;

    return 1;
}