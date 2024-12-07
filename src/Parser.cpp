#include "Parser.h"


// main point is that the whole input has been consumed
Program *Parser::parse()
{
    Program *Res = parseProgram();
    return Res;
}


DefineStmt *Parser::parseDefine() { 
    llvm::StringRef MacroName;
    llvm::StringRef MacroValue;

    if (expect(Token::KW_define)){
        goto _error;
    }
    advance();

    if (expect(Token::ident)){
        goto _error;
    }
    MacroName = Tok.getText();
    advance();

    if (!Tok.is(Token::number) && !Tok.is(Token::ident) && !Tok.is(Token::floatNumber) && !Tok.is(Token::KW_bool)){
        goto _error;
    }
    MacroValue = Tok.getText();

    return new DefineStmt(MacroName, MacroValue);

_error:
    while (Tok.getKind() != Token::eoi) advance();
    return nullptr;
}


Program *Parser::parseProgram()
{
    llvm::SmallVector<AST *> data;
    
    while (!Tok.is(Token::eoi)){
        switch (Tok.getKind()){
            case Token::KW_define: { 
                DefineStmt *ddefine = parseDefine();
                if (ddefine) data.push_back(ddefine);
                else goto _error;
                break;
            }

            case Token::KW_int: {
                IntDecleration *d;
                d = parseIntDec();
                if (d)
                    data.push_back(d);
                else
                    goto _error;
                    
                break;
            }

            case Token::KW_bool: {
                BoolDecleration *dbool;
                dbool = parseBoolDec();
                if (dbool)
                    data.push_back(dbool);
                else
                    goto _error;

                break;
            }

            case Token::KW_float: { 
                FloatDecleration *dfloat = parseFloatDec();
                if (dfloat) data.push_back(dfloat);
                else goto _error;
                break;
            }
            case Token::KW_var: { 
                VarDecleration *dvar = parseVarDec();
                if (dvar) data.push_back(dvar);
                else goto _error;
                break;
            }
            case Token::KW_const: {
                ConstDecleration *dconst = parseConstDec();
                if (dconst) data.push_back(dconst);
                else goto _error;
                break;
            }
            case Token::KW_switch: { 
                SwitchStatement *sswitch = parseSwitchStmt();
                if (sswitch) data.push_back(sswitch);
                else goto _error;
                break;
            }
            case Token::KW_do: { 
                DoWhileStatement *dwhile = parseDoWhileStmt();
                if (dwhile) data.push_back(dwhile);
                else goto _error;
                break;
            }
            case Token::ident: { // CHECK
                Token prev_token = Tok;
                const char* prev_buffer = Lex.getBuffer();
                UnaryOperation *u;

                // Parse Unary Operation
                u = parseUnaryOp();
                if (Tok.is(Token::semicolon)) {
                    if (u) {
                        data.push_back(u);
                        break;
                    } else {
                        goto _error;
                    }
                } else {
                    if (u) {
                        goto _error;
                    } else {
                        Tok = prev_token;
                        Lex.setBufferPtr(prev_buffer);
                    }
                }

                // Parse Boolean Assignment
                Assignment *a_bool;
                prev_token = Tok;
                prev_buffer = Lex.getBuffer();
                a_bool = parseBoolAssignStmt();
                if (a_bool) {
                    data.push_back(a_bool);
                    break;
                }
                Tok = prev_token;
                Lex.setBufferPtr(prev_buffer);

                // Parse Integer Assignment
                Assignment *a_int;
                a_int = parseIntAssignStmt();
                if (a_int && Tok.is(Token::semicolon)) {
                    data.push_back(a_int);
                    break;
                }
                Tok = prev_token;
                Lex.setBufferPtr(prev_buffer);

                // Parse Float Assignment
                Assignment *a_float;
                a_float = parseFloatAssignStmt();
                if (a_float && Tok.is(Token::semicolon)) {
                    data.push_back(a_float);
                    break;
                }
                Tok = prev_token;
                Lex.setBufferPtr(prev_buffer);

                // Parse `var` Assignment
                Assignment *a_var;
                a_var = parseVarAssignStmt(); 
                if (a_var && Tok.is(Token::semicolon)) {
                    data.push_back(a_var);
                    break;
                }
                Tok = prev_token;
                Lex.setBufferPtr(prev_buffer);

                goto _error;
            }

            case Token::KW_if: {
                IfStatement *i;
                i = parseIf();
                if (i)
                    data.push_back(i);
                else
                    goto _error;
                
                break;
            }
            case Token::KW_Dowhile: {
                DoWhileStatement *w;
                w = parseDoWhileStmt();
                if (w)
                    data.push_back(w);
                else {
                    goto _error;
                }
                break;
            }
            case Token::KW_for: {
                ForStatement *f;
                f = parseForStmt();
                if (f)
                    data.push_back(f);
                else {
                    goto _error;
                }
                break;
            }
            case Token::KW_print: {
                PrintStatement *p;
                p = parsePrintStmt();
                if (p)
                    data.push_back(p);
                else {
                    goto _error;
                }
                break;
            }
            case Token::start_comment: {
                parseComment();
                if (!Tok.is(Token::end_comment))
                    goto _error;
                break;
            }
            default: {
                llvm::errs() << "- Error 1 -";
                error();

                goto _error;
                break;
            }
        }
        advance();
        
    }
    
    return new Program(data);
_error:
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;
}

