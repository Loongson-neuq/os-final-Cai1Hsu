#include <setjmp.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

struct ICoroutine
{
    bool completed;
};

typedef struct ICoroutine ICoroutine;

ICoroutine create_inner()
{
    ICoroutine inner;
    inner.completed = false;

    return inner;
}

bool is_completed(ICoroutine* this)
{
    return this->completed;
}

struct FibCoroutine
{
    ICoroutine inner;
    size_t state;
    int f1;
    int f2;
};

typedef struct FibCoroutine FibCoroutine;

FibCoroutine* create_fib_coroutine()
{
    FibCoroutine* fib = malloc(sizeof(FibCoroutine));
    fib->inner = create_inner();
    fib->state = -1;
    fib->f1 = 0;
    fib->f2 = 1;

    return fib;
}

int next_fib(ICoroutine* this)
{
    FibCoroutine* _this = (FibCoroutine*)this;

    ++_this->state;

    int sum = _this->f1 + _this->f2;

    // 有多个 yield 语句则需要匹配状态。
    switch (_this->state)
    {
        case 0:
            return _this->f1;
            break;

        case 1:
            return _this->f2;  

        default:
            _this->f1 = _this->f2;
            _this->f2 = sum;
            --_this->state;

            return sum;
    }
}

int main()
{
    FibCoroutine* fib1 = create_fib_coroutine();
    FibCoroutine* fib2 = create_fib_coroutine();

    for (int i = 0; i < 10; i++)
    {
        int f1 = next_fib((ICoroutine*)fib1);
        next_fib((ICoroutine*)fib2);

        printf("%d\n", f1);
    }

    // 从第11个数开始
    for (int i = 0; i < 5; i++)
    {
        printf("%d\n", next_fib((ICoroutine*)fib1));
    }

    free(fib1);
    free(fib2);

    return 0;
}
