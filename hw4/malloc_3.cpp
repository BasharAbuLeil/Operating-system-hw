#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
#include<iostream>
#define MAX_SIZE 100000000
#define BLOCK_SIZE (1024*128)
#define MAX_ORDER 10
#define MIN_BLOCK 128
#define INITIAL_BLOCK 32

size_t allocated_block=0;
size_t allocated_bytes=0;
size_t free_bytes=0;
size_t free_block =0;
size_t metaDataBlocks = 0;
bool is_initialized =false;
unsigned long heapHead=0;//////////////////////////////////////////zeady
void* initHeap();/////////////////////////////////////////////zeady
void combine(void *p,size_t m_maxCapSize);
void* findFreeBlock(size_t size);
void deleteMappedBl(void *p);
bool isMergible(void *p);
unsigned long gtbudd(void* p, size_t custSiz);
static void putNew(size_t size,void* ptr);
void* srealloc(void* oldp, size_t size); 
int golbal_cookies=  rand();

static int findOrder(size_t size){
    int counter = 0;
    size_t right = MIN_BLOCK;
    size_t left = 0;
    while (counter <= MAX_ORDER) {
        if (size>right||size<left) {
            counter++;
            left = right;
            right *= 2;
        }
        else{
            return counter;
        }
    }
    return counter;
}
struct MallocMetaData
{
   size_t requested_size;
   bool is_free;
   int cookies;
   MallocMetaData* next;
   MallocMetaData* prev;
};
    MallocMetaData* attemptMrg(size_t targetSize,MallocMetaData* p);
    //MallocMetaData* getMetaData(void*p);
    MallocMetaData* free_blocks_array[MAX_ORDER+1];
    MallocMetaData* maped_list =nullptr;
  



static void addBFree(void* ptr){
    /*if(ptr==nullptr)return;
     MallocMetaData* temp= (MallocMetaData*)ptr;
     int find_order= findOrder(temp->requested_size+sizeof(MallocMetaData));
     if (find_order==-1)return;
     else{

        temp->is_free=true;
        if(free_blocks_array[find_order]!=nullptr)
        {
            MallocMetaData* helper= free_blocks_array[find_order];
            while ((unsigned long)ptr > (unsigned long)helper)
            {
                if(helper->next==nullptr)break;
                helper=helper->next;
            }
            if(helper->next==nullptr)
            {
                temp->prev= helper;
                temp->next=nullptr;
                helper->next=temp;
                return;  
            }

            temp->prev=helper->prev;
            temp->next= helper;
            helper->prev= temp;
            if(temp->prev==nullptr) 
            {
               free_blocks_array[find_order]=temp;
            }
            else
            {
                MallocMetaData* prev_up = temp->prev;
                prev_up->next=temp;
            }
              

            
        }

     }*/
     if(ptr == nullptr){
        return;
    }
    MallocMetaData* metaData = (MallocMetaData*)ptr;
    int order = findOrder(metaData->requested_size + sizeof(MallocMetaData));
    if(order == -1 ){
        return;
    }//todo complete
    else{
        metaData->is_free=true;
        if(free_blocks_array[order] == nullptr){
            free_blocks_array[order]  = metaData;
            return;
        }else{
            MallocMetaData* current = free_blocks_array[order];
            while((unsigned long)current < (unsigned long)ptr){
                if(current->next== nullptr){
                    break;
                }
                current = current->next;
            }
            if(current->next==nullptr){
                current->next=metaData;
                metaData->next=nullptr;
                metaData->prev=current;
                return;
            }
            metaData->prev=current->prev;
            metaData->next=current;
            current->prev=metaData;
            if(metaData->prev!= nullptr){
                metaData->prev->next=metaData;
            }else{
                free_blocks_array[order]  = metaData;
            }
        }
    }


}




