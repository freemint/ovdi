
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>


static void
dump_C_array(FILE *f, const unsigned char *buf, size_t buflen)
{
	size_t i, j;
	
	if (buflen & 0x1)
	{
		fprintf(f, "const char data[] =\n");
		fprintf(f, "{\n");
	
		for (i = 0, j = 0; i < buflen; i++)
		{
			int c = buf[i];
		
			fprintf(f, "0x%02x, ", c);
		
			if (++j == 10)
			{
				fprintf(f, "\n");
				j = 0;
			}
		}
	}
	else
	{
		fprintf(f, "const short data[] =\n");
		fprintf(f, "{\n");

		for (i = 0, j = 0; i < buflen; i += 2)
		{
			unsigned short c;

			c = buf[i] << 8;
			c |= buf[i + 1];

			fprintf(f, "0x%04x, ", c);

			if (++j == 8)
			{
				fprintf(f, "\n");
				j = 0;
			}
		}
	}
	
	if (j != 0)
		fprintf(f, "\n");
	
	fprintf(f, "};\n");
}

int
main(int argc, char *argv[])
{
	struct stat st;
	const char *file;
	FILE *f;
	unsigned char *buf;
	int i;
	size_t buflen;
	
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <inputfile>\n", argv[0]);
		exit(1);
	}
	
	file = argv[1];
	i = stat(file, &st);
	if (i != 0)
	{
		perror("stat");
		exit(1);
	}
	
	if (!S_ISREG(st.st_mode))
	{
		fprintf(stderr, "\"%s\" is not a regular file!\n", file);
		exit(1);
	}
	
	buflen = st.st_size;
	buf = malloc(buflen);
	if (!buf)
	{
		fprintf(stderr, "out of memory!\n");
		exit(1);
	}
	
	f = fopen(file, "r");
	if (!f)
	{
		perror("fopen");
		exit(1);
	}
	i = fread(buf, 1, buflen, f);
	if (i != buflen)
	{
		fprintf(stderr, "error reading file!\n");
		exit(1);
	}
	fclose(f);
	
	dump_C_array(stdout, buf, buflen);
	
	free(buf);
	return 0;
}
