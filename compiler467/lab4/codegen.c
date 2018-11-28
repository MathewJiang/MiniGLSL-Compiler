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
    int value;
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

/*
 * get_new_reg
 * allocate a new register for the input node
 */
reg* get_new_reg(node* ast_node) {
    //TODO: 
    reg* new_reg = (reg *)malloc(sizeof(reg));
    new_reg->reg_name = "new_reg";
    return new_reg;
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

/**********************************************************************
 * Lab 4: Code Generation
 *********************************************************************/ 

/*
 * Helper method for genCode
 * @INPUT: 
 * mode: 
 *      0. default mode
 *      1. constructor mode
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
            //FIXME: genCode(ast->declaration.type);
            genCode_help(ast->declaration.expr, 0);
            break;
            
        case STATEMENTS_NODE:
            break;
        
        case TYPE_NODE:
            break;

        case VAR_NODE:
        {
            char* reg_name = get_predef_reg_name_by_id(ast->var_node.id);
            if (reg_name != NULL) {
                fprintf(outputFile, "", reg_name);
            }
        }
            break;

        case INT_NODE:
            if (mode == 1) {
                fprintf(outputFile, "%d", ast->int_val);
            }
            break;

        case FLOAT_NODE:
            if (mode == 1) {
                fprintf(outputFile, "%f", ast->float_val);
            }
            break;

        case BOOL_NODE:
            if (mode == 1) {
                if (ast->bool_val == 0) {
                    fprintf(outputFile, "0.0");
                } else {
                    fprintf(outputFile, "1.0");
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
            //TODO: 
        }
            break;

        case FUNCTION_NODE:
        {
            //FUNC_ID LPARENTHESES arguments_opt RPARENTHESES
            //TODO: 
            
        }
            break;
            
        case CONSTRUCTOR_NODE:
        {
            //type LPARENTHESES arguments RPARENTHESES
            //TODO: 
            fprintf(outputFile, "MOV ");
            reg* constr_reg = get_new_reg(ast);
            if (constr_reg != NULL) {
                fprintf(outputFile, "%s, ", constr_reg->reg_name);
            } else {
                //FIXME: rm printf statements
                printf("Error: [constructor_ARB_help]: register is NULL\n");
                errorOccurred = true;
                exit(1);
            }
            fprintf(outputFile, "{");
            genCode_help(ast->constructor.args, 1);
            fprintf(outputFile, "}");
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


/*
 * genCode
 * For code generation
 * output into frag.txt file
 */
void genCode(node *ast) {
    genCode_help(ast, 0);
}
