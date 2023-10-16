<h1>Server Request Handler</h1>

**Description:**

<p>
The Server Request Handler is a program designed to process file requests on a server using different architectural approaches. File names are piped into the program, where they are treated as individual requests. These requests are then processed by either a single process or multiple threads, with statistics being counted. The program returns comprehensive file statistics upon completion. It supports both a serial architecture (single process, no threads) and a multi-threaded architecture (multiple threads within a single process). </p>

**Features:**

* Handles requests for file information (type and size).
* Supports two architectures: serial and multi-threaded.
* Utilizes the Linux system call stat() to retrieve file details.
* Identifies text files by examining their content.

<h2>Usage:</h2>

# Serial Architecture:
<p>
In the serial architecture version, the program reads file names from standard input, processes them sequentially, and provides the required file statistics. To run the program in serial mode: </p>


<h6>$ ./SRH</h6>
<p>
Enter file names one per line. Press ***Ctrl + D*** to signal the end of input.
</p>

**or**
<p>
Piper in the ***ls*** command into the SRH, by:
</p>

<h6>$ ls -1d* | ./SRH</h6>

## Multi-Threaded Architecture:

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

## Testing with Input Files:
<p>
Alternatively, you can list the file names in a text file and redirect it as stdin to the program for automated testing:
</p>
<h6>$ ./proj4 < input_file.txt </h6> 
               
### Output:
<p>
The program will provide statistics for each file, including the number of bad files, directories, regular files, special files, total bytes used, and more.
</p>

<h3>Compilation:</h3>

To compile the program with the multi-threaded architecture, use the **makefile** or use the following command:


<h6> $ g++ -o proj4 proj4.cpp -lpthread </h6>
<p>
Ensure that the pthread library is available.
</p>
