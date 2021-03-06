#include "generated/lexer_odin.h"
#include "generated/lexer_odin.cpp"

// Common
function b32 odin_is_builtin_type(Token *token)
{
    return TokenOdinKind_byte <= token->sub_kind &&
        token->sub_kind <= TokenOdinKind_u128be;
}

function b32 odin_is_builtin_proc(Token *token)
{
    return TokenOdinKind_len <= token->sub_kind &&
        token->sub_kind <= TokenOdinKind_card;
}

// Index
#include "languages/odin/index.cpp"

// Highlight
static FColor odin_get_token_color(Token token)
{
    Managed_ID color = defcolor_text_default;
    switch (token.kind){
        case TokenBaseKind_Preprocessor:
        {
            color = defcolor_preproc;
        } break;
        case TokenBaseKind_Keyword:
        {
            color = defcolor_keyword; break;
        } break;
        case TokenBaseKind_Comment:
        {
            color = defcolor_comment;
        } break;
        case TokenBaseKind_LiteralString:
        {
            color = defcolor_str_constant;
        } break;
        case TokenBaseKind_LiteralInteger:
        {
            color = defcolor_int_constant;
        } break;
        case TokenBaseKind_LiteralFloat:
        {
            color = defcolor_float_constant;
        } break;
        case TokenBaseKind_Identifier:
        {
            if (odin_is_builtin_type(&token))
                color = defcolor_type_name;
            else if (odin_is_builtin_proc(&token))
                color = defcolor_function_name;
        }break;
    }
    return(fcolor_id(color));
}

// Jump
function Parsed_Jump odin_parse_jump_location(String_Const_u8 line)
{
    Parsed_Jump jump = {};
    
    line = string_skip_chop_whitespace(line);
    u64 lparen_pos = string_find_first(line, '(');
    u64 colon_pos  = string_find_first(string_skip(line, lparen_pos), ':')+lparen_pos;
    u64 rparen_pos = string_find_first(string_skip(line, colon_pos), ')')+colon_pos;
    
    String_Const_u8 file_name     = string_prefix(line, lparen_pos);
    String_Const_u8 line_number   = string_skip(string_prefix(line, colon_pos), lparen_pos+1);
    String_Const_u8 column_number = string_skip(string_prefix(line, rparen_pos), colon_pos+1);
    
    if (file_name.size > 0 && line_number.size > 0 && column_number.size > 0)
    {
        jump.location.file   = file_name;
        jump.location.line   = (i32)string_to_integer(line_number, 10);
        jump.location.column = (i32)string_to_integer(column_number, 10);
        jump.colon_position = (i32)(rparen_pos);
        jump.success = true;
    }
    
    if (!jump.success)
        block_zero_struct(&jump);
    else
        jump.is_sub_jump = false; // @note(tyler): What is this for?
    
    return jump;
}

String_Const_u8 odin_comment_delims[3] = {SCu8("//"), SCu8("/*"), SCu8("*/")};