IntDecleration *parser::parseIntDecl(){
    Expr *E = nullptr;
    llvm::SmallVector<llvm::StringRef> Vars;
    llvm::SmallVector<Expr *> Values;

    
    if (expect(Token::KW_int)){
        goto _error;
    }
    advance();
    
    if (expect(Token::ident)){
        goto _error;
    }

    
    Vars.push_back(Tok.getText());
    advance();

    if (Tok.is(Token::assign))
    {
        advance();
        E = parseExpr();
        if(E){
            Values.push_back(E);
        }
        else{
            goto _error;
        }
    }
    else
    {
        Values.push_back(new Final(Final::Number, llvm::StringRef("0")));
    }
    
    
    while (Tok.is(Token::comma))
    {
        advance();
        if (expect(Token::ident)){
            goto _error;
        }
            
        Vars.push_back(Tok.getText());
        advance();

        if(Tok.is(Token::assign)){
            advance();
            E = parseExpr();
            if(E){
                Values.push_back(E);
            }
            else{
                goto _error;
            }
        }
        else{
            Values.push_back(new Final(Final::Number, llvm::StringRef("0")));
        }
    }

    if (expect(Token::semicolon)){
        goto _error;
    }


    return new DeclarationInt(Vars, Values);

_error: 
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;
}

FloatDecleration *parser::parseFloatDecl(){
    Expr *E = nullptr;
    llvm::SmallVector<llvm::StringRef> Vars;
    llvm::SmallVector<Expr *> Values;

    
    if (expect(Token::KW_int)){
        goto _error;
    }
    advance();
    
    if (expect(Token::ident)){
        goto _error;
    }

    
    Vars.push_back(Tok.getText());
    advance();

    if (Tok.is(Token::assign))
    {
        advance();
        E = parseExpr();
        if(E){
            Values.push_back(E);
        }
        else{
            goto _error;
        }
    }
    else
    {
        Values.push_back(new Final(Final::Number, llvm::StringRef("0.0")));
    }
    
    
    while (Tok.is(Token::comma))
    {
        advance();
        if (expect(Token::ident)){
            goto _error;
        }
            
        Vars.push_back(Tok.getText());
        advance();

        if(Tok.is(Token::assign)){
            advance();
            E = parseExpr();
            if(E){
                Values.push_back(E);
            }
            else{
                goto _error;
            }
        }
        else{
            Values.push_back(new Final(Final::Number, llvm::StringRef("0.0")));
        }
    }

    if (expect(Token::semicolon)){
        goto _error;
    }


    return new DeclarationInt(Vars, Values);
    
_error: 
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;
}


