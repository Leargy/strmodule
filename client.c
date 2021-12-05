#include "chrmod.h"

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


void print_thread_struct(struct thread_struct *thr_str) {
    printf("THREAD_STRUCT: %p\n", &thr_str);

    printf("/-------Thread_structure--------/\n");
    printf("sp: %lu\n", thr_str->sp);
    printf("es: %hu\n", thr_str->es);
    printf("ds: %hu\n", thr_str->ds);
    printf("fsindex: %hu\n", thr_str->fsindex);
    printf("gsindex: %hu\n", thr_str->gsindex);
    printf("fs: %lu\n", thr_str->fs);
    printf("gs: %lu\n", thr_str->gs);
    printf("virtual_dr6: %lu\n", thr_str->virtual_dr6);
    printf("ptrace_dr7: %lu\n", thr_str->ptrace_dr7);
    printf("cr2: %lu\n", thr_str->cr2);
    printf("trap_nr: %lu\n", thr_str->trap_nr);
    printf("error_code: %lu\n", thr_str->error_code);
    printf("io_bitmap -> %p\n", &thr_str->io_bitmap);
    printf("iopl_emul: %lu\n", thr_str->iopl_emul);
    printf("sig_on_uaccess_err:1: %u\n", thr_str->sig_on_uaccess_err);
    printf("fpu -> %p\n", &thr_str->fpu);
    printf("/-------------------------------/\n\n");
}

void print_page_struct(struct page *pg_str) {
    printf("PAGE: %p\n", &pg_str);
    
    printf("/-------Page_structure--------/\n");
    printf("flags: %lu\n", pg_str->flags);
    printf("anonim union...\n");
    printf("anonim union...\n");
    printf("_refcount {\n");
    printf("    counter: %u\n", pg_str->_refcount.counter);
    printf("}\n");
    printf("/-------------------------------/\n\n");
}


int main(int argc, char **argv) {
    if(argc < 3) {
        printf("Not enough args \n Usage: <ioctl-file>, <PID>");
        return EXIT_FAILURE;
    }

    int fdisc, ret;
    unsigned int PID = atoi(argv[2]);

    fdisc = open(argv[1], O_RDONLY);
    
    if(fdisc == -1) {
	perror("No such file.");
        return EXIT_FAILURE;
    }
    
    struct thread_struct thr_str;
    ret = ioctl(fdisc, THREAD_STRUCT_IOCTL_SET_PID, &PID);
    ret = ioctl(fdisc, THREAD_STRUCT_IOCTL_GET, &thr_str);
    
    printf("PID: %u\n", PID);   
    print_thread_struct(&thr_str);
    
    struct page pg_str; 
    ret = ioctl(fdisc, PAGE_IOCTL_GET, &pg_str);
    
    print_page_struct(&pg_str);

    close(fdisc);
    return 0;
}
