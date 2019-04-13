# CMPUT379 Assignment 3
## TOPIC: TLB behavior of a running process
## zfu@ualberta.ca
## Group47, I submitted and completed this assignment alone due to the missing partner issue.

### Knowing issue:
 
### Notes:
- I noticed that the address of output of valgrind could be 10*4=40bit, therefore the unsigned int (32bit) is not large enough,so I used unsigned long(64bit) to store it. 

- To Debug: uncomment the "#define DEBUG" in "tlb.h", than call "make clean & make" you will get the more detailed output.

- There are a set of test data "slide2_test_data.txt" (made up) , you can test by calling 
./test.sh ```cat slide2_test_data.txt | ./valtlb379 -p FIFO/LRU 65536 3``` to check the results, whether is same with results of slide2.
