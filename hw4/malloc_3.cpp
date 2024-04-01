#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
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

int golbal_cookies=  rand();

static int findOrder(size_t size){
    int order = 0;
    size_t low = 0;
    size_t high = MIN_BLOCK;
    while (order <= MAX_ORDER) {
        if (size >= low && size < high) {
            return order;
        }
        order++;
        low = high;
        high *= 2;
    }
    return order;
}
struct MallocMetaData
 {
   size_t requested_size;
   bool is_free;
   int cookies;
   MallocMetaData* next;
   MallocMetaData* prev;
  };

  MallocMetaData* free_blocks_array[MAX_ORDER+1];
  MallocMetaData* maped_list =nullptr;
  



static void addBFree(void* ptr){
    if(ptr==nullptr)return;
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

     }


}




static void removeblock(void* ptr){

   if(ptr==nullptr)return;
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
   temp->next=nullptr;
   temp->prev=nullptr;
   
}



static void split(size_t size,void* ptr){
    int find_order= findOrder(size);
    MallocMetaData* current=(MallocMetaData*)ptr;
    size_t temp_size= current->requested_size+ sizeof(MallocMetaData);
    int temp= findOrder(current->requested_size+sizeof(MallocMetaData));

    removeblock(current);////////////////
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

    }
    
}




void* smalloc(size_t size){

    if(size > MAX_SIZE || size ==0 )// check if have to size==0
    {
        return NULL;
    }

    if(!is_initialized)
    {
        allocated_block += 32;
        allocated_bytes += 0;
        metaDataBlocks=free_block=allocated_block;
        allocated_bytes=free_bytes=(BLOCK_SIZE-sizeof(MallocMetaData))*INITIAL_BLOCK;       
        is_initialized = true;

        

        size_t to_allocate = (BLOCK_SIZE * INITIAL_BLOCK);
        void* temp= sbrk(to_allocate) 
        if((int)temp==-1) return NULL;
        unsigned long head= (unsigned long) temp;
        char* current =(char*)head;

        for(int i =0; i< INITIAL_BLOCK)
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