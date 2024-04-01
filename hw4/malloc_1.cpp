#include <unistd.h>

#define MAX_SIZE 100000000

void* smalloc(size_t size){

    if(size==0||size> MAX_SIZE) {
        return NULL;
    }
    void* newAlloc= sbrk(size);

    if(*(int*)newAlloc == -1){
       return NULL;
    }
    return newAlloc;

}
