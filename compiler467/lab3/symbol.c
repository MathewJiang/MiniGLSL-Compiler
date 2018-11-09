#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol.h"

// Sentry functions
sentry* sentry_alloc() {
    sentry* new_sentry = (sentry*)malloc(sizeof(sentry));
    new_sentry->id = NULL;
    new_sentry->node = NULL;
    new_sentry->next = NULL;
    new_sentry->prev = NULL;
    return new_sentry;
}

sentry* sentry_alloc(char* id, node* node) {
    sentry* new_sentry = sentry_alloc();
    new_sentry->id = id;
    new_sentry->node = node;
    return new_sentry;
}

sentry* sentry_push(snode* snode, sentry* sentry) {
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