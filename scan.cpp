/* Simple ad-hoc scanner for the calculator language.
    Michael L. Scott, 2008-2020.
*/

#include <iostream>
#include <vector>
#include <cstring>

#include "scan.h"

using namespace std;

char token_image[MAX_TOKEN_LEN];

int line_num = 1;

token scan(){
    static int c = ' ';
    int i = 0;

    while(isspace(c)){
        if(c == '\n') {
            line_num++;
        }
        c = cin.get();
    }
    if(c == EOF)
        return t_eof;
    if(isalpha(c)){//find out what the string is and return its token
        do{
            token_image[i++] = c;
            if(i >= MAX_TOKEN_LEN){
                cout << "max token length exceeded" << endl;
                exit(1);
            }
            c = cin.get();
        }while(isalpha(c) || isdigit(c) || c == '_');
        token_image[i] = '\0';
        if (!strcmp(token_image, "read")) return t_read;
        else if (!strcmp(token_image, "write")) return t_write;
        else if (!strcmp(token_image, "if")) return t_if;//extension 'if'
        else if (!strcmp(token_image, "while")) return t_while;//extension 'while'
        else if (!strcmp(token_image, "end")) return t_end;//extension 'end'
        else return t_id;
    }
    else if(isdigit(c)){
        do {
            token_image[i++] = c;
            c = cin.get();
        } while (isdigit(c));
        token_image[i] = '\0';
        return t_literal;        
    } else switch (c){//extended with relational operations '=', '<>', '<=', '<', '>', '>='
        case ':':
            if ((c = cin.get()) != '=') {
                cout << stderr << "error for :=\n";
                exit(1);        
            }else{
                c = cin.get();
                return t_gets;
            }
            break;
        case '=':
            c = cin.get();
            return t_equal;
            break;
        case '<':
            c = cin.get();
            if(c == '='){
                c = cin.get();
                return t_lessequal;
            }else if(c == '>'){
                c = cin.get();
                return t_notequal;
            }else{
                c = cin.get();
                return t_less;
            }
            break;
        case '>':
            c = cin.get();
            if(c == '='){
                c = cin.get();
                return t_greaterequal;
            }else{
                c = cin.get();
                return t_greater;
            }
            break;
         case '$'://extended for test.txt that ends with $$
            if((c = cin.get()) != '$'){
                cerr << "error for $\n";
                exit(1);
            }else{
                c = cin.get();
                return t_eof;
            }
            break;    
        case '(': c = cin.get(); return t_lparen;
        case ')': c = cin.get(); return t_rparen;
        case '+': c = cin.get(); return t_add;
        case '-': c = cin.get(); return t_sub;
        case '*': c = cin.get(); return t_mul;
        case '/': c = cin.get(); return t_div;


        default:
            cout << "error" << endl;
            exit(1);
    }

}