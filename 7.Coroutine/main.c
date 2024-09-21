#include <setjmp.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct ICoroutine ICoroutine;
typedef struct CoroutineScheduler CoroutineScheduler;
typedef void (*CoroutineFunc)(ICoroutine *this);

void schedule_next_coroutine(CoroutineScheduler *this);
void start_coroutine(CoroutineScheduler *this, ICoroutine *coroutine);

struct ICoroutine
{
    jmp_buf ctx;
    bool completed;
    bool initialized;
    CoroutineScheduler *scheduler;
    CoroutineFunc entry;
};

struct CoroutineScheduler
{
    ICoroutine **coroutines;
    size_t size;
    size_t current;
    jmp_buf main_ctx;
};

void finished(ICoroutine *this)
{
    this->completed = true;
    longjmp(this->scheduler->main_ctx, 1);
}

void yield(ICoroutine *this)
{
    schedule_next_coroutine(this->scheduler);
}

void register_to_scheduler(ICoroutine *this, CoroutineScheduler *scheduler)
{
    this->scheduler = scheduler;
}

ICoroutine create_from_entry(CoroutineFunc entry)
{
    ICoroutine inner;
    inner.entry = entry;
    inner.completed = false;
    inner.initialized = false;

    return inner;
}

ICoroutine *get_current(CoroutineScheduler *this)
{
    if (this->coroutines == NULL)
    {
        return NULL;
    }

    return this->coroutines[this->current];
}

void schedule_next_coroutine(CoroutineScheduler *this)
{
    size_t curr_idx = this->current;
    size_t next_idx = (curr_idx + 1) % this->size;

    size_t initial_next = next_idx;

    // skip completed ones
    while (this->coroutines[next_idx]->completed)
    {
        next_idx = (next_idx + 1) % this->size;

        // All finished, should not happen.
        if (next_idx == initial_next)
            return;
    }

    ICoroutine *curr = this->coroutines[curr_idx];
    ICoroutine *next = this->coroutines[next_idx];

    this->current = next_idx;

    if (next->initialized)
    {
        if (setjmp(curr->ctx) == 0)
        {
            longjmp(next->ctx, 1);
        }
    }
    else
    {
        if (setjmp(curr->ctx) == 0)
        {
            // Back to scheduler to initialize the next coroutine
            longjmp(this->main_ctx, 1);
        }
    }
}

bool has_any_unfinished(CoroutineScheduler *this)
{
    for (size_t i = 0; i < this->size; i++)
    {
        if (!this->coroutines[i]->completed)
            return true;
    }

    return false;
}

void start_all(CoroutineScheduler *this)
{
    for (size_t i = 0; i < this->size; i++)
    {
        printf("Starting coroutine %ld\n", i);
        ICoroutine *coroutine = this->coroutines[i];

        this->current = i;

        if (setjmp(coroutine->ctx) == 0)
        {
            start_coroutine(this, coroutine);
        }
    }

    while (has_any_unfinished(this))
    {
        schedule_next_coroutine(this);
    }

    printf("All coroutines finished!\n");
}

void start_coroutine(CoroutineScheduler *this, ICoroutine *coroutine)
{
    if (setjmp(this->main_ctx) == 0)
    {
        register_to_scheduler(coroutine, this);
        coroutine->initialized = true;

        coroutine->entry(coroutine);
        finished(coroutine);
    }
}

#pragma region COROUTINE_1

struct Coroutine_1
{
    ICoroutine inner;
};

void coroutine_1_func(ICoroutine *this)
{
    for (size_t i = 0; i < 10; i++)
    {
        printf("The %ld iterations from COROUTINE_1!\n", i);

        yield(this);
    }
}

typedef struct Coroutine_1 Coroutine_1;

Coroutine_1 create_couroutine_1()
{
    Coroutine_1 new;
    new.inner = create_from_entry(coroutine_1_func);

    return new;
}

#pragma endregion // COROUTINE_1

#pragma region COROUTINE_2

struct Coroutine_2
{
    ICoroutine inner;
};

void coroutine_2_func(ICoroutine *this)
{
    for (size_t i = 0; i < 5; i++)
    {
        printf("The %ld iterations from COROUTINE_2!\n", i);

        yield(this);
    }
}

typedef struct Coroutine_2 Coroutine_2;

Coroutine_2 create_couroutine_2()
{
    Coroutine_2 new;
    new.inner = create_from_entry(coroutine_2_func);

    return new;
}

#pragma endregion // COROUTINE_2

int main()
{
    Coroutine_1 c1 = create_couroutine_1();
    Coroutine_2 c2 = create_couroutine_2();

    ICoroutine *coroutine_list[2] = {(ICoroutine *)&c1, (ICoroutine *)&c2};

    CoroutineScheduler scheduler;
    scheduler.coroutines = coroutine_list;
    scheduler.current = 0;
    scheduler.size = 2;

    start_all(&scheduler);

    printf("Back to main\n");

    return 0;
}
