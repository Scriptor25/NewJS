type pthread_once_t = i64
type pthread_mutexattr_t = u32
type pthread_key_t = u32
type pthread_condattr_t = i32

type pthread_t = u64

type sched_param_t = { sched_priority: i32 }

type pthread_attr_t = {
    p_state: u32,
    stack: void[],
    s_size: u64,
    param: sched_param_t,
}

type pthread_mutex_t = i32
type pthread_cond_t = i32

type timespec_t = {
  tv_sec: i64,
  tv_nsec: i64,
}

extern function pthread_attr_destroy(attr: pthread_attr_t[]): i32
extern function pthread_attr_getdetachstate(attr: pthread_attr_t[const], detachstate: i32[]): i32
extern function pthread_attr_getschedparam(attr: pthread_attr_t[const], param: sched_param_t[]): i32
extern function pthread_attr_getstackaddr(attr: pthread_attr_t[const], stackaddr: void[][]): i32
extern function pthread_attr_getstacksize(attr: pthread_attr_t[const], stacksize: u64[]): i32
extern function pthread_attr_init(attr: pthread_attr_t[]): i32
extern function pthread_attr_setdetachstate(attr: pthread_attr_t[], detachstate: i32): i32
extern function pthread_attr_setschedparam(attr: pthread_attr_t[], param: sched_param_t[const]): i32
extern function pthread_attr_setstackaddr(attr: pthread_attr_t[], stackaddr: void[]): i32
extern function pthread_attr_setstacksize(attr: pthread_attr_t[], stacksize: u64): i32
extern function pthread_cancel(thread: pthread_t): i32
extern function pthread_cond_broadcast(cond: pthread_cond_t[]): i32
extern function pthread_cond_destroy(cond: pthread_cond_t[]): i32
extern function pthread_cond_init(cond: pthread_cond_t[], attr: pthread_condattr_t[const]): i32
extern function pthread_cond_signal(cond: pthread_cond_t[]): i32
extern function pthread_cond_timedwait(cond: pthread_cond_t[], mutex: pthread_mutex_t[], abstime: timespec_t[const]): i32
extern function pthread_cond_wait(cond: pthread_cond_t[], mutex: pthread_mutex_t[]): i32
extern function pthread_condattr_destroy(attr: pthread_condattr_t[]): i32
extern function pthread_condattr_getpshared(attr: pthread_condattr_t[const], pshared: i32[]): i32
extern function pthread_condattr_init(attr: pthread_condattr_t[]): i32
extern function pthread_condattr_setpshared(attr: pthread_condattr_t[], pshared: i32): i32
extern function pthread_create(thread: pthread_t[], attr: pthread_attr_t[const], start_routine: (void[]) => void[], arg: void[]): i32
extern function pthread_detach(thread: pthread_t): i32
extern function pthread_equal(t1: pthread_t, t2: pthread_t): i32
extern function pthread_exit(value_ptr: void[]): void
extern function pthread_getspecific(key: pthread_key_t): void[]
extern function pthread_join(thread: pthread_t, value_ptr: void[][]): i32
extern function pthread_key_create(key: pthread_key_t[], destructor: (void[]) => void): i32
extern function pthread_key_delete(key: pthread_key_t): i32
extern function pthread_kill(thread: pthread_t, sig: i32): i32
extern function pthread_mutex_destroy(mutex: pthread_mutex_t[]): i32
extern function pthread_mutex_init(mutex: pthread_mutex_t[], attr: pthread_mutexattr_t[const]): i32
extern function pthread_mutex_lock(mutex: pthread_mutex_t[]): i32
extern function pthread_mutex_trylock(mutex: pthread_mutex_t[]): i32
extern function pthread_mutex_unlock(mutex: pthread_mutex_t[]): i32
extern function pthread_mutexattr_destroy(attr: pthread_mutexattr_t[]): i32
extern function pthread_mutexattr_getpshared(attr: pthread_mutexattr_t[const], pshared: i32[]): i32
extern function pthread_mutexattr_init(attr: pthread_mutexattr_t[]): i32
extern function pthread_mutexattr_setpshared(attr: pthread_mutexattr_t[], pshared: i32): i32
extern function pthread_once(once_control: pthread_once_t[], init_routine: () => void): i32
extern function pthread_self(): pthread_t
extern function pthread_setcancelstate(state: i32, oldstate: i32[]): i32
extern function pthread_setcanceltype(type: i32, oldtype: i32[]): i32
extern function pthread_setspecific(key: pthread_key_t, value: void[const]): i32
extern function pthread_testcancel(): void

