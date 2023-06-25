# Operating-System-IZ3
 
# Phase - 1

The first phase of the Multiprogramming Operating System (MOS) project implemented code execution, single programming concept, service request handling, program loading, and user program execution. It focused on executing assembly programs one at a time, handling specific operations through service requests, and loading and executing user programs using a predefined starting point. The phase emphasized error-free job entry and operated in a non-multiprogramming environment.

# Notable Features : 

## Operating System Code Execution: 
-> Developed an operating system code capable of executing assembly programs.
-> Implemented the necessary infrastructure for loading, executing, and managing user programs.

## Single Programming Concept:
-> Emphasized understanding the concept of single programming within the operating system.
-> Enabled the system to load and execute one program at a time.

## Service Requests (SI Interrupts):
-> Implemented service requests using SI interrupts to handle specific operations or requests.
-> SI = 1 for Read operation: Reads the next data card from the input file and stores it in specified memory locations.
-> SI = 2 for Write operation: Writes a block of memory (10 words) to the output file.
-> SI = 3 for Terminate operation: Writes two blank lines in the output file to indicate program termination.

## Program Loading (MOS/LOAD):
-> Introduced the program loading phase, which prepares the system for executing user programs.
-> Implemented the loading algorithm to read program or control cards from the input file and store them in memory.

## Execution of User Programs (SLAVE MODE):
-> Implemented the execution phase of user programs in a loop.
-> Fetches instructions from memory using the Instruction Counter Register (IC).
-> Performs various operations based on the instruction type, such as Load Register (LR), Store Register (SR), Compare Register (CR), Branch on Condition (BT), Get Data (GD), Print Data (PD), and Halt (H).

## Assumptions: 
-> The project assumes error-free job entry, no physical separation between jobs, and job outputs separated by two blank lines.

# Phase - 2

The second phase of the Multiprogramming Operating System (MOS) project focuses on several key aspects, including memory management techniques, paging mechanism, error handling, job entry and separation, and the introduction of time and line limits :

## Memory Management Techniques: 
-> This phase provides a comprehensive understanding of different memory management techniques used in operating systems. Memory management is crucial for efficient utilization of main memory resources and includes processes such as memory allocation, deallocation, and relocation.

## Paging Mechanism:
-> Paging is introduced as a mechanism to manage main memory effectively. It involves dividing memory into fixed-size blocks called pages and storing a page table in real memory. The page table tracks the allocation of program pages, enabling efficient access and management of memory.

## Error Handling 
-> The project incorporates the handling of program errors using Program Interrupts (PI). Program errors can include operation errors, operand errors, and page faults. The introduction of PI allows the system to detect and respond to these errors appropriately.

## Job Entry and Separation:
-> Similar to the first phase, jobs are entered without errors through an input file, and there is no physical separation between them. Job outputs are separated in the output file using two blank lines, ensuring clarity and organization.

## Time and Line Limits:
-> To simulate real-world constraints, time and line limits are introduced. A Time Limit Interrupt (TI) is triggered when the execution time of a program exceeds the specified limit. A Line Limit Counter (LLC) keeps track of the number of lines processed, and if it surpasses the set line limit, the system terminates the program execution.
