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
        bool boolval;
        array_t arr;
        object_t obj;
        nullptr_t nullval;
    public:
        value() : type(unknown) {};
        value(std::string s) : type(string_type), str(s) {};
        value(number_t n) : type(number_type), num(n) {};
        value(bool b) : type(boolean_type), boolval(b) {};
        value(array_t& a) : type(array_type), arr(a) {};
        value(object_t& o) : type(object_type), obj(o) {};
        value(nullptr_t n) : type(null_type), nullval(n) {};
        ~value() {};
        const std::string& get_str() { return str; };
        const double& get_num() { return num; };
        const bool& get_bool() { return boolval; };
        const array_t& get_arr() { return arr; };
        const object_t& get_obj() { return obj; };
        type_t get_type() { return type; };
};

int get_key(std::string& s, std::string& key, int current_i) {
    int start;
    int end;

    start = current_i;

    while(s[start] != '"' && start < s.length() - 1) start++;
    end = start + 1;
    while(s[end] != '"' && end < s.length() - 1) end++;
    
   key = s.substr(start + 1, end - start - 1);

   return ++end;
}

int skip_whitespaces(std::string& s, int i) {
    while(s[i] == ' ' && i < s.length() - 1) i++;
    return i;
}

int parse_string(std::string& s, object_t& obj, std::string& key, int i) {
    int start;
    int end;
    std::string v;

    start = i;

    while(s[start] != '"' && start < s.length() - 1) start++;
    end = start + 1;
    while(s[end] != '"' && end < s.length() - 1) end++;

    v = s.substr(start + 1, end-start - 1);
    obj[key] = value(v);

    return end;
}

int parse_number(std::string& s, object_t& values, std::string& key, int i) {
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
    values[key] = value(num);

    return i-1;
    
}

