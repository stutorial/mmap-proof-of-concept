#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// Example from https://www.safaribooksonline.com/library/view/linux-system-programming/0596009585/ch04s03.html

int main(int argc, char *argv[]) {
	struct stat sb;
	off_t len;
	char *p;
	int fd;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		return 1;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("open");
		return 1;
	}

	if (fstat(fd, &sb) == -1) { // fstat returns information about the given file
		perror("fstat");
		return 1;
	}

	if (!S_ISREG(sb.st_mode)) { // S_ISREG checks that the file is a regular one. Some unregular ones are unmmap-able
		fprintf(stderr, "%s is not a file\n", argv[1]);
		return 1;
	}

	// void * mmap (void *addr, size_t len, int prot, int flags, int fd, off_t offset);
	p = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	/*
	 * A call to mmap( ) asks the kernel to map len bytes
	 * 		of the object represented by the file descriptor fd,
 	 * 		starting at offset bytes into the file, into memory.
 	 * 		If addr is included, it indicates a preference to use that starting address in memory.
 	 * 		The access permissions are dictated by prot,
 	 * 			and additional behavior can be given by flags
	 */
	if (p == MAP_FAILED) {
		perror("mmap");
		return 1;
	}

	/*
	 * When you map a file descriptor, the file's reference count is incremented.
	 * Therefore, you can close the file descriptor after mapping the file, and your process will still have access to it.
	 */
	if (close(fd) == -1) {
		perror("close");
		return 1;
	}

	for (len = 0; len < sb.st_size; len++)
		putchar(p[len]); // putchar prints to the standard output

	if (munmap(p, sb.st_size) == -1) {
		perror("munmap");
		return 1;
	}

	return 0;
}
