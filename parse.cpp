#include <iostream>
#include <string>
#include <queue>
#include <map>

#include "scan.h"
using namespace std;
const string names[] = {"read", "write", "id", "literal", "gets",
                       "add", "sub", "mul", "div", "lparen", 
                       "rparen", "eof", "if", "while", "end", "=", 
                       "<=", "<>", "<", ">=", ">"};

static token input_token, next_token;

queue<string> error_stack;
map<string, string> fir_set;
map<string, string> fol_set;

void error(){
    throw "Unexpected exception";
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
    else {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
    }
}

void gen_sets() {
    fir_set["SL"] = "id, read, write, if, while,";
    fir_set["S"] = "id, read, write, if, while";
    fir_set["F"] = "lparen, id, literal";
    fir_set["TT"] = "add, sub";
    fir_set["FT"] = "mul, div";
    fir_set["ro"] = "=, <>, <, >, <=, >=";
    fir_set["ao"] = "add, sub";
    fir_set["mo"] = "mul, div";
    fir_set["T"] = "lparen, id, literal";
    fir_set["P"] = "id, read, write, if, while";
    fir_set["E"] = "lparen, id, literal";
    fir_set["C"] = "lparen, id, literal";

    fol_set["SL"] = "end";
    fol_set["S"] = "id, read, write, if, while";
    fol_set["F"] = "mul, div";
    fol_set["TT"] = "rparen, =, <>, <, >, <=, >=, id, read, write, if, while";
    fol_set["FT"] = "add, sub";
    fol_set["ro"] = "lparen, id, lit";
    fol_set["ao"] = "lparen, id, lit";
    fol_set["mo"] = "lparen, id, lit";
    fol_set["T"] = "add, sub";
    fol_set["P"] = "eof";
    fol_set["E"] = "rparen, =, <>, <, >, <=, >=, id, read, write, if, while";
    fol_set["C"] = "id, read, write, if, while";
}

// Check whether a token in a fir_set specified by a symbol
bool checkfirst (token next_token, string cur_symbol, map<string, string> fir_set) {
    string s = fir_set[cur_symbol];
    string tok_s = names[next_token];
    if (s.find(tok_s) != string::npos){
        return true;
    } 
        return false;
}

