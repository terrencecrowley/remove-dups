# remove-dups
Compare various algorithms for removing duplicates from an array of integers.

This tests a variety of different approaches in order to compare timing and memory usage.
All the approaches use a consistent algorithm of walking over the array with two pointers,
copying new unique values to the front of the array as they proceed.
The difference between the approaches is how they keep track of which values have already been seen.
The available algorithms are:
- Baseline. Just touches all the data to provide a baseline performance for linear.
- NSquare. An algorithm that simply iterates the current unique elements at the front of the array. This is O(N^2) - or really O(n * u) where u is the number of unique elements in the array.
- Trie. Uses a simple trie data structure to store seen elements. This is efficient for elements in a restricted range.
- Trie16. A variant of the trie data structure that branches on nibbles rather than bytes (but seems to be just worse).
- Sort. This sorts the array and then compacts out duplicate elements.
- Set. This uses std::unordered_set to track seen elements.

# Command line options
Options are:

1. [-n count] Indicates how many of the fixed number (0 through 10) of array sizes to use. The default is 10. The values range from 2^4 to 2^22 by factors of 4.

2. [-u count] Indicates how many of the fixed number (0 through 10) of unique values to use. Same ranges as size.

3. [-l] Only generate a limited range of values (0 to u-1). Otherwise will run tests with both a limited
range and with random 32bit integers.

4. [-t (all | baseline | nsquare | sort | trie | trie16 | set)] Specify which tests to run. This option may be specified multiple times to include multiple tests. Default is all.

5. [-s] Perform a Knuth shuffle on the array before removing duplicates. Otherwise the approach to generating the initial array is to generate all the unique values at the front of the array and then replicate as necessary to fill the rest of the array.

# Output
The output is in comma-separated-value format describing the options for each run and the number of microseconds it took.
