/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#ifndef ERROR_H
#define ERROR_H

#define ERRORS \
	X(SILENT) \
	X(OK) \
	X(COMMAND) \
	X(CHECKSUM) \
	X(RECTYPE) \
	X(BYTECOUNT) \
	X(VERIFY) \
	X(DEVICE) \
	X(ROM_WRITE)

#define X(name) E_##name,
enum {
	ERRORS
};
#undef X

#define X(name) [E_##name] = #name,
static const char *error_string[] = {
	ERRORS
};
#undef X

#endif
