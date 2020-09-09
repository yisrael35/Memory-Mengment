#ifndef sm1
#define sm1
using namespace std;
#include <queue> 
/*yisrael bar 05/06/20  */

#define MEMORY_SIZE 100
extern char main_memory[MEMORY_SIZE];

typedef struct page_descriptor{
int V; //vaild
int D; //dirty
int P; //permission
int frame; //the number of a frame if in case it is page-mapped
}page_descriptor;

class sim_mem {     
    int swapfile_fd;                                //swap file fd     
    int program_fd;                                 //executable file fd     
    int text_size;     
    int data_size;     
    int bss_size;     
    int heap_stack_size;     
    int num_of_pages;     
    int page_size;
    page_descriptor *page_table;       //pointer to page table 
    
    int num_of_frames;
    int *frame_array; //allocate an array for frame- avilable? 0/1
    queue <int> q1;

    public:     
    //1.     
    sim_mem(char exe_file_name[], char swap_file_name[], int text_size,int data_size, int bss_size, int heap_stack_size,int num_of_pages, int page_size);         
    //2.
    char load(int address);         
    //3. 
    void store(int address, char value); 
    //4.
    void print_memory();         
    //5.
    void print_swap ();         
    //6.
    void print_page_table(); 
    //7.
     ~sim_mem(); 

    private:
    //help mathod
    int convert_logic_to_real(int num);
    void alloc_frame(int address);
    char p_equal_zero(int address);
    char d_equal_one(int address);
    void assigned_zeros(int address);

};

#endif