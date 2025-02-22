# C-AutoHeaders
C-AutoHeaders is a program made for C developers, with the purpose to help with the annoying header (.h) files!

## How does it work?
Every C project has a main file, which may import many functions from other C files, however, any minor changes to your code may require headers to be updated, this is what AutoHeaders desires to solve!

### the AutoHeader specifications
AutoHeader takes file names as inputs, but before those, it analyzes if there are any specification flags to the header generation.

#### confirm flag
using "confirm" as an argument before your files will make AutoHeaders ask for confirmation before including a found import to the automatic header generation, that confirmation prompt has a timeout of 20 seconds.

#### no-add flag
using "no-add" as an argument before your files will make AutoHeaders IGNORE automatic inclusions, and will change the default prompt response to make it not include files after a timeout.

#### auto-add flag
using "no-add" as an argument before your files will make AutoHeaders include files automatically, and will set the default prompt response to make it include the file. (this is the default behavior).