static void removeblock(void* ptr){

   /*if(ptr==nullptr)return;
   MallocMetaData* temp= (MallocMetaData*)ptr;
   if(!temp->is_free)return;
   int find_order= findOrder(temp->requested_size);
   if (find_order==-1)return;
   else
   {
        if(free_blocks_array[find_order]==temp){
            free_blocks_array[find_order]= temp->next;
            if(temp->next!=nullptr) temp->next->prev=nullptr;
        }
        else
        {
            if(temp->prev==nullptr&&temp->next==nullptr)return;
            MallocMetaData* prev = temp->prev;
            MallocMetaData* next = temp->next;
            prev->next= next;
            if(next!=nullptr)
            {
               next->prev=prev;
            }
        }
        
   }
   temp->prev=nullptr;
   temp->next=nullptr;
   */
  if(ptr == nullptr){
        return;
    }

    MallocMetaData* metaData = (MallocMetaData*)ptr;
    if(!metaData->is_free){
        return ;
    }
    int order = findOrder(metaData->requested_size);
    if(order ==-1 ){
        return;
    }//todo complete
    else{
        if(free_blocks_array[order] == metaData){
            MallocMetaData* next = metaData->next;
            free_blocks_array[order] = next;
            if(next != nullptr){
                next->prev=nullptr;
            }
        }
        else{
            if(metaData->next == nullptr && metaData->prev == nullptr){
                return;
            }
            metaData->prev->next=metaData->next;
            if(metaData->next!= nullptr){
                metaData->next->prev=metaData->prev;
            }
        }
    }
    metaData->next=metaData->prev=nullptr;
    
}



static void putNew(size_t size,void* ptr){
    /*int find_order= findOrder(size);
    MallocMetaData* current=(MallocMetaData*)ptr;
    size_t temp_size= current->requested_size+ sizeof(MallocMetaData);
    int temp= findOrder(current->requested_size+sizeof(MallocMetaData));

    removeblock(current);////////////////
    allocated_block--;
    while (temp > find_order)
    {
        temp_size= temp_size/2;
        temp--;
        MallocMetaData* buddy = (MallocMetaData*)((char*)current+temp_size);
        // checking cookies
        buddy->is_free=true;
        current->requested_size=temp_size-sizeof(MallocMetaData);
        buddy->requested_size= temp_size-sizeof(MallocMetaData);
        addBFree(buddy); //addfreeeblock

        allocated_bytes-=sizeof(MallocMetaData);
        allocated_block++;
        free_bytes=free_bytes-sizeof(MallocMetaData);
        free_block++;
        metaDataBlocks++;

    }*/
    int order = findOrder(size);
    MallocMetaData* current = (MallocMetaData*) ptr;
    int currentOrder = findOrder(current->requested_size+sizeof(MallocMetaData));
    size_t currentSize = current->requested_size + sizeof(MallocMetaData);
    removeblock(current);
    while(currentOrder > order){
        currentOrder--;
        currentSize=(currentSize)/2;
        MallocMetaData* buddy = (MallocMetaData*)((char*)current+currentSize);
        //buddy->setCookie();
        buddy->is_free=true;
        buddy->requested_size=currentSize - sizeof(MallocMetaData);
        current->requested_size=currentSize - sizeof(MallocMetaData);
        addBFree(buddy);
        free_block++;
        free_bytes-=sizeof(MallocMetaData);
        allocated_block++;
        allocated_bytes-=sizeof(MallocMetaData);
        metaDataBlocks++;
    }
    
}




