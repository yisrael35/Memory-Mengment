#include "sim_mem.h"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <queue>
using namespace std;
#define MEMORY_SIZE 100
char main_memory[MEMORY_SIZE];
/*yisrael bar 02-05/06/20 

*/



//constructor//1. ctor 
sim_mem::sim_mem(char exe_file_name[], char swap_file_name[], int text_size,  int data_size, int bss_size, int heap_stack_size, int num_of_pages, int page_size) {
    //init
    this->text_size = text_size;
    this->data_size = data_size;
    this->bss_size = bss_size;
    this->heap_stack_size = heap_stack_size;
    this->num_of_pages = num_of_pages;
    this->page_size = page_size;
    //check if the names of the file is null
    if(exe_file_name==NULL){
        perror("the neme of exe is null\n");
        exit(1);
    }
    if(swap_file_name==NULL)
    swap_file_name = "swap_file";
    //open exe file
    program_fd = open(exe_file_name, O_RDONLY);
    if(program_fd==-1){
    perror("failed to open the exe file /exe file does not exist\n");
    exit(1);
    }
    
    //open swap file
    swapfile_fd = open(swap_file_name, O_RDWR | O_CREAT| O_APPEND ,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); 
    if(swapfile_fd==-1){
        perror("open swap file failed\n");
        close(program_fd);
        return;
    }
    //init swap file - put zeros
    for (int i = 0; i < page_size*num_of_pages; i++)
    {
        int temp = write(swapfile_fd, "0", 1);
        if(temp == -1){
        perror("failed to write to swap file\n");
        close(program_fd);
        close(swapfile_fd);
        return;
        }
    }

    //fill the main memory with zeros
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        main_memory[i]='0';
    }
    //page table-malloc
    page_table =(page_descriptor*)malloc(num_of_pages*sizeof(page_descriptor));
    if( page_table == NULL){
        close(program_fd);
        close(swapfile_fd);
        exit(1);
    }
    //fill the table
    
    for (int  i = 0; i < num_of_pages; i++)
    {
        if (i < text_size/page_size )
        {
            page_table[i].P = 0;//not a permission to write
        }else 
            page_table[i].P = 1;//permission to write
        
        page_table[i].V = 0;//vaild
        page_table[i].D = 0;//dirty
        page_table[i].frame = -1; //the number of a frame if in case it is page-mappe
    }  
    
    //update the num of frames
    this->num_of_frames = MEMORY_SIZE / page_size;
    this->frame_array = (int*)malloc(num_of_frames * sizeof(int));
    if(frame_array==NULL){
        //in case of failer - clean memory and close files
        free(page_table);
        close(program_fd);
        close(swapfile_fd);
        exit(1);
    }
    for (int i = 0; i < num_of_frames; i++)
    {
        frame_array[i] = 1;
    }  
}
//2. load 
char sim_mem::load( int address) {
    if (address > num_of_pages*page_size || address < 0)
    {
        perror("address number is too big - not exist\n");
        return '\0';
    }
    int page = address / page_size;
    if (page_table[page].V == 1)//page in memory
    {
        return main_memory[convert_logic_to_real(address)]; 
    }else if (page_table[page].V == 0)
    {
        if (page_table[page].P == 0)//there is no permission -  take chars from exe
        {
             return p_equal_zero(address);

        }else if (page_table[page].P == 1)//there is permissin- need to check if dirty or not
        {
            if (page_table[page].D == 1)//is dirty
            {
            
            return d_equal_one(address); 
            
            }else if (page_table[page].D == 0)//not dierty
            {
                //calculate the type of address
                bool data = false, heap_stack = false,  bss = false;
                int data_start = text_size;
                int bss_start = data_size + text_size ;
                int heap_stack_start = bss_start + bss_size;

                if (address <= bss_start && address >= data_start )
                {
                    data =  true;
                }else if (address <= heap_stack_start && address >= bss_start)
                {
                    bss = true;
                }else if (address <= num_of_pages*page_size && address >= heap_stack_start)
                {
                    heap_stack = true;
                }
                
                
                //in case of type data
                if (data)
                {
                    //bring the file from exe
                    return p_equal_zero(address);

                }else if (heap_stack  || bss){
                    if (heap_stack )
                    {   
                        //heap and stack not assiged yet
                    perror("cent do a load to heap and stack- its not assigned yet\n");
                    // this->~sim_mem();
                    return '\0';
                        
                    }else if (bss)
                    {
                        //put zeros and bring to main memory and return the value=0
                    assigned_zeros(address);
                    return main_memory[convert_logic_to_real(address)]; 

                    }
                }   
            }
        }
    }
}

