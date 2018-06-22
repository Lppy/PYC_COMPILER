#ifndef TABLE_H_
#define TABLE_H_

#define TABSIZE 127

typedef struct binder_ *binder;
struct binder_ {
    void *key; 
    void *value; 
    binder next; 
    void *prevtop;
};

typedef struct TAB_table_ *TAB_table;
struct TAB_table_ {
	binder table[TABSIZE];
	void *top;
};

TAB_table TAB_empty(void);

void TAB_enter(TAB_table t, void *key, void *value);

void *TAB_look(TAB_table t, void *key);

void *TAB_pop(TAB_table t);

void TAB_dump(TAB_table t, void (*show)(void *key, void *value));

#endif