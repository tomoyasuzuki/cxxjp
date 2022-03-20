#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <sstream>

class value;

using number_t = double;
using array_t = std::vector<value>;
using object_t = std::map<std::string, value>;

object_t parse_object(std::string& s, int& i);

enum type_t {
    unknown,
    string_type,
    number_type,
    boolean_type,
    array_type,
    object_type,
    null_type
};

class value {
    private: 
        type_t type;
        std::string str;
        number_t num;
        bool bol;
        array_t arr;
        object_t obj;
        nullptr_t nul;
    public:
        value() : type(unknown) {};
        value(std::string s) : type(string_type), str(s) {};
        value(number_t n) : type(number_type), num(n) {};
        value(bool b) : type(boolean_type), bol(b) {};
        value(array_t& a) : type(array_type), arr(a) {};
        value(object_t& o) : type(object_type), obj(o) {};
        value(nullptr_t n) : type(null_type), nul(n) {};
        ~value() {};
        const std::string& get_str() { return str; };
        const double& get_num() { return num; };
        const bool& get_bool() { return bol; };
        const array_t& get_arr() { return arr; };
        const object_t& get_obj() { return obj; };
        const nullptr_t& get_null() { return nul; };
        type_t get_type() { return type; };
};

int get_key(std::string& s, std::string& key, int current_i) {
    int start;

    start = current_i;

    while(s[start] != '"' && start < s.length() - 1) start++;
    current_i = start + 1;
    while(s[current_i] != '"' && current_i < s.length() - 1) current_i++;
    
   key = s.substr(start + 1, current_i - start - 1);

   return ++current_i;
}

int skip_whitespaces(std::string& s, int i) {
    while(s[i] == ' ' && i < s.length() - 1) i++;
    return i;
}

int skip_first_non_space_or_comma(int i) { return ++i; };
int skip_last_non_space(int i) { return ++i; };

std::string parse_string(std::string& s, int& i) {
    int start;
    std::string v;

    start = i;

    while(s[start] != '"' && start < s.length() - 1) start++;
    i = start + 1;
    while(s[i] != '"' && i < s.length() - 1) i++;

    v = s.substr(start + 1, i - start - 1);

    return v;
}

number_t parse_number(std::string& s, int& i) {
    int start;
    number_t num;
    std::string substr;
    std::istringstream ss;

    start = i;
    
    if (s[i] < 0x30 || s[i] > 0x39) {
        std::cout << "error!!(parse_number_type)" << std::endl;
        return 0;
    }

    while((s[i] >= '0' && s[i] <= '9') && i < s.length() - 1 || s[i] == '.' || s[i] == 'e') i++;

    substr = s.substr(start, i-start);
    ss = std::istringstream(substr);
    ss >> num;

    i--;

    return num;
    
}

array_t parse_array(std::string& s, int& i) {
    array_t arr;

    while(s[i] != ']' && i < s.length() - 1) {
        std::istringstream ss;
        value v;

        i = skip_first_non_space_or_comma(i);
        i = skip_whitespaces(s, i);
        
        if (s[i] == '{') {
            object_t obj = parse_object(s, i);
            v = value(obj);
        }  else if (s[i] == '[') {
            array_t arr = parse_array(s, i);
            v = value(arr);
        } else if (s[i] == '"') {
            int start = ++i;

            while(s[i] != '"' && i < s.length() - 1) i++;
            
            v = value(s.substr(start, i-start));
        } else if (s[i] >= '0' && s[i] <= '9') {
            number_t num;
            int start = i;
            
            while((s[i] >= '0' && s[i] <= '9') && i < s.length() - 1) i++; 

            ss = std::istringstream(s.substr(start, i-start));
            ss >> num;
            v = value(num);  
            i -= 1;      
        } else if (s.substr(i,  4) == "true") {
            i += 3;
            v = value(true);
        } else if (s.substr(i, 5) == "false") {
            i += 4;
            v = value(false);
        } else if (s.substr(i, 4) == "null") {
            i += 3;
            v = value(nullptr);
        } else {
            std::cout << "unknown: " << s[i] << std::endl;
        }

        arr.push_back(v);

        i = skip_last_non_space(i);
        i = skip_whitespaces(s, i);
    }

    return arr;
}

