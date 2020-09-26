#include <iostream>
#include <string>
#include <queue>
#include <map>

#include "scan.h"
using namespace std;
//names[token] used to output the token name("read", "write", etc.) when matched
const string names[] = {"read", "write", "id", "literal", "gets",
                       "add", "sub", "mul", "div", "lparen", 
                       "rparen", "eof", "if", "while", "end", "=", 
                       "<=", "<>", "<", ">=", ">"};

static token input_token, next_token;

queue<string> error_stack;
map<string, string> fir_set;
map<string, string> fol_set;

// throw a simple error
void error(){
    throw "Unexpected exception";
}

void match(token expected){
    if(input_token == expected){
        /*
        cout << "matched " << names[input_token];
        if(input_token == t_id || input_token == t_literal){
            cout << ": " << token_image;
        }
        cout << endl;
        */
        input_token = scan();
    }
    else {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
    }
}

// generate the first and follow sets of all symbols
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

    fol_set["SL"] = "end, eof";
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

// Check if the next token is in the first _set of current symbol
bool checkfirst (token next_token, string cur_symbol, map<string, string> fir_set) {
    string s = fir_set[cur_symbol];
    string tok_s = names[next_token];
    //find if given token s is found in the first set of the current symbol
    if (s.find(tok_s) != string::npos){
        return true;
    } 
        return false;
}

// Check if the next token is in the follow _set of current symbol
bool checkfollow (token next_token, string cur_symbol, map<string, string> fol_set) {
    string s = fol_set[cur_symbol];
    string tok_s = names[next_token];
    //find if given token s is found in the follow set of the current symbol
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
//P -> [ SL ]
string program () {
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    try {
        ast += "(program ";
        switch (input_token) {
            case t_id:
            case t_read:
            case t_write:
            case t_if:
            case t_while:
            case t_eof:
                //ast recursively adds square brackets and its subtree stmt_list()
                ast += "[ ";
                ast += stmt_list ();
                ast += "] ";
                match (t_eof);
                break;
            default: 
                error();
        }
    // here we implemented the exception-based error recovery 
    } catch (const char* &e) {
        // record the error message
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            // take in the next token
            next_token = scan ();
            // check if in the first set
            if (checkfirst (next_token, "P", fir_set)) {
                input_token = next_token;
                // start over
                program ();
                return "";
            // check if in the follow set
            } else if (checkfollow (next_token, "P", fol_set)) {
                input_token = next_token;
                return "";
            } else {
            //continue to find the next token
                continue;
            }
        }
    }

    ast += ")";
    return ast;
}

