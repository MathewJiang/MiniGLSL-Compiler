#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "ast.h"
#include "semantic.h"
#include "symbol.h"
#include "common.h"

/**********************************************************************
 * Global stuffs go here
 *********************************************************************/ 
// Global structures
typedef enum reg_type {
    UNKNOWN_REG_TYPE = 3056,
    PARAM_REG_TYPE = 3057,      // Used for const declarations 
    TEMP_REG_TYPE = 3058,       // Used for all usual cases
    GLOB_REG_TYPE = 3059        // Used for global predefined variables
} reg_type;

typedef struct ast_register {
    struct ast_register* next;
    node* ast_node;
    char* reg_name;
    enum reg_type type;
    int value;
} reg;

// Function declarations

// Register helpers
reg* get_register(node* ast_node);
void add_register(reg* _reg);
reg* alloc_register(node* ast_node);
reg* find_register(node* ast_node);
char* get_predef_reg_name_by_id(char* id);
reg* find_register_by_ast(node* ast_node);
reg* find_register_by_reg_name(char* reg_name);
char* get_glob_reg_name_by_id(char* id);
reg* get_latest_register_by_id(char* id, snode* current_scope);
void print_index_from_num(int i);

// Global variables
reg* reg_head = NULL;
snode* codegen_root_scope = NULL;
int param_reg_counter = 0;
int temp_reg_counter = 0;
/**********************************************************************
 * Lab 4: Code Generation
 *********************************************************************/ 

/*
 * Helper method for genCode
 * @INPUT: 
 * mode: 
 *      0. default mode
 *      1. constructor mode
 *      2. function mode
 *      3. contains predefined variables
 *      TBA.
 * 
 */
