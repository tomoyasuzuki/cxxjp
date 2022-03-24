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
    std::string json2 = R"(["hoge", "huga", 2.313, true, [10], {"object": "object_string"}])";
    std::string json3 = R"("hoge")";
    std::string json4 = R"(100)";
    std::string json5 = R"(false)";
    std::string json6 = R"(null)";
    
    cxxjp::value value1, value2, value3, value4, value5, value6;

    cxxjp::object_t object;
    cxxjp::array_t array;
    std::string string;
    cxxjp::number_t number;
    bool boolean;
    nullptr_t null;

    parse(json1, value1);
    parse(json2, value2);
    parse(json3, value3);
    parse(json4, value4);
    parse(json5, value5);
    parse(json6, value6);

    // Dump 
    std::cout << value1.dump() << std::endl;
    std::cout << value2.dump() << std::endl;
    std::cout << value3.dump() << std::endl;
    std::cout << value4.dump() << std::endl;
    std::cout << value5.dump() << std::endl;
    std::cout << value6.dump() << std::endl;

    // Read
    object = value1.get<cxxjp::object_t>();
    array = value2.get<cxxjp::array_t>();
    string = value3.get<std::string>();
    number = value4.get<cxxjp::number_t>();
    boolean = value5.get<bool>();
    null = value6.get<nullptr_t>();

    std::cout << "json1[\"string\"] = " << object["string"].get<std::string>() << std::endl;
    std::cout << "json2[4] = " << array[4].dump() << std::endl;
    std::cout << "json3 = " << '"' << string << '"' << std::endl;
    std::cout << "json4 = " << number << std::endl;
    std::cout << "json5 = " << (boolean ? "true" : "false") << std::endl;
    std::cout << "json6 = " << (null == nullptr ? "null" : "") << std::endl;

    // Write
    WIP
    
    return 0;
} 
```

## TODO
- Enable using only the header-file.
- Implement other useful member functions.
- Support for file input.
- Support for escape character.
- Support for cxxjp namespace.
- Support for error message.
