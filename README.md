# CS 149 Assignment 6

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

Expected PID.err output:

```text
Warning - file test/names1.txt line 3 is empty.
```

Expected PID.out output:

```text
Jenn Xu: 2
Tom Wu: 4
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

Expected PID.err output:

```text
Warning - file test/names.txt line 2 is empty.
Warning - file test/names.txt line 5 is empty.
```

Expected PID.out output:

```text
Nicky: 1
Dave Joe: 2
Yuan Cheng Chang: 3
John Smith: 1
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

Expected PID.err output:

```text
Warning - file test/namesB.txt line 2 is empty.
Warning - file test/namesB.txt line 5 is empty.
Warning - file test/testCase2.txt line 2 is empty.
Warning - file test/testCase2.txt line 6 is empty.
Warning - file test/testCase2.txt line 10 is empty.
Warning - file test/testCase2.txt line 14 is empty.
```

Expected PID.out output:

```text
Nicky: 1
Jose Mendez: 1
Dave Joe: 2
Yuan Cheng Chang: 3
John Smith: 1
Ana Nguyen: 2
Charlie Kirk: 1
Barrack Obama: 1
Tommy Obama: 1
 : 1
 Tim Tran: 1
Andrew White: 2
ANDREW WHITE: 1
Nguyen Ana: 1
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

Expected PID.err output:

```text
Warning - file test/testCase1.txt line 4 is empty.
Warning - file test/testCase1.txt line 6 is empty.
Warning - file test/testCase3.txt line 6 is empty.
Warning - file test/testCase3.txt line 10 is empty.
Warning - file test/testCase3.txt line 13 is empty.
Warning - file test/testCase3.txt line 16 is empty.
```

Expected PID.out output:

```text
Mike Lam: 1
Jimmy Le: 1
J i m m y L e: 1
ABRAHAM LINCOLN: 1
ABRAHAM RAMIREZ: 1
 ABRAHAM LINCOLN: 1
ADRIAN ADRIAN: 1
ADRIAN: 1
M ike Lam: 1
JeSsE: 1
JESSE: 1
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

Expected PID.err output:

```text
Warning - file test/names_long_redundant1.txt line 2 is empty.
Warning - file test/names_long_redundant1.txt line 4 is empty.
Warning - file test/names_long_redundant1.txt line 6 is empty.
Warning - file test/names_long_redundant1.txt line 8 is empty.
```

Expected PID.out output:

```text
MARY SMITH: 3
STEPHANIE MITCHELL: 1
CAROLYN PEREZ: 1
CHRISTINE ROBERTS: 1
MARIE TURNER: 1
JANET PHILLIPS: 1
CATHERINE CAMPBELL: 1
FRANCES PARKER: 1
ANN EVANS: 1
JOYCE EDWARDS: 1
DIANE COLLINS: 1
ALICE STEWART: 1
JULIE SANCHEZ: 1
HEATHER MORRIS: 1
TERESA ROGERS: 1
DORIS REED: 1
GLORIA COOK: 1
EVELYN MORGAN: 1
JEAN BELL: 1
CHERYL MURPHY: 1
MILDRED BAILEY: 1
KATHERINE RIVERA: 1
JOAN COOPER: 1
ASHLEY RICHARDSON: 1
JUDITH COX: 1
ROSE HOWARD: 1
JANICE WARD: 1
KELLY TORRES: 1
NICOLE PETERSON: 1
JUDY GRAY: 1
CHRISTINA RAMIREZ: 1
KATHY JAMES: 1
THERESA WATSON: 1
BEVERLY BROOKS: 1
DENISE KELLY: 1
TAMMY SANDERS: 1
IRENE PRICE: 1
JANE BENNETT: 1
LORI WOOD: 1
RACHEL BARNES: 1
MARILYN ROSS: 1
ANDREA HENDERSON: 1
KATHRYN COLEMAN: 1
LOUISE B JENKINS: 1
SARA A PERRY: 1
PATRICIA JOHNSON: 3
LINDA WILLIAMS: 3
BARBARA JONES: 3
ELIZABETH BROWN: 3
JENNIFER DAVIS: 2
MARIA MILLER: 2
SUSAN WILSON: 2
MARGARET MOORE: 2
DOROTHY TAYLOR: 2
LISA ANDERSON: 2
NANCY THOMAS: 2
KAREN JACKSON: 2
BETTY WHITE: 2
HELEN HARRIS: 2
SANDRA MARTIN: 2
DONNA THOMPSON: 2
CAROL GARCIA: 2
RUTH MARTINEZ: 2
SHARON ROBINSON: 2
MICHELLE CLARK: 2
LAURA RODRIGUEZ: 2
SARAH LEWIS: 2
KIMBERLY LEE: 2
DEBORAH WALKER: 2
JESSICA HALL: 1
SHIRLEY ALLEN: 1
CYNTHIA YOUNG: 1
ANGELA HERNANDEZ: 1
MELISSA KING: 1
BRENDA WRIGHT: 1
AMY LOPEZ: 1
ANNA HILL: 1
REBECCA SCOTT: 1
VIRGINIA GREEN: 1
KATHLEEN ADAMS: 1
PAMELA BAKER: 1
MARTHA GONZALEZ: 1
DEBRA NELSON: 1
AMANDA CARTER: 1
ANNE J POWELL: 1
JACQUELINE K LONG: 1
WANDA M PATTERSON: 1
BONNIE HUGHES: 1
JULIA FLORES: 1
RUBY WASHINGTON: 1
LOIS BUTLER: 1
TINA SIMMONS: 1
PHYLLIS FOSTER: 1
NORMA GONZALES: 1
PAULA BRYANT: 1
DIANA ALEXANDER: 1
ANNIE RUSSELL: 1
LILLIAN GRIFFIN: 1
EMILY DIAZ: 1
ROBIN HAYES: 1
```

### Test 6

Command:

```bash
./countnames test/names99.txt test/names1.txt test/names2.txt
```

What it tests:
- Error handling for a missing file
- Remaining valid files continue to run

Expected PID.err output:

```text
error: cannot open file test/names99.txt
Warning - file test/names1.txt line 3 is empty.
```

Expected PID.out output:

```text
Jenn Xu: 2
Tom Wu: 4
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

Thanks to the course instructor for the assignment guidance, classmates for discussing edge cases, ZyBooks for the testing environment, and online references for C.