//3. store
void sim_mem::store(int address, char value){
    //address not in limit
    if (address > num_of_pages*page_size || address < 0)
    {
        perror("address number is too big - not exist\n");
        return;
    }
    
    int page = address / page_size;
    if (page_table[page].V == 1)//page in memory
    {
        //put the value
        main_memory[convert_logic_to_real(address)] = value;
        page_table[address].D = 1;
        return; 
    }else if (page_table[page].V == 0)
    {
        if (page_table[page].P == 0)//there is no permission - 
        {
            perror("there is no permission to write in this area\n");
           return;

        }else if (page_table[page].P == 1)//there is permissin- 
        {
            if (page_table[page].D == 1)//is dirty
            {
                //bring page from swap file
                d_equal_one(address);
                main_memory[convert_logic_to_real(address)] = value;
                page_table[address].D = 1;
                return; 
            
            }else if (page_table[page].D == 0)//not dierty
            {
                //calculate the type of address
                bool data = false, heap_stack = false,  bss = false;
                int data_start = text_size;
                int bss_start = data_size + text_size ;
                int heap_stack_start = bss_start + bss_size;

                if (address <= bss_start && address >= data_start )
                {
                    data =  true;
                }else if (address <= heap_stack_start && address >= bss_start)
                {
                    bss = true;
                }else if (address <= num_of_pages*page_size && address >= heap_stack_start)
                {
                    heap_stack = true;
                }
                
                
                //in case of type data
                if (data)
                {
                    //bring page from memory and put value in memory
                    p_equal_zero(address);
                    main_memory[convert_logic_to_real(address)] = value;
                    page_table[address].D = 1;
                    return; 


                }else if (heap_stack  || bss){
                    //put zeros to make it look like a malloc and put value in memory
                    assigned_zeros(address);
                    // p_equal_zero(address);//?
                    main_memory[convert_logic_to_real(address)] = value;
                    page_table[address].D = 1;
                    return; 

                }   
            }
        }
    }

}

//4. print memory
void sim_mem::print_memory() {
    int i;
    printf("\n Physical memory\n");
        for(i = 0; i < MEMORY_SIZE; i++) {
            printf("[%c]\n", main_memory[i]);
        }
}

//5. print swap
void sim_mem::print_swap() {
    char* str =(char*)malloc(this->page_size *sizeof(char));
    int i;
    printf("\n Swap memory\n");
    lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
        while(read(swapfile_fd, str, this->page_size) == this->page_size) {
            for(i = 0; i < page_size; i++) {
                printf("%d - [%c]\t", i, str[i]);
            }
            printf("\n");
        }
    free(str);
}

//6. page table
void sim_mem::print_page_table() {
    int i;
    printf("\n page table \n");
    printf("Valid\t Dirty\t Permission \t Frame\n");
        for(i = 0; i < num_of_pages; i++) {
            printf("[%d]\t[%d]\t[%d]\t[%d]\n",
            page_table[i].V,
            page_table[i].D,
            page_table[i].P,
            page_table[i].frame);
        }
}

//7. destructor 
sim_mem::~sim_mem(){

    close(program_fd);
    close(swapfile_fd);
    free (page_table);
    free(frame_array);
}


//help mathod that convert a logical address to a real one in the memory
int sim_mem::convert_logic_to_real(int num){
    int page = num / page_size;
    int offset = num % page_size;
    int temp = page_table[page].frame;
    int temp1 = temp * page_size + offset;
    return temp1;
}