void genCode_help(node* ast, int mode, snode* curr_scope) {
    if (ast == NULL) {
        return;
    }

    node_kind kind = ast->kind;
    switch (kind) {
        case SCOPE_NODE:
            if (!curr_scope) {
                // Prepare the root scope.
                codegen_root_scope = snode_alloc(NULL);
                curr_scope = codegen_root_scope;
            }
            genCode_help(ast->scope.declarations, 0, curr_scope);
            genCode_help(ast->scope.statements, 0, curr_scope);
            
            // Destroy the root scope.
            snode_destroy(codegen_root_scope);
            codegen_root_scope = NULL;
            break;
            
        case NESTED_SCOPE_NODE:
            // push nested scope onto symbol table
            curr_scope = snode_alloc(curr_scope);
            genCode_help(ast->scope.declarations, 0, curr_scope);
            genCode_help(ast->scope.statements, 0, curr_scope);
            // Destroy current scope.
            snode_destroy(curr_scope);
            curr_scope = NULL;
            break;
            
        case DECLARATIONS_NODE:
            genCode_help(ast->declarations.declarations, 0, curr_scope);
            genCode_help(ast->declarations.declaration, 0, curr_scope);
            break;

        case DECLARATION_NODE:
        {
            //type ID SEMICOLON
            //|   type ID ASSIGNMENT expression SEMICOLON
            //|   CONST type ID ASSIGNMENT expression SEMICOLON
            fprintf(outputFile, "#DECLARATION @ line %d\n", ast->line_num);
            // Push the declared variable onto symbol table.
            sentry* newly_declared_sentry = ast_node_to_sentry(ast);
            sentry_push(newly_declared_sentry, curr_scope);
            
            //FIXME: genCode(ast->declaration.type);
            genCode_help(ast->declaration.expr, 0, curr_scope);
            
            reg* decl_id_reg = get_register(ast);
            fprintf(outputFile, "TEMP ");
            fprintf(outputFile, "%s;\n", decl_id_reg->reg_name);
            
            if (ast->declaration.expr != NULL) {
                predef_var* pv = get_predef_var_by_id(ast->declaration.expr->var_node.id);
                if (pv != NULL) {
                    fprintf(outputFile, "MOV ");
                    fprintf(outputFile, "%s, ", decl_id_reg->reg_name);
                    fprintf(outputFile, "%s;\n", get_glob_reg_name_by_id(ast->declaration.expr->var_node.id));
                } else {
                    reg* decl_expr_reg = get_register(ast->declaration.expr);
                    fprintf(outputFile, "MOV ");
                    fprintf(outputFile, "%s, ", decl_id_reg->reg_name);
                    fprintf(outputFile, "%s;\n", decl_expr_reg->reg_name);
                }
            }
        }
            break;
            
        case STATEMENTS_NODE:
            genCode_help(ast->statements.statements, 0, curr_scope);
            genCode_help(ast->statements.statement, 0, curr_scope);
            break;
        
        case TYPE_NODE:
            break;

        case VAR_NODE:
        {
            //TODO: 
            if (mode == 2) {
                //if a function call
                reg* var_reg = get_latest_register_by_id(ast->var_node.id, curr_scope);
                fprintf(outputFile, "%s", var_reg->reg_name);
            }
        }
            break;

        case INT_NODE:
            if (mode == 1) {
                fprintf(outputFile, "%d", ast->int_val);
            } else {
                //FIXME: 
                reg* int_reg = get_register(ast);
                fprintf(outputFile, "PARAM %s = %d;\n", int_reg->reg_name, ast->int_val);
            }
            break;

        case FLOAT_NODE:
            if (mode == 1) {
                fprintf(outputFile, "%f", ast->float_val);
            }else {
                reg* float_reg = get_register(ast);
                fprintf(outputFile, "PARAM %s = %f;\n", float_reg->reg_name, ast->float_val);
            }
            break;

        case BOOL_NODE:
            if (mode == 1) {
                if (ast->bool_val == 0) {
                    float f_var = 0.0;
                    fprintf(outputFile, "%f", f_var);
                } else {
                    float t_var = 1.0;
                    fprintf(outputFile, "%f", t_var);
                }
            }
            break;
           
//      case EXPRESSION_NODE: 
//          break;

        case UNARY_EXPRESSION_NODE:
            break;

        case BINARY_EXPRESSION_NODE:
        {
            //TODO: 
            
        }
            break;
            
        case IF_STATEMENT_NODE:
        {
            //TODO:
            //IF LPARENTHESES expression RPARENTHESES statement else_statement
            //CMP R0, R1, R2, R3; // R0.x = (R1.x < 0.0) ? R2.x : R3.x; Same applies for y, z, w
            
            //set a variable passing in for statement
            //if the value is 1, print all the code
            //else, don't print anything
            
            
            
        }
            break;
            
        case ELSE_STATEMENT_NODE:
            break;
            
        case ASSIGNMENT_STATEMENT_NODE:
        {
            //MUST OVERRIDE SYMBOL ENTRY'S NODE_REF TO THIS NODE WHEN REASSIGNING VARIABLE VALUES!!!
            //TODO: variable ASSIGNMENT expression SEMICOLON
            fprintf(outputFile, "#ASSIGMENT @ line %d\n", ast->line_num);
            genCode_help(ast->assign_statement.var, 0, curr_scope);
            genCode_help(ast->assign_statement.expr, 0, curr_scope);
            
            reg* assign_var_reg = get_latest_register_by_id(ast->assign_statement.var->var_node.id, curr_scope);
            reg* assign_expr_reg = get_register(ast->assign_statement.expr);
            
            fprintf(outputFile, "MOV ");
            if (ast->assign_statement.var != NULL) {
                if (ast->assign_statement.var->var_node.is_vec) {
                    fprintf(outputFile, "%s.", assign_var_reg->reg_name);
                    print_index_from_num(ast->assign_statement.var->var_node.vec_idx);
                    fprintf(outputFile, ", ");
                } else {
                    fprintf(outputFile, "%s, ", assign_var_reg->reg_name);
                }
                
                // IMPORTANT: Change the node reference in symbol table to current node.
                if (!get_predef_var_by_id(ast->assign_statement.var->var_node.id)) {
                    find_latest_sentry_by_id(ast->assign_statement.var->var_node.id, curr_scope)->node_ref = ast; // Let it segfault if this returns NULL. What happened?
                }
            } else {
                printf("Error: [ASSIGNMENT_STMT_NODE]: ast->assign_statement.var is NULL\n");
                exit(1);
            }
            
            
            if (ast->assign_statement.expr->kind == VAR_NODE) {
                reg* assign_expr_var_reg = get_latest_register_by_id(ast->assign_statement.expr->var_node.id, curr_scope);
                if (assign_expr_var_reg == NULL) {
                    printf("ERROR: [ASSIGNMENT] var_reg is NULL\n");
                    exit(1);
                }
                fprintf(outputFile, "%s;\n", assign_expr_var_reg->reg_name);
            } else {
                fprintf(outputFile, "%s;\n", assign_expr_reg->reg_name);
            }
        }
            break;

        case FUNCTION_NODE:
        {
            //FUNC_ID LPARENTHESES arguments_opt RPARENTHESES
            //TODO: 
            fprintf(outputFile, "#FUNCTION @ line %d\n", ast->line_num);
            reg* func_reg = get_register(ast);
            fprintf(outputFile, "TEMP %s;\n", func_reg->reg_name);
            
            char* func_id = ast->function.func_id;
            if (!strcmp(func_id, "rsq")) {
                fprintf(outputFile, "RSQ ");
            } else if (!strcmp(func_id, "dp3")) {
                fprintf(outputFile, "DP3 ");
            } else if (!strcmp(func_id, "lit")) {
                fprintf(outputFile, "LIT ");
            } else {
                printf("Error: [FUNCTION]: unknown func_id\n");
                exit(1);
            }
            
            fprintf(outputFile, "%s, ", func_reg->reg_name);
            genCode_help(ast->function.args, 2, curr_scope);
            fprintf(outputFile, ";\n");
        }
            break;
            
        case CONSTRUCTOR_NODE:
        {
            //type LPARENTHESES arguments RPARENTHESES
            //TODO: 
            reg* constr_reg = get_register(ast);
            fprintf(outputFile, "TEMP %s;\n", constr_reg->reg_name);
            fprintf(outputFile, "MOV ");
            if (constr_reg != NULL) {
                fprintf(outputFile, "%s, ", constr_reg->reg_name);
                //fprintf(outputFile, "%s, ", garbage_reg->reg_name);
            } else {
                //FIXME: rm printf statements
                printf("Error: [constructor_ARB_help]: register is NULL\n");
                errorOccurred = true;
                exit(1);
            }
            fprintf(outputFile, "{");
            genCode_help(ast->constructor.args, 1, curr_scope);
            fprintf(outputFile, "};\n");
        }
            break;
            
        case ARGS_NODE:
            genCode_help(ast->args_node.args, mode, curr_scope);
            if (ast->args_node.args != NULL) {
                fprintf(outputFile, ", ");
            }
            genCode_help(ast->args_node.expr, mode, curr_scope);
            break;
            
        default:
            //printf("[debug]AST Print: unknown AST node\n");
            break;
    }
}
    
