# cxxjp
cxxjp is a very simple JSON parser written in C++. 

This project is for hobbyist use only and should not be used in a production environment.

## example 
```c++
#include <iostream>
#include "cxxjp.h"

int main() {
    std::string json = R"({"name": "Tom", "age": 23, "double1": 5.232, "double2": 1e9, "null": null,
                           "boolean": true, "array": [10, {}],  "object": {"string": "object_string", "object_double": 1.234, 
                           "object_boolean": true, "object_array": [2,"hogehuga"], "object_object": {"hoge": "huga"}}})";
    
    cxxjp::value value;
    cxxjp::error_t err;

    err = parse(json, value);
    if (err != cxxjp::success) {
        std::cerr << "Failed: " << cxxjp::err_message(err) << std::endl;
        exit(2);
    }
    
    cxxjp::object_t object;
    cxxjp::array_t array;


    // Dump 
    std::cout << "json = " << value.dump() << std::endl;

    // Read
    object = value.get<cxxjp::object_t>();
    array = object["array"].get<cxxjp::array_t>();

    std::cout << "json[\"name\"] = " << object["name"].get<std::string>() << std::endl;
    std::cout << "json[\"double1\"] = " << object["double1"].get<cxxjp::number_t>() << std::endl;
    std::cout << "json[\"boolean\"] = " << (object["boolean"].get<cxxjp::boolean>() ? "true" : "false") << std::endl;
    std::cout << "json[\"null\"] = " << (object["null"].get<nullptr_t>() == nullptr ? "null" : "") << std::endl;
    std::cout << "json[\"array\"] = " << object["array"].dump() << std::endl;

    // Write
    object["name"] = "Mike";
    object["double1"] = 2.334;
    object["boolean"] = false;

    std::cout << "json1[\"name\"] = " << object["name"].get<std::string>() << std::endl;
    std::cout << "json[\"double1\"] = " << object["double1"].get<cxxjp::number_t>() << std::endl;
    std::cout << "json[\"boolean\"] = " << (object["boolean"].get<cxxjp::boolean>() ? "true" : "false") << std::endl;
    
    return 0;
} 
```
```
json = {"age": 23, "array": [10, {}], "boolean": true, "double1": 5.232, 
        "double2": 1e+09, "name": "Tom", "null": null, "object": {"object_array": [2, "hogehuga"], 
        "object_boolean": true, "object_double": 1.234, "object_object": {"hoge": "huga"}, "string": "object_string"}}
json["name"] = Tom
json["double1"] = 5.232
json["boolean"] = true
json["null"] = null
json["array"] = [10, {}]
json1["name"] = Mike
json["double1"] = 2.334
json["boolean"] = false
```

## TODO
- Implement other useful member functions.
- Support for file input.
- Support for escape character.
- Support for error message.
