#include "http_parser.h"
#include <stdio.h>

int main() {
    printf("%s\n", http_method_str((enum http_method) HTTP_ACL));
}
