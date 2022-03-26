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