void* smalloc(size_t size){

    /*if(size > MAX_SIZE || size ==0 )// check if have to size==0
    {
        return NULL;
    }

    if(!is_initialized)
    {
        allocated_block=metaDataBlocks=free_block =32;
        allocated_bytes = free_bytes = BLOCK_SIZE*INITIAL_BLOCK - sizeof(MallocMetaData)*INITIAL_BLOCK;      
        is_initialized = true;
        

        size_t to_allocate = (BLOCK_SIZE * INITIAL_BLOCK);
        void* temp= sbrk(to_allocate); 
        if(temp==nullptr) return nullptr;
        unsigned long head= (unsigned long) temp;
        char* current =(char*)head;

        for(int i =0; i< INITIAL_BLOCK;i++)
        {
            MallocMetaData* meta_temp= (MallocMetaData*) current;
            if(i==0) meta_temp->prev=nullptr;
            else
            {
                meta_temp->prev= (MallocMetaData*)((char*)current-BLOCK_SIZE);
            }
            if(i==INITIAL_BLOCK-1) meta_temp->next=nullptr;
            else
            {
                meta_temp->next= (MallocMetaData*)((char*)current+BLOCK_SIZE);
            }

            meta_temp->is_free= true;
            meta_temp->cookies=golbal_cookies;
            meta_temp->requested_size= BLOCK_SIZE-sizeof(MallocMetaData);
            current+=BLOCK_SIZE;  
            
        }

        free_blocks_array[MAX_ORDER]=(MallocMetaData*)head;
        for (int i = 0; i < MAX_ORDER; i++)
        {
            free_blocks_array[i]=nullptr;
        }
        if (initHeap() == nullptr) { //initHeap Failed
            return nullptr;
        }
    }

    if(BLOCK_SIZE < sizeof(MallocMetaData)+size)
    {
        void* temp = (void*)mmap(nullptr,sizeof(MallocMetaData)+size,PROT_READ | PROT_WRITE,MAP_ANONYMOUS | MAP_PRIVATE,-1,0);
        if(temp == nullptr)return NULL;
        MallocMetaData* meta_data= (MallocMetaData*)temp;
        meta_data->cookies= golbal_cookies;
        meta_data->requested_size= size;
        meta_data->is_free=false;
        // check cookies
        metaDataBlocks=metaDataBlocks+1;
        allocated_bytes+=size;
        allocated_block++;
        if (maped_list!= nullptr)
        {
            maped_list->prev= meta_data;
            meta_data->next=maped_list;
            meta_data->prev=nullptr;
            maped_list=meta_data;
        }
        else
        {
            maped_list= meta_data;
        }

        return (void*)((char*)meta_data+sizeof(MallocMetaData));
        
    }
    else
    {
         int find_order= findOrder(size+sizeof(MallocMetaData));
         if(find_order==-1) return NULL;
         while (find_order <= MAX_ORDER)
         {
            if (free_blocks_array[find_order]!=nullptr)
            {
                break;// end the while(check if need void*)
            }
            find_order++;     
         }
         if(find_order== MAX_ORDER+1)return NULL;// the while didnt break
           
          putNew(size+sizeof(MallocMetaData),free_blocks_array[find_order]);
          free_blocks_array[find_order]->is_free=false;
          free_block= free_block-1;
          free_bytes -= free_blocks_array[find_order]->requested_size;
          
          return (void*)((char*)free_blocks_array[find_order]+sizeof(MallocMetaData));
            
    }
    void* freeBlock = findFreeBlock(sizeof(MallocMetaData) + size);
    if(freeBlock == nullptr){
        return nullptr;
    }
    putNew(sizeof(MallocMetaData) + size,freeBlock);
    MallocMetaData* headerData=(MallocMetaData*)freeBlock;
    headerData->is_free=true;
    free_block--;
    free_bytes-=headerData->requested_size;
    return (void*)((char*)headerData+sizeof(MallocMetaData));*/
    if (!is_initialized) {
        size_t additionalMemory = (INITIAL_BLOCK*BLOCK_SIZE-(unsigned long)sbrk(0))%(INITIAL_BLOCK*BLOCK_SIZE);// first sbrk call
        if(sbrk(additionalMemory)== nullptr){//second sbrk call
             return nullptr;
         }
        void* result =sbrk(INITIAL_BLOCK*BLOCK_SIZE) ;//third and last sbrk call
        if(result == nullptr){
            return nullptr;
        }
        heapHead = (unsigned long)result;
        char* currentAddress =(char*) heapHead;
        for(int i=0;i<INITIAL_BLOCK;++i){
            MallocMetaData* metaDataPtr = (MallocMetaData*)currentAddress;
            MallocMetaData* prev = (i == 0) ? nullptr : (MallocMetaData*)((char*)currentAddress-BLOCK_SIZE);
            MallocMetaData* next = (i == INITIAL_BLOCK-1) ? nullptr : (MallocMetaData*)((char*)currentAddress+BLOCK_SIZE);
            metaDataPtr->prev = prev;
            metaDataPtr->next = next;
            metaDataPtr->is_free = true;
            metaDataPtr->requested_size = BLOCK_SIZE-sizeof(MallocMetaData);
            currentAddress+=BLOCK_SIZE;
        }
        free_blocks_array[MAX_ORDER] = (MallocMetaData*)heapHead;
        for(int i=0;i<MAX_ORDER;++i){ // initate others to nullptr;
            free_blocks_array[i]= nullptr;
        }
        allocated_block=metaDataBlocks=free_block =INITIAL_BLOCK;
        allocated_bytes = free_bytes = BLOCK_SIZE*INITIAL_BLOCK - sizeof(MallocMetaData)*INITIAL_BLOCK;
        is_initialized = true;
    }
    if(size > MAX_SIZE){
        return nullptr;
    }
    size_t totalSize = sizeof(MallocMetaData) + size;
    if (totalSize > BLOCK_SIZE) {
        void* temp = (void*)mmap(nullptr,sizeof(MallocMetaData)+size,PROT_READ | PROT_WRITE,MAP_ANONYMOUS | MAP_PRIVATE,-1,0);
        if(temp == nullptr)return NULL;
        MallocMetaData* meta_data= (MallocMetaData*)temp;
        //meta_data->cookies= golbal_cookies;
        meta_data->requested_size= size;
        meta_data->is_free=false;
        // check cookies
        metaDataBlocks=metaDataBlocks+1;
        allocated_bytes+=size;
        allocated_block++;
        if (maped_list!= nullptr)
        {
            maped_list->prev= meta_data;
            meta_data->next=maped_list;
            meta_data->prev=nullptr;
            maped_list=meta_data;
        }
        else
        {
            maped_list= meta_data;
        }

        return (void*)((char*)meta_data+sizeof(MallocMetaData));
    }
    void* freeBlock = findFreeBlock(totalSize);
    if(freeBlock == nullptr){
        return nullptr;
    }
    putNew(totalSize,freeBlock);
    MallocMetaData* metaData = (MallocMetaData*) freeBlock;
    metaData->is_free=false;
    free_bytes-=metaData->requested_size;
    free_block--;
    return (void*)((char*)metaData+sizeof(MallocMetaData));
}


