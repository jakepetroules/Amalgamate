//
//  amgexport.h
//  Amalgamate
//
//  Created by Jake Petroules on 2014-08-24.
//
//

#ifndef AMALGAMATE_EXPORT_H
#define AMALGAMATE_EXPORT_H

#if BUILDING_LIBAMALGAMATE
#define AMG_EXPORT __attribute((visibility("default")))
#else
#define AMG_EXPORT
#endif

#ifdef __cplusplus
#define AMG_EXTERN extern "C"
#else
#define AMG_EXTERN extern
#endif

#endif
