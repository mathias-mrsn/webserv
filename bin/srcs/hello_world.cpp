#include <iostream>

int
main (__attribute__((unused))int ac, __attribute__((unused))char **av, __attribute__((unused))char **env) {
    std::cout   << "X-Powered-By: HW/0.0.0.0.0.1\n"
                << "Content-type: text/html; charset=UTF-8\r\n\r\n";

    std::cout   << "<!DOCTYPE html>\n" 
                << "<html>\n"
                << "<head>\n"
                << "<title>Page Title</title>\n"
                << "</head>\n"
                << "<body>\n"
                << "<h1>Hello World</h1>\n"
                << "</body>\n"
                << "</html>";
    return (0);
}