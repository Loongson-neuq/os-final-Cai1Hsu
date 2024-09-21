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
                started = p.StartAsync().GetAwaiter().GetResult();
                
                if (!started)
                    _state = State.Completed;
                else
                    _state = State.RunState1;
                break;
            case RunState1:
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
