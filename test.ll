; ModuleID = 'module'
source_filename = "module"

@0 = private unnamed_addr constant [6 x i8] c"Felix\00", align 1
@1 = private unnamed_addr constant [10 x i8] c"Schreiber\00", align 1
@2 = private unnamed_addr constant [16 x i8] c"App Development\00", align 1
@u1 = internal global { double, ptr, ptr, ptr } zeroinitializer
@printUser.a = internal global double 0.000000e+00
@printUser.n = internal global ptr null
@printUser.s = internal global ptr null
@printUser.t = internal global ptr null
@3 = private unnamed_addr constant [2 x i8] c" \00", align 1
@4 = private unnamed_addr constant [2 x i8] c" \00", align 1
@5 = private unnamed_addr constant [2 x i8] c" \00", align 1
@recurse.n = internal global double 0.000000e+00
@n = internal global double 1.000000e+01
@r = internal global double 0.000000e+00
@6 = private unnamed_addr constant [9 x i8] c"recurse(\00", align 1
@7 = private unnamed_addr constant [5 x i8] c") = \00", align 1
@8 = private unnamed_addr constant [6 x i8] c"Felix\00", align 1
@t1 = internal global { ptr, { double, ptr, ptr, ptr } } zeroinitializer
@a1 = internal global { ptr, i64 } zeroinitializer
@printArray.array = internal global { ptr, i64 } zeroinitializer
@foo.x = internal global double 0.000000e+00

define i32 @main() {
entry:
  %0 = alloca { double, ptr, ptr, ptr }, align 8
  %1 = alloca i8, i64 1024, align 1
  %2 = alloca { ptr, { double, ptr, ptr, ptr } }, align 8
  %3 = alloca { ptr, i64 }, align 8
  %4 = getelementptr inbounds { double, ptr, ptr, ptr }, ptr %0, i32 0, i32 0
  store double 1.800000e+01, ptr %4, align 8
  %5 = getelementptr inbounds { double, ptr, ptr, ptr }, ptr %0, i32 0, i32 1
  store ptr @0, ptr %5, align 8
  %6 = getelementptr inbounds { double, ptr, ptr, ptr }, ptr %0, i32 0, i32 2
  store ptr @1, ptr %6, align 8
  %7 = getelementptr inbounds { double, ptr, ptr, ptr }, ptr %0, i32 0, i32 3
  store ptr @2, ptr %7, align 8
  %8 = load { double, ptr, ptr, ptr }, ptr %0, align 8
  store { double, ptr, ptr, ptr } %8, ptr @u1, align 8
  %9 = load { double, ptr, ptr, ptr }, ptr @u1, align 8
  call void @printUser({ double, ptr, ptr, ptr } %9)
  %10 = call double @recurse(double 1.000000e+01)
  store double %10, ptr @r, align 8
  %11 = load ptr, ptr @6, align 8
  %12 = load double, ptr @n, align 8
  %13 = load ptr, ptr @7, align 8
  %14 = load double, ptr @r, align 8
  call void (ptr, i64, ...) @format(ptr %1, i64 1024, i8 1, ptr %11, i8 2, i64 2, double %12, i8 1, ptr %13, i8 2, i64 2, double %14, i8 0)
  call void @println(ptr %1)
  %15 = getelementptr inbounds { ptr, { double, ptr, ptr, ptr } }, ptr %2, i32 0, i32 0
  store ptr @8, ptr %15, align 8
  %16 = getelementptr inbounds { ptr, { double, ptr, ptr, ptr } }, ptr %2, i32 0, i32 1
  %17 = load { double, ptr, ptr, ptr }, ptr @u1, align 8
  store { double, ptr, ptr, ptr } %17, ptr %16, align 8
  %18 = load { ptr, { double, ptr, ptr, ptr } }, ptr %2, align 8
  store { ptr, { double, ptr, ptr, ptr } } %18, ptr @t1, align 8
  %19 = call ptr @malloc(i64 24)
  %20 = getelementptr inbounds { ptr, i64 }, ptr %3, i32 0, i32 0
  store ptr %19, ptr %20, align 8
  %21 = getelementptr inbounds { ptr, i64 }, ptr %3, i32 0, i32 1
  store i64 3, ptr %21, align 4
  %22 = getelementptr double, ptr %19, i64 0
  store double 1.000000e+00, ptr %22, align 8
  %23 = getelementptr double, ptr %19, i64 1
  store double 2.000000e+00, ptr %23, align 8
  %24 = getelementptr double, ptr %19, i64 2
  store double 3.000000e+00, ptr %24, align 8
  %25 = load { ptr, i64 }, ptr %3, align 8
  store { ptr, i64 } %25, ptr @a1, align 8
  %26 = load { ptr, { double, ptr, ptr, ptr } }, ptr @t1, align 8
  call void @printArray({ ptr, { double, ptr, ptr, ptr } } %26)
  %27 = load { ptr, i64 }, ptr @a1, align 8
  call void @printArray({ ptr, i64 } %27)
  ret i32 0
}

