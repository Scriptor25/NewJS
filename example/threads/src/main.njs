#NULL "(0 as void[])"

extern function sleep(seconds: u32): u32
extern function println(line: string)

type pthread_t = u64
type pthread_attr_t

extern function pthread_create(thread: pthread_t[], attr: pthread_attr_t[const], start_routine: (void[]) => void[], arg: void[]): i32
extern function pthread_join(thread: pthread_t, value_ptr: void[][]): i32

function myThreadFun(vargp: void[]): void[] {
    sleep(1)
    println("Hello World from other Thread!")
    return NULL
}

let thread_id: pthread_t

println("Before Thread")

pthread_create(&thread_id, NULL, myThreadFun, NULL)
pthread_join(thread_id, NULL)

println("After Thread")
