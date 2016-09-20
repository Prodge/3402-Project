## CITS3402 Project 1


### Functions
```
create_all_blocks(column, row_ids, size=4):
  Given an arbitrary length list of 'row_ids' in 'column'
  return every possible grouping (any order) of these elements into groupts of length 'size'
  return this as a list of structs -> thisstruct is defined below

find_all_neighbourhood_groups(column):
  returns a list of lists
  each list is a group of elements that are all within the same neighbourhood in 'column'
  return the element row_ids instead of the element value, these will be passed to create_all_blocks
  Will need sub-functions

unique_blocks()

build a struct for holding a block
  signature : int
  row_ids: list of ints // length(4)
  column: length

is_collision(block1, block2):
  return true if the blocks collide // signature is the same
  // Collision occurs when they are in DIFFERENT columns
  // this maybe needs to return the column ids as a list




```
