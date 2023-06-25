# Operating-System-IZ3

**The MULTIPROGRAMMING OPERATING SYSTEM (MOS)** project consists of three phases aimed at developing an operating system capable of managing multiple processes and executing user assembly programs. Each phase builds upon the previous one, adding new features and improvements. This introduction provides an overview of the work done in all three phases and highlights the enhancements made in the subsequent phases.

### Phase 1:
The first phase of the MOS project focused on the basic functionalities of process management and memory management. It involved developing mechanisms for process creation, termination, and scheduling. The implementation included a simple memory management scheme to allocate memory blocks to processes. The goal was to establish the foundation for subsequent phases by laying out the fundamental concepts of process management and memory allocation.

### Phase 2:
In the second phase, the MOS project expanded its capabilities by introducing more advanced memory management techniques. The focus shifted towards implementing a paging system to efficiently manage memory usage. Paging allowed the system to divide programs into fixed-sized pages and allocate them to available memory frames. This approach provided better utilization of memory resources and facilitated the execution of larger programs.

### Phase 3:
The third phase marked a significant milestone in the MOS project, where the emphasis shifted towards process scheduling and I/O management. The objective was to develop a robust process scheduling mechanism to allocate CPU time slices to multiple processes. The phase also introduced the concept of virtual memory, enabling the system to handle larger programs by utilizing secondary storage devices like the Drum. I/O processing was enhanced through the introduction of spooling, buffering, and multiple I/O channels.

![image](https://github.com/AdityaPatil-AP/Operating-System-Simulator-IZ3/assets/94468283/bb3d8216-0abe-4ff3-9302-c26ca11145d2)

# Phase - 1

The first phase of the Multiprogramming Operating System (MOS) project implemented code execution, single programming concept, service request handling, program loading, and user program execution. It focused on executing assembly programs one at a time, handling specific operations through service requests, and loading and executing user programs using a predefined starting point. The phase emphasized error-free job entry and operated in a non-multiprogramming environment.

## Notable Features : 

### Operating System Code Execution : 
-> Developed an operating system code capable of executing assembly programs.  
-> Implemented the necessary infrastructure for loading, executing, and managing user programs.  

### Single Programming Concept : 
-> Emphasized understanding the concept of single programming within the operating system.   
-> Enabled the system to load and execute one program at a time.   

### Service Requests (SI Interrupts) : 
-> Implemented service requests using SI interrupts to handle specific operations or requests.  
-> SI = 1 for Read operation: Reads the next data card from the input file and stores it in specified memory locations.    
-> SI = 2 for Write operation: Writes a block of memory (10 words) to the output file.       
-> SI = 3 for Terminate operation: Writes two blank lines in the output file to indicate program termination.     

### Program Loading (MOS/LOAD) : 
-> Introduced the program loading phase, which prepares the system for executing user programs.   
-> Implemented the loading algorithm to read program or control cards from the input file and store them in memory.     

### Execution of User Programs (SLAVE MODE) : 
-> Implemented the execution phase of user programs in a loop.   
-> Fetches instructions from memory using the Instruction Counter Register (IC).    
-> Performs various operations based on the instruction type, such as Load Register (LR), Store Register (SR), Compare Register (CR), Branch on Condition (BT), Get Data (GD), Print Data (PD), and Halt (H).    

### Assumptions : 
-> The project assumes error-free job entry, no physical separation between jobs, and job outputs separated by two blank lines.    

# Phase - 2

The second phase of the Multiprogramming Operating System (MOS) project focuses on several key aspects, including memory management techniques, paging mechanism, error handling, job entry and separation, and the introduction of time and line limits :

### Memory Management Techniques : 
-> This phase provides a comprehensive understanding of different memory management techniques used in operating systems. Memory management is crucial for efficient utilization of main memory resources and includes processes such as memory allocation, deallocation, and relocation.     

### Paging Mechanism : 
-> Paging is introduced as a mechanism to manage main memory effectively. It involves dividing memory into fixed-size blocks called pages and storing a page table in real memory. The page table tracks the allocation of program pages, enabling efficient access and management of memory.    

### Error Handling : 
-> The project incorporates the handling of program errors using Program Interrupts (PI). Program errors can include operation errors, operand errors, and page faults. The introduction of PI allows the system to detect and respond to these errors appropriately.    

### Job Entry and Separation : 
-> Similar to the first phase, jobs are entered without errors through an input file, and there is no physical separation between them. Job outputs are separated in the output file using two blank lines, ensuring clarity and organization.     

### Time and Line Limits : 
-> To simulate real-world constraints, time and line limits are introduced. A Time Limit Interrupt (TI) is triggered when the execution time of a program exceeds the specified limit. A Line Limit Counter (LLC) keeps track of the number of lines processed, and if it surpasses the set line limit, the system terminates the program execution.    

# Phase - 3

## Notable Features : 

### Scheduling Processes : 
The primary objective of the third phase is to understand and implement process scheduling in the operating system. It involves managing the execution order of multiple processes and allocating CPU time slices to them. The specific CPU scheduling algorithm chosen for implementation can be customized according to the project requirements.    

### Spooling and Buffering : 
The phase introduces spooling (input and output) mechanisms for efficient I/O processing. Before program execution, input cards (program and data) are transferred from the card reader to the Drum using channels 1 and 3. During program termination, output lines stored on Drum tracks are sent to the printer using channels 3 and 2. Buffers are used to store and transfer data between different components of the system, including the channels, Drum, and memory.    

### Paging and Virtual Memory : 
Paging, a memory management technique, is retained from the previous phase. It allows for efficient allocation of program pages to memory blocks. The page table, which contains the mapping between logical and physical addresses, is stored in real memory. The introduction of virtual memory enables the system to handle larger programs by using secondary storage, such as the Drum, as an extension of the main memory.    

### Interrupts and Error Handling : 
The third phase includes several types of interrupts to handle various events. The Time Slice (TI) interrupt occurs when a process exceeds its allocated time slice. I/O interrupts (IOI) are generated by channels 1, 2, and 3 upon completion of their respective tasks. The interrupt routines (IR) associated with each channel perform specific actions based on the interrupt values.    

### Algorithm Execution : 
The execution of user assembly programs is performed in the Slave Mode of operation. The program instructions are fetched and executed iteratively until completion. The execution process includes address mapping, where virtual addresses are translated into physical addresses using the Page Table. The Slave Mode simulates the behavior of a processor executing the user program.     

### Simulation and Time Management : 
The MOS project implements a simulation of an operating system environment. It tracks and manages time-related aspects such as Time Slice Counters (TSC) and Time Slice limits (TS). The simulation increments the TSC and checks for timeouts and time slice exhaustion, which can trigger interrupts and context switches between processes.    
   