void* scalloc(size_t num, size_t size){
    if (num == 0 || size == 0 || num * size > MAX_SIZE)  
        return nullptr;
    void* new_block= smalloc(num*size);
    if(new_block==nullptr)    
        return nullptr;
    memset(new_block, 0, num*size);
    return new_block;
}

void sfree(void* p){
    if(p==nullptr)
        return ;
    MallocMetaData* m_metaData=(MallocMetaData*)((char*)p-sizeof(MallocMetaData));
    if(m_metaData->is_free)
        return;////////already freed
    if(m_metaData->requested_size<=BLOCK_SIZE){
        m_metaData->is_free=true;
        combine(m_metaData,0);
    }
    else{
        deleteMappedBl(m_metaData);
        m_metaData->is_free=true;
        munmap((void*)(m_metaData),m_metaData->requested_size+sizeof(*m_metaData));
    }
}

size_t _num_free_blocks(){
    return free_block;
}

size_t _num_free_bytes(){
    return free_bytes;
}

size_t _num_allocated_blocks(){
    return allocated_block;
}

size_t _num_allocated_bytes(){
    return allocated_bytes;
}
size_t _num_meta_data_bytes(){
    return sizeof(MallocMetaData)*metaDataBlocks;
}
size_t _size_meta_data(){
    return sizeof(MallocMetaData);
}





void combine(void *p,size_t m_maxCapSize){
    MallocMetaData* m_merged;
    MallocMetaData* m_bud;
    free_block=free_block+1;
    MallocMetaData * m_meta=(MallocMetaData*)p;
    free_bytes=free_bytes+m_meta->requested_size;
    while(isMergible(m_meta)){
        //need to implement isMergible.
        if (m_maxCapSize>0 &&m_meta->requested_size>=m_maxCapSize){
            break;
        }
        m_bud=(MallocMetaData*)gtbudd(m_meta,0);
        m_merged= (MallocMetaData*)((unsigned long)m_bud > (unsigned long)m_meta) ? m_meta : m_bud;
        size_t mshndlSize=2*(sizeof(*m_meta)+m_bud->requested_size)-sizeof(MallocMetaData);//*m_meta  instead of MallocMetaData struct.
        removeblock(m_bud);
        removeblock(m_meta);
        free_block=free_block-1;
        m_merged->is_free=true;
        m_merged->requested_size=mshndlSize;
        m_meta=m_merged;
        allocated_block=allocated_block-1;
        metaDataBlocks=metaDataBlocks-1;
        free_bytes=sizeof(MallocMetaData)+free_bytes;
        allocated_bytes=sizeof(MallocMetaData)+allocated_bytes;
    }
    addBFree(m_meta);
}



