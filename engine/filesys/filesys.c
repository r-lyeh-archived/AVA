#ifdef CONFIG_H
#include CONFIG_H
#endif

#ifdef FILESYS_C
#pragma once
#define DIR_C
#define FILE_C
#define FOLDER_C
#define FTYPE_C
#define KVS_C
#define URI_C
#define VFS_C
#define PAK_C
#endif

#include <stdio.h>

#include "filesys_kvs.c"
#include "filesys_folder.c"
#include "filesys_dir.c"
#include "filesys_file.c"
#include "filesys_type.c"
#include "filesys_uri.c"
#include "filesys_vfs.c"
#include "filesys_pak.c"
