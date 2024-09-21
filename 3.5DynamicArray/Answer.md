## Vector<T> 的原理

Vector<T> 含有一下三个字段：
- size: 描述当前数组的元素个数
- capacity: 描述当前数组的容量
- p_data: 指向数组的指针

数组的实际内容分配在 heap 上，p_data 指向该数组的起始地址。当 size 达到 capacity 时，Vector<T> 会重新分配一个更大的数组，并将原数组的内容复制到新数组中，然后释放原数组。复制过程通常可以由标准库的realloc自动完成。realloc可能需要拷贝，也可能不需要，如果在原数组后有足够的空间，realloc通常会直接扩展原数组

## 区别分析
```C++
vector<int> vec1;
```

C++ 中，可以将类对象分配在栈上（会损失类一些特性），上面这行代码就是在栈上分配一个空的 vector<int> 对象。在离开函数时，随着栈展开，vec1 会被销毁。并且由于 RAII 机制，vec1 中的数组也会随之被释放。

```C++
vector<int>* vec2 = new vector<int>();
```

这行代码将vector<int>对象分配在堆上（包含上面三个字段以及vtable）。vec2 是一个指向 vector<int> 对象的指针，vec2 本身是分配在栈上的。在离开函数时，vec2 指针会被销毁，但是 vector<int> 对象不会被销毁，需要手动释放。如果忘记释放，会造成内存泄漏。vector<int>对象以及数组都会一直存在。
