#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <sstream>


namespace cxxjp {
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

    std::string get_key(std::string& s, int& current_i) {
        std::string key;
        int start;

        start = current_i;

        while(s[start] != '"' && start < s.length() - 1) start++;
        current_i = start + 1;
        while(s[current_i] != '"' && current_i < s.length() - 1) current_i++;
        
        key = s.substr(start + 1, current_i - start - 1);

        current_i++;
        return key;
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

        while((s[i] >= '0' && s[i] <= '9') && i < s.length() || s[i] == '.' || s[i] == 'e') i++;

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
                
                while((s[i] >= '0' && s[i] <= '9' || s[i] == '.' || s[i] == 'e') && i < s.length() - 1) i++; 

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
            key = get_key(s, i);
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
            array_t arr = parse_array(s, i);
            v = value(arr);
        } else if (s[i] == '"') {
            std::string str = parse_string(s, i);
            v = value(str);
        } else if (s[i] >= 0x30 && s[i] <= 0x39) {
            number_t num = parse_number(s, i);
            v = value(num);
        } else if (s.substr(i, 4) == "true") {
            v = value(true);
        } else if (s.substr(i, 5) == "false") {
            v = value(false);
        } else if (s.substr(i, 4) == "null") {
            v = value(nullptr);
        } else {
            std::cout << "err at parse();" << std::endl;
        }
    }

    void test() {

    }

    void print_array(array_t& arr) {
        std::cout << "[";

        for (int i = 0; i < arr.size(); i++) {
            type_t type = arr[i].get_type();
            if (type == string_type) {
                std::cout << '"' << arr[i].get_str() << '"';
            } else if (type == number_type) {
                std::cout << arr[i].get_num();
            } else if (type == boolean_type) {
                std::cout << (arr[i].get_bool() ? "true" : "false");
            } else if (type == null_type) {
                std::cout << "null";
            } else if (type == array_type) {
                array_t a = arr[i].get_arr();
                print_array(a);
            } else if (type == object_type) {
                std::cout << "{}";
            }

            if (i != arr.size() - 1) {
                std::cout << ", ";
            }
        }

        std::cout << "]";
    }
}


int main() {
    std::string json1 = R"({"string": "hoge", "int": 543, "double1": 5.232, "double2": 1e9, "null": null,
                        "boolean": true, "array": [10, 20, "string", false, {"string": "string"}], 
                        "object": { "string": "object_string", "object_double": 1.234, "object_boolean": true, 
                        "object_array": [2,"hogehuga", false, {"number": 234}], "object_object": {"hoge": "huga"}}})";
    std::string json2 = R"(["hoge", 2.313, true, [10], {"object": "object_string"}])";
    std::string json3 = R"("hoge")";
    std::string json4 = R"(100)";
    std::string json5 = R"(false)";
    std::string json6 = R"(null)";
    
    cxxjp::value v1, v2, v3, v4, v5, v6;

    cxxjp::object_t obj;
    cxxjp::array_t arr;
    std::string str;
    cxxjp::number_t num;
    bool bol;
    nullptr_t nul;

    parse(json1, v1);
    parse(json2, v2);
    parse(json3, v3);
    parse(json4, v4);
    parse(json5, v5);
    parse(json6, v6);

    obj = v1.get_obj();
    arr = v2.get_arr();
    str = v3.get_str();
    num = v4.get_num();
    bol = v5.get_bool();
    nul = v6.get_null();

    // ================= v1 test =========================

    // string
    std::cout << '"' << obj["string"].get_str() << '"' << std::endl;
    // number
    std::cout << obj["double1"].get_num() << std::endl;
    // boolean
    std::cout << (obj["boolean"].get_bool() ? "true" : "false") << std::endl;
    // null
    std::cout << (obj["null"].get_null() == nullptr ? "nullptr" : "") << std::endl;
    // array
    cxxjp::array_t a = obj["array"].get_arr();
    cxxjp::print_array(a);
    std::cout << "\n";
    // object
    cxxjp::object_t o = obj["obj"].get_obj();

    // ================= v2 test =========================
    std::cout << "v2 = ";
    print_array(arr);
    std::cout << "\n";
    // ================= v3 test =========================
    std::cout << "v3 = " << '"' << str << '"' << std::endl;
    // ================= v4 test =========================
    std::cout << "v4 =  " << num << std::endl;
    // ================= v5 test =========================
    std::cout << "v5 = " << (bol ? "true" : "false") << std::endl;
    // ================= v6 test =========================
    std::cout << "v6 = " << (nul == nullptr ? "nullptr" : "") << std::endl;

    
    return 1;
} 