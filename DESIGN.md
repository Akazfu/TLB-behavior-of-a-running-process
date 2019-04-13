## Design and Known Issues
I managed to made up a file named slide2_test_data.txt which I used as a sample of valgrind outputs to test the algorithm in slide2.  The test data set has the parameters of (pagesize = 65536, tlbsize = 3), and it's results can be tested by calling ./test.sh

A known issue is the size of valgrind output can reach up to 40 bits, it exceeded the max storage of unsigned int(32 bit).  To address this issue, I used unsigned long(64 bit) to store the pgnum.

The miscounts have different interpretations for diffrent policies.
      [1] For FIFO, miscounts indicates how many times a node stores in the linked list
      [2] For LRU, if the pgnum did not hit, than for every miscounts stored miscounts +1;
          elseif it hits, reset the correspoding miscounts to 0.
      The algorithm of LRU will find the node with maxinum miscounts, 
      and evict it right after.

In case an address/pagesize != (address + size)/pgsize, it mean its overfeed the page. Therefore, we need to cut it to two sections when processing.

### how you addressed the implementation requirements in a space and time efficient way
For space efficiency, I used the linked list data structure to store the tlb nodes instead of array. Since the linked list is generally dynamic, I can mapped the tlbsize corresponsively to the length of linked list.  Thus make it space efficient.

For time efficiency, In order to increase the efficiency of FIFO search, I have setup a pointer, and make it indicates the very first tlb node after first initialization.  After each iteration, the pointer will then points toward the next node.  In case the pointer already points at the very last node, it will return to points the first node again.  Therefore, the pointer will always point to the node that have highest priority to the queue.

Another trick I used is, instead of free the nodes and malloc again, I actually replace the pgnum, since that is all we wanted.

### what kinds of tools (and how) you used them to generate the performance result plots.
The output of this program has three lines as mentioned in PDF.  For simplicity, I usded gnuplot with python3 script to create the result plots.  The script is also included in the folder, I executed the source code 162 times with multi-threads, and get the results of miss rates.  Then I separated the datasets accordingly and draw 9 graphs, each graph has 18 parameters. Since the parameters are too many, I referenced a histograms model from the website, and inserted 18 parameters with different bars and colors.

###Reference
https://github.com/gnuplot/gnuplot-old/blob/master/demo/histograms.dem  
http://gnuplot.sourceforge.net/demo/histograms2.html