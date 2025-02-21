type<T> shared_ptr = {
    base: T[],
    uses: u64[],
}

function<T> make(value: T): shared_ptr<T> {
    println("A");

    const base: T[] = malloc(sizeof<T>);
    const uses: u64[] = malloc(sizeof<u64>);

    println("B");

    (*base) = value;
    (*uses) = 0;

    println("C");

    return {
        base,
        uses,
    }
}

function<T> use(shared: shared_ptr<T>&) {
    (*shared.uses)++
}

function<T> drop(shared: shared_ptr<T>&) {
    if (--(*shared.uses))
        return void
    free(shared.base)
    free(shared.uses)
}

function<T> get(shared: shared_ptr<T>&): T& {
    return *shared.base
}

function<T> get_uses(shared: shared_ptr<T>&): u64 {
    return *shared.uses
}
