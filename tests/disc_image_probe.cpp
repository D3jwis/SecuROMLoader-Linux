#include <windows.h>
#include <ntddscsi.h>
#include <cstdio>
#include <cstring>
#include <cstdint>

static int failures;
static FILE* report;
static void check(bool result, const char* name) {
    fprintf(report, "%s %s\n", result ? "PASS" : "FAIL", name);
    fflush(report);
    if (!result) ++failures;
}
static DWORD be32(const BYTE* p) {
    return (DWORD(p[0]) << 24) | (DWORD(p[1]) << 16) | (DWORD(p[2]) << 8) | p[3];
}
static bool request(HANDLE drive, BYTE opcode, BYTE* data, DWORD length, DWORD lba=0, WORD blocks=0) {
    SCSI_PASS_THROUGH_DIRECT s = {};
    s.Length=sizeof(s); s.CdbLength=10; s.DataIn=SCSI_IOCTL_DATA_IN;
    s.TimeOutValue=10; s.DataBuffer=data; s.DataTransferLength=length;
    s.Cdb[0]=opcode;
    if (opcode==0x28) {
        s.Cdb[2]=BYTE(lba>>24); s.Cdb[3]=BYTE(lba>>16); s.Cdb[4]=BYTE(lba>>8); s.Cdb[5]=BYTE(lba);
        s.Cdb[7]=BYTE(blocks>>8); s.Cdb[8]=BYTE(blocks);
    } else if (opcode==0x43) { s.Cdb[7]=BYTE(length>>8); s.Cdb[8]=BYTE(length); }
    DWORD bytes=0;
    return DeviceIoControl(drive, IOCTL_SCSI_PASS_THROUGH_DIRECT, &s, sizeof(s), &s, sizeof(s), &bytes, nullptr) && s.ScsiStatus==0;
}
int main(int argc, char** argv) {
    if (argc!=2) return 2;
    report=fopen("probe-results.txt","w"); if (!report) return 3;
    FILE* image=fopen(argv[1],"rb"); if (!image) return 4;
    _fseeki64(image,0,SEEK_END); const int64_t size=_ftelli64(image);
    BYTE expected[2048]; _fseeki64(image,16LL*2048,SEEK_SET);
    if (fread(expected,1,sizeof(expected),image)!=sizeof(expected)) return 5;
    fclose(image);
    check(LoadLibraryA("version.dll")!=nullptr,"load rebuilt DLL without interactive prompts");
    HANDLE drive=CreateFileA("\\\\.\\D:",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_EXISTING,0,nullptr);
    check(drive!=INVALID_HANDLE_VALUE,"Kernel32 optical device mapping");
    using OpenFn=HANDLE (WINAPI*)(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
    OpenFn open=reinterpret_cast<OpenFn>(GetProcAddress(GetModuleHandleA("kernelbase.dll"),"CreateFileA"));
    HANDLE second=open("\\\\.\\D:",GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_EXISTING,0,nullptr);
    check(second!=INVALID_HANDLE_VALUE,"KernelBase optical device mapping");
    if (second!=INVALID_HANDLE_VALUE) CloseHandle(second);
    BYTE data[2049]; memset(data,0xA5,sizeof(data));
    check(request(drive,0x28,data,2048,16,1) && memcmp(data,expected,2048)==0 && data[2048]==0xA5,"READ(10) matches image and preserves guard byte");
    check(request(drive,0x25,data,8) && be32(data)==size/2048-1 && be32(data+4)==2048,"READ CAPACITY reports actual image geometry");
    memset(data,0xA5,sizeof(data));
    check(request(drive,0x43,data,1024) && data[1]==0x12 && be32(data+16)==size/2048 && data[20]==0xA5 && data[1024]==0xA5,"TOC copy ends at descriptor boundary");
    memset(data,0xA5,sizeof(data));
    check(request(drive,0x43,data,4) && data[4]==0xA5,"short TOC allocation preserves guard byte");
    memset(data,0xA5,sizeof(data));
    check(!request(drive,0x28,data,2048,DWORD(size/2048),1) && data[0]==0xA5,"out-of-range LBA rejected before writing");
    check(!request(drive,0x28,data,4,16,1) && data[4]==0xA5,"undersized READ buffer rejected");
    CloseHandle(drive); fprintf(report,"failures=%d\n",failures); fclose(report);
    return failures ? 1 : 0;
}
