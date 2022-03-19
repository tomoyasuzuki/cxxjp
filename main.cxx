#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <sstream>

enum type {
    unknown,
    string,
    double_type,
    boolean,
    array,
    object,
    null_type
}; 

class value {
    private: 
        type t;
        std::string s;
        double d;
        bool b;
        std::vector<value> a;
        nullptr_t n;
        std::map<std::string, value> o;
    public:
        value() : t(unknown) {};
        value(std::string str) : t(string), s(str) {};
        value(double dv) : t(double_type), d(dv) {};
        value(bool bv) : t(boolean), b(bv) {};
        value(std::vector<value>& arr) : t(array), a(arr) {};
        value(nullptr_t nl) : t(null_type), n(nl) {};
        value(std::map<std::string, value>& ob) : t(object), o(ob) {};
        ~value() {};
        const std::string& get_str() { return s; };
        const double& get_double() { return d; };
        const bool& get_boolean() { return b; };
        const std::vector<value>& get_arr() { return a; };
        const std::map<std::string, value>& get_obj() { return o; };
        type gettype() { return t; };
};

int get_key(std::string& s, std::string& key, int ki_start) {
    int ki_end = ki_start;

    while(s[ki_start] != '"' && ki_start < s.length() - 1) ki_start++;
    ki_end = ki_start + 1;
    while(s[ki_end] != '"' && ki_end < s.length() - 1) ki_end++;
    
   key = s.substr(ki_start+1, ki_end-ki_start-1);

   return ki_end + 1;
}

int skip_whitespaces(std::string& s, int i) {
    while(s[i] == ' ' && i < s.length() - 1) i++;
    return i;
}

int parse_string(std::string& s, std::map<std::string, value>& values, std::string& key, int i) {
    int vi_start = i;
    int vi_end = i;
    std::string v;

    while(s[vi_start] != '"' && vi_start < s.length() - 1) vi_start++;
    vi_end = vi_start + 1;
    while(s[vi_end] != '"' && vi_end < s.length() - 1) vi_end++;

    v = s.substr(vi_start+1, vi_end-vi_start-1);
    values[key] = value(v);

    return vi_end;
}

double parse_double(std::string& s, std::map<std::string, value>& values, std::string& key, int i) {
    int si = i;
    double v = 0;
    std::string substr;
    std::istringstream ss;
    
    if (s[i] < 0x30 || s[i] > 0x39) {
        std::cout << "error!!(parse_double_type)" << std::endl;
        return 0;
    }

    while((s[i] >= '0' && s[i] <= '9') && i < s.length() - 1 || s[i] == '.' || s[i] == 'e') i++;

    substr = s.substr(si, i-si);
    ss = std::istringstream(substr);
    ss >> v;
    values[key] = value(v);

    return i-1;
    
}

int parse_array(std::string& s, std::map<std::string, value>& values, std::string& key, int i) {
    std::vector<value> arr;
    int si;

    while(s[i] != ']' && i < s.length() - 1) {
        std::istringstream ss;
        std::string substr;
        double v = 0;
        std::string vs;
        value val;
        i++;
        i = skip_whitespaces(s, i);
        si = i;
        
        if (s[i] == '"') {
            int j = i;
            i++;
            std::cout << "ss" << s[i+1] << std::endl;

            while(s[i] != '"' && i < s.length() - 1) i++;
            
            substr = s.substr(j, i-j+1);
            val = value(substr);
        } else if (s[i] >= '0' && s[i] <= '9') {

            while((s[i] >= '0' && s[i] <= '9') && i < s.length() - 1) i++; 

            substr = s.substr(si, i-si);
            ss = std::istringstream(substr);
            ss >> v;
            val = value(v);  
            i -= 1; 
            std::cout << "arrint" << s[i] << std::endl;      
        } else if (s.substr(i,  4) == "true") {
            i += 3;
            val = value(true);
        } else if (s.substr(i, 5) == "false") {
            i += 4;
            val = value(false);
            std::cout << "false: " << s[i+1] << std::endl;
        } else {
            std::cout << "unknown: " << s[i] << std::endl;
        }

        arr.push_back(val);

        // skip last not space value
        i++;
        // skipt till ","
        i = skip_whitespaces(s, i);
    }

    values[key] = value(arr);

    return i;
}

