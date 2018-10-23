static void jo_write_tga(const char *filename, void *rgba, int width, int height, int numChannels) {
    FILE *fp = fopen(filename, "wb");
    if(!fp) {
        return;   
    }
    // Header
    fwrite("\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00", 12, 1, fp);
    fwrite(&width, 2, 1, fp);
    fwrite(&height, 2, 1, fp);
    int bpc = numChannels * 8 | 0x2000; // 8 bits per channel
    fwrite(&bpc, 2, 1, fp);
    // Swap RGBA to BGRA if using 3 or more channels
    int remap[4] = {numChannels >= 3 ? 2 : 0, 1, numChannels >= 3 ? 0 : 2, 3};
    char *s = (char *)rgba;
    for(int i = 0; i < width*height; ++i) {
        for(int j = 0; j < numChannels; ++j) {
            fputc(s[remap[j]], fp);
        }
        s += numChannels;
    }
    fclose(fp);
}
