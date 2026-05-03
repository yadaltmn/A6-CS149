# CS 149 Assignment 6 - Threaded Countnames

## Student Names

Jada-Lien Nguyen  
Jesse Mendoza

## Description

This project implements a threaded version of `countnames`. The program creates one pthread for each input file. Every thread reads its assigned file and updates one shared dynamically allocated name-count table. Updates to the shared table are protected with a `pthread_mutex_t` so that lookup, insertion, resizing, and count increments are atomic.

If a file cannot be opened, that thread prints an error message and exits. The other threads continue processing their files.

## Files Submitted

- `shell.c`
- `countnames.c`
- `README.md`
- `output.pdf`
- `test/` directory with test files

## How to Compile

Run these commands in the project directory:

```bash
gcc -o shell shell.c -Wall -Werror
gcc -D_REENTRANT -pthread -o countnames countnames.c -Wall -Werror
```

## How to Run

Run `countnames` directly with one to three input files:

```bash
./countnames test/names1.txt test/names2.txt
```

You can also start the shell:

```bash
./shell
```

Then run commands inside the shell:

```bash
./countnames test/names1.txt test/names2.txt
exit
```

Because each input file is processed by a separate thread, the order in which brand-new names first appear in the output can vary slightly between runs. The counts shown for each name are the expected results.

## Test Cases

### Test 1

Command:

```bash
./countnames test/names1.txt test/names2.txt
```

What it tests:
- Two threads running concurrently
- Duplicate name aggregation across two files

Expected output:

```text
Tom Wu: 4
Jenn Xu: 2
```

Expected stderr:

```text
Warning - file test/names1.txt line 3 is empty.
```

### Test 2

Command:

```bash
./countnames test/names.txt
```

What it tests:
- Single-thread behavior
- Empty line warnings
- Duplicate names within one file
- Names containing spaces

Expected stdout:

```text
Nicky: 1
Dave Joe: 2
Yuan Cheng Chang: 3
John Smith: 1
```

Expected stderr:

```text
Warning - file test/names.txt line 2 is empty.
Warning - file test/names.txt line 5 is empty.
```

### Test 3

Command:

```bash
./countnames test/testCase2.txt test/namesB.txt
```

What it tests:
- Empty lines
- Leading spaces
- A line containing only a space
- Case sensitivity
- Two threads updating the same shared table

Expected output:

```text
Jose Mendez: 1
Ana Nguyen: 2
Charlie Kirk: 1
Barrack Obama: 1
Tommy Obama: 1
 : 1
 Tim Tran: 1
Andrew White: 2
ANDREW WHITE: 1
Nguyen Ana: 1
Nicky: 1
Dave Joe: 2
Yuan Cheng Chang: 3
John Smith: 1
```

The names from `test/namesB.txt` may appear before or after the names from `test/testCase2.txt` depending on thread scheduling.

### Test 4

Command:

```bash
./countnames test/testCase1.txt test/testCase3.txt
```

What it tests:
- Custom student-created test files
- Internal spacing differences
- Leading spaces
- Case sensitivity
- Duplicate-looking names that should remain separate

Expected output:

```text
Mike Lam: 1
Jimmy Le: 1
J i m m y L e: 1
M ike Lam: 1
JeSsE: 1
JESSE: 1
ABRAHAM LINCOLN: 1
ABRAHAM RAMIREZ: 1
 ABRAHAM LINCOLN: 1
ADRIAN ADRIAN: 1
ADRIAN: 1
JADA NGUYEn: 1
GEORGE WASHINGTON: 2
JADA NGUYEN: 1
JORGE PEREZ: 1
JESSE MENDOZA: 1
MENDOZA JESSE: 1
```

### Test 5

Command:

```bash
./countnames test/names_long_redundant1.txt test/names_long_redundant2.txt test/names_long_redundant3.txt
```

What it tests:
- Three concurrent threads
- More than 10 distinct names so the dynamic table grows with `realloc`
- Duplicate names spread across multiple files

Expected output begins with:

```text
MARY SMITH: 3
PATRICIA JOHNSON: 3
LINDA WILLIAMS: 3
BARBARA JONES: 3
ELIZABETH BROWN: 3
```

The full output is shown in `output.pdf`.

### Test 6

Command:

```bash
./countnames test/names1.txt test/does_not_exist.txt test/names2.txt
```

What it tests:
- Error handling for a missing file
- Remaining valid files continue to run

Expected stdout:

```text
Tom Wu: 4
Jenn Xu: 2
```

Expected stderr includes:

```text
error: cannot open file test/does_not_exist.txt
```

## Lessons learned

Name: Jada-Lien Nguyen
Description: I completed the threaded `countnames` implementation by creating one pthread for each input file and protecting the shared dynamic name-count table with a mutex. I also tested the program with single-file, multi-file, missing-file, and resizing cases and documented how to compile, run, and verify the assignment.

Jesse Mendoza - I learned how to replace process-based parallelism with thread-based parallelism. This assignment helped me understand `pthread_create`, `pthread_join`, and how mutex locking prevents corrupted counts when several threads update the same heap table.

## References

- Course materials, lecture slides, and ZyBooks notes for CS 149.
- `pthread_create`, `pthread_join`, and `pthread_mutex_lock` manual pages.
- `fgets`, `malloc`, `realloc`, and `free` manual pages.
- Stack Overflow discussions linked in the assignment prompt about `fgets`, `getline`, and string memory handling.
- AI assistant support for implementation planning and documentation wording.

## Acknowledgements

Thanks to the course instructor for the assignment guidance, classmates for discussing edge cases, ZyBooks for the testing environment, and online references fo