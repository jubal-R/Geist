//Copyright (C) 2016  Jubal
#include "templates.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

Templates::Templates()
{
}

string asmb = "section .text\n\tglobal _start:\n_start:\n\nsection .data\n";
string c = "#include <stdio.h>\n\nint main(int argc,char *argv[]){\n\t\n\n\treturn 0;\n}";
string cplusplus = "#include <iostream>\n\nusing namespace std;\n\nint main(){\n\t\n\n\treturn 0;\n}";
string html = "<!DOCTYPE html>\n<head lang=\"en-US\">\n\t<meta charset=\"UTF-8\">\n\t<link rel=\"stylesheet\" href=\"\" />\n\t<title></title>\n\n\t<script>\n\t\t\n\t</script>\n</head>\n<body>\n\t<header>\n\t\t\n\t</header>\n\n</body>\n</html>";
string java = "public class name {\n\n\tpublic static void main(String args[]){\n\t\t\n\t}\n\n}";
string php = "<?php\n\n\t\n\n?>";
string css = "* {\n\t\n}\n\nhtml {\n\t\n}\n\nbody {\n\tdisplay: block;\n\t\n}\n\nh1 {\n\t\n}\n\np {\n\t\n}\n\na {\n\t\n}\n\nimg {\n\t\n}";

string languages[7] = {asmb, c, cplusplus, html, java, php, css};

string Templates::getTemplate(int e){
    return languages[e];
}
