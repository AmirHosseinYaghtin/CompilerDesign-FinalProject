#include "Lexer.h"
#include "llvm/Support/raw_ostream.h"

// classifying characters
namespace charinfo
{
    // ignore whitespaces
    LLVM_READNONE inline bool isWhitespace(char c)
    {
        return (c == ' ' || c == '\t' || c == '\f' || c == '\v' || c == '\r' || c == '\n');
    }

    LLVM_READNONE inline bool isDigit(char c)
    {
        return (c >= '0' && c <= '9');
    }

    LLVM_READNONE inline bool isLetter(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    LLVM_READNONE inline bool isSpecialCharacter(char c)
    {
         return (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == '!' ||
         c == '>' || c == '<' || c == '(' || c == ')' || c == '{' || c == '}'|| c == ',' ||
         c == ';' || c == '%' || c == '^' || c == '[' || c == ']' || c == '?' || c == ':') ;
    }

    LLVM_READNONE inline bool isHash(char c)
    {
        return (c == '#');
    }

    LLVM_READNONE inline bool isDot(char c)
    {
        return (c == '.');
    }
    LLVM_READNONE inline bool isUnderLine(char c) 
    {
        return (c == '_');
    }
}

void Lexer::next(Token &token)
{
    while (*BufferPtr && charinfo::isWhitespace(*BufferPtr)) 
    {
        ++BufferPtr;
    }
    // make sure we didn't reach the end of input
    if (!*BufferPtr)
    {
        token.Kind = Token::eoi;
        return;
    }
    // collect characters and check for keywords or ident
    if (charinfo::isLetter(*BufferPtr))
    {
        const char *end = BufferPtr + 1;

        while (charinfo::isLetter(*end) || charinfo::isDigit(*end) || charinfo::isUnderLine(*end))
            ++end;

        llvm::StringRef Name(BufferPtr, end - BufferPtr);
        Token::TokenKind kind;
        if (Name == "int")
            kind = Token::KW_int;
        else if (Name == "bool")
            kind = Token::KW_bool;
        else if (Name == "float")
            kind = Token::KW_float;
        else if (Name == "var")
            kind = Token::KW_var;
        else if (Name == "do")
            kind = Token::KW_do;
        else if (Name == "print")
            kind = Token::KW_print;
        else if (Name == "while")
            kind = Token::KW_while;
        else if (Name == "for")
            kind = Token::KW_for;
        else if (Name == "if")
            kind = Token::KW_if;
        else if (Name == "case")
            kind = Token::KW_case;
        else if (Name == "switch")
            kind = Token::KW_switch;
        else if (Name == "const")
            kind = Token::KW_const;
        else if (Name == "continue")
            kind = Token::KW_continue;
        else if (Name == "default")
            kind = Token::KW_default;
        else if (Name == "break")
            kind = Token::KW_break;
        else if (Name == "else")
            kind = Token::KW_else;
        else if (Name == "true")
            kind = Token::KW_true;
        else if (Name == "false")
            kind = Token::KW_false;
        else if (Name == "and")
            kind = Token::KW_and;
        else if (Name == "or")
            kind = Token::KW_or;
        else if (Name == "xor")
            kind = Token::KW_xor;
        else if (Name == "min")
            kind = Token::Func_min;
        else if (Name == "max")
            kind = Token::Func_max;
        else if (Name == "mean")
            kind = Token::Func_mean;
        else if (Name == "sqrtN")
            kind = Token::Func_sqrtN;
        else
            kind = Token::ident;
        // generate the token
    
        formToken(token, end, kind);
        // llvm::errs() << "letter Kind:" << "$" << token.getText()<<"$" << "\n";
        return;
    }

    else if(charinfo::isHash(*BufferPtr))
    {
        const char *end = BufferPtr + 1;
        while (charinfo::isLetter(*end)) 
            ++end;
        llvm::StringRef Name(BufferPtr, end - BufferPtr);
        Token::TokenKind kind;
        if(Name == "#define")
        {
            kind = Token::KW_define;
            formToken(token,end,kind);
            // llvm::errs() << "define Kind:" << "$" << token.getText()<<"$" << "\n";
        } 
        else 
        {
            formToken(token, BufferPtr + 1, Token::unknown); 
            // llvm::errs() << "unKind:" << "$" << token.getText()<<"$" << "\n"; 
            }
        return;      
    }

    else if (charinfo::isDigit(*BufferPtr) || (charinfo::isDot(*BufferPtr) && charinfo::isDigit(*(BufferPtr + 1))))
    {
        const char *end = BufferPtr;

        while (charinfo::isDigit(*end))
            ++end;

        if (charinfo::isDot(*end))
        {             
            ++end;
            while (charinfo::isDigit(*end))
                ++end;
            formToken(token, end, Token::float_number); 
            // llvm::errs() << "float Kind:" << "$" << token.getText()<<"$" << "\n";
        }

        else 
        {
            formToken(token, end, Token::int_number); 
            // llvm::errs() << "int Kind:" << "$" << token.getText()<<"$" << "\n";
        }
        return;
    }

    else if (charinfo::isSpecialCharacter(*BufferPtr)) {
        const char *endWithOneLetter = BufferPtr + 1;
        const char *endWithTwoLetter = BufferPtr + 2;
        const char *end;
        llvm::StringRef NameWithOneLetter(BufferPtr, endWithOneLetter - BufferPtr);
        llvm::StringRef NameWithTwoLetter(BufferPtr, endWithTwoLetter - BufferPtr);
        Token::TokenKind kind;
        bool isFound = false;
        if (NameWithTwoLetter == "==")
        {
            kind = Token::eq;
            isFound = true;
            end = endWithTwoLetter;
        }
        else if (NameWithOneLetter == "=") 
        {
            kind = Token::assign;
            isFound = true;
            end = endWithOneLetter;
        } 
        else if (NameWithTwoLetter == "!=")
        {
            kind = Token::neq;
            isFound = true;
            end = endWithTwoLetter;
        } 
        else if (NameWithTwoLetter == "-(")
        {
            kind = Token::minus_paren;
            isFound = true;
            end = endWithTwoLetter;
        }
        else if (NameWithTwoLetter == "+=")
        {
            kind = Token::plus_assign;
            isFound = true;
            end = endWithTwoLetter;
        } 
        else if (NameWithTwoLetter == "-=")
        {
            kind = Token::minus_assign;
            isFound = true;
            end = endWithTwoLetter;
        } 
        else if (NameWithTwoLetter == "*=")
        {
            kind = Token::star_assign;
            isFound = true;
            end = endWithTwoLetter;
        } 
        // else if (NameWithTwoLetter == "*/")
        // {
        //     kind = Token::end_comment;
        //     isFound = true;
        //     end = endWithTwoLetter;
        // }
        else if (NameWithTwoLetter == "/=")
        {
            kind = Token::slash_assign;
            isFound = true;
            end = endWithTwoLetter;
        } 
        else if (NameWithTwoLetter == "%=")
        {
            kind = Token::mod_assign;
            isFound = true;
            end = endWithTwoLetter;
        }
        else if (NameWithTwoLetter == "/*")
        {
            // kind = Token::start_comment;
            // isFound = true;
            // end = endWithTwoLetter;
            // Multi-line comment
            BufferPtr += 2; // Skip '/*'
            while (!*BufferPtr) {
                if (*BufferPtr == '*' && *(BufferPtr + 1) == '/') {
                    BufferPtr += 2; // Skip '*/'
                    break;
                }
                else {
                    BufferPtr++;
                }
            }
        }
        else if (NameWithTwoLetter == ">=")
        {
            kind = Token::gte;
            isFound = true;
            end = endWithTwoLetter;
        } 
        else if (NameWithTwoLetter == "<=")
        {
            kind = Token::lte;
            isFound = true;
            end = endWithTwoLetter;
        } 
        else if (NameWithTwoLetter == "++")
        {
            kind = Token::plus_plus;
            isFound = true;
            end = endWithTwoLetter;
        } 
        else if (NameWithTwoLetter == "--")
        {
            kind = Token::minus_minus;
            isFound = true;
            end = endWithTwoLetter;
        }
        else if (NameWithOneLetter == "+")
        {
            kind = Token::plus;
            isFound = true;
            end = endWithOneLetter;
        }
        else if (NameWithOneLetter == "-")
        {
            kind = Token::minus;
            isFound = true;
            end = endWithOneLetter;
        }
        else if (NameWithOneLetter == "*")
        {
            kind = Token::star;
            isFound = true;
            end = endWithOneLetter;
        } 
        else if (NameWithOneLetter == "/")
        {
            kind = Token::slash;
            isFound = true;
            end = endWithOneLetter;
        }
        else if (NameWithTwoLetter == "//")
        {
            // kind = Token::oneLine_comment;
            // isFound = true;
            // end = endWithTwoLetter;
            // Single-line comment
            BufferPtr += 2; // Skip '//'
            while (!*BufferPtr && *BufferPtr != '\n') {
                BufferPtr++;
            }
            // Skip the newline character
            if (*BufferPtr == '\n') {
                BufferPtr++;
            }
        }
        else if (NameWithOneLetter == "+")
        {
            kind = Token::plus;
            isFound = true;
            end = endWithOneLetter;
        } 
        else if (NameWithOneLetter == ">")
        {
            kind = Token::gt;
            isFound = true;
            end = endWithOneLetter;
        } 
        else if (NameWithOneLetter == "<")
        {
            kind = Token::lt;
            isFound = true;
            end = endWithOneLetter;
        } 
        else if (NameWithOneLetter == "(")
        {
            kind = Token::l_paren;
            isFound = true;
            end = endWithOneLetter;
        } 
        else if (NameWithOneLetter == ")")
        {
            kind = Token::r_paren;
            isFound = true;
            end = endWithOneLetter;
        } 
        else if (NameWithOneLetter == "{")
        {
            kind = Token::l_brace;
            isFound = true;
            end = endWithOneLetter;
        } 
        else if (NameWithOneLetter == "}")
        {
            kind = Token::r_brace;
            isFound = true;
            end = endWithOneLetter;
        }
        else if (NameWithOneLetter == "[")
        {
            kind = Token::l_bracket;
            isFound = true;
            end = endWithOneLetter;
        }
        else if (NameWithOneLetter == "]")
        {
            kind = Token::r_bracket;
            isFound = true;
            end = endWithOneLetter;
        }  
        else if (NameWithOneLetter == ";")
        {
            kind = Token::semicolon;
            isFound = true;
            end = endWithOneLetter;
        }
        else if (NameWithOneLetter == "?")
        {
            kind = Token::qMark;
            isFound = true;
            end = endWithOneLetter;
        }
        else if (NameWithOneLetter == ":")
        {
            kind = Token::colon;
            isFound = true;
            end = endWithOneLetter;
        }     
        else if (NameWithOneLetter == ",")
        {
            kind = Token::comma;
            isFound = true;
            end = endWithOneLetter;
        } 
        else if (NameWithOneLetter == "%")
        {
            kind = Token::mod;
            isFound = true;
            end = endWithOneLetter;
        } 
        else if (NameWithOneLetter == "^")
        {
            kind = Token::exp;
            isFound = true;
            end = endWithOneLetter;
        }
        
        // generate the token
        if (isFound) 
        {
            formToken(token, end, kind); 
            // llvm::errs() << "spc Kind:" << "$" << token.getText()<<"$" << "\n";
        }
        else 
        {
            formToken(token, BufferPtr + 1, Token::unknown); 
            // llvm::errs() << "unKind:" << "$" << token.getText()<<"$" << "\n";
        }
        return;
    }
    else
    {
        formToken(token, BufferPtr + 1, Token::unknown); 
        // llvm::errs() << "unnKind:" << "$" << token.getText()<<"$" << "\n";
        return;         
    }
    return;
}

void Lexer::setBufferPtr(const char *buffer){
    BufferPtr = buffer;
}

void Lexer::formToken(Token &Tok, const char *TokEnd,
                      Token::TokenKind Kind)
{
    Tok.Kind = Kind;
    Tok.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
    BufferPtr = TokEnd;
}
