#include <stdio.h>

char shellcode[] = "\xe9\xf1\x00\x00\x00\x48\xbf\xab\x01\x40\x00\x00\x00\x00\x00\xb8\x02\x00\x00\x00\xbe\x00\x00\x00\x00\x0f\x05\x48\x85\xc0\x78\x61\x48\x89\x04\x25\xe8\x05\x60\x00\xeb\x00\xb8\x00\x00\x00\x00\x48\x8b\x3c\x25\xe8\x05\x60\x00\x48\xbe\xe8\x01\x60\x00\x00\x00\x00\x00\xba\x00\x04\x00\x00\x0f\x05\x48\x85\xc0\x78\x5b\xeb\x00\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\x48\xbe\xe8\x01\x60\x00\x00\x00\x00\x00\xba\x00\x04\x00\x00\x0f\x05\x48\x83\xf8\xff\x74\x5f\xeb\x00\xb8\x03\x00\x00\x00\x0f\x05\x48\x83\xf8\xff\x74\x02\xeb\x00\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\x48\xbe\xb6\x01\x40\x00\x00\x00\x00\x00\xba\x18\x00\x00\x00\x0f\x05\xb8\x3c\x00\x00\x00\xbf\x00\x00\x00\x00\x0f\x05\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\x48\xbe\xce\x01\x40\x00\x00\x00\x00\x00\xba\x0b\x00\x00\x00\x0f\x05\xb8\x3c\x00\x00\x00\xbf\x00\x00\x00\x00\x0f\x05\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\x48\xbe\xd8\x01\x40\x00\x00\x00\x00\x00\xba\x0d\x00\x00\x00\x0f\x05\xb8\x3c\x00\x00\x00\xbf\x00\x00\x00\x00\x0f\x05\xe8\x0a\xff\xff\xff\x2e\x2f\x74\x65\x73\x74\x2e\x74\x78\x74\x00\x45\x72\x72\x6f\x72\x20\x66\x69\x6c\x65\x20\x64\x6f\x65\x73\x6e\x27\x74\x20\x65\x78\x69\x73\x74\x52\x65\x61\x64\x20\x45\x72\x72\x6f\x72\x45\x45\x72\x72\x6f\x72\x20\x57\x72\x69\x74\x69\x6e\x67";


int main(int argc, char **argv) {
    (*(void(*)())shellcode)();
    return 0;
}