#ifndef SYMBOL_H_
#define SYMBOL_H_ 1

#include <stdarg.h>
#include <stdbool.h>
#include "ast.h"

typedef enum { // vec type_id = vec_base_type + vec_len
    UNKNOWN_TYPE = 0,
    BOOL_TYPE = 10,
//    BVEC2_TYPE = 12,
//    BVEC3_TYPE = 13,
//    BVEC4_TYPE = 14,
    INT_TYPE = 20,
//    IVEC2_TYPE = 22,
//    IVEC3_TYPE = 23,
//    IVEC4_TYPE = 24,
    FLOAT_TYPE = 30,
//    VEC2_TYPE = 32,
//    VEC3_TYPE = 33,
//    VEC4_TYPE = 34,
    DEFAULT_TYPE = 100
} sentry_type;

struct snode_entry {
    char* id;
    sentry_type stype; 
    bool is_const;
    bool is_vec;
    int vec_size;
    bool valid;
    struct snode_entry* next;
    struct snode_entry* prev;
};
typedef struct snode_entry sentry;

struct scope_node {
    struct scope_node* parent_scope; 
    sentry* sentry_head;
    sentry* sentry_tail;
    int sentry_count;
};
typedef struct scope_node snode;

// Sentry functions
sentry* sentry_alloc();
sentry* sentry_alloc(char* id, sentry_type stype, int is_const, int is_vec, int vec_size);
sentry* sentry_push(sentry* sentry, snode* snode);
sentry* sentry_pop(snode* snode);
void sentry_print(sentry* sentry);
sentry* find_sentry_in_snode_by_id(char* id, snode* snode);
sentry* remove_sentry_from_snode(sentry* sentry, snode* snode);
sentry* ast_node_to_sentry(node* ast_node);
        
// Snode functions
snode* snode_alloc(snode* _parent_node);
int snode_destroy(snode* snode);
void snode_print(snode* snode);

// Helper functions
sentry_type name_to_stype(char* type);

#endif /* SYMBOL_H_ */