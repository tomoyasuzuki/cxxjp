#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace cxxjp {
    class value;

    using number_t = double;
    using boolean = bool;
    using array_t = std::vector<value>;
    using object_t = std::map<std::string, value>;
    
    enum error_t {
        unknown_error,
        syntax_error,
        success
    };
    
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
            boolean bol;
            nullptr_t nul;
            array_t arr;
            object_t obj;
        public:
            value() : type(unknown) {};
            explicit value(std::string s) : type(string_type), str(s) {};
            explicit value(char *c) : type(string_type), str(std::string(c)) {};
            explicit value(number_t n) : type(number_type), num(n) {};
            explicit value(boolean b) : type(boolean_type), bol(b) {};
            explicit value(nullptr_t n) : type(null_type), nul(n) {};
            explicit value(array_t& a) : type(array_type), arr(a) {};
            explicit value(object_t& o) : type(object_type), obj(o) {};
            ~value() {};
            template <typename T> T& get() const {
                if (type == string_type) {
                    return *(T*)(void*)(&str);
                } else if (type == number_type) {
                    return *(T*)(void*)(&num);
                } else if (type == boolean_type) {
                    return *(T*)(void*)(&bol);
                } else if (type == null_type) {
                    return *(T*)(void*)(&nul);
                } else if (type == array_type) {
                    return *(T*)(void*)(&arr);
                } else if (type == object_type) {
                    return *(T*)(void*)(&obj);
                } else {
                    return *(T*)(void*)nullptr;
                }
            }
            const std::string& operator=(const char *s) {
                str = std::string(s);
                type = string_type;
                return str;
            };
            const number_t& operator=(number_t d) {
                num = d;
                type = number_type;
                return num;
            }
            const number_t& operator=(int d) {
                num = static_cast<double>(d);
                type = number_type;
                return num;
            };
            const array_t& operator=(std::vector<value> a) {
                arr = a;
                type = array_type;
                return arr;
            }
            const boolean& operator=(boolean b) {
                bol = b;
                type = boolean_type;
                return bol;
            };

            std::string dump() const;
    };

    error_t parse_object(const std::string& s, object_t& obj, int& i);

    std::string value::dump() const {
        std::stringstream ss;
       if (type == string_type) {
           ss << '"' << str << '"';
       } else if (type == number_type) {
           ss << num; 
       } else if (type == boolean_type) {
           ss << (bol ? "true" : "false");
       } else if (type == null_type) {
           ss << "null";
       } else if (type == array_type) {
           ss << "[";
           for (int i = 0; i < arr.size(); ++i) {
               if (arr[i].type == string_type) {
                   ss << '"' << arr[i].str << '"';
               } else if (arr[i].type == number_type) {
                   ss << arr[i].num;
               } else if (arr[i].type == boolean_type) {
                   ss << (arr[i].bol ? "true" : "false");
               } else if (arr[i].type == null_type) {
                   ss << "null";
               } else if (arr[i].type == array_type) {
                   ss << arr[i].dump();
               } else if (arr[i].type == object_type) {
                   ss << arr[i].dump();
               }

               if (i != arr.size() - 1) ss << ", ";
           }
           ss << "]";
       } else if (type == object_type) { 
           ss << "{";
           for (auto i = obj.begin(); i != obj.end(); i++) {
               ss << "\"" << i->first << "\"" << ": ";
               
               if (i->second.type == string_type) {
                   ss << '"' << i->second.str << '"';
               } else if (i->second.type == number_type) {
                   ss << i->second.num;
               } else if (i->second.type == boolean_type) {
                   ss << (i->second.bol ? "true" : "false");
               } else if (i->second.type == null_type) {
                   ss << "null";
               } else if (i->second.type == array_type) {
                   ss << i->second.dump();
               } else if (i->second.type == object_type) {
                   ss << i->second.dump();
               }
               
               if (++i != obj.end()) ss << ", ";
               --i;
           }
           ss << "}";
       }

       return ss.str();
    };

    std::string get_key(const std::string& s, int& current_i) {
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

    static inline int skip_whitespaces(const std::string& s, int i) { while(s[i] == ' ' && i < s.length() - 1) i++; return i; };
    static inline int skip_first_non_space_or_comma(int i) { return ++i; };
    static inline int skip_last_non_space(int i) { return ++i; };

    error_t parse_string(const std::string& s, std::string& v, int& i) {
        int start;

        start = i;

        while(s[start] != '"' && start < s.length() - 1) start++;
        i = start + 1;
        while(s[i] != '"' && i < s.length() - 1) i++;

        v = s.substr(start + 1, i - start - 1);

        return success;
    }

    error_t parse_number(const std::string& s, number_t& num, int& i) {
        int start;
        std::string substr;
        std::istringstream ss;
        error_t err = success;

        start = i;
        
        if (s[i] < 0x30 || s[i] > 0x39) {
            return syntax_error;
        }

        while((s[i+1] >= '0' && s[i+1] <= '9') && i < s.length() || s[i+1] == '.' || s[i+1] == 'e') ++i;

        substr = s.substr(start, i-start+1);
        ss = std::istringstream(substr);
        ss >> num;

        return err;
        
    }

    error_t parse_array(const std::string& s, array_t& arr, int& i) {
        error_t err = success;

        if (s[i+1] == ']') {
            i++;
            return err;
        }

        while(s[i] != ']' && i < s.length() - 1) {
            std::istringstream ss;
            value v;

            i = skip_first_non_space_or_comma(i);
            i = skip_whitespaces(s, i);
            
            if (s[i] == '{') {
                object_t obj;
                err = parse_object(s, obj, i);
                if (err != success) {
                    return err;
                }
                v = value(obj);
            }  else if (s[i] == '[') {
                array_t arr;
                err = parse_array(s, arr, i);
                if (err != success) {
                    return err;
                }
                v = value(arr);
            } else if (s[i] == '"') {
                int start = ++i;

                while(s[i] != '"' && i < s.length() - 1) i++;
                
                v = value(s.substr(start, i-start));
            } else if (s[i] >= '0' && s[i] <= '9') {
                number_t num;
                int start = i;
                
                while((s[i+1] >= '0' && s[i+1] <= '9' || s[i+1] == '.' || s[i+1] == 'e') && i+1 < s.length() - 1) ++i; 

                ss = std::istringstream(s.substr(start, i-start+1));
                ss >> num;
                v = value(num);   
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
                return syntax_error;
            }

            arr.push_back(v);

            i = skip_last_non_space(i);
            i = skip_whitespaces(s, i);
        }

        return success;
    }

    error_t parse_object(const std::string& s, object_t& obj, int& i) {
        std::string key;
        error_t err = success;

        if (s[i+1] == '}') {
            i++;
            return err;
        }

        while(s[i] != '}') {
            key = get_key(s, i);
            i = skip_whitespaces(s, i);
            
            if (s[i] == ':') {
                i = skip_whitespaces(s, i + 1);
            
                if (s[i] == '"') {
                    std::string str = "";
                    err = parse_string(s, str, i);
                    if (err != success) {
                        return err;
                    }
                    obj[key] = value(str);
                } else if (s[i] >= 0x30 && s[i] <= 0x39) {
                    number_t num = 0;
                    err = parse_number(s, num, i);
                    if (err != success) {
                        return err;
                    }
                    obj[key] = value(num);
                } else if (s.substr(i, 4) == "true")  { 
                    i += 3;
                    obj[key] = value(true);
                } else if (s.substr(i, 5) == "false") {
                    i += 4;
                    obj[key] = value(false);
                } else if (s[i] == '[') {
                    array_t arr;
                    err = parse_array(s, arr, i);
                    if (err != success) {
                        return err;
                    }
                    obj[key] = value(arr);
                } else if (s.substr(i, 4) == "null") {
                    obj[key] = value(nullptr);
                } else if (s[i] == '{') {
                    object_t object;
                    err = parse_object(s, object, i);
                    if (err != success) {
                        return err;
                    }
                    obj[key] = value(object);
                } else {
                    return syntax_error;
                }
            } else {
                i = skip_whitespaces(s, i);
                return syntax_error;
            }

            i = skip_last_non_space(i);
            i = skip_whitespaces(s, i);
        }

        return err;
    }

    error_t parse(std::string& s, value& v, int i = 0) {
        error_t err = success;
        if (s[i] == '{') {
            object_t object;
            err = parse_object(s, object, i);
            if (err != success) {
                return err;
            }
            v = value(object);
        } else if (s[i] == '[') {
            array_t arr;
            err = parse_array(s, arr, i);
            if (err != success) {
                return err;
            }
            v = value(arr);
        } else if (s[i] == '"') {
            std::string str;
            err = parse_string(s, str, i);
            if (err != success) {
                return err;
            }
            v = value(str);
        } else if (s[i] >= 0x30 && s[i] <= 0x39) {
            number_t num;
            err = parse_number(s, num, i);
            if (err != success) {
                return err;
            }
            v = value(num);
        } else if (s.substr(i, 4) == "true") {
            v = value(true);
        } else if (s.substr(i, 5) == "false") {
            v = value(false);
        } else if (s.substr(i, 4) == "null") {
            v = value(nullptr);
        } else {
            err = syntax_error;
        }

        return err;
    }

    std::string err_message(error_t err) {
        if (err == unknown_error) {
            return "unknown error";
        } else if (err == syntax_error) {
            return "syntax error";
        } else {
            return "success";
        }
    } 
}