//SL -> S SL | epsilon
string stmt_list () {
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    try {
        switch (input_token) {
            case t_id:
            case t_read:
            case t_write:
            case t_if:
            case t_while:
                ast += stmt ();
                ast += stmt_list ();
                break;
            case t_end:
            case t_eof:
                return "";
                break;          /* epsilon production */
            default: error();
        }
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            // take in next token
            next_token = scan ();
            // check if in the first set
            if (checkfirst (next_token, "SL", fir_set)) {
                input_token = next_token;
                cout << names[next_token];
                stmt_list ();
                return "";
            // check if in the follow set
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

//S -> id:= E | read id | write E | if C SL end | while C SL end
string stmt() {
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    ast += "(";//fixed number of fields in subtrees, start with parentheses
    try{
        switch (input_token) {
            case t_id:
                ast = ast + ":= " + "\"" +  token_image + "\"";// append := and "id" to ast
                match (t_id);
                match (t_gets);
                ast += expr ();
                break;
            case t_read:
                match (t_read);
                ast += "read ";
                ast = ast + "\"" + token_image + "\"";// append "id" to ast
                match (t_id);
                break;
            case t_write:
                match (t_write);
                ast += "write ";
                ast += expr ();
                break;
            case t_if:
                match (t_if); 
                ast += "if ";
                ast += cond ();
                ast += "[";
                ast += stmt_list ();
                ast += "]";
                match (t_end);
                break;
            case t_while:
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
        ast += ")";// end of parentheses of subtrees
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            next_token = scan ();
            // check if in the first set
            if (checkfirst (next_token, "S", fir_set)) {
                input_token = next_token;
                stmt();
                return "";
            // check if in the follow set
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

//C -> E ro E
string cond (){
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    try {
        //temps store two expressions and one relational operation
        string temp1 = "";
        string temp2 = "";
        string temp3 = "";
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                temp1 += expr ();
                temp2 += rela_op ();
                temp3 += expr ();
                ast += "(" + temp2 + temp1 + temp3 + ")";// put temps in correct order to output linear, parenthesized form
                break;
            default: error ();
        }
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            next_token = scan();
            // check if in the first set
            if (checkfirst (next_token, "C", fir_set)) {
                input_token = next_token;
                cond();
                return "";
            // check if in the follow set
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

//E -> T TT
string expr () {
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                ast += term_tail (term ());//first excutes term (), use what it returns as the parameter in term_tail (term ())
                break;
            default: error ();
        }
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            next_token = scan();
            // check if in the first set
            if (checkfirst (next_token, "E", fir_set)) {
                input_token = next_token;
                expr();
                return "";
            // check if in the follow set
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

//T -> F FT
string term () {
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    try {
        switch (input_token) {
            case t_id:
            case t_literal:
            case t_lparen:
                ast += factor_tail (factor ());//first executes factor(), then use what it returns as the parameter in factor_tail (factor ())
                break;
            default: error ();
        }
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            next_token = scan();
            // check if in the first set
            if (checkfirst (next_token, "T", fir_set)) {
                input_token = next_token;
                term();
                return "";
            // check if in the follow set
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

//TT -> ao T TT | epsilon
string term_tail (string te) {
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    try {
        ast += "(";
        switch (input_token) {
            case t_add:
            case t_sub:
                ast += add_op ();// appends add or minus
                ast += te;// appends term ()
                ast += term_tail (term ());// appends term_tail( term ())
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
                return te;         /* epsilon production, returns the parameter, which is the previous return of term() */
            default: error();
        }
        ast += ")";
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            next_token = scan();
            // check if in the first set
            if (checkfirst (next_token, "TT", fir_set)) {
                input_token = next_token;
                term_tail (te);
                return "";
            // check if in the follow set
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

//F -> (E) | id | lit
string factor () {
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    try {
        switch (input_token) {
        case t_literal:// F-> lit
            ast += "(";
            ast = ast + "num " + "\"" + token_image + "\"";
            ast += ")";            
            match (t_literal);
            break;
        case t_id :// F-> id
            ast += "(";
            ast = ast + "id " + "\"" + token_image + "\"";
            ast += ")";
            match (t_id);
            break;
        case t_lparen:// F -> (E)
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
            next_token = scan();
            // check if in the first set
            if (checkfirst (next_token, "F", fir_set)) {
                input_token = next_token;
                factor ();
                return "";
            // check if in the follow set
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

//FT -> mo F FT | epsilon
string factor_tail (string fa) {
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    try {
        ast += "(";
        switch (input_token) {
            case t_mul:
            case t_div:
                ast += mul_op ();
                ast += fa;//appends return of factor()
                ast += factor_tail (factor ());//appends return of factor_tail(factor ())
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
                return fa;       /* epsilon production, returns the parameter, which is the return of previous factor() */
            default: ;
        }
    ast += ")";
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            next_token = scan();
            // check if in the first set
            if (checkfirst (next_token, "FT", fir_set)) {
                input_token = next_token;
                factor_tail (fa);
                return "";
            // check if in the follow set
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

//ro -> = | <> | < | > | <= | >=
string rela_op (){
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    try {
        switch (input_token) {
            case t_equal:
                match (t_equal);
                ast += "= ";
                break;
            case t_notequal:
                match (t_notequal);
                ast += "<> ";
                break;
            case t_less:
                match (t_less);
                ast += "< ";
                break;
            case t_greater:
                match (t_greater);
                ast += "> ";
                break;
            case t_lessequal:
                match (t_lessequal);
                ast += "<= ";
                break;
            case t_greaterequal:
                match (t_greaterequal);
                ast += ">= ";
                break;
            default: error ();
        }
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            next_token = scan();
            // check if in the first set
            if (checkfirst (next_token, "ro", fir_set)) {
                input_token = next_token;
                rela_op ();
                return "";
             // check if in the follow set
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

//ao -> + | -
string add_op () {
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    try {
        switch (input_token) {
            case t_add:
                match (t_add);
                ast += "+ ";            
                break;
            case t_sub:
                match (t_sub);
                ast += "- ";            
                break;
            default: error ();
        }
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {        
            next_token = scan();
            // check if in the first set
            if (checkfirst (next_token, "ao", fir_set)) {
                input_token = next_token;
                add_op ();
                return "";
            // check if in the follow set
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

//mo -> * | /
string mul_op () {
    string ast = "";//ast outputs the linear, parenthesized syntax tree
    try {
        switch (input_token) {
            case t_mul:
                match (t_mul);
                ast += "* ";            
                break;
            case t_div:
                match (t_div);
                ast += "/ ";            
                break;
            default: error ();
        }
    } catch (const char* &e) {
        string emgs = "[Error] Line " + to_string(line_num) + ": " + token_image + " is unexpected.\n";
        error_stack.push (emgs);
        while (true) {
            next_token = scan();
            // check if in the first set
            if (checkfirst (next_token, "mo", fir_set)) {
                input_token = next_token;
                mul_op ();
                return "";
            // check if in the follow set
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
    gen_sets ();
    input_token = scan ();
    string AST = program ();//starts the program and return the linear, parenthesized syntax tree and save in AST
     
    if (!error_stack.empty ()) { // print the error stack if it is not empty
        while (!error_stack.empty ()){
            cout << error_stack.front () << endl;
            error_stack.pop ();
        }
    } else {
      cout << AST << endl;
    }
}
