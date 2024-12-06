#ifndef _PARSER_H
#define _PARSER_H

#include "AST.h"
#include "Lexer.h"
#include "llvm/Support/raw_ostream.h"



class Parser {
	Lexer& Lex;
	Token Tok;
	bool HasError;

	void error()
	{
		llvm::errs() << "Unexpected: " << Tok.getText() << "\n";
		HasError = true;
	}


	void advance() { Lex.next(Tok); }

	bool expect(Token::TokenKind Kind)
	{
		if (Tok.getKind() != Kind)
		{
			error();
			return true;
		}
		return false;
	}

	bool consume(Token::TokenKind Kind)
	{
		if (expect(Kind))
			return true;
		advance();
		return false;
	}

	// parsing functions according to the regex
	// pattern specified. each one produces its own node
	// one node can have multiple subnodes inside it

	Program *parseS();
	Program *parseProgram();
	Assignment *parseAssignList();
	void parseComment(); // should only error if not valid

	IntDecleration *parseIntDecl();
	Expr *parseMathIntExpr();
	Expr *parseTermInt();
	Expr *parseFactorInt();
	Expr *parseFinalInt();

	FloatDecleration *parseFloatDecl();
	Expr *parseMathFloatExpr();
	Expr *parseTermFloat();
	Expr *parseFactorFloat();
	Expr *parseFinalFloat();

	BoolDecleration *parseBoolDecl();
	LogicExpr *parseLogicalExpr(); // not sure to use Expr or LogicExpr
	LogicExpr *parseComparison();

	VarDecleration *parseVarDecl();

	ConstDecleration *parseConstDecl();

	Assignment *parseIntAssignStmt(); // not sure about the Assignment return type
	Assignment *parseFloatAssignStmt(); // not sure about the Assignment return type
	Assignment *parseBoolAssignStmt(); // not sure about the Assignment return type

	Expr *parseFunction(); // not sure about Expr return type? Shouldn't it be FunctionStatement for example?
	Expr *parseMinStmt();
	Expr *parseMaxStmt();
	Expr *parseMeanStmt();
	Expr *parseSqrtStmt();

	IfStatement *parseIf();
	ElifStatement *parseElif();
	ElseStatement *parseElse();

	Expr *parseTernaryStmt();
	// Expr *parseTypeTernary(); // do we even need it?

	DoWhileStatement *parseDoWhileStmt();
	ForStatement *parseForStmt();
	// do we need parseThirtPartFor

	SwitchStatement *parseSwitchStmt();
	CaseStatement *parseCaseStmt();

	PrintStatement *parsePrintStmt();


	
	// initializes all members and retrieves the first token
	Parser(Lexer& Lex) : Lex(Lex), HasError(false)
	{
		advance();
	}

	// get the value of error flag
	bool hasError() { return HasError; }

	Base* parse();
};

#endif