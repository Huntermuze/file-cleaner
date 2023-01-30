# Operating System Principles - File Cleaning

### File cleaning approached in various different ways, in order to attain the best performance.

## Running a Task

If you are running a task on the **csit teaching servers**, then you **must** do the following before attempting to run
any task:
> **scl enable devtoolset-11 bash**

To run each task, simply navigate to the sub-folder of the task you wish to test and run the following commands:

> 1. make clean
>2. make
>3. ./task{number} {dirty_file} {clean_file} {graceful_exit_threshold}, where:
> - **number** is the task number of the folder you are in,
> - **dirty_file** is the path to the unfiltered (i.e., dirty/unclean) file,
> - **clean_file** is the output path and,
> - **graceful_exit_threshold** is the maximum execution time and is **OPTIONAL**. See heading: **Graceful Exit
    Configuration**.

For example, if we wanted to run **task 2**, we would do the following:

> 1. cd /task_2
>2. make clean
>3. make
>4. ./task2 ../data/dirty.txt ../data/task2_sorted_list.txt

## Task Permissions

> Task 1. *Normal* privileges (includes *both* the bash and cpp programs).
>
> Task 2. *Normal* privileges.
>
> Task 3. **Superuser** privileges (i.e., must run with sudo).
>
> Task 4. **Superuser** privileges (i.e., must run with sudo).
>
> Task 5. **Superuser** privileges (i.e., must run with sudo).

### Permissions Examples

To run Task 2:
> ./task2 ../data/dirty.txt ../data/task2_sorted_list.txt

To run Task 4:
> sudo ./task4 ../data/dirty.txt ../data/task4_sorted_list.txt

To run Task 5:
> sudo ./task5 ../data/dirty.txt ../data/task5_sorted_list.txt

## Graceful Exit Configuration

To configure the graceful exit threshold (i.e., the amount of seconds that will have elapsed before the process is
forcibly terminated), you simply pass in an additional commandline argument to each task when you run it. This argument
is numerical and is treated in seconds.

For example, if we wanted to run **task 1** (cpp version) with a graceful exit threshold of **30 seconds**, then we can
do
the following:

> 1. ./task1 ../data/dirty.txt ../data/task1_cpp_sorted_list.txt **30**

**NOTE:** If no command-line argument is present for the graceful exit parameter, then the default threshold will be
assumed, which has been defined (via a macro) to be _**15 seconds**_. Feel free to change the default if you wish. So,
if we call
> sudo ./task4 ../data/dirty.txt ../data/task4_sorted_list.txt

with no argument passed for the threshold, then a _**15-second**_ graceful exit period will be
assumed. 