export function attr_destroy(&attr: pthread_attr_t): i32 { return pthread_attr_destroy(&attr) }
export function attr_getdetachstate(const &attr: pthread_attr_t, &detachstate: i32): i32 { return pthread_attr_getdetachstate(&attr, &detachstate) }
export function attr_getschedparam(const &attr: pthread_attr_t, &param: sched_param_t): i32 { return pthread_attr_getschedparam(&attr, &param) }
export function attr_getstackaddr(const &attr: pthread_attr_t, &stackaddr: void[]): i32 { return pthread_attr_getstackaddr(&attr, &stackaddr) }
export function attr_getstacksize(const &attr: pthread_attr_t, &stacksize: u64): i32 { return pthread_attr_getstacksize(&attr, &stacksize) }
export function attr_init(&attr: pthread_attr_t): i32 { return pthread_attr_init(&attr) }
export function attr_setdetachstate(&attr: pthread_attr_t, detachstate: i32): i32 { return pthread_attr_setdetachstate(&attr, detachstate) }
export function attr_setschedparam(&attr: pthread_attr_t, const &param: sched_param_t): i32 { return pthread_attr_setschedparam(&attr, &param) }
export function attr_setstackaddr(&attr: pthread_attr_t, stackaddr: void[]): i32 { return pthread_attr_setstackaddr(&attr, stackaddr) }
export function attr_setstacksize(&attr: pthread_attr_t, stacksize: u64): i32 { return pthread_attr_setstacksize(&attr, stacksize) }
export function cancel(thread: pthread_t): i32 { return pthread_cancel(thread) }
export function cond_broadcast(&cond: pthread_cond_t): i32 { return pthread_cond_broadcast(&cond) }
export function cond_destroy(&cond: pthread_cond_t): i32 { return pthread_cond_destroy(&cond) }
export function cond_init(&cond: pthread_cond_t, const &attr: pthread_condattr_t): i32 { return pthread_cond_init(&cond, &attr) }
export function cond_signal(&cond: pthread_cond_t): i32 { return pthread_cond_signal(&cond) }
export function cond_timedwait(&cond: pthread_cond_t, &mutex: pthread_mutex_t, const &abstime: timespec_t): i32 { return pthread_cond_timedwait(&cond, &mutex, &abstime) }
export function cond_wait(&cond: pthread_cond_t, &mutex: pthread_mutex_t): i32 { return pthread_cond_wait(&cond, &mutex) }
export function condattr_destroy(&attr: pthread_condattr_t): i32 { return pthread_condattr_destroy(&attr) }
export function condattr_getpshared(const &attr: pthread_condattr_t, &pshared: i32): i32 { return pthread_condattr_getpshared(&attr, &pshared) }
export function condattr_init(&attr: pthread_condattr_t): i32 { return pthread_condattr_init(&attr) }
export function condattr_setpshared(&attr: pthread_condattr_t, pshared: i32): i32 { return pthread_condattr_setpshared(&attr, pshared) }
export function create(&thread: pthread_t, attr: pthread_attr_t[const], start_routine: (void[]) => void[], arg: void[]): i32 { return pthread_create(&thread, attr, start_routine, arg) }
export function detach(thread: pthread_t): i32 { return pthread_detach(thread) }
export function equal(t1: pthread_t, t2: pthread_t): i32 { return pthread_equal(t1, t2) }
export function exit(value_ptr: void[]): void { pthread_exit(value_ptr) }
export function getspecific(key: pthread_key_t): void[] { return pthread_getspecific(key) }
export function join(thread: pthread_t, value_ptr: void[][]): i32 { return pthread_join(thread, value_ptr) }
export function key_create(&key: pthread_key_t, destructor: (void[]) => void): i32 { return pthread_key_create(&key, destructor) }
export function key_delete(key: pthread_key_t): i32 { return pthread_key_delete(key) }
export function kill(thread: pthread_t, sig: i32): i32 { return pthread_kill(thread, sig) }
export function mutex_destroy(&mutex: pthread_mutex_t): i32 { return pthread_mutex_destroy(&mutex) }
export function mutex_init(&mutex: pthread_mutex_t, const &attr: pthread_mutexattr_t): i32 { return pthread_mutex_init(&mutex, &attr) }
export function mutex_lock(&mutex: pthread_mutex_t): i32 { return pthread_mutex_lock(&mutex) }
export function mutex_trylock(&mutex: pthread_mutex_t): i32 { return pthread_mutex_trylock(&mutex) }
export function mutex_unlock(&mutex: pthread_mutex_t): i32 { return pthread_mutex_unlock(&mutex) }
export function mutexattr_destroy(&attr: pthread_mutexattr_t): i32 { return pthread_mutexattr_destroy(&attr) }
export function mutexattr_getpshared(const &attr: pthread_mutexattr_t, &pshared: i32): i32 { return pthread_mutexattr_getpshared(&attr, &pshared) }
export function mutexattr_init(&attr: pthread_mutexattr_t): i32 { return pthread_mutexattr_init(&attr) }
export function mutexattr_setpshared(&attr: pthread_mutexattr_t, pshared: i32): i32 { return pthread_mutexattr_setpshared(&attr, pshared) }
export function once(&once_control: pthread_once_t, init_routine: () => void): i32 { return pthread_once(&once_control, init_routine) }
export function self(): pthread_t { return pthread_self() }
export function setcancelstate(state: i32, &oldstate: i32): i32 { return pthread_setcancelstate(state, &oldstate) }
export function setcanceltype(type: i32, &oldtype: i32): i32 { return pthread_setcanceltype(type, &oldtype) }
export function setspecific(key: pthread_key_t, value: void[const]): i32 { return pthread_setspecific(key, value) }
export function testcancel(): void { pthread_testcancel() }
