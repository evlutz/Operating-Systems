# Server Request Handler

**Description:**

The Server Request Handler is a program designed to process file requests on a server using different architectural approaches. File names can be inputed by the user or piped into the program, where they are treated as individual requests. These requests are then processed by either a single-threaded or multi-threaded, with statistics being counted. The program returns comprehensive file statistics. It supports both a serial architecture (single process, no threads) and a multi-threaded architecture (multiple threads within a single process) utilizing mutual exclusion, or mutex.

**Features:**

- Handles requests for file information (type and size).
- Supports two architectures: serial and multi-threaded.
- Utilizes the Linux system call `stat()` to retrieve file details.
- Identifies text files by examining their content.

## Example Run:
##### $ ls -1d /dev/* | ./SRH thread 4
<p>
Bad Files: 0<br>
Directories: 17<br>
Regular Files: 1<br>
Special Files: 183<br>
Regular File Bytes: 79671296<br>
Text Files: 0<br>
Text File Bytes: 0<br>

$ </p>
## Compilation:

To compile the program with the multi-threaded architecture, use the **makefile** or use the following command:<br>
**make clean**<br>
**make**<br>

***or***

**$ g++ -o proj4 proj4.cpp -lpthread**

## Usage:

### Serial Architecture:

In the serial architecture version, the program reads file names from standard input, processes them sequentially, and provides the required file statistics. To run the program in serial mode:


<h6>$ ./SRH</h6>

Enter file names one per line. Press ***Ctrl + D*** to signal the end of input.


**or**

Piper in the ***ls*** command into the SRH, by:


<h6>$ ls -1d* | ./SRH</h6>

### Multi-Threaded Architecture:

The multi-threaded architecture allows for concurrent handling of file requests. To use this mode, include the argument thread followed by the maximum number of threads to execute at a time.


###### $ ./SRH thread ***[max_threads]***
<p>
Example, to run with at most 10 threads:</p>

**$ ./SRH thread 10**
<p>
Enter file names as in serial mode. Press Ctrl + D to signal the end of input.
</p>

**or**
<p>
Piper in the <strong>ls -1d*</strong> command into the SRH, by:
</p>

<h6>$ ls -1d* | ./SRH thread 10</h6>
           
## Output:
<p>
The program will provide statistics for each file, including the number of bad files, directories, regular files, special files, total bytes used, and more.
</p>


Ensure that the pthread library is available.


