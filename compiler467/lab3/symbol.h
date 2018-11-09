#ifndef SYMBOL_H_
#define SYMBOL_H_ 1

#include <stdarg.h>
#include <stdbool.h>
#include "ast.h"

struct snode_entry {
    char* id;
//    void* content;
//    void* ref;
    struct node_* node;
    struct snode_entry* next;
    struct snode_entry* prev;
};
typedef snode_entry sentry;

struct scope_node {
    struct scope_node* parent_scope; 
    sentry* sentry_head;
    sentry* sentry_tail;
    int sentry_count;
};
typedef scope_node snode;

// Sentry functions
sentry* sentry_alloc();
sentry* sentry_alloc(char* id, node* node);
sentry* sentry_push(snode* snode, sentry* sentry);
sentry* sentry_pop(snode* snode);
sentry* find_sentry_in_snode_by_id(char* id, snode* snode);
sentry* remove_sentry_from_snode(sentry* sentry, snode* snode);
        
// Snode functions
snode* snode_alloc(snode* _parent_node);
int snode_destroy(snode* snode);
// Upper scope?


#endif /* SYMBOL_H_ */