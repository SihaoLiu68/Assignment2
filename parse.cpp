#include <iostream>
#include <string>

#include "scan.h"
using namespace std;
const string names[] = {"read", "write", "id", "literal", "gets",
                       "add", "sub", "mul", "div", "lparen", 
                       "rparen", "eof", "if", "while", "end", "=", 
                       "<=", "<>", "<", ">=", ">"};

static token input_token;


void error(){
    cout << "syntax error************************\n" << endl;
    throw new exception();
}

void match(token expected){
    if(input_token == expected){
        cout << "matched " << names[input_token];
        if(input_token == t_id || input_token == t_literal){
            cout << ": " << token_image;
        }
        cout << endl;
        input_token = scan();
    }
    else error();
}

string program ();
string stmt_list ();
string stmt ();
string cond(); // condition E ro E
string expr ();
string term ();
string term_tail (string term);
string factor ();
string factor_tail (string factor);
string rela_op ();// relationap operation for =|<>|<|>|<=|>=
string add_op ();
string mul_op ();

/*
The square brackets delimit lists, which have an arbitrary number of elements.  
The parentheses delimit tuples (structs), which have a fixed number of fields.
*/
string program () {
    string ast = "";
    ast += "(program ";
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_eof:
            cout << "predict program --> stmt_list eof" << endl;
            ast += "[ ";
            ast += stmt_list ();
            ast += "] ";
            match (t_eof);
            break;
        default: 
            error ();
    }
    ast += ")";
    return ast;
}

string stmt_list () {
    string ast = "";
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
            cout << "predict stmt_list -->stmt stmt_list" << endl;
            ast += stmt ();
            ast += stmt_list ();
            break;
        case t_eof:
            cout << "predict stmt_list --> epsilon" << endl;
            return "";
            break;          /* epsilon production */
        default: ;
    }
    return ast;
}

string stmt () {
    string ast = "";
    ast += "(";
    switch (input_token) {
        case t_id:
            cout << "predict stmt --> id gets expr" << endl;
            match (t_id);
            ast = ast + ":= " + "\"" +  token_image + "\"";
            match (t_gets);
            ast += expr ();
            break;
        case t_read:
            cout << "predict stmt --> read id" << endl;
            match (t_read);
            ast += "read ";
            match (t_id);
            ast = ast + "\"" + token_image + "\"";
            break;
        case t_write:
            cout << "predict stmt --> write expr" << endl;
            match (t_write);
            ast += "write ";
            ast += expr ();
            break;
        case t_if:
            cout << "predict stmt --> if condition stmt_list end" << endl;
            match (t_if); 
            ast += "if ";
            ast += "(";
            ast += cond ();
            ast += ")";
            ast += "[";
            ast += stmt_list ();
            ast += "]";
            match (t_end);
            break;
        case t_while:
            cout << "predict stmt --> while condition stmt_list end" << endl;
            match (t_while);
            ast += "while";
            ast += "(";
            ast += cond ();
            ast += ")";
            ast += "[";
            ast += stmt_list ();
            ast += "]";
            match (t_end);
            break;
        default: error ();
    }
    ast += ")";
    return ast;
}

//comdition expression ro expression
string cond (){
    string ast = "";
    string temp1 = "";
    string temp2 = "";
    string temp3 = "";
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict comp --> expr rela_op expr" << endl;        
            temp1 += expr ();
            temp2 += rela_op ();
            temp3 += expr ();
            ast += "(" + temp2 + temp1 + temp3 + ")";
            break;
        default: error ();
    }
    return ast;
}

string expr () {
    string ast = "";
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict expr --> term term_tail" << endl;
            //ast += "(";
            //ast += term ();
            ast += term_tail (term ());
            //ast += ")";
            break;
        default: error ();
    }
    return ast;
}

string term () {
    string ast = "";
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict term --> factor factor_tail" << endl;
            //ast += "(";
            //ast += factor ();
            ast += factor_tail (factor ());
            //ast += ")";
            break;
        default: error ();
    }
    return ast;
}

string term_tail (string te) {
    string ast = "";
    ast += "(";
    switch (input_token) {
        case t_add:
        case t_sub:
            cout << "predict term_tail --> add_op term term_tail" << endl;
            ast += add_op ();
            ast += te;
            //ast += term ();
            ast += term_tail (term ());
            break;
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_equal:
        case t_notequal:
        case t_less:
        case t_greater:
        case t_lessequal:
        case t_greaterequal:
        case t_eof:
            cout << "predict term_tail --> epsilon" << endl;
            return te;         /* epsilon production */
        default: ;
    }
    ast += ")";
    return ast;
}

string factor () {
    string ast = "";
    switch (input_token) {
        case t_literal:
            cout << "predict factor --> literal"<< endl;
            match (t_literal);
            ast += "(";
            ast = ast + "num " + "\"" + token_image + "\"";
            ast += ")";
            break;
        case t_id :
            cout << "predict factor --> id"<< endl;
            match (t_id);
            ast += "(";
            ast = ast + "id " + "\"" + token_image + "\"";
            ast += ")";
            break;
        case t_lparen:
            cout << "predict factor --> lparen expr rparen"<< endl;
            match (t_lparen);
            ast += expr ();
            match (t_rparen);
            break;
        default: error ();
    }
    return ast;
}

string factor_tail (string fa) {
    string ast = "";
    ast += "(";
    switch (input_token) {
        case t_mul:
        case t_div:
            cout << "predict factor_tail --> mul_op factor factor_tail" << endl;
            ast += mul_op ();
            ast += fa;
            ast += factor_tail (factor ());
            break;
        case t_add:
        case t_sub:
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_equal:
        case t_notequal:
        case t_less:
        case t_greater:
        case t_lessequal:
        case t_greaterequal:
        case t_eof:
            cout << "predict factor_tail --> epsilon" << endl;
            return fa;       /* epsilon production */
        default: ;
    }
    ast += ")";
    return ast;
}

string rela_op (){
    string ast = "";
    switch (input_token) {
        case t_equal:
            cout << "predict rela_op --> equal" << endl;
            match (t_equal);
            ast += "= ";
            break;
        case t_notequal:
            cout << "predict rela_op --> not_equal" << endl;
            match (t_notequal);
            ast += "<> ";
            break;
        case t_less:
            cout << "predict rela_op --> less" << endl;
            match (t_less);
            ast += "< ";
            break;
        case t_greater:
            cout << "predict rela_op --> greater" << endl;
            match (t_greater);
            ast += "> ";
            break;
        case t_lessequal:
            cout << "predict rela_op --> less_equal" << endl;
            match (t_lessequal);
            ast += "<= ";
            break;
        case t_greaterequal:
            cout << "predict rela_op --> greater_equal" << endl;
            match (t_greaterequal);
            ast += ">= ";
            break;
        default: error ();
    }
    return ast;
}

string add_op () {
    string ast = "";
    switch (input_token) {
        case t_add:
            cout << "predict add_op --> add" << endl;
            match (t_add);
            ast += "+ ";            
            break;
        case t_sub:
            cout << "predict add_op --> sub" << endl;
            match (t_sub);
            ast += "- ";            
            break;
        default: error ();
    }
    return ast;
}

string mul_op () {
    string ast = "";
    switch (input_token) {
        case t_mul:
            cout << "predict mul_op --> mul" << endl;
            match (t_mul);
            ast += "* ";            
            break;
        case t_div:
            cout << "predict mul_op --> div" << endl;
            match (t_div);
            ast += "/ ";            
            break;
        default: error ();
    }
    return ast;
}

int main () {
    input_token = scan ();
    string AST = program ();
    cout << AST << endl;
    return 0;
}