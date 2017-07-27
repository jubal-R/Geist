#ifndef TEMPLATES_H
#define TEMPLATES_H
#include <string>

class Templates
{
public:
    Templates();
    std::string getTemplate(int e);
private:
    std::string asmb;
    std::string c;
    std::string cplusplus;
    std::string html;
    std::string java;
    std::string css;

};

#endif // TEMPLATES_H
