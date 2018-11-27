#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "ast.h"

/**********************************************************************
 * Global stuffs go here
 *********************************************************************/ 
// Function declarations
typedef enum reg_type {
    UNKNOWN_REG_TYPE = 3056,
    PARAM_REG_TYPE = 3057,
    TEMP_REG_TYPE = 3058,
    GLOB_REG_TYPE = 3059        
} reg_type;

// Global structures
typedef struct ast_register {
    struct ast_register* next;
    node* ast_node;
    char* reg_name;
    enum reg_type type;
} reg;

reg* get_register(node* ast_node) {
    return NULL;
}

//void generate_reg_name(reg* theReg, bool is) {
//    if (!theReg || ! regName) return;
//    theReg->reg_name = strdup(regName);
//    return;
//}

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

// Global variables
/**********************************************************************
 * Lab 4: Code Generation
 *********************************************************************/ 

/*
 * genCode
 * For code generation
 * output into frag.txt file
 */
void genCode(node *ast) {
    if (ast == NULL) {
        return;
    }

    node_kind kind = ast->kind;
    switch (kind) {
        case SCOPE_NODE:
            break;
            
        case NESTED_SCOPE_NODE:
            break;
            
        case DECLARATIONS_NODE:
            break;

        case DECLARATION_NODE:
            break;
            
        case STATEMENTS_NODE:
            break;
        
        case TYPE_NODE:
            break;

        case VAR_NODE:
            break;

        case INT_NODE:
            break;

        case FLOAT_NODE:
            break;

        case BOOL_NODE:
            break;
           
//      case EXPRESSION_NODE: 
//          break;

        case UNARY_EXPRESSION_NODE:
            break;

        case BINARY_EXPRESSION_NODE:        
            break;
            
        case IF_STATEMENT_NODE:
            break;
            
        case ELSE_STATEMENT_NODE:
            break;
            
        case ASSIGNMENT_STATEMENT_NODE:
            break;

        case FUNCTION_NODE:
            break;
            
        case CONSTRUCTOR_NODE:
            break;
            
        case ARGS_NODE:
            break;
            
        default:
            printf("[debug]AST Print: unknown AST node\n");
            break;
    }
}



