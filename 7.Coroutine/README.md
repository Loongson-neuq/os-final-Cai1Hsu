## 7. [主观题] 手动实现一个无栈协程

### 背景信息
协程可以在一个线程中实现多任务调度，一个简单的例子是，考虑一下计算斐波那契数列的函数：

```Csharp
static T Fib<T>(T n)
    where T : INumber<T>
{
    T f1 = T.Zero;
    T f2 = T.One;

    for (T i = T.Zero; i < n; i = i + T.One)
    {
        T f = f1 + f2;
        f1 = f2;
        f2 = f;
    }

    return f1;
}
```

这样的实现，为了打印从第 1 个到第 n 个斐波那契数列，我们需要调用 `Fib` 函数 n 次，每次都需要重新计算一遍，这样的实现效率是很低的。通常情况下，为了改善这个问题，你会这样写：

```Csharp
static void Fib<T>(T n)
    where T : INumber<T>
{
    T f1 = T.Zero;
    T f2 = T.One;

    for (T i = T.Zero; i < n; i = i + T.One)
    {
        T f = f1 + f2;
        f1 = f2;
        f2 = f;

        Console.WriteLine($"The {i + T.One}th Fibonacci number is {f}");
    }
}
```

但是这种实现方式，我们无法在中途停止，然后继续计算。并且由于打印输出的相关代码与算法紧密偶合在一起，假如我们需要在其它地方使用这个算法，我们就需要重新实现一遍。代码复用性很差。

无栈协程能够在避免有栈协程保存/恢复上下文的巨大开销的同时，实现多任务调度。

C# 语言自带了协程支持，因此，你可以这样实现：

```Csharp
static IEnumerable<T> Fib<T>(T n)
    where T : INumber<T>
{
    T f1 = T.Zero;
    T f2 = T.One;

    while (true)
    {
        T f = f1 + f2;
        f1 = f2;
        f2 = f;

        yield return f;
    }
}
```

对于同一个协程上下文，第一次调用时，Fib 函数返回第一个数，第二次调用时，Fib 函数从上次的 yield return 语句处继续执行，直到再次遇到 yield return 语句，然后返回第二个数，以此类推。

#### 优势

计算结果的个数不需要提前指定，而是在调用方根据需要调用，这样的实现方式，可以在中途停止，然后继续计算，也可以在其它地方使用这个算法，代码复用性很好。并且，调用方（消费者）可以决定何时调用，何时停止，这样的实现方式，可以实现多任务调度。

## 题目描述

你的任务是编写一个 Fib 函数的无栈协程上下文，并且无需提前指定计算结果的个数。

注：你可以使用任何语言来实现。

你实现的协程应该使得以下代码能够正常运行：

```C

int main()
{
    for (int i = 0; i < 10; i++)
    {
        int f = next_fib(/*???*/);
        
        printf("The %dth Fibonacci number is %d\n", i + 1, f);
    }
}

```

提示：谨记“闭包是函数+执行环境”

### 进阶要求

同时支持多个协程上下文，使得以下代码能够正常运行：

```C

int main()
{
    for (int i = 0; i < 10; i++)
    {
        int f1 = next_fib(/*第一个协程上下文*/);
        int p = next_prime(/*质数协程上下文*/); // 计算下一个质数
        
        printf("The %dth Fibonacci number is %d\n", i + 1, f1);
        printf("The %dth Prime number is %d\n", i + 1, p);
    }

    for (int i = 0; i < 5; i++)
    {
        // 不应该从第一个协程上下文继续计算，而是从第一数开始。
        // 这意味着应该是一个新的协程上下文，但是你必须使用同一个方法/函数。
        int f2 = next_fib(/*第二个协程上下文*/);

        printf("The %dth Fibonacci number is %d\n", i + 1, f2);
    }
}

```

### 思考题

1. 根据你的实现代码，谈谈你对“闭包是函数+执行环境”的理解，并思考为什么原来的`f1`和`f2`不能够定义成局部变量。

2. 谈谈你对“上下文”的理解，对于函数来说，上下文是什么？对于协程来说，上下文是什么？对于进程来说，上下文是什么？

3. 根据第二个问题，思考有栈协程要保存的上下文涉及哪些内容，它们可以被保存在哪里？以及为什么有栈协程必须保存/恢复上下文，而无栈协程不需要保存/恢复上下文。

4. 利用你的实现代码，思考下一道题目[Async/Await Underlying](../8.AsyncAwaitUnderlying/README.md)。

### 扩展阅读

[How Async/Await Really Works](https://devblogs.microsoft.com/dotnet/how-async-await-really-works/)
