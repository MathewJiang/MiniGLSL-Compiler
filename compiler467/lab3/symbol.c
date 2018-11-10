#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"
#include "ast.h"

// Sentry functions
sentry* sentry_alloc() {
    sentry* new_sentry = (sentry*)malloc(sizeof(sentry));
    new_sentry->id = NULL;
    new_sentry->stype = UNKNOWN_TYPE;
    new_sentry->valid = 0;
    new_sentry->is_const = 0;
    new_sentry->is_vec = 0;
    new_sentry->vec_size = 0;
    
    new_sentry->next = NULL;
    new_sentry->prev = NULL;
    return new_sentry;
}

sentry* sentry_alloc(char* id, sentry_type stype, int is_const, int is_vec, int vec_size) {
    sentry* new_sentry = sentry_alloc();
    new_sentry->id = id;
    new_sentry->stype = stype;
    new_sentry->is_const = is_const;
    new_sentry->is_vec = is_vec;
    new_sentry->vec_size = vec_size;
    
    new_sentry->valid = 1;
    return new_sentry;
}

sentry* sentry_push(sentry* sentry, snode* snode) {
    if (!snode || !sentry) {
        printf("[Error]Snode push: snode or sentry is NULL!!!\n");
        return NULL;
    }
    
    if (find_sentry_in_snode_by_id(sentry->id, snode)) {
        printf("[Error]Snode push: Entry with id %s already in snode!!!\n", sentry->id);
        return NULL;
    }

    if (!snode->sentry_head) {
        snode->sentry_head = sentry;
        sentry->next = NULL;
        sentry->prev = NULL;
    } else {
        sentry->next = snode->sentry_head;
        sentry->prev = NULL;
        sentry->next->prev = sentry;
        snode->sentry_head = sentry;
    }
    
    if (!snode->sentry_tail) {
        snode->sentry_tail = sentry;
    }
    
    snode->sentry_count++;
    
    return sentry;
}

sentry* find_sentry_in_snode_by_id(char* id, snode* snode) {
    if (!id || !snode) return NULL;
    sentry* finder = snode->sentry_head;
    while (finder) {
        if (!strcmp(finder->id, id)) {
            return finder;
        }
        finder = finder->next;
    }
    return NULL;
}

sentry* sentry_pop(snode* snode) {
    if (!snode || !snode->sentry_head) return NULL;
    return remove_sentry_from_snode(snode->sentry_head, snode);
}

sentry* remove_sentry_from_snode(sentry* sentry, snode* snode) {
    if (!sentry || !snode || !find_sentry_in_snode_by_id(sentry->id, snode)) return NULL;
    if (!find_sentry_in_snode_by_id(sentry->id, snode)) {
        printf("[Error]Sentry-remove-from-snode: Cannot find entry with id %s in snode!!!\n", sentry->id);
        return NULL;
    }
    
    if (snode->sentry_head == sentry) {
        snode->sentry_head = sentry->next;
    }
    if (snode->sentry_tail == sentry) {
        snode->sentry_tail = sentry->prev;
    }
    if (sentry->next) {
        sentry->next->prev = sentry->prev;
    }
    if (sentry->prev) {
        sentry->prev->next = sentry->next;
    }
    sentry->next = NULL;
    sentry->prev = NULL;
    snode->sentry_count--;
    
    return sentry;
}

void sentry_print(sentry* sentry) {
    if (!sentry) return;
    printf("id: %s, type_code:%d", sentry->id, sentry->stype);
}

sentry* ast_node_to_sentry(node* ast_node) {
    if (!ast_node) return NULL;
    if (ast_node->kind != DECLARATION_NODE) {
        printf("[ast_to_entry]: cannot create sentry from ast node of kind %d\n", ast_node->kind);
        return NULL;
    }
    
    char* id = ast_node->declaration.id;
    int is_const = ast_node->declaration.is_const;
    int is_vec = ast_node->declaration.type->type_node.is_vec;
    int vec_size = ast_node->declaration.type->type_node.vec_size;
    sentry_type type = name_to_stype(ast_node->declaration.type->type_node.type_name);
    if (type == UNKNOWN_TYPE) return NULL;
    
    return sentry_alloc(id, type, is_const, is_vec, vec_size);
}

// Snode functions
snode* snode_alloc(snode* _parent_node) {
    snode* new_snode = (snode*)malloc(sizeof(snode));
    new_snode->parent_scope = _parent_node;
    new_snode->sentry_head = NULL;
    new_snode->sentry_count = 0;
    return new_snode;
}

int snode_destroy(snode* snode) {
    if (!snode) return 0;
    snode->parent_scope = NULL;
    while(snode->sentry_head) {
        free(sentry_pop(snode));
    }
    if (snode->sentry_count != 0) {
        printf("[Error]Snode-destroy: sentry_count is not zero after destroying all snodes!!!\n");
    }
    free(snode);
    return 1;
}

void snode_print(snode* snode) {
    if (!snode) return;
    sentry* printer = snode->sentry_head;
    printf("********Printing scope table with %d entries********\n", snode->sentry_count);
    while(printer) {
        sentry_print(printer);
        if (printer->next) {
            printf("| ");
        }
        printer = printer->next;
    }
    printf("\n*************************end**************************\n\n");
}


// Other helper functions
sentry_type name_to_stype(char* type) {
    if (!type) return UNKNOWN_TYPE;
    if (type[0] == 'i') return INT_TYPE;
    if (type[0] == 'f' || type[0] == 'v') return FLOAT_TYPE;
    if (type[0] == 'b') return BOOL_TYPE;
    printf("[Name-to-stype]: Unknown type for ast type_name %s\n", type);
    return DEFAULT_TYPE; 
}

type_id stype_to_type_id(sentry_type type) {
    switch(type) {
        case UNKNOWN_TYPE:
            return ANY;
        case INT_TYPE:
            return INT;
        case FLOAT_TYPE:
            return FLOAT;
        case BOOL_TYPE:
            return BOOL;
        default:
            printf("[error]Stype_to_type_id: Unexpected stype, aborting\n");
            exit(1);
    }
}