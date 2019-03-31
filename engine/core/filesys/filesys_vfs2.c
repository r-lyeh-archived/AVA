//API int             vfs_size( const char *file );
//API int             vfs_exists( const char *file );
//API char *          vfs_chunk( const char *file, int offset, int len );
//API int vfs_mount(dir) { add dir to found; merge(scan(dir)); }
//API int vfs_scan() { delete found[]; for each(dir) merge(scan(dir)); }
//API int vfs_unmount(dir) { delete dir in found; scan(); }
//