int parse_object(std::string& s, std::map<std::string, value>& values, std::string& key, int i) {
    int ki_start = i;
    int ki_end = i;
    int current_i = i;
    std::string obj_key;
    std::map<std::string, value> obj;

    // next of {
    i++;

    while(s[current_i] != '}') {
        current_i++;
        current_i = skip_whitespaces(s, current_i);
        ki_start = current_i;
        ki_end = current_i;

        while(s[ki_start] != '"' && ki_start < s.length() - 1) ki_start++;
        ki_end = ki_start + 1;
        while(s[ki_end] != '"' && ki_end < s.length() - 1) ki_end++;

        current_i = get_key(s, obj_key, ki_start);
    
        current_i = skip_whitespaces(s, ki_end + 1);
        
        if (s[current_i] == ':') {
            current_i = skip_whitespaces(s, current_i + 1);
          
            if (s[current_i] == '"') {
                current_i = parse_string(s, obj, obj_key, current_i);
            } else if (s[current_i] >= 0x30 && s[current_i] <= 0x39) {
                current_i = parse_double(s, obj, obj_key, current_i);
                std::cout << "aaa: " << s[current_i] << std::endl;
            } else if (s.substr(current_i, 4) == "true")  { 
                current_i += 3;
                obj[obj_key] = value(true);
                std::cout << "trr: " << s[current_i] << std::endl;
            } else if (s.substr(current_i, 5) == "false") {
                current_i += 4;
                obj[obj_key] = value(false);
                std::cout << "adada: " << s[current_i] << std::endl;
            } else if (s[current_i] == '[') {
                current_i = parse_array(s, obj, obj_key, current_i);
            } else if (s.substr(current_i, 4) == "null") {
                values[obj_key] = value(nullptr);
            } else if (s[current_i] == '{') {
                //current_i = parse_object(s, obj, key, current_i+1);
            } else {
                std::cout << "else: " << s[current_i] << std::endl;
                break;
            }
        } else {
            current_i = skip_whitespaces(s, current_i);
            std::cout << "not: " << s[current_i] << std::endl;
        }

        current_i++;
        current_i = skip_whitespaces(s, current_i);
        std::cout << "end: " << s[current_i] << std::endl;
    }
    
    values[key] = value(obj);

    return current_i+1;
}

void parse(std::string& s, std::map<std::string, value>& values, int i=0) {
    int ki_start = i;
    int ki_end = i;
    int current_i = i;
    std::string key;
    
    while(s[current_i] != '\0') {
        // skip spaces till '"'
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
                std::cout << "after string " << s[current_i] << std::endl;
            } else if (s[current_i] >= 0x30 && s[current_i] <= 0x39) {
                current_i = parse_double(s, values, key, current_i);
            } else if (s.substr(current_i, 4) == "true")  { 
                current_i += 3;
                values[key] = value(true);
            } else if (s.substr(current_i, 5) == "false") {
                current_i += 4;
                values[key] = value(false);
            } else if (s[current_i] == '[') {
                current_i = parse_array(s, values, key, current_i);
            } else if (s.substr(current_i, 4) == "null") {
                values[key] = value(nullptr);
            } else if (s[current_i] == '{') {
                current_i = parse_object(s, values, key, current_i);
            } else {
                std::cout << "else: " << s[current_i] << std::endl;
                break;
            }
        } else {
            std::cout << "not : " << s[current_i] << std::endl;
        }
        
        // skip last "not space value" 
        current_i++;
        // increment index at ","
        current_i = skip_whitespaces(s, current_i);
        // skip ","
        current_i++;
    }
}

int main() {
    std::string json = R"({"hoge": "huga", "hogeint": 543, "hugaint": 5.232, "piyoint": 1e9, "hogebool": true, "piyo": "piyoyo", "arr": [10, 20, "arrstr", false], "obj": {"objstr": "obobob", "objdouble": 1.234, "objbool": true, "objarr": [2,"ddd", false]}})";
    std::map<std::string, value> values;

    parse(json, values);

    std::cout << "======================" << std::endl;
    std::cout << "hoge: " << values["hoge"].get_str() << std::endl;
    std::cout << "piyo: " << values["piyo"].get_str() << std::endl;
    std::cout << "hogeint: " << values["hogeint"].get_double() << std::endl;
    std::cout << "hugaint: " << values["hugaint"].get_double() << std::endl;
    std::cout << "piyoint: " << values["piyoint"].get_double() << std::endl;
    std::cout << "hogebool: " << (values["hogebool"].get_boolean() ? "true" : "false") << std::endl;
    
    std::vector<value> arr = values["arr"].get_arr();
    std::cout << "arr: ";
    for (int i = 0; i < arr.size(); i++) {
        type t = arr[i].gettype();

        if (t == string) {
            std::cout << arr[i].get_str();
        } else if (t == double_type) {
            std::cout << arr[i].get_double();
        } else if (t == boolean) {
            std::cout << (arr[i].get_boolean() ? "true" :  "false");
        }

        if (i != arr.size() -1) {
            std::cout << ",";
        } else {
            std::cout << "\n";
        }
    }

    std::map<std::string, value> obj = values["obj"].get_obj();
    std::cout << "objstr: " << obj["objstr"].get_str() << std::endl;
    std::cout << "objdouble: " << obj["objdouble"].get_double() << std::endl;
    std::cout << "objbool: " << (obj["objbool"].get_boolean() ? "true" : "false") << std::endl;

    std::vector<value> aarr = obj["objarr"].get_arr();
    std::cout << "aarr: ";
    for (int i = 0; i < aarr.size(); i++) {
        type t = aarr[i].gettype();

        if (t == string) {
            std::cout << aarr[i].get_str();
        } else if (t == double_type) {
            std::cout << aarr[i].get_double();
        } else if (t == boolean) {
            std::cout << (aarr[i].get_boolean() ? "true" :  "false");
        }

        if (i != aarr.size() -1) {
            std::cout << ",";
        } else {
            std::cout << "\n";
        }
    }
   
    return 1;
} 