BoolDecleration *Parser::parseBoolDecl() {
    Logic *L = nullptr;
    llvm::SmallVector<llvm::StringRef> Vars;
    llvm::SmallVector<Logic *> Values;

    if (expect(Token::KW_bool)) {
        goto _error;
    }
    advance();

    while (true) {
        if (expect(Token::ident)) {
            goto _error;
        }

        Vars.push_back(Tok.getText());
        advance();

        // Stop processing if there are no more variables or assignments
        if (!Tok.is(Token::comma) && !Tok.is(Token::assign)) {
            break;
        }

        // Handle explicit assignments
        if (Tok.is(Token::comma)) {
            advance(); // Move to next variable
        } else if (Tok.is(Token::assign)) {
            advance(); // Move past `=`

            // Parse the list of explicit values
            while (true) {
                L = parseLogic();
                if (!L) {
                    goto _error;
                }
                Values.push_back(L);

                // Continue parsing more values if a comma is found
                if (Tok.is(Token::comma)) {
                    advance();
                } else {
                    break; // Stop when no more values
                }
            }
            break; // Stop variable parsing after explicit values
        }
    }

    if (expect(Token::semicolon)) {
        goto _error;
    }

    // Assign default values for variables without explicit values
    while (Values.size() < Vars.size()) {
        Values.push_back(new Comparison(nullptr, nullptr, Comparison::False));
    }

    // Ensure the number of values matches the number of variables
    if (Values.size() != Vars.size()) {
        goto _error;
    }

    return new DeclarationBool(Vars, Values);

_error:
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;
}



VarDecleration *Parser::parseVarDecl() {
    llvm::SmallVector<llvm::StringRef> Vars;
    llvm::SmallVector<Expr *> Values;

    if (expect(Token::KW_var)) goto _error;
    advance();

    while (true) {
        if (expect(Token::ident)) {
            goto _error;
        }
        Vars.push_back(Tok.getText());
        advance();

        if (!Tok.is(Token::comma) && !Tok.is(Token::assign)) {
            break;
        }

        if (Tok.is(Token::comma)) {
            advance(); 
        } else if (Tok.is(Token::assign)) {
            advance(); 

            while (true) {
                Expr *E = parseExpr();
                if (!E) {
                    goto _error;
                }
                Values.push_back(E);

                if (Tok.is(Token::comma)) {
                    advance();
                } else {
                    break; 
                }
            }
            break; 
        }
    }

    if (expect(Token::semicolon)) goto _error;

    while (Values.size() < Vars.size()) {
        Values.push_back(new Final(Final::Number, llvm::StringRef("0.0")));
    }
    if (Values.size() != Vars.size()) {
        goto _error;
    }

    return new DeclarationVar(Vars, Values);

_error:
    while (Tok.getKind() != Token::eoi) advance();
    return nullptr;
}



Assignment *Parser::parseIntAssign()
{
    Expr *E = nullptr;
    Final *F = nullptr;
    Assignment::AssignKind AK;
    F = (Final *)(parseFinal());
    if (F == nullptr)
    {
        goto _error;
    }
    
    if (Tok.is(Token::assign))
    {
        AK = Assignment::Assign;
    }
    else if (Tok.is(Token::star_assign))
    {
        AK = Assignment::Star_assign;
    }
    else if (Tok.is(Token::minus_assign))
    {
        AK = Assignment::Minus_assign;
    }
    else if (Tok.is(Token::Devide_assign))
    {
        AK = Assignment::Devide_assign;
    }
    else if (Tok.is(Token::plus_assign))
    {
        AK = Assignment::Plus_assign;
    }
    else
    {
        goto _error;
    }
    advance();
    E = parseExpr();  
    if(E){
        return new Assignment(F, E, AK, nullptr);
    }
    else{
        goto _error;
    }

_error:
        while (Tok.getKind() != Token::eoi)
            advance();
        return nullptr;
}


Expr *Parser::parseExpr()
{
    Expr *Left = parseTerm();

    if (Left == nullptr)
    {
        goto _error;
    }
    
    if (Tok.is(Token::questionmark)) {
        return parseTernary();
    }
    while (Tok.isOneOf(Token::plus, Token::minus, Token::KW_xor))
    {
        BinaryOp::Operator Op;
        if (Tok.is(Token::plus))
            Op = BinaryOp::Plus;
        else if (Tok.is(Token::minus))
            Op = BinaryOp::Minus;
        else if (Tok.is(Token::KW_in)) {
            Op = BinaryOp::In;
        } else if (Tok.is(Token::KW_not_in)) {
            Op = BinaryOp::NotIn;
        else if (Tok.is(Token::KW_xor))
            Op = BinaryOp::Xor;
        } else {
            error();

            goto _error;
        }
        advance();
        Expr *Right = parseTerm();
        if (Right == nullptr)
        {
            goto _error;
        }
        Left = new BinaryOp(Op, Left, Right);
    }
    return Left;

_error:
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;
}

