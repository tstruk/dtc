// SPDX-License-Identifier: LGPL-2.1-or-later
/*
 * libfdt - Flat Device Tree manipulation
 *	Testcase for fdt_next_tag()
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <libfdt.h>
#include "tests.h"
#include "testdata.h"

int main(int argc, char *argv[])
{
	struct fdt_header *hdr;
	struct fdt_property *prp;
	void *fdt;
	int size, nextoff = 0;
	uint32_t tag;

	test_init(argc, argv);
	size = sizeof(*hdr) + sizeof(*prp) + 256;
	fdt = calloc(1, size);
	if (!fdt)
		FAIL("Can't allocate memory");

	hdr = fdt;
	prp = (struct fdt_property *)(((char *) fdt) + sizeof(*hdr));
	fdt_set_magic(fdt, FDT_MAGIC);
	fdt_set_totalsize(fdt, size);
	fdt_set_version(fdt, 0x10);
	prp->tag = cpu_to_fdt32(FDT_PROP);
	prp->len = cpu_to_fdt32(256);
	prp->nameoff = 0;

	tag = fdt_next_tag(fdt, sizeof(*hdr), &nextoff);
	if (tag != FDT_PROP)
		FAIL("Invalid tag %X", tag);

	if (nextoff != size)
		FAIL("Invalid next_offset");

	/* int overflow case */
	prp->len = cpu_to_fdt32(0xFFFFFFFA);
	tag = fdt_next_tag(fdt, sizeof(*hdr), &nextoff);
	if (tag != FDT_END)
		FAIL("Invalid tag, expected premature end");

	if (nextoff != -FDT_ERR_BADSTRUCTURE)
		FAIL("Invalid nextoff, expected error FDT_ERR_BADSTRUCTURE");

	/* negative offset case */
	prp->len = cpu_to_fdt32(0x7FFFFFFA);
	tag = fdt_next_tag(fdt, sizeof(*hdr), &nextoff);
	if (tag != FDT_END)
		FAIL("Invalid tag, expected premature end");

	if (nextoff != -FDT_ERR_BADSTRUCTURE)
		FAIL("Invalid nextoff, expected error FDT_ERR_BADSTRUCTURE");

	free(fdt);
	PASS();
}
