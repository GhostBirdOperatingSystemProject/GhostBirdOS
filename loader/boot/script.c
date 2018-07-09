/**
 * Copyright 2013-2018 by Explorer Developers.
 * made by Hu Wenjie<1@hwj.me>
 * Explorer Loader Script support
 * /loader/boot/script.c
 * version:1.0.0
 * 7/8/2018 23:06
 */

#include "script.h"
#include "../problem.h"
#include "VI.h"
#include "lib/string.h"
#include "lib/stdio.h"

#define SCRIPT_BUF_SIZE 256

struct script_node script_init(const char *script_start, unsigned long length)
{
	unsigned long actual_length;
	struct script_node node_root;

	// Check if there is \0 in this script segment
	actual_length = strnlen(script_start, length);

	if (actual_length != length)
		warning(WARN_SCRIPT_SIZE_BAD, "Script's length incorrect!");
	
	// Fill the node_root
	node_root.name = "root";
	node_root.name_len = 4;
	node_root.body = script_start;
	node_root.body_len = actual_length;

	return node_root;
}

struct script_node script_child(struct script_node parent, const char *child_name)
{
	char *buffet[SCRIPT_BUF_SIZE];

	const char *node_end;
	
	struct script_node child =
	{
		.name_len = 0
	};
	
	sprintf(buffet, "[%s]", child_name);
	
	child.body = strnstr(parent.body, buffet, parent.body_len);
	
	if (child.body == parent.body)
		return child;
	
	node_end = strnstr(parent.body, "[end]", parent.body_len);

	if (node_end == parent.body)
		return child;

	child.body_len = node_end - child.body;

	script_node_print(child);
}

void script_node_print(struct script_node node)
{
	
	printak("<0x87CEFA>script node name:%.*s</>, <0xFFA500>body:%.*s\n</>",
		node.name_len, node.name, node.body_len, node.body);
	
}

