# C-AutoHeaders
C-AutoHeaders is a program made for C developers, with the purpose to help with the annoying header (.h) files!

## How does it work?
Every C project has a main file, which may import many functions from other C files, however, any minor changes to your code may require headers to be updated, this is what AutoHeaders desires to solve!

### the AutoHeader specifications
AutoHeader takes file names as inputs, but before those, it analyzes if there are any specification flags to the header generation.

#### confirm flag family
using one of the "confirm" arguments/flags before your files will make AutoHeaders ask for confirmation before including a found import to the automatic header generation, that confirmation prompt has a timeout of 20 seconds. \
these arguments/flags are: \
"confirm-func", asks for confimation before adding functions to the header file.\
"confirm-def", asks for confirmation before adding definitions (i.e. typedef/#define/struct definitions) to the header file.\
"confirm-file", asks for confirmation before adding a file to the header making list.\
"confirm-all" enables all previous.
#### no-add flag family
using one of the "no-add" arguments/flags before your files will make AutoHeaders IGNORE automatic inclusions, and will change the default prompt response to make it not include files after a timeout.
"no-add-func", changes addition of functions to the header file.\
"no-add-def", changes addition of definitions (i.e. typedef/#define/struct definitions) to the header file.\
"no-add-file", changes additions of files to the header making list.\
"no-read-all" enables all previous.

#### read-head flag
using "read-head" as an argument before your files will make AutoHeaders try to read standalone header files (default behavior).
#### no-read-head flag
using "no-read-head" as an argument before your files will inhibit standalone header file reading.
#### root-dir flag
Using root-dir as an argument will mark the next argument as the 'root', or current working directory of the application, reading files relative to it.
#### read-only flag
Using this flag will make autoHeaders only 'check' for imports, in case it is desired to use automatic the project compile suggestion.