//when p = 0 we bring the page from file to memory
char sim_mem::p_equal_zero(int address){
    int page = address / page_size;

    char * buf = (char*)malloc(page_size*sizeof(char)); 
    if(buf==NULL){
        //in case of failer clean memory and close files
        this->~sim_mem();
        exit(1);
    }
    //put pointer to the right place in exe
    lseek(program_fd, page*page_size, SEEK_SET);

    //alloc frame
    alloc_frame(address);


    int temp = page_table[page].frame;
    int real_address = temp * page_size;
    int check = read(program_fd, buf, page_size);
    if (check == -1)
    {
        free(buf);
        return '\0';
        // this->~sim_mem();
    }
    //put in main memory page from exe file
    for (int i = 0; i < page_size; i++)
    {
        main_memory[real_address+i] =  buf[i];
    }
        // update v in the table: 
    page_table[address/page_size].V = 1;
    //clean memory
    free(buf);
    return main_memory[convert_logic_to_real(address)];
}
//when d = 1 we bring the page from swap file to memory
char sim_mem::d_equal_one(int address){
    int page = address / page_size;

    char * buf = (char*)malloc (page_size*sizeof(char)); 
    if(buf==NULL){
        //in case of failer clean memory and close files
        this->~sim_mem();
        exit(1);
    }
    //put pointer to the right place in swap
    lseek(swapfile_fd, page*page_size, SEEK_SET);

    //alloc frame
    alloc_frame(address);

    int temp = page_table[page].frame;
    int real_address = temp * page_size;
    //read form swap file a page
    int check = read(swapfile_fd, buf, page_size);
    if (check ==-1)
    {
        perror("the call read filed \n");
        free(buf);
        return '\0';
    }
    
    //enter the data from buf into the main memory
    for (int i = 0; i < page_size; i++)
        {
            main_memory[real_address+i] =  buf[i];
        }
    // update v in the table: 
    page_table[address/page_size].V = 1;            
    free(buf);
    return main_memory[convert_logic_to_real(address)]; 
            
}
//put zeros in a page - its a simulition of malloc
void sim_mem::assigned_zeros(int address){
    int page = address / page_size;
    
    //alloc frame 
    alloc_frame(address);

    int temp = page_table[page].frame;
    int real_address = temp * page_size;
    for (int i = 0; i < page_size; i++)
        {
            main_memory[real_address+i] = '0';
        }
    // update v in the table: 
    page_table[address/page_size].V = 1; 
    page_table[address/page_size].D = 1;            

}


//alloc a frame to pages
void sim_mem::alloc_frame(int address){
    //if its already in memory return
    if (page_table[address/page_size].V==1)
    {
        return;
    }
    
    bool empty_frame = false;
    int temp_frame = -1;
    //search for an empty frame in memory- i saved an array of frames 
    for (int i = 0; i < num_of_frames; i++)
    {
        if (frame_array[i]==1)
        {
            this->q1.push(i);
            frame_array[i]=0;
            temp_frame = i;
            empty_frame = true;
            break;
        }  
    }
    if (empty_frame)//in case that where a frame avilable
    {
        
        page_table[address/page_size].frame = temp_frame;

    }else //in case there not frame avilable
    {
        //we pop out the first from the queue
        temp_frame = this->q1.front();
        this->q1.pop();
        
        //calculate the real address for a frame
        int temp_address = -1;
        for (int i = 0; i < num_of_pages; i++)
        {
            if (page_table[i].frame == temp_frame)
            {
               temp_address = i; 
               break;
            }
            
        }
        
        if(page_table[temp_address].P == 0){//in case there is not a permission to write
            page_table[temp_address].V = 0;
        }
        else if(page_table[temp_address].P == 1)//in case there is a permission to write
        {

            if (page_table[temp_address].D == 1)//in case its been writen
            {

            //write the page into swap file
                lseek(swapfile_fd, temp_address*page_size, SEEK_SET);
                char buf[1];
               for (int i = 0; i < page_size; i++)
                {   
                    buf [0] = main_memory[temp_frame * page_size + i];
                   int check =  write(swapfile_fd , buf, 1);
                   if( check == -1){
                       perror("write to swap file failed\n");
                       return;
                   }
                } 
                page_table[temp_address].V = 0;

            }else
            {   
                //in case its havent been writen just clear the v
                
                page_table[temp_address].V = 0;
            }
        }
        //assiged the frame to the address needed 
        page_table[address/page_size].frame = temp_frame;
        this->q1.push(temp_frame); //put back the frame in queue
    }    
}


