/**
 * Copyright 2013-2018 by Explorer Developers.
 * made by Hu Wenjie<1@hwj.me>
 * Explorer Loader Script
 * /loader/boot/script.h
 * version:1.0.0
 * 7/8/2018 23:06
 */

#ifndef SCRIPT_H_
#define SCRIPT_H_

struct script_node
{
	const char *name;
	unsigned long name_len;
	const char *body;
	unsigned long body_len;
};

struct script_node script_init(const char *script_start, unsigned long length);

struct script_node script_child(struct script_node parent, const char *child_name);

void script_node_print(struct script_node node);

#endif