// Check whether a token in a follow set specified by a symbol
bool checkfollow (token next_token, string cur_symbol, map<string, string> fol_set) {
    string s = fol_set[cur_symbol];
    string tok_s = names[next_token];
    if (s.find(tok_s) != string::npos){
        return true;
    } 
        return false;
    
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
    try {
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
                error();
        }
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            next_token = scan ();
            if (checkfirst (next_token, "P", fir_set)) {
                input_token = next_token;
                program ();
                return "";
            } else if (checkfollow (next_token, "P", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }

    ast += ")";
    return ast;
}

string stmt_list () {
    string ast = "";
    try {
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
            case t_end:
            case t_eof:
                cout << "predict stmt_list --> epsilon" << endl;
                return "";
                break;          /* epsilon production */
            default: error();
        }
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            cout << " Entered  sl ";
            next_token = scan ();
            cout << names[input_token];
            cout << names[next_token];
            if (checkfirst (next_token, "SL", fir_set)) {
                input_token = next_token;
                cout << names[next_token];
                stmt_list ();
                return "";
            } else if (checkfollow (next_token, "SL", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }
    return ast;
}

string stmt() {
    string ast = "";
    ast += "(";
    try{
        switch (input_token) {
            case t_id:
                cout << "predict stmt --> id gets expr" << endl;
                ast = ast + ":= " + "\"" +  token_image + "\"";
                match (t_id);
                match (t_gets);
                ast += expr ();
                break;
            case t_read:
                cout << "predict stmt --> read id" << endl;
                match (t_read);
                ast += "read ";
                ast = ast + "\"" + token_image + "\"";
                match (t_id);
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
                ast += cond ();
                ast += "[";
                ast += stmt_list ();
                ast += "]";
                match (t_end);
                break;
            case t_while:
                cout << "predict stmt --> while condition stmt_list end" << endl;
                match (t_while);
                ast += "while";
                ast += cond ();
                ast += "[";
                ast += stmt_list ();
                ast += "]";
                match (t_end);
                break;
            default: error();
        }
        ast += ")";
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            cout << "Entered  s";
            next_token = scan ();
            if (checkfirst (next_token, "S", fir_set)) {
                input_token = next_token;
                stmt();
                return "";
            } else if (checkfollow (next_token, "S", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }
    return ast;
}

//comdition expression ro expression
string cond (){
    string ast = "";
    try {
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
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            cout << "Entered  c";
            next_token = scan();
            if (checkfirst (next_token, "C", fir_set)) {
                input_token = next_token;
                cond();
                return "";
            } else if (checkfollow (next_token, "C", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }
    return ast;
}

string expr () {
    string ast = "";
    try {
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
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            cout << "Entered  E";
            next_token = scan();
            if (checkfirst (next_token, "E", fir_set)) {
                input_token = next_token;
                expr();
                return "";
            } else if (checkfollow (next_token, "E", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }
    return ast;
}

string term () {
    string ast = "";
    try {
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
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            cout << "Entered  T";
            next_token = scan();
            if (checkfirst (next_token, "T", fir_set)) {
                input_token = next_token;
                term();
                return "";
            } else if (checkfollow (next_token, "T", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }
    return ast;
}

string term_tail (string te) {
    string ast = "";
    try {
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
            case t_end:
            case t_eof:
                cout << "predict term_tail --> epsilon" << endl;
                return te;         /* epsilon production */
            default: error();
        }
        ast += ")";
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            cout << "Entered";
            next_token = scan();
            if (checkfirst (next_token, "TT", fir_set)) {
                input_token = next_token;
                term_tail (te);
                return "";
            } else if (checkfollow (next_token, "TT", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }
    return ast;
}

string factor () {
    string ast = "";
    try {
        switch (input_token) {
        case t_literal:
            cout << "predict factor --> literal"<< endl;
            ast += "(";
            ast = ast + "num " + "\"" + token_image + "\"";
            ast += ")";            
            match (t_literal);
            break;
        case t_id :
            cout << "predict factor --> id"<< endl;
            ast += "(";
            ast = ast + "id " + "\"" + token_image + "\"";
            ast += ")";
            match (t_id);
            break;
        case t_lparen:
            cout << "predict factor --> lparen expr rparen"<< endl;
            match (t_lparen);
            ast += expr ();
            match (t_rparen);
            break;
        default: error ();
    }
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            cout << "Entered  factor";
            next_token = scan();
            if (checkfirst (next_token, "F", fir_set)) {
                input_token = next_token;
                factor ();
                return "";
            } else if (checkfollow (next_token, "F", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }
    return ast;
}

string factor_tail (string fa) {
    string ast = "";
    try {
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
            case t_end:
            case t_eof:
                cout << "predict factor_tail --> epsilon" << endl;
                return fa;       /* epsilon production */
            default: ;
        }
    ast += ")";
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            cout << "Entered  factor ta";
            next_token = scan();
            if (checkfirst (next_token, "FT", fir_set)) {
                input_token = next_token;
                factor_tail (fa);
                return "";
            } else if (checkfollow (next_token, "FT", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }
    
    return ast;
}

string rela_op (){
    string ast = "";
    try {
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
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            cout << "Entered  ro";
            next_token = scan();
            if (checkfirst (next_token, "ro", fir_set)) {
                input_token = next_token;
                rela_op ();
                return "";
            } else if (checkfollow (next_token, "ro", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }
    
    return ast;
}

string add_op () {

    string ast = "";
    try {
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
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            cout << "Entered  ao";
            next_token = scan();
            if (checkfirst (next_token, "ao", fir_set)) {
                input_token = next_token;
                add_op ();
                return "";
            } else if (checkfollow (next_token, "ao", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }
    return ast;
}

string mul_op () {

    string ast = "";
    try {
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
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            cout << "Entered  mo";
            next_token = scan();
            if (checkfirst (next_token, "mo", fir_set)) {
                input_token = next_token;
                mul_op ();
                return "";
            } else if (checkfollow (next_token, "mo", fol_set)) {
                input_token = next_token;
                return "";
            } else {
                continue;
            }
        }
    }

    return ast;
}

int main () {
    // input_token = scan ();
    // string AST = program ();
    // cout << AST << endl;
    // return 0;
    gen_sets ();
    input_token = scan ();
    string AST = program ();

    if (!error_stack.empty ()) {
        while (!error_stack.empty ()){
            cout << error_stack.front () << endl;
            error_stack.pop ();
        }
        // cout << AST << endl;
    } else {
      cout << AST << endl;
    }
}