object_t parse_object(std::string& s, int& i) {
    object_t obj;
    std::string key;

    while(s[i] != '}') {
        i = get_key(s, key, i);
        i = skip_whitespaces(s, i);
        
        if (s[i] == ':') {
            i = skip_whitespaces(s, i + 1);
          
            if (s[i] == '"') {
                std::string str = parse_string(s, i);
                obj[key] = value(str);
            } else if (s[i] >= 0x30 && s[i] <= 0x39) {
                number_t num = parse_number(s, i);
                obj[key] = value(num);
            } else if (s.substr(i, 4) == "true")  { 
                i += 3;
                obj[key] = value(true);
            } else if (s.substr(i, 5) == "false") {
                i += 4;
                obj[key] = value(false);
            } else if (s[i] == '[') {
                array_t arr = parse_array(s, i);
                obj[key] = value(arr);
            } else if (s.substr(i, 4) == "null") {
                obj[key] = value(nullptr);
            } else if (s[i] == '{') {
                object_t object = parse_object(s, i);
                obj[key] = value(object);
            } else {
                std::cout << "else: " << s[i] << std::endl;
                break;
            }
        } else {
            i = skip_whitespaces(s, i);
            std::cout << "err: cannot find coron" << s[i] << std::endl;
        }

        i = skip_last_non_space(i);
        i = skip_whitespaces(s, i);
    }

    return obj;
}

void parse(std::string& s, value& v, int i = 0) {
    if (s[i] == '{') {
        object_t object = parse_object(s, i);
        v = value(object);
    } else if (s[i] == '[') {
        //i = parse_array(s, root, key, i);
    } else if (s[i] == '"') {
        //i = parse_string(s, root, key, i);
    } else if (s[i] >= 0x30 && s[i] <= 0x39) {
        //i = parse_number(s, root, key, i);
    } else if (s.substr(i, 4) == "true") {
        //root[key] = value(true);
    } else if (s.substr(i, 5) == "false") {
        //root[key] = value(false);
    } else if (s.substr(i, 4) == "null") {
        //root[key] = value(nullptr);
    } else {
        std::cout << "err at parse();" << std::endl;
    }
}

void test() {

}

int main() {
    std::string json1 = R"({"hoge": "huga", "hogeint": 543, "hugaint": 5.232, "piyoint": 1e9, 
                        "hogebool": true, "piyo": "piyoyo", "arr": [10, 20, "arrstr", false, {"arrs": "arrdd"}], 
                        "obj": { "objstr": "obobob", "objdouble": 1.234, "objbool": true, 
                        "objarr": [2,"ddd", false, {"arrint": 234}], "objobj": {"str": "stroj"}}})";
    std::string json2 = R"(["json2str": "hogehugapiyo", "json2num": 2.313, "json2bool": true])";
    
    value v;
    object_t values;

    parse(json1, v);

    values = v.get_obj();

    std::cout << "======================" << std::endl;
    std::cout << "hoge: " << values["hoge"].get_str() << std::endl;
    std::cout << "piyo: " << values["piyo"].get_str() << std::endl;
    std::cout << "hogeint: " << values["hogeint"].get_num() << std::endl;
    std::cout << "hugaint: " << values["hugaint"].get_num() << std::endl;
    std::cout << "piyoint: " << values["piyoint"].get_num() << std::endl;
    std::cout << "hogebool: " << (values["hogebool"].get_bool() ? "true" : "false") << std::endl;
    
    std::vector<value> arr = values["arr"].get_arr();
    std::cout << "arr: ";
    for (int i = 0; i < arr.size(); i++) {
        type_t t = arr[i].get_type();

        if (t == string_type) {
            std::cout << arr[i].get_str();
        } else if (t == number_type) {
            std::cout << arr[i].get_num();
        } else if (t == boolean_type) {
            std::cout << (arr[i].get_bool() ? "true" :  "false");
        } else if (t == object_type) {
            object_t o = arr[i].get_obj();
            std::cout << "arr[i].obj[arrs]=" << o["arrs"].get_str() << std::endl;
        }

        if (i != arr.size() -1) {
            std::cout << ",";
        } else {
            std::cout << "\n";
        }
    }

    std::map<std::string, value> obj = values["obj"].get_obj();
    std::cout << "objstr: " << obj["objstr"].get_str() << std::endl;
    std::cout << "objdouble: " << obj["objdouble"].get_num() << std::endl;
    std::cout << "objbool: " << (obj["objbool"].get_num() ? "true" : "false") << std::endl;
    
    object_t obj2 = obj["objobj"].get_obj();
    std::cout << "str: " << obj2["str"].get_str() << std::endl;

    array_t aarr = obj["objarr"].get_arr();
    std::cout << "aarr: ";
    for (int i = 0; i < aarr.size(); i++) {
        type_t t = aarr[i].get_type();

        if (t == string_type) {
            std::cout << aarr[i].get_str();
        } else if (t == number_type) {
            std::cout << aarr[i].get_num();
        } else if (t == boolean_type) {
            std::cout << (aarr[i].get_bool() ? "true" :  "false");
        } else if (t == object_type) {
            object_t o = aarr[i].get_obj();
            std::cout << "arrint: " << o["arrint"].get_num() << std::endl;
        }

        if (i != aarr.size() -1) {
            std::cout << ",";
        } else {
            std::cout << "\n";
        }
    }
   
    return 1;
} 