declare void @println(ptr)

define void @printUser(ptr %0) {
entry:
  %1 = alloca { double, ptr, ptr, ptr }, align 8
  %2 = alloca i8, i64 1024, align 1
  %3 = load { double, ptr, ptr, ptr }, ptr %1, align 8
  store { double, ptr, ptr, ptr } %3, ptr %1, align 8
  %4 = getelementptr inbounds { double, ptr, ptr, ptr }, ptr %1, i32 0, i32 0
  %5 = load double, ptr %4, align 8
  store double %5, ptr @printUser.a, align 8
  %6 = getelementptr inbounds { double, ptr, ptr, ptr }, ptr %1, i32 0, i32 1
  %7 = load ptr, ptr %6, align 8
  store ptr %7, ptr @printUser.n, align 8
  %8 = getelementptr inbounds { double, ptr, ptr, ptr }, ptr %1, i32 0, i32 2
  %9 = load ptr, ptr %8, align 8
  store ptr %9, ptr @printUser.s, align 8
  %10 = getelementptr inbounds { double, ptr, ptr, ptr }, ptr %1, i32 0, i32 3
  %11 = load ptr, ptr %10, align 8
  store ptr %11, ptr @printUser.t, align 8
  %12 = load ptr, ptr @printUser.n, align 8
  %13 = load ptr, ptr @3, align 8
  %14 = load ptr, ptr @printUser.s, align 8
  %15 = load ptr, ptr @4, align 8
  %16 = load double, ptr @printUser.a, align 8
  %17 = load ptr, ptr @5, align 8
  %18 = load ptr, ptr @printUser.t, align 8
  call void (ptr, i64, ...) @format(ptr %2, i64 1024, i8 2, i64 3, ptr %12, i8 1, ptr %13, i8 2, i64 3, ptr %14, i8 1, ptr %15, i8 2, i64 2, double %16, i8 1, ptr %17, i8 2, i64 3, ptr %18, i8 0)
  call void @println(ptr %2)
  ret void
}

declare void @format(ptr, i64, ...)

define double @recurse(double %n) {
entry:
  store double %n, ptr @recurse.n, align 8
  %0 = load double, ptr @recurse.n, align 8
  %1 = fcmp ole double %0, 1.000000e+00
  br i1 %1, label %then, label %end

then:                                             ; preds = %entry
  %2 = load double, ptr @recurse.n, align 8
  ret double %2

end:                                              ; preds = %entry
  %3 = load double, ptr @recurse.n, align 8
  %4 = fsub double %3, 1.000000e+00
  %5 = call double @recurse(double %4)
  %6 = load double, ptr @recurse.n, align 8
  %7 = fsub double %6, 2.000000e+00
  %8 = call double @recurse(double %7)
  %9 = fadd double %5, %8
  ret double %9
}

declare ptr @malloc(i64)

define void @printArray(ptr %array) {
entry:
  %0 = alloca i8, i64 1024, align 1
  store ptr %array, ptr @printArray.array, align 8
  call void (ptr, i64, ...) @format(ptr %0, i64 1024, i8 2, i64 4, ptr @printArray.array, i8 0)
  call void @println(ptr %0)
  ret void
}

define ptr @foo(double %x) {
entry:
  store double %x, ptr @foo.x, align 8
  ret ptr @bar
}

define double @bar() {
entry:
  %0 = load double, ptr @foo.x, align 8
  ret double %0
}
