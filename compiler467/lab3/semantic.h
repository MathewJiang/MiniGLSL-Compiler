#ifndef _SEMANTIC_H_ 
#define _SEMANTIC_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"

void ast_semantic_check(node* ast);
void ast_semantic_check_help(node* ast, snode* curr_scope);
sentry* find_var_reference_by_id(char* id, snode* curr_scope);
bool scope_check_var_declaration_valid(node* ast, snode* curr_scope);
bool semantic_check_constructor_call(node* cstr_stmt);
bool semantic_check_function_call(node* func_call);

predef_var* alloc_predef_var(char* id, type_class clazz, type_id type_name, bool is_vec, int vec_size);
predef_var* get_predef_var_by_id(char* id);

#endif
