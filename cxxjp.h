#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace cxxjp {
    class value;

    using number = double;
    using boolean = bool;
    using array = std::vector<value>;
    using object = std::map<std::string, value>;
    
    enum class error {
        unknown_error,
        syntax_error,
        success
    };
    
    enum class type {
        unknown_type,
        string_type,
        numberype,
        boolean_type,
        arrayype,
        objectype,
        null_type
    };
    
    class value {
        private: 
            type t;
            std::string str;
            number num;
            boolean bol;
            nullptr_t nul;
            array arr;
            object obj;
        public:
            value() : t(type::unknown_type) {};
            explicit value(std::string s) : t(type::string_type), str(s) {};
            explicit value(char *c) : t(type::string_type), str(std::string(c)) {};
            explicit value(number n) : t(type::numberype), num(n) {};
            explicit value(boolean b) : t(type::boolean_type), bol(b) {};
            explicit value(nullptr_t n) : t(type::null_type), nul(n) {};
            explicit value(array& a) : t(type::arrayype), arr(a) {};
            explicit value(object& o) : t(type::objectype), obj(o) {};
            ~value() {};
            template <typename T> T& get() const {
                if (t == type::string_type) {
                    return *(T*)(void*)(&str);
                } else if (t == type::numberype) {
                    return *(T*)(void*)(&num);
                } else if (t == type::boolean_type) {
                    return *(T*)(void*)(&bol);
                } else if (t == type::null_type) {
                    return *(T*)(void*)(&nul);
                } else if (t == type::arrayype) {
                    return *(T*)(void*)(&arr);
                } else if (t == type::objectype) {
                    return *(T*)(void*)(&obj);
                } else {
                    return *(T*)(void*)nullptr;
                }
            }
            const std::string& operator=(const char *s) {
                str = std::string(s);
                t = type::string_type;
                return str;
            };
            const number& operator=(number d) {
                num = d;
                t = type::numberype;
                return num;
            }
            const number& operator=(int d) {
                num = static_cast<double>(d);
                t = type::numberype;
                return num;
            };
            const array& operator=(std::vector<value> a) {
                arr = a;
                t = type::arrayype;
                return arr;
            }
            const boolean& operator=(boolean b) {
                bol = b;
                t = type::boolean_type;
                return bol;
            };

            std::string dump() const;
    };

    static inline int skip_whitespaces(const std::string& s, int i) { while(s[i] == ' ' && i < s.length() - 1) i++; return i; };
    static inline int skip_first_non_space_or_comma(int i) { return ++i; };
    static inline int skip_last_non_space(int i) { return ++i; };
    static inline bool is_overflow(const std::string& s, const int& i) { return (i > s.size()-1 ? true : false); };

    error parse_object(const std::string& s, object& obj, int& i);

    std::string value::dump() const {
        std::stringstream ss;
       if (t == type::string_type) {
           ss << '"' << str << '"';
       } else if (t == type::numberype) {
           ss << num; 
       } else if (t == type::boolean_type) {
           ss << (bol ? "true" : "false");
       } else if (t == type::null_type) {
           ss << "null";
       } else if (t == type::arrayype) {
           ss << "[";
           for (int i = 0; i < arr.size(); ++i) {
               if (arr[i].t == type::string_type) {
                   ss << '"' << arr[i].str << '"';
               } else if (arr[i].t == type::numberype) {
                   ss << arr[i].num;
               } else if (arr[i].t == type::boolean_type) {
                   ss << (arr[i].bol ? "true" : "false");
               } else if (arr[i].t == type::null_type) {
                   ss << "null";
               } else if (arr[i].t == type::arrayype) {
                   ss << arr[i].dump();
               } else if (arr[i].t == type::objectype) {
                   ss << arr[i].dump();
               }

               if (i != arr.size() - 1) ss << ", ";
           }
           ss << "]";
       } else if (t == type::objectype) { 
           ss << "{";
           for (auto i = obj.begin(); i != obj.end(); i++) {
               ss << "\"" << i->first << "\"" << ": ";
               
               if (i->second.t == type::string_type) {
                   ss << '"' << i->second.str << '"';
               } else if (i->second.t == type::numberype) {
                   ss << i->second.num;
               } else if (i->second.t == type::boolean_type) {
                   ss << (i->second.bol ? "true" : "false");
               } else if (i->second.t == type::null_type) {
                   ss << "null";
               } else if (i->second.t == type::arrayype) {
                   ss << i->second.dump();
               } else if (i->second.t == type::objectype) {
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
        int start = current_i;

        while(s[start] != '"' && !is_overflow(s,start)) start++;
        current_i = start + 1;
        while(s[current_i] != '"' && !is_overflow(s,current_i)) current_i++;
        
        key = s.substr(start + 1, current_i - start - 1);

        ++current_i;
        return key;
    }

    error parse_string(const std::string& s, std::string& v, int& end) {
        int start = end;

        while(s[start] != '"' && !is_overflow(s,start)) ++start;
        end = start + 1;
        while(s[end] != '"' && !is_overflow(s,end)) ++end;

        v = s.substr(start + 1, end-start-1);

        return error::success;
    }

    error parse_number(const std::string& s, number& num, int& i) {
        int start = i;
        std::string substr;
        std::istringstream ss;
        error err = error::success;
        
        if (s[i] < '0' || s[i] > '9') { return error::syntax_error; };

        while(((s[i+1] >= '0' && s[i+1] <= '9') || s[i+1] == '.' || s[i+1] == 'e') && !is_overflow(s,i)) ++i;

        substr = s.substr(start, i-start+1);
        ss = std::istringstream(substr);
        ss >> num;

        return err;
        
    }

    error parse_array(const std::string& s, array& arr, int& i) {
        error err = error::success;

        if (s[i+1] == ']') { ++i; return err; };

        while(s[i] != ']' && !is_overflow(s,i)) {
            std::istringstream ss;
            value v;

            i = skip_first_non_space_or_comma(i);
            i = skip_whitespaces(s, i);
            
            if (s[i] == '{') {
                object obj;
                err = parse_object(s, obj, i);
                if (err != error::success) {
                    return err;
                }
                v = value(obj);
            }  else if (s[i] == '[') {
                array arr;
                err = parse_array(s, arr, i);
                if (err != error::success) {
                    return err;
                }
                v = value(arr);
            } else if (s[i] == '"') {
                int start = ++i;

                while(s[i] != '"' && !is_overflow(s,i)) ++i;
                
                v = value(s.substr(start, i-start));
            } else if (s[i] >= '0' && s[i] <= '9') {
                number num;
                int start = i;
                
                while((s[i+1] >= '0' && s[i+1] <= '9' || s[i+1] == '.' || s[i+1] == 'e') && !is_overflow(s,i+1)) ++i; 

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
                return error::syntax_error;
            }

            arr.push_back(v);

            i = skip_last_non_space(i);
            i = skip_whitespaces(s, i);
        }

        return error::success;
    }

    error parse_object(const std::string& s, object& obj, int& i) {
        std::string key;
        error err = error::success;

        if (s[i+1] == '}') { ++i; return err; };

        while(s[i] != '}') {
            key = get_key(s, i);
            i = skip_whitespaces(s, i);
            
            if (s[i] == ':') {
                i = skip_whitespaces(s, i + 1);
            
                if (s[i] == '"') {
                    std::string str = "";
                    err = parse_string(s, str, i);
                    if (err != error::success) {
                        return err;
                    }
                    obj[key] = value(str);
                } else if (s[i] >= '0' && s[i] <= '9') {
                    number num = 0;
                    err = parse_number(s, num, i);
                    if (err != error::success) {
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
                    array arr;
                    err = parse_array(s, arr, i);
                    if (err != error::success) {
                        return err;
                    }
                    obj[key] = value(arr);
                } else if (s.substr(i, 4) == "null") {
                    obj[key] = value(nullptr);
                } else if (s[i] == '{') {
                    object object;
                    err = parse_object(s, object, i);
                    if (err != error::success) {
                        return err;
                    }
                    obj[key] = value(object);
                } else {
                    return error::syntax_error;
                }
            } else {
                i = skip_whitespaces(s, i);
                return error::syntax_error;
            }

            i = skip_last_non_space(i);
            i = skip_whitespaces(s, i);
        }

        return err;
    }

    error parse(std::string& s, value& v, int i = 0) {
        error err = error::success;
        if (s[i] == '{') {
            object object;
            err = parse_object(s, object, i);
            if (err != error::success) {
                return err;
            }
            v = value(object);
        } else if (s[i] == '[') {
            array arr;
            err = parse_array(s, arr, i);
            if (err != error::success) {
                return err;
            }
            v = value(arr);
        } else if (s[i] == '"') {
            std::string str;
            err = parse_string(s, str, i);
            if (err != error::success) {
                return err;
            }
            v = value(str);
        } else if (s[i] >= 0x30 && s[i] <= 0x39) {
            number num;
            err = parse_number(s, num, i);
            if (err != error::success) {
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
            err = error::syntax_error;
        }

        return err;
    }

    std::string err_message(const error err) {
        if (err == error::unknown_error) {
            return "unknown error";
        } else if (err == error::syntax_error) {
            return "syntax error";
        } else {
            return "success";
        }
    } 
}