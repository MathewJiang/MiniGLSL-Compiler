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

/* REMOVE THIS 
 * get_new_reg
 * allocate a new register for the input node
 */
reg* get_new_reg(node* ast_node) {
    //TODO: 
    reg* new_reg = (reg *)malloc(sizeof(reg));
    new_reg->reg_name = "new_reg";
    return new_reg;
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

//
///*
// * constructor_ARB_help
// * helper methods specifically for constructor ARB code print
// */
//void constructor_ARB_help(node* constr_node) {
//    if (ast == NULL) {
//        return;
//    }
//
//    node_kind kind = ast->kind;
//    switch (kind) {
//        case SCOPE_NODE:
//        {
//            printf("scope node\n");
//        }
//            break;
//            
//        case CONSTRUCTOR_NODE: 
//        {
//            fprintf(outputFile, "MOV ");
//            reg* constr_reg = get_new_reg(ast);
//            if (!constr_reg) {
//                fprintf(outputFile, "%s, ", constr_reg->reg_name);
//            } else {
//                //FIXME: rm printf statements
//                printf("Error: [constructor_ARB_help]: register is NULL\n");
//                errorOccurred = true;
//                exit(1);
//            }
//            fprintf(outputFile, "{");
//            constructor_ARB_help(&(ast->constructor.args));
//            fprintf(outputFile, "}\n");
//        }
//            break;
//            
//        case ARGS_NODE:
//            constructor_ARB_help(&(ast->args_node.args));
//            fprintf(outputFile, ", ");
//            constructor_ARB_help(&(ast->args_node.expr));
//            break;
//        
//        case INT_NODE:
//            fprintf(outputFile, "%d", ast->int_val);
//            break;
//
//        case FLOAT_NODE:
//            fprintf(outputFile, "%f", ast->float_val);
//            break;
//
//        case BOOL_NODE:
//            if (ast->bool_val == 0) {
//                fprintf(outputFile, "0.0");
//            } else {
//                fprintf(outputFile, "1.0");
//            }
//            break;
//        
//        default:
//            printf("Error: [constructor_ARB_help] unexpected node type\n");
//            break;
//    }
//}

// Global variables
reg* reg_head = NULL;
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
 *      TBA.
 * 
 */
void genCode_help(node* ast, int mode) {
    if (ast == NULL) {
        return;
    }

    node_kind kind = ast->kind;
    switch (kind) {
        case SCOPE_NODE:
            genCode_help(ast->scope.declarations, 0);
            genCode_help(ast->scope.statements, 0);
            break;
            
        case NESTED_SCOPE_NODE:
            break;
            
        case DECLARATIONS_NODE:
            genCode_help(ast->declarations.declarations, 0);
            genCode_help(ast->declarations.declaration, 0);
            break;

        case DECLARATION_NODE:
        {
            //type ID SEMICOLON
            //|   type ID ASSIGNMENT expression SEMICOLON
            //|   CONST type ID ASSIGNMENT expression SEMICOLON
            fprintf(outputFile, "#DECLARATION @ line %d\n", ast->line_num);
            
            //FIXME: genCode(ast->declaration.type);
            genCode_help(ast->declaration.expr, 0);
            reg* decl_id_reg = get_register(ast);
            
            fprintf(outputFile, "TEMP ");
            fprintf(outputFile, "%s;\n", decl_id_reg->reg_name);
            
            if (ast->declaration.expr != NULL) {
                reg* decl_expr_reg = get_register(ast->declaration.expr);
                fprintf(outputFile, "MOV ");
                fprintf(outputFile, "%s, ", decl_id_reg->reg_name);
                fprintf(outputFile, "%s;\n", decl_expr_reg->reg_name);
            }
        }
            break;
            
        case STATEMENTS_NODE:
            genCode_help(ast->statements.statements, 0);
            genCode_help(ast->statements.statement, 0);
            break;
        
        case TYPE_NODE:
            break;

        case VAR_NODE:
        {
            //TODO: 
            char* var_reg_name = get_predef_reg_name_by_id(ast->var_node.id);
            if (var_reg_name != NULL) {
                fprintf(outputFile, "%s", var_reg_name);
            } else {
                if (mode == 2) {
                    //if a function call
                    reg* var_reg = get_register(ast);
                    fprintf(outputFile, "%s", var_reg->reg_name);
                }
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
            //TODO: variable ASSIGNMENT expression SEMICOLON
            fprintf(outputFile, "#ASSIGMENT @ line %d\n", ast->line_num);
            genCode_help(ast->assign_statement.var, 0);
            genCode_help(ast->assign_statement.expr, 0);
            
            reg* assign_var_reg = get_register(ast->assign_statement.var);
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
            } else {
                printf("Error: [ASSIGNMENT_STMT_NODE]: ast->assign_statement.var is NULL\n");
                exit(1);
            }
            
            fprintf(outputFile, "%s;\n", assign_expr_reg->reg_name);
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
            if (strcmp(func_id, "rsq")) {
                fprintf(outputFile, "RSQ ");
            } else if (strcmp(func_id, "dp3")) {
                fprintf(outputFile, "DP3 ");
            } else if (strcmp(func_id, "lit")) {
                fprintf(outputFile, "LIT ");
            } else {
                printf("Error: [FUNCTION]: unknown func_id\n");
                exit(1);
            }
            genCode_help(ast->function.args, 2);
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
            genCode_help(ast->constructor.args, 1);
            fprintf(outputFile, "};\n");
        }
            break;
            
        case ARGS_NODE:
            genCode_help(ast->args_node.args, mode);
            if (ast->args_node.args != NULL) {
                fprintf(outputFile, ", ");
            }
            genCode_help(ast->args_node.expr, mode);
            break;
            
        default:
            //printf("[debug]AST Print: unknown AST node\n");
            break;
    }
}
    
reg* get_register(node* ast_node) {
    if (!ast_node) return NULL;
    if (find_register(ast_node)) return find_register(ast_node);
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
    switch(ast->kind) {
        case DECLARATION_NODE:
            // May be Param type, check if the declr is const.
            if (ast->declaration.is_const) {
                result->type = PARAM_REG_TYPE;
                sprintf(nameBuf, "param_reg_%d", param_reg_counter++);
                result->reg_name = nameBuf;
            } else {
                result->type = TEMP_REG_TYPE;
                sprintf(nameBuf, "temp_reg_%d", temp_reg_counter++);
                result->reg_name = nameBuf;
            }
            break;
        case VAR_NODE:
            // Can be Global(predef_var) type and specific name.
            if (get_predef_var_by_id(ast->var_node.id)) {
                free(nameBuf);
                nameBuf = NULL;
                result->type = GLOB_REG_TYPE;
                result->reg_name = get_predef_reg_name_by_id(ast->var_node.id);
            } else {
                result->type = TEMP_REG_TYPE;
                sprintf(nameBuf, "temp_reg_%d", temp_reg_counter++);
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

reg* find_register(node* ast_node) {
    if (!reg_head || !ast_node) return NULL;
    reg* finder = reg_head;
    while(finder) {
        if (finder->ast_node == ast_node) return finder;
        finder = finder->next;
    }
    return NULL;
}

char* get_predef_reg_name_by_id(char* id) {
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


/*
 * genCode
 * For code generation
 * output into frag.txt file
 */
void genCode(node *ast) {
    fprintf(outputFile, "!!ARBfp1.0\n\n");
    genCode_help(ast, 0);
    fprintf(outputFile, "\nEND\n");
}