reg* get_register(node* ast_node) {
    if (!ast_node) return NULL;
    if (find_register_by_ast(ast_node)) return find_register_by_ast(ast_node);
    // Allocate register structure and insert into global table.
    reg* result = alloc_register(ast_node);
    add_register(result);
    return result;
}

reg* alloc_register(node* ast_node) {
    if (!ast_node) return NULL;
    reg* result = (reg*)malloc(sizeof(reg));
    result->next = NULL;
    result->ast_node = ast_node;
    result->type = UNKNOWN_REG_TYPE;
    result->reg_name = NULL;
    result->value = -1; // ????
    
    char* nameBuf = (char*)malloc(20);
    // Determine reg type and name based on ast_node
    switch(ast_node->kind) {
        case DECLARATION_NODE:
            // May be Param type, check if the declr is const.
            if (ast_node->declaration.is_const) {
                result->type = PARAM_REG_TYPE;
                sprintf(nameBuf, "param_reg_%d_%s", param_reg_counter++, ast_node->declaration.id);
                result->reg_name = nameBuf;
            } else {
                result->type = TEMP_REG_TYPE;
                sprintf(nameBuf, "temp_reg_%d_%s", temp_reg_counter++, ast_node->declaration.id);
                result->reg_name = nameBuf;
            }
            break;
        case VAR_NODE:
            // Can be Global(predef_var) type and specific name.
            if (get_predef_var_by_id(ast_node->var_node.id)) {
                free(nameBuf);  // buffer for glob reg name is allocated elsewhere.
                nameBuf = NULL;
                result->ast_node = NULL;    //Global registers shouldn't be associated with any node.
                result->type = GLOB_REG_TYPE;
                result->reg_name = get_glob_reg_name_by_id(ast_node->var_node.id);
            } else {
                result->type = TEMP_REG_TYPE;
                sprintf(nameBuf, "temp_reg_%d_%s", temp_reg_counter++, ast_node->var_node.id);
                result->reg_name = nameBuf;
            }
            break;
            
        default:
            // All other nodes deserve temp register.
            result->type = TEMP_REG_TYPE;
            sprintf(nameBuf, "temp_reg_%d", temp_reg_counter++);
            result->reg_name = nameBuf;
            break;
        
    }
        
    return result;
}

