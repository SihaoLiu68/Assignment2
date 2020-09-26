/* Definitions the scanner shares with the parser
    Michael L. Scott, 2008-2020.
*/

//token extended with if, while, end, and conditional operators
typedef enum {t_read, t_write, t_id, t_literal, t_gets,
              t_add, t_sub, t_mul, t_div, t_lparen, t_rparen, 
              t_eof, t_if, t_while, t_end, t_equal, t_lessequal,
              t_notequal, t_less, t_greaterequal, t_greater} token;

#define MAX_TOKEN_LEN 128
extern char token_image[MAX_TOKEN_LEN];

extern token scan();
extern int line_num;