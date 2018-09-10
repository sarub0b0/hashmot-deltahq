#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

int main(void) {

    std::string json;

    std::cin.tie(0);
    std::ios::sync_with_stdio(false);

    rapidjson::Document doc;
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    doc.SetObject();
    doc.AddMember("init", 10, doc.GetAllocator());

    doc.Accept(writer);
    printf("%s\n", buffer.GetString());

    doc["init"] = 5;

    doc.Accept(writer);
    printf("%s\n", buffer.GetString());

    return 0;
}
