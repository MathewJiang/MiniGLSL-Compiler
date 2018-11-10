#ifndef _SEMANTIC_H_ 
#define _SEMANTIC_H_


#include <stdio.h>
#include "symbol.h"

void ast_semantic_check(node* ast);
void ast_semantic_check_help(node* ast, snode* curr_scope);
sentry* find_var_reference_by_id(char* id, snode* curr_scope);
bool scope_check_var_declaration_valid(node* ast, snode* curr_scope);

#endif