void* srealloc(void* oldp, size_t size){
    if(size> MAX_SIZE || size == 0) 
        return nullptr;
    if (oldp == nullptr) {
        return smalloc(size);
    }
    MallocMetaData* m_meta =(MallocMetaData*)((char*)oldp-sizeof(MallocMetaData));
    if (BLOCK_SIZE<size)
    {
        if (m_meta->requested_size!=size){
            void* mshndlHugeBlock = smalloc(size);
            memmove(mshndlHugeBlock,oldp,size);
            sfree(oldp);
            return mshndlHugeBlock;
        }
        return oldp;
    }
    if(m_meta->requested_size>=size){
        return oldp;
    }
    void * res=nullptr;
    MallocMetaData* m_mrg=attemptMrg(size,m_meta);
    if(m_mrg==nullptr){
        res=smalloc(size);
        if(res==nullptr)
            return nullptr;
        MallocMetaData* temp=(MallocMetaData*)((char*)oldp-sizeof(MallocMetaData));
        memmove(res,oldp,temp->requested_size);
        sfree(oldp);
        return res;
    }
    res=(void*)((char*)m_mrg+sizeof(MallocMetaData));
    memmove(res,oldp,size);
    free_bytes=free_bytes-m_mrg->requested_size;
    free_block=free_block-1;
    m_mrg->is_free=false;
    return res;
}



MallocMetaData* attemptMrg(size_t targetSize,MallocMetaData* p){
    if(p==nullptr)
        return nullptr;
    MallocMetaData* m_bud = nullptr;
    size_t m_tot = sizeof(MallocMetaData)+targetSize ;
    MallocMetaData* m_meta = p;
    size_t m_currLength=sizeof(*m_bud)+m_meta->requested_size;
    while (m_tot >m_currLength&&m_currLength<BLOCK_SIZE){
        m_bud=(MallocMetaData*)gtbudd(m_meta,m_currLength-sizeof(MallocMetaData));
        if(m_bud->is_free==false){
            break;
        }
        m_currLength=2*m_currLength;
        m_meta=(MallocMetaData*)((unsigned long)m_bud <= (unsigned long)m_meta) ? m_bud : m_meta;

    }
    if(m_currLength<m_tot||m_currLength>=BLOCK_SIZE){
        return nullptr;
    }
    combine(p,m_currLength-sizeof(*m_meta));
    return m_meta;
}



bool isMergible(void *p){
    /*
    if(ptr == nullptr){
        return false;
    }
    MallocMetaData* current =(MallocMetaData*) ptr;
    if(current->getSize() +sizeof(MallocMetaData) >= INITIAL_BLOCK_SIZE){
        return false;
    }
    MallocMetaData* buddy= (MallocMetaData*)getBuddy(current);
    if(!buddy->checkFree()){
        return false;
    }
    if(buddy->getSize()!= current->getSize()){
        return false;
    }
    return true;
    */
    MallocMetaData* m_bud;
    MallocMetaData* m_meta;
    if(!p)
        return false;
    m_meta=(MallocMetaData*)p;
    if(BLOCK_SIZE <=sizeof(*m_meta)+m_meta->requested_size)
        return false;
    m_bud=(MallocMetaData*)gtbudd(p,0);
    if(m_bud->is_free==false)
        return false;
    if(m_meta->requested_size==m_bud->requested_size)
        return true;
    return false;
}


unsigned long gtbudd(void* p, size_t custSiz){
    /*MallocMetaData* metaData = (MallocMetaData*)ptr;
    size_t size = (customSize == 0) ? metaData->getSize() : customSize;
    return ((unsigned long)(size+sizeof(MallocMetaData)) ^ (unsigned long)metaData);*/
    MallocMetaData* m_meta = (MallocMetaData*)p;
    size_t length;
    if(custSiz!=0)
        length=custSiz;
    else 
        length=m_meta->requested_size;
    return ((unsigned long)(sizeof(*m_meta)+length) ^ (unsigned long)m_meta);
}



