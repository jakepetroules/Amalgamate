//
//  dsstore.c
//  Amalgamate
//
//  Created by Jake Petroules on 2014-05-18.
//
//

#include "dsrecord.h"
#include "dsstore.h"

void dsstore_header_init(dsstore_header_t *header)
{
    assert(header);
    header->version = 1;
    header->magic = kDSHeaderMagic;
}
