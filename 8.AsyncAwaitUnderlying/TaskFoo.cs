class TaskFoo : Task<int>
{
    public bool Completed => _state == State.Completed;
    public int? ReturnValue { get; private set; } = null;

    private Process _process;
    private State _state = State.NotStarted;

    public TaskFoo(Process process)
    {
        _process = process;
    }

    private bool started = default;
    public void Run()
    {
        switch (_state)
        {
            case State.NotStarted:
                // 假设 StartAsync 只需要可以立即返回
                // 但是实际上，StartAsync 是像本方法一样的状态机
                // 你需要持续调用 StartAsync 状态机的 Run 方法，Complete 属性为 true 时，获取返回值
                // 然后你才能进行下面的 if 判断
                started = p.StartAsync().GetAwaiter().GetResult();
                
                if (!started)
                    _state = State.Completed;
                else
                    _state = State.RunState1;
                break;
            case RunState1:
                // 同上，这里简化了模型
                p.WaitForExitAsync().Wait();
                _state = State.RunState2;
                break;
            case RunState2:
                ReturnValue = p.ExitCode;
                _state = State.Completed;
                break;
            
            default:
                break;
        }
    }

    enum State
    {
        NotStarted,
        RunState1,
        RunState2,
        Completed,
        Error
    }
}
