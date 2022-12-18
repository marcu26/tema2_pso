/*
 * Loader Implementation
 *
 * 2022, Operating Systems
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "exec_parser.h"

#define page_size getpagesize()

static so_exec_t *exec;

static int descriptor;

so_seg_t *FindSegment(char *addres)
{
	for (int i = 0; i < exec->segments_no; i++)
	{
		if ((int)addres >= exec->segments[i].vaddr && (int)addres <= (exec->segments[i].vaddr + exec->segments[i].mem_size))
			return &(exec->segments[i]);
	}

	return NULL;
}

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	/* TODO - actual loader implementation */
	if (signum != SIGSEGV)
	{
		exit(-1);
	}

	if (info->si_code == SEGV_ACCERR)
	{
		signal(SIGSEGV, SIG_DFL);
	}

	so_seg_t *segment = FindSegment((char *)info->si_addr);

	if (segment == NULL)
	{
		signal(SIGSEGV, SIG_DFL);
	}

	int pageNumber = ((int)(info->si_addr) - segment->vaddr) / page_size;

	int pageStartAddress = pageNumber * page_size + segment->vaddr;
	int pageAddressInSegment = pageNumber * page_size;
	int nextPageAddressInSegment = pageAddressInSegment + page_size;

	if (pageAddressInSegment < segment->file_size)
	{
		void *address = mmap((void *)pageStartAddress, page_size, segment->perm, MAP_FIXED | MAP_PRIVATE, descriptor, segment->offset + pageAddressInSegment);

		if (address == MAP_FAILED)
		{
			exit(-1);
		}

		if (nextPageAddressInSegment >= segment->file_size)
		{
			if (segment->mem_size != segment->file_size)
			{
				int fileEnd = segment->vaddr + segment->file_size;
				memset((void *)fileEnd, 0, nextPageAddressInSegment - segment->file_size);
			}
		}
	}
	else
	{
		void *address = mmap((void *)pageStartAddress, page_size, segment->perm, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (address == MAP_FAILED)
		{
			return;
		}
	}
}

int so_init_loader(void)
{
	int rc;
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = segv_handler;
	sa.sa_flags = SA_SIGINFO;
	rc = sigaction(SIGSEGV, &sa, NULL);
	if (rc < 0)
	{
		perror("sigaction");
		return -1;
	}
	return 0;
}

int so_execute(char *path, char *argv[])
{
	descriptor = open(path, O_RDONLY);

	if (descriptor < 0)
		return -1;

	exec = so_parse_exec(path);
	if (!exec)
		return -1;

	so_start_exec(exec, argv);

	return -1;
}