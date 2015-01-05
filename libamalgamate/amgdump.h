/*
 * Copyright (c) 2014 Petroules Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AMALGAMATE_DUMP_H
#define AMALGAMATE_DUMP_H

#include <stdio.h>
#include "dsstore.h"

AMG_EXPORT AMG_EXTERN int amg_dump_file(const char *filename);
AMG_EXPORT AMG_EXTERN int amg_dump_allocator_state(dsstore_buddy_allocator_state_t *allocator_state, FILE *file);
AMG_EXPORT AMG_EXTERN int amg_dump_header_block(dsstore_header_block_t *header_block, FILE *file);
AMG_EXPORT AMG_EXTERN int amg_dump_block(dsstore_buddy_allocator_state_t *allocator_state,
                                         dsstore_header_block_t *header_block,
                                         uint32_t block_number, FILE *file);
AMG_EXPORT AMG_EXTERN int amg_dump_record(FILE *file);

#endif // AMALGAMATE_DUMP_H
