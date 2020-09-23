#include <iostream>
#include <string>

#include "scan.h"
using namespace std;
const string names[] = {"read", "write", "id", "literal", "gets",
                       "add", "sub", "mul", "div", "lparen", 
                       "rparen", "eof", "if", "while", "end", "=", 
                       "<=", "<>", "<", ">=", ">"};

static token input_token;
string ast = "";

void error(){
    cout << "syntax error************************\n" << endl;
    throw new exception();
}

void match(token expected){
    if(input_token == expected){
        cout << "matched " << names[input_token];
        if(input_token == t_id || input_token == t_literal){
            cout << ": " << token_image;
            ast = ast + "\"" + token_image + "\" ";
        }
        if(input_token == t_equal) ast = ast + "= ";
        if(input_token == t_lessequal) ast = ast + "<= ";
        if(input_token == t_notequal) ast = ast + "<> ";
        if(input_token == t_less) ast = ast + "< ";
        if(input_token == t_greaterequal) ast = ast + ">= ";
        if(input_token == t_greater) ast = ast + "> ";
        cout << endl;
        input_token = scan();
    }
    else error();
}

void program ();
void stmt_list ();
void stmt ();
void cond(); // condition E ro E
void expr ();
void term ();
void term_tail ();
void factor ();
void factor_tail ();
void rela_op ();// relationap operation for =|<>|<|>|<=|>=
void add_op ();
void mul_op ();

/*
The square brackets delimit lists, which have an arbitrary number of elements.  
The parentheses delimit tuples (structs), which have a fixed number of fields.
*/
void program () {
    ast += "(";
    ast += "program";
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
        case t_eof:
            cout << "predict program --> stmt_list eof" << endl;
            ast += "[";
            stmt_list ();
            ast += "]";
            match (t_eof);
            break;
        default: 
            error ();
    }
    ast += ")";
}

void stmt_list () {
    switch (input_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_if:
        case t_while:
            cout << "predict stmt_list -->stmt stmt_list" << endl;
            ast += "(";
            stmt ();
            ast += ")";
            stmt_list ();
            break;
        case t_eof:
            cout << "predict stmt_list --> epsilon" << endl;
            break;          /* epsilon production */
        default: ;
    }
}

void stmt () {
    switch (input_token) {
        case t_id:
            cout << "predict stmt --> id gets expr" << endl;
            ast += ":= ";
            match (t_id);
            match (t_gets);
            //ast += "(";
            expr ();
            //ast += ")";
            break;
        case t_read:
            cout << "predict stmt --> read id" << endl;
            ast += "read ";
            match (t_read);
            match (t_id);

            break;
        case t_write:
            cout << "predict stmt --> write expr" << endl;
            ast += "write ";
            match (t_write);
            //ast += "(";
            expr ();
            //ast += ")";
            break;
        case t_if:
            cout << "predict stmt --> if condition stmt_list end" << endl;
            ast += "if";
            match (t_if);
            ast += "(";
            cond ();
            ast += ")";
            ast += "[";
            stmt_list ();
            ast += "]";
            match (t_end);
            break;
        case t_while:
            cout << "predict stmt --> while condition stmt_list end" << endl;
            ast += "while";
            match (t_while);
            ast += "(";
            cond ();
            ast += ")";
            ast += "[";
            stmt_list ();
            ast += "]";
            match (t_end);
            break;
        default: error ();
    }
}

//comdition expression ro expression
void cond (){
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict comp --> expr rela_op expr" << endl;
            //ast += "(";
            expr ();
            //ast += ")";
            rela_op ();//将来改成 ro E E
            //ast += "(";
            expr ();
            //ast += ")";
            break;
        default: error ();
    }
}

void expr () {
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict expr --> term term_tail" << endl;
            term ();
            term_tail ();
            break;
        default: error ();
    }
}

void term () {
    switch (input_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict term --> factor factor_tail" << endl;
            ast += "(";
            factor ();
            ast += ")";
            factor_tail ();
            break;
        default: error ();
    }
}

void term_tail () {
    switch (input_token) {
        case t_add:
        case t_sub:
            cout << "predict term_tail --> add_op term term_tail" << endl;
            add_op ();
            term ();
            term_tail ();
            break;
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            cout << "predict term_tail --> epsilon" << endl;
            break;          /* epsilon production */
        default: ;
    }
}

void factor () {
    switch (input_token) {
        case t_literal:
            cout << "predict factor --> literal"<< endl;
            //ast += "(";
            ast += "num ";
            match (t_literal);
            //ast += ")";
            break;
        case t_id :
            cout << "predict factor --> id"<< endl;
            //ast += "(";
            ast += "id ";
            match (t_id);
            //ast += ")";
            break;
        case t_lparen:
            cout << "predict factor --> lparen expr rparen"<< endl;
            match (t_lparen);
            ast += "(";
            expr ();
            ast += ")";
            match (t_rparen);
            break;
        default: error ();
    }
}

void factor_tail () {
    switch (input_token) {
        case t_mul:
        case t_div:
            cout << "predict factor_tail --> mul_op factor factor_tail" << endl;
            mul_op ();
            ast += "(";
            factor ();
            ast += ")";
            factor_tail ();
            break;
        case t_add:
        case t_sub:
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            cout << "predict factor_tail --> epsilon" << endl;
            break;       /* epsilon production */
        default: ;
    }
}

void rela_op (){
    switch (input_token) {
        case t_equal:
            cout << "predict rela_op --> equal" << endl;
            match (t_equal);
            break;
        case t_notequal:
            cout << "predict rela_op --> not_equal" << endl;
            match (t_notequal);
            break;
        case t_less:
            cout << "predict rela_op --> less" << endl;
            match (t_less);
            break;
        case t_greater:
            cout << "predict rela_op --> greater" << endl;
            match (t_greater);
            break;
        case t_lessequal:
            cout << "predict rela_op --> less_equal" << endl;
            match (t_lessequal);
            break;
        case t_greaterequal:
            cout << "predict rela_op --> greater_equal" << endl;
            match (t_greaterequal);
            break;
        default: error ();
    }
}

void add_op () {
    switch (input_token) {
        case t_add:
            cout << "predict add_op --> add" << endl;
            ast += "+ ";
            match (t_add);
            break;
        case t_sub:
            cout << "predict add_op --> sub" << endl;
            ast += "- ";
            match (t_sub);
            break;
        default: error ();
    }
}

void mul_op () {
    switch (input_token) {
        case t_mul:
            cout << "predict mul_op --> mul" << endl;
            ast += "* ";
            match (t_mul);
            break;
        case t_div:
            cout << "predict mul_op --> div" << endl;
            ast += "/ ";
            match (t_div);
            break;
        default: error ();
    }
}

int main () {
    input_token = scan ();
    program ();
    cout << ast << endl;
    return 0;
}
