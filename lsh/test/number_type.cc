#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

int main(void) {

    std::string json = R"({"a": 0, "b": 0.1})";

    std::cin.tie(0);
    std::ios::sync_with_stdio(false);

    while (1) {
        rapidjson::Document doc;
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        // getline(std::cin, json);

        doc.Parse(json.c_str());

        doc.Accept(writer);

        printf("%s\n", buffer.GetString());
        printf("GetInt\"a\": %d\n", doc["a"].GetInt());
        printf("GetDouble\"a\": %f\n", doc["a"].GetDouble());
    }

    return 0;
}
