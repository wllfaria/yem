struct yem_fs_dir {
    char** paths;
    int size;
};

int yem_fs_init();
struct yem_fs_dir* yem_fs_read_recurse(char* path);