void add_register(reg* _reg) {
    // Add at head.
    if (!_reg) return;
    _reg->next = reg_head;
    reg_head = _reg;
}

reg* find_register_by_ast(node* ast_node) {
    if (!reg_head || !ast_node) return NULL;
    reg* finder = reg_head;
    while(finder) {
        if (finder->ast_node == ast_node) return finder;
        finder = finder->next;
    }
    return NULL;
}

reg* find_register_by_reg_name(char* reg_name) {
    if (!reg_name) return NULL;
    reg* finder = reg_head;
    while(finder) {
        if (!strcmp(finder->reg_name, reg_name)) return finder;
        finder = finder->next;
    }
    return NULL;
}

char* get_glob_reg_name_by_id(char* id) {
    char* regName = NULL;
    if (!id) return regName;
    if (!strcmp(id, "gl_FragColor")) {
        regName = strdup("result.color");
    } else if (!strcmp(id, "gl_FragDepth")) {
        regName = strdup("result.depth");
    } else if (!strcmp(id, "gl_FragCoord")) {
        regName = strdup("fragment.position");
    } else if (!strcmp(id, "gl_TexCoord")) {
        regName = strdup("fragment.texcoord");
    } else if (!strcmp(id, "gl_Color")) {
        regName = strdup("fragment.color");
    } else if (!strcmp(id, "gl_Secondary")) {
        regName = strdup("fragment.color.secondary");
    } else if (!strcmp(id, "gl_FogFragCoord")) {
        regName = strdup("fragment.fogcoord");
    } else if (!strcmp(id, "gl_Light_Half")) {
        regName = strdup("state.light[0].half");
    } else if (!strcmp(id, "gl_Light_Ambient")) {
        regName = strdup("state.lightmodel.ambient");
    } else if (!strcmp(id, "gl_Material_Shininess")) {
        regName = strdup("state.material.shininess");
    } else if (!strcmp(id, "env1")) {
        regName = strdup("program.env[1]");
    } else if (!strcmp(id, "env2")) {
        regName = strdup("program.env[2]");
    } else if (!strcmp(id, "env3")) {
        regName = strdup("program.env[3]");
    }
    return regName;
}

reg* get_glob_reg_by_predef_id(char* predef_id) {
    if (!predef_id) return NULL;
    char* glob_reg_name = predef_id;
    reg* glob_reg = find_register_by_reg_name(glob_reg_name);
    if (!glob_reg) {
        // Allocate a global register
        glob_reg = (reg*)malloc(sizeof(reg));
        glob_reg->next = NULL;
        glob_reg->ast_node = NULL;  //global registers shouldn't be associated with any node.
        glob_reg->type = GLOB_REG_TYPE;
        glob_reg->reg_name = glob_reg_name;
        glob_reg->value = -1;
        add_register(glob_reg); // Add to list
    }
    return glob_reg;
}

// Find the register for a variable in the most local scope. Return global register for predefined variables.
// Purpose of this is to get the register for a variable id in any scope.
reg* get_latest_register_by_id(char* id, snode* current_scope) {
    if (!id || !current_scope) return NULL;
    if (get_glob_reg_name_by_id(id)) return get_glob_reg_by_predef_id(get_glob_reg_name_by_id(id));
    // Consult the symbol table for the most local variable's node.
    sentry* latest_sentry = find_latest_sentry_by_id(id, current_scope);
    
    if (!latest_sentry) return NULL; // Very unlikely that variable with id is undeclared globally, given passed semantic checking.
    
    return get_register(latest_sentry->node_ref);
}

/*
 * 
 */
void print_index_from_num(int i) {
    switch(i) {
        case 0:
            printf("x");
            break;
        case 1:
            printf("y");
            break;
        case 2:
            printf("z");
            break;
        case 3:
            printf("w");
            break;
        default:
            printf("Error: [print_index_from_num]: i not in range\n");
            break;
    }
}

/*
 * genCode
 * For code generation
 * output into frag.txt file
 */
void genCode(node *ast) {
    fprintf(outputFile, "!!ARBfp1.0\n\n");
    genCode_help(ast, 0, codegen_root_scope);
    fprintf(outputFile, "\nEND\n");
}