Expr *Parser::parseTerm()
{
    Expr *Left = parseFactor();
    if (Left == nullptr)
    {
        goto _error;
    }
    while (Tok.isOneOf(Token::Mul, Token::Mod, Token::Div))
    {
        BinaryOp::Operator Op;
        if (Tok.is(Token::Mul))
            Op = BinaryOp::Mul;
        else if (Tok.is(Token::Devide))
            Op = BinaryOp::Div;
        else if (Tok.is(Token::Mod))
            Op = BinaryOp::Mod;
        else {
            error();

            goto _error;
        }
        advance();
        Expr *Right = parseFactor();
        if (Right == nullptr)
        {
            goto _error;
        }
        Left = new BinaryOp(Op, Left, Right);
    }
    return Left;

_error:
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;
}

Expr *Parser::parseFactor() {
    Expr *Left = nullptr;

    if ((Tok.getText() == "int" || Tok.getText() == "float" || Tok.getText() == "bool")) {
        return parseCastExpr(); // Handle casting
    }

    if ((Tok.getText() == "min" || Tok.getText() == "max" || Tok.getText() == "sqrtN" || Tok.getText() == "mean")) {
        return parseFunctionCall(); // Handle function calls
    }

    switch (Tok.getKind()) {
        case Token::number: {
            Left = new Final(Final::Number, Tok.getText());
            advance();
            break;
        }
        case Token::floatNumber: {
            Left = new Final(Final::FloatNumber, Tok.getText());
            advance();
            break;
        }
        case Token::ident: {
            Left = new Final(Final::Ident, Tok.getText());
            advance();
            break;
        }
        case Token::KW_false: {
            Left = new Final(Final::Bool, Tok.getText());
            advance();
            break;
        }
        case Token::KW_true: {
            Left = new Final(Final::Bool, Tok.getText());
            advance();
            break;
        }
        case Token::l_paren: {
            advance();
            Left = parseExpr();
            if (!consume(Token::r_paren)) {
                goto _error;
            }
            break;
        }
        default: {
            error();
            goto _error;
        }
    }

    return Left;

_error:
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;
}

Expr *Parser::parseFinal()
{
    Expr *Res = nullptr;
    switch (Tok.getKind())
    {
    case Token::number:{
        Res = new Final(Final::Number, Tok.getText());
        advance();
        break;
    }
    case Token::floatNumber:{
        Res = new Final(Final::FloatNumber, Tok.getText());
        advance();
        break;
    }
    case Token::ident: {
        Res = new Final(Final::Ident, Tok.getText());
        Token prev_tok = Tok;
        const char* prev_buffer = Lex.getBuffer();
        Expr* u = parseUnary();
        if(u)
            return u;
        else{
            Tok = prev_tok;
            Lex.setBufferPtr(prev_buffer);
            advance();
        }
        break;
    }
    case Token::plus:{
        advance();
        if(Tok.getKind() == Token::int_number){
            Res = new SignedNumber(SignedNumber::Plus, Tok.getText());
            advance();
            break;
        }
        else if(Tok.getKind() == Token::float_number){
            Res = new SignedNumber(SignedNumber::Plus, Tok.getText());
            advance();
            break;
        }
        goto _error;
    }

    case Token::minus:{
        advance();
        if (Tok.getKind() == Token::int_number){
            Res = new SignedNumber(SignedNumber::Minus, Tok.getText());
            advance();
            break;
        }
        else if(Tok.getKind() == Token::float_number){
            Res = new SignedNumber(SignedNumber::Minus, Tok.getText());
            advance();
            break;
        }
        goto _error;
    }
    case Token::minus_paren:{
        advance();
        Expr *math_expr = parseExpr();
        if(math_expr == nullptr)
            goto _error;
        Res = new NegExpr(math_expr);
        if (!consume(Token::r_paren))
            break;
        
        goto _error;

    }
    case Token::l_paren:{
        advance();
        Res = parseExpr();
        if(Res == nullptr){
            goto _error;
        }
        if (!consume(Token::r_paren))
            break;
        
    }
    default:{
        error();
        goto _error;
    }
    }
    return Res;

_error:
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;
}



PrintStmt *Parser::parsePrint()
{
    // print format: print(variable_name)
    llvm::StringRef Var;
    if (expect(Token::KW_print)){
        goto _error;
    }
    advance();
    if (expect(Token::l_paren)){
        goto _error;
    }
    advance();
    if (expect(Token::ident)){
        goto _error;
    }
    Var = Tok.getText();
    advance();
    if (expect(Token::r_paren)){
        goto _error;
    }
    advance();
    if (expect(Token::semicolon)){
        goto _error;
    }
    return new PrintStmt(Var);

_error:
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;
}


