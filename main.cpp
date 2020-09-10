#include "sim_mem.h"
#include <iostream>
using namespace std;
/*yisrael bar 02-05/06/20 

 */




int main(){
  char val; 
  sim_mem mem_sm("exec_file", "swap_file" ,25, 50, 25,25, 25, 5); 



  mem_sm.store( 95,'X');  
  mem_sm.store( 90,'a');  
  mem_sm.store( 85,'b');  
  mem_sm.store( 80,'c');  


  mem_sm.store( 75,'d');  
  mem_sm.store( 70,'e');  
  mem_sm.store( 65,'f');  
  mem_sm.store( 60,'g');  

  mem_sm.store( 55,'h');  
  mem_sm.store( 50,'i');  
  mem_sm.store( 45,'j');  
  mem_sm.store( 40,'k');  

  mem_sm.store( 35,'l');  
  mem_sm.store( 30,'m');  
  mem_sm.store( 125,'n');  
  mem_sm.store( 120,'o');  

  mem_sm.store( 115,'p');  
  mem_sm.store( 110,'q');  
  mem_sm.store( 105,'r');  
  mem_sm.store( 100,'s');  


  mem_sm.store( 95,'X');  
  mem_sm.store( 90,'y');  
  mem_sm.store( 85,'z');  
  mem_sm.store( 80,'1');  

  mem_sm.store( 95,'2');  
  mem_sm.store( 90,'3');  
  mem_sm.store( 85,'4');  
  mem_sm.store( 80,'5');  

  mem_sm.store( 95,'6');  
  mem_sm.store( 90,'7');  
  mem_sm.store( 85,'8');  
  mem_sm.store( 80,'9');  

  mem_sm.store( 95,'t');  
  mem_sm.store( 90,'u');  
  mem_sm.store( 85,'v');  
  mem_sm.store( 80,'w');  

mem_sm.load ( 1); 
mem_sm.load ( 5); 
mem_sm.load ( 10); 
mem_sm.load ( 15); 
mem_sm.load ( 20); 
mem_sm.load ( 25); 

  val = mem_sm.load ( 98); 
  mem_sm.print_memory(); 
//  printf("the val is: %c\n", val);
 mem_sm.print_swap();
  // mem_sm.print_page_table();
  
  
  return 0;
}
