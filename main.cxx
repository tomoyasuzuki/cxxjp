#include <iostream>
#include "cxxjp.h"

int main() {
    std::string json = R"({"name": "Tom", "age": 23, "double1": 5.232, "double2": 1e9, "null": null,
                           "boolean": true, "array": [10, {}],  "object": {"string": "object_string", "object_double": 1.234, 
                           "object_boolean": true, "object_array": [2,"hogehuga"], "object_object": {"hoge": "huga"}}})";
    
    cxxjp::value value;
    cxxjp::error err;

    err = parse(json, value);
    if (err != cxxjp::error::success) {
        std::cerr << "Failed: " << cxxjp::err_message(err) << std::endl;
        exit(2);
    }
    
    cxxjp::object object;
    cxxjp::array array;


    // Dump 
    std::cout << "json = " << value.dump() << std::endl;

    // Read
    object = value.get<cxxjp::object>();
    array = object["array"].get<cxxjp::array>();

    std::cout << "json[\"name\"] = " << object["name"].get<std::string>() << std::endl;
    std::cout << "json[\"double1\"] = " << object["double1"].get<cxxjp::number>() << std::endl;
    std::cout << "json[\"boolean\"] = " << (object["boolean"].get<cxxjp::boolean>() ? "true" : "false") << std::endl;
    std::cout << "json[\"null\"] = " << (object["null"].get<nullptr_t>() == nullptr ? "null" : "") << std::endl;
    std::cout << "json[\"array\"] = " << object["array"].dump() << std::endl;

    // Write
    object["name"] = "Mike";
    object["double1"] = 2.334;
    object["boolean"] = false;

    std::cout << "json1[\"name\"] = " << object["name"].get<std::string>() << std::endl;
    std::cout << "json[\"double1\"] = " << object["double1"].get<cxxjp::number>() << std::endl;
    std::cout << "json[\"boolean\"] = " << (object["boolean"].get<cxxjp::boolean>() ? "true" : "false") << std::endl;
    
    return 0;
} 