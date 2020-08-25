
enum json_token_type
{
    json_object,
    json_array,
    json_string,
    json_number,
};
struct parse_ast
{
    int status;
};
struct json_parse_result
{
    char message[255];
    char error[255];
    parse_ast *ast;
    int status;
};
json_parse_result
jsonParse(char *json, size_t length)
{
    json_parse_result result = {};

    char c;

    // if(buf_info->fileInfo->size == 0)
    // {
    //     c = EOF;
    //     result.status = -1;
    //     strcpy(result.message, "Error: Parse error on line 1:");
    //     // sprintf(result.error, "Expected one of: %s, got %s",
    //     //         parseJSON_expecting(JSON_ANY), parseJSON_saw(JSON_EOF));
    // }

    return result;
}