SwitchStmt *Parser::parseSwitchStmt() {// TODO added for switch-case construct
    Logic *Cond = nullptr;
    llvm::SmallVector<CaseStmt *> Cases;
    DefaultStmt *Default = nullptr; // Declare Default as a pointer to DefaultStmt
    llvm::SmallVector<AST *> DefaultBody;

    if (expect(Token::KW_switch)){
        goto _error;
    }
    advance();

    if (expect(Token::l_paren)){
            goto _error;
    }
    advance();

    Cond = parseLogic();
    if (!Cond){
        goto _error;
    }

    if (expect(Token::r_paren)){
        goto _error;
    }
    advance();

    if (expect(Token::l_brace)){
        goto _error;
    }
    advance();

    while (Tok.is(Token::KW_case)) {
        advance();

        Expr *CaseValue = parseExpr();
        if (!CaseValue){ 
            goto _error;
        }

        if (expect(Token::colon)){
            goto _error;
        }
        advance();

        llvm::SmallVector<AST *> CaseBody = getBody();
        Cases.push_back(new CaseStmt(CaseValue, CaseBody));
    }

    if (Tok.is(Token::KW_default)) { // TODO
        advance();
        if (expect(Token::colon)) {
            goto _error;
        }
        advance();
        llvm::SmallVector<AST *> DefaultBody = getBody();
        Default = new DefaultStmt(DefaultBody); // Wrap the body in DefaultStmt 
        } else {
            Default = nullptr; // No default case
        }
        if (!consume(Token::r_brace)) {
            goto _error;
        }
        advance();
        return new SwitchStmt(Cond, Cases, Default);

_error:
    while (Tok.getKind() != Token::eoi) advance();
    return nullptr;
}



WhileStmt *Parser::parseDoWhileStmt()
{
    llvm::SmallVector<AST *> Body;
    Logic *Cond = nullptr;

    if (expect(Token::KW_while)){
        goto _error;
    }
        
    advance();

    if(expect(Token::l_paren)){
        goto _error;
    }

    advance();

    Cond = parseLogic();
    if (Cond == nullptr)
    {
        goto _error;
    }
    if(expect(Token::r_paren)){
        goto _error;
    }

    advance();

    if (expect(Token::l_brace)){
        goto _error;
    }

    advance();

    Body = getBody();
    if(Body.empty())
        goto _error;
        

    return new WhileStmt(Cond, Body);

_error:
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;
}

ForStmt *Parser::parseForStmt()
{
    Assignment *First = nullptr;
    Logic *Second = nullptr;
    Assignment *ThirdAssign = nullptr;
    UnaryOp *ThirdUnary = nullptr;
    llvm::SmallVector<AST *> Body;
    Token prev_token;
    const char* prev_buffer;

    if (expect(Token::KW_for)){
        goto _error;
    }
        
    advance();

    if(expect(Token::l_paren)){
        goto _error;
    }

    advance();

    First = parseIntAssign();

    if (First == nullptr)
        goto _error;
        
    if (First->getAssignKind() != Assignment::Assign)    // The first part can only have a '=' sign
        goto _error;

    if(expect(Token::semicolon)){
        goto _error;
    }

    advance();

    Second = parseLogic();

    if (Second == nullptr)
        goto _error;
        
    if(expect(Token::semicolon)){
        goto _error;
    }

    advance();

    prev_token = Tok;
    prev_buffer = Lex.getBuffer();

    ThirdAssign = parseIntAssign();

    if (ThirdAssign == nullptr){
        Tok = prev_token;
        Lex.setBufferPtr(prev_buffer);

        ThirdUnary = parseUnary();
        if (ThirdUnary == nullptr){
            goto _error;
        }

    }
    else{
        if(ThirdAssign->getAssignKind() == Assignment::Assign)   // The third part cannot have only '=' sign
            goto _error;
    }


    if(expect(Token::r_paren)){
        goto _error;
    }

    advance();

    if(expect(Token::l_brace)){
        goto _error;
    }

    advance();

    Body = getBody();

    if (Body.empty())
        goto _error;

    return new ForStmt(First, Second, ThirdAssign, ThirdUnary, Body);

_error:
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;  

}



