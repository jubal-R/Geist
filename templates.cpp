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
string html = "<!doctype html>\n<head>\n\t<script type=\"text/javascript\">\n\t\n\t</script>\n</head>\n<body>\n\t\n</body>\n</html>";
string java = "public class name {\n\n\tpublic static void main(String args[]){\n\t\t\n\t}\n\n}";
string php = "<?php\n\t\n?>";
string css = "@font-face{}\n\nhtml\n{\n\n}\n\nbody\n{\n\n}\n\nh1, h2, h3, h4, h5, h6\n{\n\n}\n\np\n{\n\n}\n\na\n{\n\n}\n\nimg\n{\n\n}";

string languages[7] = {asmb, c, cplusplus, html, java, php, css};

string Templates::getTemplate(int e){
    return languages[e];
}