int parse_array(std::string& s, object_t& obj, std::string& key, int i) {
    array_t arr;

    while(s[i] != ']' && i < s.length() - 1) {
        std::istringstream ss;
        std::string str;
        number_t num;
        value val;

        i++;
        i = skip_whitespaces(s, i);
        
        if (s[i] == '"') {
            int start = ++i;

            while(s[i] != '"' && i < s.length() - 1) i++;
            
            val = value(s.substr(start, i-start));
        } else if (s[i] >= '0' && s[i] <= '9') {
            int start = i;
            
            while((s[i] >= '0' && s[i] <= '9') && i < s.length() - 1) i++; 

            ss = std::istringstream(s.substr(start, i-start));
            ss >> num;
            val = value(num);  
            i -= 1;      
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

    obj[key] = value(arr);

    return i;
}

int parse_object(std::string& s, object_t& obj, std::string& key, int i) {
    int end, current_i;
    std::string new_obj_key;
    object_t new_obj;

    end = current_i = i;

    while(s[current_i] != '}') {
        //current_i++;
        //current_i = skip_whitespaces(s, current_i);
        //end = current_i;

        while(s[current_i] != '"' && current_i < s.length() - 1) current_i++;
        end = current_i + 1;
        while(s[end] != '"' && end < s.length() - 1) end++;

        current_i = get_key(s, new_obj_key, current_i);
        current_i = skip_whitespaces(s, end + 1);
        
        if (s[current_i] == ':') {
            current_i = skip_whitespaces(s, current_i + 1);
          
            if (s[current_i] == '"') {
                current_i = parse_string(s, new_obj, new_obj_key, current_i);
            } else if (s[current_i] >= 0x30 && s[current_i] <= 0x39) {
                current_i = parse_number(s, new_obj, new_obj_key, current_i);
            } else if (s.substr(current_i, 4) == "true")  { 
                current_i += 3;
                new_obj[new_obj_key] = value(true);
            } else if (s.substr(current_i, 5) == "false") {
                current_i += 4;
                new_obj[new_obj_key] = value(false);
            } else if (s[current_i] == '[') {
                current_i = parse_array(s, new_obj, new_obj_key, current_i);
            } else if (s.substr(current_i, 4) == "null") {
                new_obj[new_obj_key] = value(nullptr);
            } else if (s[current_i] == '{') {
                current_i = parse_object(s, new_obj, new_obj_key, current_i);
            } else {
                std::cout << "else: " << s[current_i] << std::endl;
                break;
            }
        } else {
            current_i = skip_whitespaces(s, current_i);
            std::cout << "err: cannot find coron" << s[current_i] << std::endl;
        }

        current_i++;
        current_i = skip_whitespaces(s, current_i);
    }
    
    obj[key] = value(new_obj);

    return current_i;
}



// void parse(std::string& s, std::map<std::string, value>& values, int i=0) {
//     int ki_start = i;
//     int ki_end = i;
//     int current_i = i;
//     std::string key = "root";

//     // if (s[current_i] == '{') {
//     //     current_i = parse_object(s, values, key, current_i);
//     // } else if (s[current_i] == '[') {
//     //     current_i = parse_array(s, values, key, current_i);
//     // } else if (s[current_i] == '"') {
//     //     current_i = parse_string(s, values, key, current_i);
//     // } else if (s[current_i] >= 0x30 && s[current_i] <= 0x39) {
//     //     current_i = parse_number(s, values, key, current_i);
//     // } else if (s.substr(current_i, 4) == "true") {

//     // } else if (s.substr(current_i, 5) == "false") {

//     // } else if (s.substr(current_i, 4) == "null") {

//     // } else {
//     //     std::cout << "err at parse();" << std::endl;
//     // }

//     //ki_end = ki_start = current_i;
//     // start from {
    
//     while(s[current_i] != '\0') {
//         // skip spaces till '"'
//         //current_i = skip_whitespaces(s, current_i);
//         ki_end = ki_start = current_i;

//         while(s[ki_start] != '"' && ki_start < s.length() - 1) ki_start++;
//         ki_end = ki_start + 1;
//         while(s[ki_end] != '"' && ki_end < s.length() - 1) ki_end++;

//         current_i = get_key(s, key, ki_start);
    
//         current_i = skip_whitespaces(s, ki_end + 1);
        
//         if (s[current_i] == ':') {
//             current_i = skip_whitespaces(s, current_i + 1);
          
//             if (s[current_i] == '"') {
//                 current_i = parse_string(s, values, key, current_i);
//             } else if (s[current_i] >= 0x30 && s[current_i] <= 0x39) {
//                 current_i = parse_number(s, values, key, current_i);
//             } else if (s.substr(current_i, 4) == "true")  { 
//                 current_i += 3;
//                 values[key] = value(true);
//             } else if (s.substr(current_i, 5) == "false") {
//                 current_i += 4;
//                 values[key] = value(false);
//             } else if (s[current_i] == '[') {
//                 current_i = parse_array(s, values, key, current_i);
//             } else if (s.substr(current_i, 4) == "null") {
//                 values[key] = value(nullptr);
//             } else if (s[current_i] == '{') {
//                 current_i = parse_object(s, values, key, current_i);
//             } else {
//                 std::cout << "else: " << s[current_i] << std::endl;
//                 break;
//             }
//         } else {
//             std::cout << "not : " << s[current_i] << std::endl;
//         }
        
//         // skip last "not space value" 
//         current_i++;
//         // increment index at ","
//         current_i = skip_whitespaces(s, current_i);
//         // skip ","
//         //current_i++;
//     }
// }

void parse(std::string& s, object_t& root, int i = 0) {
    std::string key = "root";

    if (s[i] == '{') {
        i = parse_object(s, root, key, i);
    } else if (s[i] == '[') {
        i = parse_array(s, root, key, i);
    } else if (s[i] == '"') {
        i = parse_string(s, root, key, i);
    } else if (s[i] >= 0x30 && s[i] <= 0x39) {
        i = parse_number(s, root, key, i);
    } else if (s.substr(i, 4) == "true") {
        root[key] = value(true);
    } else if (s.substr(i, 5) == "false") {
        root[key] = value(false);
    } else if (s.substr(i, 4) == "null") {
        root[key] = value(nullptr);
    } else {
        std::cout << "err at parse();" << std::endl;
    }
}

void test() {
    
}

int main() {
    std::string json1 = R"({"hoge": "huga", "hogeint": 543, "hugaint": 5.232, "piyoint": 1e9, 
                        "hogebool": true, "piyo": "piyoyo", "arr": [10, 20, "arrstr", false], 
                        "obj": { "objstr": "obobob", "objdouble": 1.234, "objbool": true, 
                        "objarr": [2,"ddd", false], "objobj": {"str": "stroj"}}})";
    std::string json2 = R"(["json2str": "hogehugapiyo", "json2num": 2.313, "json2bool": true])";
    
    object_t root;
    object_t values;

    parse(json1, root);

    values = root["root"].get_obj();

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

    std::vector<value> aarr = obj["objarr"].get_arr();
    std::cout << "aarr: ";
    for (int i = 0; i < aarr.size(); i++) {
        type_t t = aarr[i].get_type();

        if (t == string_type) {
            std::cout << aarr[i].get_str();
        } else if (t == number_type) {
            std::cout << aarr[i].get_num();
        } else if (t == boolean_type) {
            std::cout << (aarr[i].get_bool() ? "true" :  "false");
        }

        if (i != aarr.size() -1) {
            std::cout << ",";
        } else {
            std::cout << "\n";
        }
    }
   
    return 1;
} 