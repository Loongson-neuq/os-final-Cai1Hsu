## 8. [附加题][异步编程] 将以下异步函数转化成状态机闭包类

定义以下类表示含返回值的异步任务：
```
class Task<T>
{
  bool completed;
  T? return_value;
}
```

```
async Task<int> Foo(Process p)
{
  var started = await p.StartAsync();
  
  if (started)
  {
    await p.WaitForExitAsync();
    
    return p.ExitCode;
  }
  
  throw new Exception("Process not started!");
}
```

提示：思考 coroutine 的原理

本题没有注明语言，你可以使用任何语言，包括伪代码
