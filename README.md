## ft_malloc
### Implementing a simplistic malloc, for fun & for school

This malloc implementation is pretty simple, but I had my share of fun toying around with it. Made me realize that chunk allocators sound nice, too - the day I need some really high performance, I might just make one.

This project supports multithreading, defragmentation, debugging functions (`show_alloc_mem_ex`), debugging builds, a few environment variables, and simple memory corruption detection.

If you're a 42 student and want to play around with it, you'll have to use your own libft. 
