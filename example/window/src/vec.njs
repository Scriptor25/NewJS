type<T> vec = {
    base: T[],
    cap: u64,
    size: u64,
}

function<T> vec_new(): vec<T> {
    const cap: u64 = 10
    const base: T[] = malloc(cap * sizeof<T>)
    return {
        base,
        cap,
        size: 0,
    }
}

function<T> vec_del(&self: vec<T>) {
    free(self.base)
    self.base = 0
    self.cap = 0
    self.size = 0
}

function<T> vec_clear(&self: vec<T>) {
    self.base = realloc(self.base, 0)
    self.size = 0
    self.cap = 0
}

function<T> vec_push(&self: vec<T>, element: T) {
    if (self.cap == self.size) {
        self.cap += self.cap
        self.base = realloc(self.base, self.cap * sizeof<T>)
    }
    self.base[self.size++] = element
}

function<T> vec_pop(&self: vec<T>): T {
    return self.base[--self.size]
}

function<T> vec_at(&self: vec<T>, pos: u64): T& {
    return self.base[pos]
}
