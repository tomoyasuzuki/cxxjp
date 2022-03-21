# cxxjp
cxxjp is a very simple JSON parser written in C++. 

This project is for hobbyist use only and should not be used in a production environment.

## example 
```c++
int main() {
    std::string json1 = R"({"string": "hoge", "int": 543, "double1": 5.232, "double2": 1e9, "null": null,
                        "boolean": true, "array": [10, 20, "string", false, {"string": "string"}], 
                        "object": { "string": "object_string", "object_double": 1.234, "object_boolean": true, 
                        "object_array": [2,"hogehuga", false, {"number": 234}], "object_object": {"hoge": "huga"}}})";
 
    ...
    
    value v1;
    object_t obj;
  
    parse(json1, v1);
    
    obj = v1.get_obj();
   
    // string
    std::cout << '"' << obj["string"].get_str() << '"' << std::endl;
    // number
    std::cout << obj["double1"].get_num() << std::endl;
    // boolean
    std::cout << (obj["boolean"].get_bool() ? "true" : "false") << std::endl;
    // null
    std::cout << (obj["null"].get_null() == nullptr ? "nullptr" : "") << std::endl;
    // array
    array_t a = obj["array"].get_arr();
    print_array(a);
    std::cout << "\n";
    // WIP: object
    object_t o = obj["obj"].get_obj();
    

    ...
    
    return 1;
} 
```

## TODO
- Enable using only the header-file.
- Implement other useful member functions.
- Support for file input.
- Support for escape character.
- Support for cxxjp namespace.
- Support for error message.
