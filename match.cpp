#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct Node{
    Node* nxt;
    int beg;
    int siz;
}; 

Node head;

struct Ans{
   int beg;
   int siz;
};
Ans* ans = new Ans();

struct Request{
    int beg;
    int siz;
} request;

int blocks;
bool isWorst;

void malloc_worst();
void malloc_best();
void show_list();
void free_mem();
void write_back();

int main(int argc, char* argv[]){
    if(argc == 5){
        request.siz = atoi(argv[3]);
        request.beg = atoi(argv[2]);
        if(argv[4][0] == 'b'){
            isWorst = false;
        }else{
            isWorst = true;
        }
    }else if(argc == 4){
        request.siz = atoi(argv[2]);
        request.beg = -1;
        printf("!\n");
        if(argv[3][0] == 'b'){
            isWorst = false;
        }else{
            isWorst = true;
        }
    }else{
        printf("EX0: match.exe free 12 20 b(release 20 bytes, begin at 12, using best-matching).\nEX1: match.exe malloc 28 w(allocate 28 bytes, using worst-matching)\n");
        exit(0);
    }
    
    int begin, size;
    Node* p = &head;
    FILE* in;
    if(!(in = fopen("stats.txt", "r"))){
        printf("Fail to acquire memory States!\n");
    }
    
    while(~fscanf(in, "%d %d", &begin, &size)){
        printf("%d %d\n", begin, size);
        Node* newNode = new Node();
        p->nxt = newNode;
        p->beg = begin;
        p->siz = size;
        newNode->beg = -1;
        newNode->siz = 0;
        newNode->nxt = NULL;
        p = p->nxt;
        blocks ++;
    }
    fclose(in);
    
    printf("We get:\n");
    show_list();
    
    if(request.beg != -1){
        free_mem();
    }else if(isWorst){
        malloc_worst();
    }else{
        malloc_best();
    }
        
    printf("Current Mem-states:\n");
    show_list();
    write_back();
    return 0;
}

void malloc_worst(){
    Node* p = &head;
    ans->beg = -1;
    ans->siz = 0;
    
    Node* record = NULL;
    while(p){
        if(p->siz > request.siz && p->siz > ans->siz){
            ans->beg = p->beg;
            ans->siz = p->siz;
            record = p;
        }
        p = p->nxt;
    }
    
    printf("answer:\n");
    if(ans->beg == -1){
        printf("No matching blocks!\n");
    }else{
        printf("Hit! Block begins at %d with %d size!\n", ans->beg, ans->siz);
        if(record){
            int malloc_siz = request.siz;
            if(request.siz % 4){
                malloc_siz += (4 - (request.siz % 4));
            }
            record->beg += malloc_siz;
            record->siz -= malloc_siz;
        }
    }
}

void malloc_best(){
    Node* p = &head;
    ans->beg = -1;
    ans->siz = INT_MAX;
    
    Node* record = NULL;
    while(p){
        if(p->siz > request.siz && p->siz < ans->siz){
            ans->beg = p->beg;
            ans->siz = p->siz;
            record = p;
        }
        p = p->nxt;
    }
    
    printf("answer:\n");
    if(ans->beg == -1){
        printf("No matching blocks!\n");
    }else{
        printf("Hit! Block begins at %d with %d size!\n", ans->beg, ans->siz);
        if(record){
            int malloc_siz = request.siz;
            if(request.siz % 4){
                malloc_siz += (4 - (request.siz % 4));
            }
            record->beg += malloc_siz;
            record->siz -= malloc_siz;
        }
    }
}

void show_list(){
    int i = 0;
    Node* p = &head;
    //printf("p->next->beg: %d\n", p->nxt->beg);
    while(p){
        if(p->nxt){
            printf("Block%d begins at %d with size %d.\n", i, p->beg, p->siz);        
        }
        p = p->nxt;
        i++;
    }
}

void free_mem(){
    Node* p = &head;
    int end = request.siz + request.beg;
    bool flag = true;
    while(flag){
        if(p->beg == end){
            p->beg = request.beg;
            p->siz += request.siz;
            return;
        }else if((p->beg + p->siz) == request.beg){
            p->siz += request.siz;
            return;
        }
        else{
            if(p->nxt){
                p = p->nxt;
            }
            else{
                flag = false;
            }
        }
    }
    
    Node* newNode = new Node();
    newNode->nxt = NULL;
    newNode->beg = request.beg;
    newNode->siz = request.siz;
    p->nxt = newNode;
    return;
}

void write_back(){
    FILE* out;
    if(!(out = fopen("stats.txt", "w"))){
        printf("Fail to write back memory States!\n");
    }
    
    Node* p = &head;
    while(p){
        if(p->nxt){
            fprintf(out, "%d %d\n", p->beg, p->siz);
        }
        p = p->nxt;
    }
    
    fclose(out);
}