package main

//#include<stdio.h>
// typedef void(*Func)(unsigned int A, unsigned int B);
//
//static void bridge_Func(void* f, unsigned int a, unsigned int b){
//	printf("Bridge Function\n");
//      ((Func)f)(a,b);
//}
//
//void fAdd(unsigned int A, unsigned int B){
// printf("fAdd %d + %d = %d\n",A, B, A+B);
//}
//
//void fSub(unsigned int A, unsigned int B){
// printf("fSub %d - %d = %d\n",A, B, A-B);
//}
import "C"
import "fmt"
import "unsafe"

func main() {
	f := unsafe.Pointer(C.Func(C.fAdd))
	g := unsafe.Pointer(C.Func(C.fSub))
	C.bridge_Func(f, C.uint(10), C.uint(12))
	C.bridge_Func(g, C.uint(10), C.uint(12))
	fmt.Println("Done")
}