void deleteMappedBl(void *p){
    /*
    if(ptr == nullptr){
        return;
    }
    MallocMetaData* metaData = (MallocMetaData*) ptr;
    metaData->freed(true);
    MallocMetaData* prev = metaData->getPrev();
    MallocMetaData* next = metaData->getNext();
    allocatedBlocks--;
    metaDataBlocks--;
    allocatedBytes-=metaData->getSize();
    if(ptr == mmappedBlocksHead){
        if(next == nullptr){
            mmappedBlocksHead = nullptr;
            return;
        }else{
            mmappedBlocksHead = next;
            next->setPrev(nullptr);
        }
    }else{
        if(next == nullptr){
            prev->setNext(nullptr);
        }else{
            next->setPrev(prev);
            prev->setNext(next);
        }
    }
    metaData->setNext(nullptr);
    metaData->setPrev(nullptr);
    */
    if(p == nullptr){
        return;
    }
    metaDataBlocks=metaDataBlocks-1;
    MallocMetaData* m_meta = (MallocMetaData*) p;
    m_meta->is_free=true;
    MallocMetaData* m_next = m_meta->next;
    allocated_block=allocated_block-1;
    MallocMetaData* m_pre = m_meta->prev;
    allocated_bytes=allocated_bytes-m_meta->requested_size;
    if (maped_list==p){
        if(m_next!=nullptr){
            maped_list=m_next;
            m_next->prev=nullptr;
        }
        else{
            maped_list=nullptr;
            return;
        }
    }
    else{
        if (m_next!=nullptr)
        {
            m_pre->next=m_next;
            m_next->prev=m_pre;
        }
        else{
            m_pre->next=nullptr;
        }
    }
    m_meta->next=nullptr;
    m_meta->prev=nullptr;
}



// addition

void* findFreeBlock(size_t size){
    int order = findOrder(size);
    if(order == -1){
        return nullptr; // should not be the case.
    }
    int firstAvailableOrderBlocks = order;
    while(firstAvailableOrderBlocks <= MAX_ORDER){
        if(free_blocks_array[firstAvailableOrderBlocks]!=nullptr){
            return free_blocks_array[firstAvailableOrderBlocks] ;
        }
        firstAvailableOrderBlocks++;
    }
    return nullptr;

}

//////// another ns7

void* initHeap(){
    size_t additionalMemory = (INITIAL_BLOCK*BLOCK_SIZE-(unsigned long)sbrk(0))%(INITIAL_BLOCK*BLOCK_SIZE);// first sbrk call
    if(sbrk(additionalMemory)== nullptr){//second sbrk call
        return nullptr;
    }
    void* result =sbrk(INITIAL_BLOCK*BLOCK_SIZE) ;//third and last sbrk call
    if(result == nullptr){
        return nullptr;
    }
    heapHead = (unsigned long)result;
    char* currentAddress =(char*) heapHead;
    for(int i=0;i<INITIAL_BLOCK;++i){
        MallocMetaData* metaDataPtr = (MallocMetaData*)currentAddress;
        MallocMetaData* prev = (i == 0) ? nullptr : (MallocMetaData*)((char*)currentAddress-BLOCK_SIZE);
        MallocMetaData* next = (i == INITIAL_BLOCK-1) ? nullptr : (MallocMetaData*)((char*)currentAddress+BLOCK_SIZE);
        metaDataPtr->prev = prev;
        metaDataPtr->next = next;
        metaDataPtr->is_free = true;
        metaDataPtr->requested_size = BLOCK_SIZE-sizeof(MallocMetaData);
        currentAddress+=BLOCK_SIZE;
    }
    free_blocks_array[MAX_ORDER] = (MallocMetaData*)heapHead;
    for(int i=0;i<MAX_ORDER;++i){ // initate others to nullptr;
        free_blocks_array[i]= nullptr;
    }
    allocated_block=metaDataBlocks=free_block =INITIAL_BLOCK;
    allocated_bytes = free_bytes = BLOCK_SIZE*INITIAL_BLOCK - sizeof(MallocMetaData)*INITIAL_BLOCK;
    is_initialized = true;
    return (void*)heapHead;
}
